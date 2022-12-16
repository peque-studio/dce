#include "dcore/common.h"
#include "dcore/debug.h"
#include <dcore/graphics.h>
#include <dcore/graphics/internal.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

// extensions that we might need.
static const char *suggestedExtensions[] = {
	[DCGI_SUGGESTED_EXTENSION_DEBUG_REPORT]
		= "VK_EXT_debug_report",
	[DCGI_SUGGESTED_EXTENSION_GET_PHYSICAL_DEVICE_PROPERTIES2]
		= "VK_KHR_get_physical_device_properties2",
};

// layers that we might need.
static const char *suggestedLayers[] = {
	[DCGI_SUGGESTED_LAYER_VALIDATION]
		= "VK_LAYER_KHRONOS_validation",
};

static void createInstance(DCgState *state, uint32_t appVersion, const char *appName) {
	DCD_MSGF(DEBUG, "creating instance...");

	state->suggestedExtensions.count = ARRAYSIZE(suggestedExtensions);
	state->suggestedExtensions.extensions = dcmemAllocate(sizeof(DCgiSuggestedExtension) * state->suggestedExtensions.count);
	state->suggestedLayers.count = ARRAYSIZE(suggestedLayers);
	state->suggestedLayers.layers = dcmemAllocate(sizeof(DCgiSuggestedLayer) * state->suggestedLayers.count);

	for(int j = 0; j < ARRAYSIZE(suggestedExtensions); ++j) {
		state->suggestedExtensions.extensions[j].name = suggestedExtensions[j];
	}

	for(int j = 0; j < ARRAYSIZE(suggestedLayers); ++j) {
		state->suggestedLayers.layers[j].name = suggestedLayers[j];
	}

	// extensions required by glfw
	uint32_t requiredExtensionCount;
	const char **requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionCount);

	// allocate maximum number of extensions.
	const char **enabledExtensions = dcmemAllocate((requiredExtensionCount + ARRAYSIZE(suggestedExtensions)) * sizeof(const char *));
	size_t enabledExtensionCount = requiredExtensionCount; // filled with requiredExtensionCount extensions when used

	DCD_MSGF(DEBUG, "Required extension count: %d", requiredExtensionCount);
	// copy over the required extensions
	for(int i = 0; i < requiredExtensionCount; ++i) {
		enabledExtensions[i] = requiredExtensions[i];
	}

	// all available extensions
	VkExtensionProperties *extensionProperties;
	uint32_t extensionPropertiesCount;

	vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertiesCount, NULL);
	extensionProperties = dcmemAllocate(sizeof(VkExtensionProperties) * extensionPropertiesCount);
	vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertiesCount, extensionProperties);

	// check if we have a matching suggestedExtension.
	for(int i = 0; i < extensionPropertiesCount; ++i) {
		for(int j = 0; j < state->suggestedExtensions.count; ++j) {
			if(!state->suggestedExtensions.extensions[j].enabled
			&& strcmp(extensionProperties[i].extensionName, state->suggestedExtensions.extensions[j].name) == 0
			) {
				enabledExtensions[enabledExtensionCount++] = extensionProperties[i].extensionName;
				state->suggestedExtensions.extensions[j].enabled = true;
				break;
			}
		} 
	}

	for(int i = 0; i < state->suggestedExtensions.count; ++i) {
		if(state->suggestedExtensions.extensions[i].enabled) {
			DCD_MSGF(INFO, "Instance extension: '%s'.", state->suggestedExtensions.extensions[i].name);
		} else {
			DCD_MSGF(WARNING, "Instance extension: '%s'.", state->suggestedExtensions.extensions[i].name);
			DCD_MSGF(WARNING, "  `- not available!");
		}
	}

	uint32_t enabledLayerCount = 0;
	const char **enabledLayers = dcmemAllocate(sizeof(const char *) * ARRAYSIZE(suggestedLayers));

	// all available layers
	uint32_t layerPropertiesCount;
	vkEnumerateInstanceLayerProperties(&layerPropertiesCount, NULL);
	VkLayerProperties *layerProperties = dcmemAllocate(sizeof(VkLayerProperties) * layerPropertiesCount);
	vkEnumerateInstanceLayerProperties(&layerPropertiesCount, layerProperties);

	for(int j = 0; j < state->suggestedLayers.count; ++j) {
		DCD_DEBUG("Suggested layer #%d, '%s', enabled: %s",
			j, state->suggestedLayers.layers[j].name, state->suggestedLayers.layers[j].enabled ? "true" : "false");
	}

	// check if we have a matching suggestedLayers.
	for(int i = 0; i < layerPropertiesCount; ++i) {
		for(int j = 0; j < state->suggestedLayers.count; ++j) {
			DCD_DEBUG(
				"comparing layers: #%d '%s' and suggested #%d '%s', is already enabled: %s",
				i,
				layerProperties[i].layerName,
				j,
				state->suggestedLayers.layers[j].name,
				(state->suggestedLayers.layers[j].enabled ? "true" : "false")
			);
			if(!state->suggestedLayers.layers[j].enabled
			&& strcmp(layerProperties[i].layerName, state->suggestedLayers.layers[j].name) == 0
			) {
				DCD_DEBUG(" `- enabling");
				enabledLayers[enabledLayerCount++] = layerProperties[i].layerName;
				state->suggestedLayers.layers[j].enabled = true;
				break;
			}
		}
	}

	for(int i = 0; i < state->suggestedLayers.count; ++i) {
		if(state->suggestedLayers.layers[i].enabled) {
			DCD_MSGF(INFO, "Instance layer: '%s'.", state->suggestedLayers.layers[i].name);
		} else {
			DCD_MSGF(WARNING, "Instance layer: '%s'.", state->suggestedLayers.layers[i].name);
			DCD_MSGF(WARNING, "  `- not available!", state->suggestedLayers.layers[i].name);
		}
	}

	DCD_MSGF(INFO, "To be enabled extensions: %zu.", enabledExtensionCount);
	for(int i = 0; i < enabledExtensionCount; ++i) {
		DCD_MSGF(INFO, "Extension: '%s'.", enabledExtensions[i]);
	}

	DCD_MSGF(INFO, "To be enabled layers: %zu.", enabledLayerCount);
	for(int i = 0; i < enabledLayerCount; ++i) {
		DCD_MSGF(INFO, "Layer: '%s'.", enabledLayers[i]);
	}

	VkApplicationInfo applicationInfo = {0};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.applicationVersion = appVersion;
	applicationInfo.pApplicationName = appName;
	applicationInfo.engineVersion = VK_MAKE_VERSION(
		DCE_VERSION_MAJOR,
		DCE_VERSION_MINOR,
		DCE_VERSION_PATCH
	);
	applicationInfo.pEngineName = "DCE";
	applicationInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;
	createInfo.enabledExtensionCount = enabledExtensionCount;
	createInfo.ppEnabledExtensionNames = enabledExtensions;
	createInfo.enabledLayerCount = enabledLayerCount;
	createInfo.ppEnabledLayerNames = enabledLayers;
	
	DC_RASSERT(vkCreateInstance(&createInfo, state->allocator, &state->instance) == VK_SUCCESS,
		"Failed to create Vulkan instance");
	
	dcmemDeallocate(enabledExtensions);
	dcmemDeallocate(enabledLayers);
	dcmemDeallocate(extensionProperties);
	dcmemDeallocate(layerProperties);
	DCD_MSGF(DEBUG, "Done creating instance...");
}

static const char *physicalDeviceTypeNames[] = {
	"Other",
	"Integrated GPU",
	"Discrete GPU",
	"Virtual GPU",
	"CPU"
};

static size_t physicalDeviceTypeScores[] = {
	0,
	5000,
	10000,
	7500,
	2500
};

typedef struct QueueFamilies {
	uint32_t graphics, compute, present;
} QueueFamilies;

static void findQueueFamilies(DCgState *state, VkPhysicalDevice physicalDevice, QueueFamilies *families) {
	families->compute = UINT32_MAX;
	families->graphics = UINT32_MAX;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
	VkQueueFamilyProperties *queueFamilies = dcmemAllocate(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);
	for(uint32_t i = 0; i < queueFamilyCount; ++i) {
		if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			families->graphics = i;
		if(queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			families->compute = i;
		VkBool32 supported;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, state->surface, &supported);
		if(supported)
			families->present = i;
	}

	dcmemDeallocate(queueFamilies);
}

static bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice, size_t extCount, const char **exts) {
	uint32_t propertiesCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &propertiesCount, NULL);
	VkExtensionProperties *properties = dcmemAllocate(sizeof(VkExtensionProperties) * propertiesCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &propertiesCount, properties);

	size_t notFound = extCount; // number of extensions that are not yet found.

	// TODO: optimize this search.
	for(uint32_t i = 0; i < extCount; ++i) {
		for(size_t j = 0; j < propertiesCount; ++j) {
			if(strcmp(properties[j].extensionName, exts[i]) == 0) {
				notFound -= 1;
				break;
			}
		}
	}
	
	dcmemDeallocate(properties);
	return notFound == 0;
}

static void selectPhysicalDevice(DCgState *state) {
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(state->instance, &deviceCount, NULL);
	VkPhysicalDevice *devices = dcmemAllocate(sizeof(VkPhysicalDevice) * deviceCount);
	vkEnumeratePhysicalDevices(state->instance, &deviceCount, devices);

	int maxScrore = 0;
	for(int i = 0; i < deviceCount; ++i) {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(devices[i], &properties);
		DCD_MSGF(INFO, "Device '%s', %s",
			properties.deviceName,
			physicalDeviceTypeNames[properties.deviceType]
		);
	
		size_t score = physicalDeviceTypeScores[properties.deviceType]
			+ properties.limits.maxDescriptorSetSamplers * 100
			+ properties.limits.maxBoundDescriptorSets * 100
			+ properties.limits.maxDescriptorSetUniformBuffers * 100
			+ properties.limits.maxUniformBufferRange
			+ (properties.limits.maxFramebufferWidth / 100)
			* (properties.limits.maxFramebufferHeight / 100);
		
		QueueFamilies queueFamilies;
		findQueueFamilies(state, devices[i], &queueFamilies);

		if(queueFamilies.graphics == INT32_MAX) {
			DCD_WARNING("No graphics queue family");
			score = 0;
		}

		if(queueFamilies.present == INT32_MAX) {
			DCD_WARNING("No present queue family");
			score = 0;
		}
		if(queueFamilies.graphics == queueFamilies.present) score += score / 10;

		const char *requiredExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		bool supported = checkDeviceExtensionSupport(devices[i], ARRAYSIZE(requiredExtensions), requiredExtensions);
		if(!supported) {
			DCD_FATAL("Required extensions not supported.");
			score = 0;
		}
		
		DCD_MSGF(INFO, "Score: %zu", score);
		if(score >= maxScrore) {
			state->computeQueueFamily = queueFamilies.compute;
			state->graphicsQueueFamily = queueFamilies.graphics;
			state->presentQueueFamily = queueFamilies.present;
			state->physicalDevice = devices[i];
		}
	}

	dcmemDeallocate(devices);

	DC_RASSERT(state->graphicsQueueFamily != UINT32_MAX, "Could not find a graphics queue family (required)");
	DC_RASSERT(state->presentQueueFamily != UINT32_MAX, "Could not find a present queue family (required)");
}

static void createSurface(DCgState *state) {
	DCD_DEBUG("Creating surface...");
	DC_RASSERT(glfwCreateWindowSurface(state->instance, state->window, state->allocator, &state->surface) == VK_SUCCESS,
		"Failed to create window surface!");
	DCD_DEBUG("Done creating surface");
}

static size_t addUniqueQueueFamily(uint32_t *families, size_t last, uint32_t new) {
	if(new == UINT32_MAX) return last; // don't add non-existent families.
	for(size_t i = 0; i < last + 1; ++i) if(new == families[i]) return last;
	families[last] = new;
	return last + 1;
}

static void createLogicalDevice(DCgState *state) {
	DCD_DEBUG("Creating a logical device...");
	uint32_t *queueFamilies = dcmemAllocate(sizeof(uint32_t) * 2); // maximum possible number of queue families.
	size_t queueFamilyCount = 0;
	queueFamilyCount = addUniqueQueueFamily(queueFamilies, queueFamilyCount, state->graphicsQueueFamily);
	queueFamilyCount = addUniqueQueueFamily(queueFamilies, queueFamilyCount, state->computeQueueFamily);
	queueFamilyCount = addUniqueQueueFamily(queueFamilies, queueFamilyCount, state->presentQueueFamily);

	VkDeviceQueueCreateInfo *queues = dcmemAllocate(sizeof(VkDeviceQueueCreateInfo) * queueFamilyCount);
	memset(queues, 0, sizeof(VkDeviceQueueCreateInfo) * queueFamilyCount);
	float queuePriority = 1.0f;
	for(int i = 0; i < queueFamilyCount; ++i) {
		queues[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queues[i].queueCount = 1;
		queues[i].queueFamilyIndex = queueFamilies[i];
		queues[i].pQueuePriorities = &queuePriority;
	}

	VkPhysicalDeviceFeatures features = {0};

	uint32_t propertiesCount;
	vkEnumerateDeviceExtensionProperties(state->physicalDevice, NULL, &propertiesCount, NULL);
	VkExtensionProperties *properties = dcmemAllocate(sizeof(VkExtensionProperties) * propertiesCount);
	vkEnumerateDeviceExtensionProperties(state->physicalDevice, NULL, &propertiesCount, properties);
	const char **enabledExtensions = dcmemAllocate(sizeof(const char *) * propertiesCount); // TODO: this is the maximum number of names

	size_t enabledExtensionCount = 0;
	enabledExtensions[enabledExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	for(uint32_t i = 0; i < propertiesCount; ++i) {
		if(strcmp(properties[i].extensionName, "VK_KHR_portability_subset") == 0)
			enabledExtensions[enabledExtensionCount++] = properties[i].extensionName;
	}

	DCD_DEBUG("total enabled extensions: %zu", enabledExtensionCount);
	for(size_t i = 0; i < enabledExtensionCount; ++i) {
		DCD_DEBUG("enabled extension: %s", enabledExtensions[i]);
	}

	VkDeviceCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queues;
	createInfo.queueCreateInfoCount = queueFamilyCount;
	createInfo.enabledExtensionCount = enabledExtensionCount;
	createInfo.ppEnabledExtensionNames = enabledExtensions;
	createInfo.pEnabledFeatures = &features;

	DC_RASSERT(vkCreateDevice(state->physicalDevice, &createInfo, state->allocator, &state->device) == VK_SUCCESS,
		"Failed to create logical device");

	dcmemDeallocate(properties);
	dcmemDeallocate(enabledExtensions);
	dcmemDeallocate(queueFamilies);
	dcmemDeallocate(queues);
	DCD_DEBUG("Logical device created!");
}

VkQueue dcgiGetQueue(DCgState *state, uint32_t index) {
	DC_ASSERT(index < 3, "Queue family index too big!");
	static VkQueue queues[3] = { 0 }; // TODO: this is the maximum possible number of queues 
	if(queues[index] != NULL) return queues[index];
	vkGetDeviceQueue(state->device, index, 0, &queues[index]);
	return queues[index];
}

void createRenderPasses(DCgState *state) {
	state->renderPassCount = 1;
	state->renderPasses = dcmemAllocate(sizeof(VkRenderPass) * state->renderPassCount);
}

static void selectPresentMode(DCgState *state) {
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(state->physicalDevice, state->surface, &presentModeCount, NULL);
	VkPresentModeKHR *presentModes = dcmemAllocate(sizeof(VkPresentModeKHR) * presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(state->physicalDevice, state->surface, &presentModeCount, presentModes);
	for(uint32_t i = 0; i < presentModeCount; ++i) {
		if(presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			state->presentMode = presentModes[i];
			dcmemDeallocate(presentModes);
			return;
		}
	}
	dcmemDeallocate(presentModes);
	state->presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

static void selectSurfaceFormat(DCgState *state) {
	uint32_t surfaceFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(state->physicalDevice, state->surface, &surfaceFormatCount, NULL);
	VkSurfaceFormatKHR *surfaceFormats = dcmemAllocate(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(state->physicalDevice, state->surface, &surfaceFormatCount, surfaceFormats);
	for(uint32_t i = 0; i < surfaceFormatCount; ++i) {
		if(surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB
		&& surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			state->surfaceFormat = surfaceFormats[i];
			dcmemDeallocate(surfaceFormats);
			return;
		}
	}

	state->surfaceFormat = surfaceFormats[0];
	dcmemDeallocate(surfaceFormats);
}

static void createSwapchain(DCgState *state) {
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(state->physicalDevice, state->surface, &capabilities);

	uint32_t imageCount = capabilities.minImageCount + 1;
	if(capabilities.maxImageCount > 0
	&& imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}
	
	VkExtent2D extent;
	glfwGetFramebufferSize(state->window, (int*)&extent.width, (int*)&extent.height);

	selectSurfaceFormat(state);
	selectPresentMode(state);

	VkSwapchainCreateInfoKHR createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = state->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = state->surfaceFormat.format;
	createInfo.imageColorSpace = state->surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = state->presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE; // TODO: allow setting the oldSwapchain

	uint32_t queueFamilyIndices[] = { state->graphicsQueueFamily, state->presentQueueFamily };

	if(state->graphicsQueueFamily != state->presentQueueFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = NULL;
	}

	DC_RASSERT(vkCreateSwapchainKHR(state->device, &createInfo, state->allocator, &state->swapchain) == VK_SUCCESS,
		"Failed to create swapchain");
}

DCgState *dcgNewState() {
	DCgState *state = dcmemAllocate(sizeof(DCgState));
	state->allocator = NULL; // TODO: custom state->allocator for logging
	state->instance = NULL;
	state->physicalDevice = NULL;
	state->renderPassCount = 0;

	state->renderPassCount = 0;
	state->descriptorSetLayoutsCount = 0;
	state->vertexAttributesCount = 0;
	state->vertexBindingsCount = 0;
	state->pushConstantRangesCount = 0;
	return state;
}

void dcgFreeState(DCgState *state) {
	dcmemDeallocate(state);
}

void dcgInit(DCgState *state, uint32_t appVersion, const char *appName) {
	if(!glfwInit()) dcgiPrintGlfwErrors();
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	state->window = glfwCreateWindow(640, 480, appName, NULL, NULL);
	if(state->window == NULL) dcgiPrintGlfwErrors();

	createInstance(state, appVersion, appName);
	createSurface(state);
	selectPhysicalDevice(state);
	createLogicalDevice(state);
	createSwapchain(state);
}

void dcgDeinit(DCgState *state) {
	if(state->swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(state->device, state->swapchain, state->allocator);
	}

	if(state->renderPassCount) {
		DC_ASSERT(state->renderPasses != NULL, "state->renderPasses == NULL and state->renderPassCount != 0");
		for(size_t i = 0; i < state->renderPassCount; ++i)
			if(state->renderPasses[i] != VK_NULL_HANDLE)
				vkDestroyRenderPass(state->device, state->renderPasses[i], state->allocator);
			else DCD_WARNING("state->renderPasses[%zu] == VK_NULL_HANDLE", i);
		dcmemDeallocate(state->renderPasses);
	}

	if(state->descriptorSetLayoutsCount) {
		DC_ASSERT(state->descriptorSetLayouts != NULL, "state->descriptorSetLayouts == NULL and state->descriptorSetLayoutsCount != 0");
		for(size_t i = 0; i < state->descriptorSetLayoutsCount; ++i)
			for(size_t j = 0; j < state->descriptorSetLayouts[i].count; ++j) {
				if(state->descriptorSetLayouts[i].layouts[j] != VK_NULL_HANDLE)
					vkDestroyDescriptorSetLayout(state->device, state->descriptorSetLayouts[i].layouts[j], state->allocator);
				else DCD_WARNING("state->descriptorSetLayouts[%zu].layouts[%zu] == VK_NULL_HANDLE", i, j);
			}
		dcmemDeallocate(state->descriptorSetLayouts);
	}

	if(state->pushConstantRangesCount) {
		DC_ASSERT(state->pushConstantRanges != NULL, "state->pushConstantRanges == NULL and state->pushConstantRangesCount != 0");
		dcmemDeallocate(state->pushConstantRanges);
	}

	if(state->vertexAttributesCount) {
		DC_ASSERT(state->vertexAttributes != NULL, "state->vertexAttributes == NULL and state->vertexAttributesCount != 0");
		dcmemDeallocate(state->vertexAttributes);
	}

	if(state->vertexBindingsCount) {
		DC_ASSERT(state->vertexBindings != NULL, "state->vertexBindings == NULL and state->vertexBindingsCount != 0");
		dcmemDeallocate(state->vertexBindings);
	}

	vkDestroySurfaceKHR(state->instance, state->surface, state->allocator);
	vkDestroyDevice(state->device, state->allocator);
	vkDestroyInstance(state->instance, state->allocator);
	glfwDestroyWindow(state->window);
	glfwTerminate();

	dcmemDeallocate(state->suggestedLayers.layers);
	dcmemDeallocate(state->suggestedExtensions.extensions);
}

void dcgiPrintGlfwErrors() {
	const char *error;
	if(glfwGetError(&error)) {
		DCD_MSGF(ERROR, "Glfw erorr: '%s'", error);
	}
}

size_t dcgiGetPushConstantRanges(DCgState *state, int index, const VkPushConstantRange **ranges) {
	DC_ASSERT(index < state->pushConstantRangesCount, "Tried to access push constant renages with index out of bounds.");
	if(ranges != NULL) *ranges = state->pushConstantRanges[index].ranges;
	return state->pushConstantRanges[index].count;
}

size_t dcgiGetSetLayouts(DCgState *state, int index, const VkDescriptorSetLayout **layouts) {
	DC_ASSERT(index < state->descriptorSetLayoutsCount, "Tried to access descriptor set layouts with index out of bounds.");
	if(layouts != NULL) *layouts = state->descriptorSetLayouts[index].layouts;
	return state->descriptorSetLayouts[index].count;
}

size_t dcgiGetVertexBindings(DCgState *state, int index, const VkVertexInputBindingDescription **descriptions) {
	DC_ASSERT(index < state->vertexBindingsCount, "Tried to access vertex bindings with index out of bounds.");
	if(descriptions != NULL) *descriptions = state->vertexBindings[index].bindings;
	return state->vertexBindings[index].count;
}

size_t dcgiGetVertexAttributes(DCgState *state, int index, const VkVertexInputAttributeDescription **descriptions) {
	DC_ASSERT(index < state->vertexAttributesCount, "Tried to access vertex attributes with index out of bounds.");
	if(descriptions != NULL) *descriptions = state->vertexAttributes[index].attributes;
	return state->vertexAttributes[index].count;
}

VkRenderPass dcgiGetRenderPass(DCgState *state, size_t index) {
	DC_ASSERT(index < state->renderPassCount, "Tried to access render pass with index out of bounds.");
	return state->renderPasses[index];
}


VkPushConstantRange *dcgiAddPushConstantRanges(DCgState *state, size_t count) {
	if(state->pushConstantRangesCount == 0) {
		state->pushConstantRanges = dcmemAllocate(
			sizeof(*state->pushConstantRanges) +
			sizeof(VkPushConstantRange) * count
		);
		state->pushConstantRangesCount += 1;
	} else {
		/* due to each element in the array being a different size, we manually
		   compute the size of the array. */
		
		size_t totalSize = 0;
		for(size_t i = 0; i < state->pushConstantRangesCount; ++i) { 
			totalSize += sizeof(state->pushConstantRanges[i]);
			totalSize += state->pushConstantRanges[i].count * sizeof(VkPushConstantRange);
		}

		state->pushConstantRanges = dcmemReallocate(
			state->pushConstantRanges,
			totalSize +
			sizeof(*state->pushConstantRanges) +
			sizeof(VkPushConstantRange) * count
		);
	}

	state->pushConstantRanges[state->pushConstantRangesCount - 1].count = count;
	return state->pushConstantRanges[state->pushConstantRangesCount - 1].ranges;
}

VkDescriptorSetLayout *dcgiAddDescriptorSetLayouts(DCgState *state, size_t count) {
	if(state->descriptorSetLayoutsCount == 0) {
		state->descriptorSetLayouts = dcmemAllocate(
			sizeof(*state->descriptorSetLayouts) +
			sizeof(VkDescriptorSetLayout) * count
		);
		state->descriptorSetLayoutsCount += 1;
	} else {
		/* due to each element in the array being a different size, we manually
		   compute the size of the array. */
		
		size_t totalSize = 0;
		for(size_t i = 0; i < state->descriptorSetLayoutsCount; ++i) { 
			totalSize += sizeof(state->descriptorSetLayouts[i]);
			totalSize += state->descriptorSetLayouts[i].count * sizeof(VkDescriptorSetLayout);
		}

		state->descriptorSetLayouts = dcmemReallocate(
			state->descriptorSetLayouts,
			totalSize +
			sizeof(*state->descriptorSetLayouts) +
			sizeof(VkDescriptorSetLayout) * count
		);
	}

	state->descriptorSetLayouts[state->descriptorSetLayoutsCount - 1].count = count;
	return state->descriptorSetLayouts[state->descriptorSetLayoutsCount - 1].layouts;
}

VkVertexInputBindingDescription *dcgiAddVertexBindings(DCgState *state, size_t count) {
	if(state->vertexBindingsCount == 0) {
		state->vertexBindings = dcmemAllocate(
			sizeof(*state->vertexBindings) +
			sizeof(VkVertexInputBindingDescription) * count
		);
		state->vertexBindingsCount += 1;
	} else {
		/* due to each element in the array being a different size, we manually
		   compute the size of the array. */
		
		size_t totalSize = 0;
		for(size_t i = 0; i < state->vertexBindingsCount; ++i) { 
			totalSize += sizeof(state->vertexBindings[i]);
			totalSize += state->vertexBindings[i].count * sizeof(VkVertexInputBindingDescription);
		}

		state->vertexBindings = dcmemReallocate(
			state->vertexBindings,
			totalSize +
			sizeof(*state->vertexBindings) +
			sizeof(VkVertexInputBindingDescription) * count
		);
	}

	state->vertexBindings[state->vertexBindingsCount - 1].count = count;
	return state->vertexBindings[state->vertexBindingsCount - 1].bindings;
}

VkVertexInputAttributeDescription *dcgiAddVertexAttributes(DCgState *state, size_t count) {
	if(state->vertexAttributesCount == 0) {
		state->vertexAttributes = dcmemAllocate(
			sizeof(*state->vertexAttributes) +
			sizeof(VkVertexInputAttributeDescription) * count
		);
		state->vertexAttributesCount += 1;
	} else {
		/* due to each element in the array being a different size, we manually
		   compute the size of the array. */
		
		size_t totalSize = 0;
		for(size_t i = 0; i < state->vertexAttributesCount; ++i) { 
			totalSize += sizeof(state->vertexAttributes[i]);
			totalSize += state->vertexAttributes[i].count * sizeof(VkVertexInputAttributeDescription);
		}

		state->vertexAttributes = dcmemReallocate(
			state->vertexAttributes,
			totalSize +
			sizeof(*state->vertexAttributes) +
			sizeof(VkVertexInputAttributeDescription) * count
		);
	}

	state->vertexAttributes[state->vertexAttributesCount - 1].count = count;
	return state->vertexAttributes[state->vertexAttributesCount - 1].attributes;
}

VkRenderPass dcgiAddRenderPass(
	DCgState *state,
	size_t attachmentCount,
	VkAttachmentDescription *attachments,
	size_t subpassCount,
	VkSubpassDescription *subpasses,
	size_t dependencyCount,
	VkSubpassDependency *dependencies
) {
	VkRenderPassCreateInfo createInfo = {0};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.subpassCount = subpassCount;
	createInfo.pSubpasses = subpasses;
	createInfo.attachmentCount = attachmentCount;
	createInfo.pAttachments = attachments;
	createInfo.dependencyCount = dependencyCount;
	createInfo.pDependencies = dependencies;
	if(state->renderPassCount == 0) {
		state->renderPassCount = 1;
		state->renderPasses = dcmemAllocate(sizeof(VkRenderPass));
	} else {
		state->renderPasses = dcmemReallocate(
			state->renderPasses,
			sizeof(VkRenderPass) * ++state->renderPassCount
		);
	}

	DC_RVASSERT(vkCreateRenderPass(
		state->device, &createInfo, state->allocator,
		&state->renderPasses[state->renderPassCount - 1]
	) == VK_SUCCESS, "Failed to create render pass!", 0);

	return state->renderPasses[state->renderPassCount - 1];
}
