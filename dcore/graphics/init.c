#include "dcore/common.h"
#include "dcore/debug.h"
#include <dcore/graphics.h>
#include <dcore/graphics/internal.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

// extensions that we might need.
DCgiSuggestedExtension suggestedExtensions[] = {
	[DCGI_SUGGESTED_EXTENSION_DEBUG_REPORT]
		= { "VK_EXT_debug_report", false },
	[DCGI_SUGGESTED_EXTENSION_GET_PHYSICAL_DEVICE_PROPERTIES2]
		= { "VK_KHR_get_physical_device_properties2", false },
};

// layers that we might need.
DCgiSuggestedLayer suggestedLayers[] = {
	[DCGI_SUGGESTED_LAYER_VALIDATION]
		= { "VK_LAYER_KHRONOS_validation", false },
};

static void createInstance(DCgState *state, uint32_t appVersion, const char *appName) {
	DCD_MSGF(DEBUG, "creating instance...");

	// extensions required by glfw
	uint32_t requiredExtensionCount;
	const char **requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionCount);

	// allocate maximum number of extensions.
	const char **enabledExtensions = malloc((requiredExtensionCount + ARRAYSIZE(suggestedExtensions)) * sizeof(const char *));
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
	extensionProperties = malloc(sizeof(VkExtensionProperties) * extensionPropertiesCount);
	vkEnumerateInstanceExtensionProperties(NULL, &extensionPropertiesCount, extensionProperties);

	// check if we have a matching suggestedExtension.
	for(int i = 0; i < extensionPropertiesCount; ++i) {
		for(int j = 0; j < ARRAYSIZE(suggestedExtensions); ++j) {
			if(!suggestedExtensions[j].available
			&& strcmp(extensionProperties[i].extensionName, suggestedExtensions[j].name) == 0
			) {
				enabledExtensions[enabledExtensionCount++] = extensionProperties[i].extensionName;
				suggestedExtensions[j].available = true;
				break;
			}
		} 
	}

	for(int i = 0; i < ARRAYSIZE(suggestedExtensions); ++i) {
		if(suggestedExtensions[i].available) {
			DCD_MSGF(INFO, "Instance extension: '%s'.", suggestedExtensions[i].name);
		} else {
			DCD_MSGF(WARNING, "Instance extension: '%s'.", suggestedExtensions[i].name);
			DCD_MSGF(WARNING, "  `- not available!", suggestedExtensions[i].name);
		}
	}

	uint32_t enabledLayerCount = 0;
	const char **enabledLayers = malloc(sizeof(const char *) * ARRAYSIZE(suggestedLayers));

	// all available layers
	uint32_t layerPropertiesCount;
	vkEnumerateInstanceLayerProperties(&layerPropertiesCount, NULL);
	VkLayerProperties *layerProperties = malloc(sizeof(VkLayerProperties) * layerPropertiesCount);
	vkEnumerateInstanceLayerProperties(&layerPropertiesCount, layerProperties);

	// check if we have a matching suggestedLayers.
	for(int i = 0; i < layerPropertiesCount; ++i) {
		for(int j = 0; j < ARRAYSIZE(suggestedLayers); ++j) {
			if(!suggestedLayers[j].available
			&& strcmp(layerProperties[i].layerName, suggestedLayers[j].name) == 0
			) {
				enabledLayers[enabledLayerCount++] = layerProperties[i].layerName;
				suggestedLayers[j].available = true;
				break;
			}
		}
	}

	for(int i = 0; i < ARRAYSIZE(suggestedLayers); ++i) {
		if(suggestedLayers[i].available) {
			DCD_MSGF(INFO, "Instance layer: '%s'.", suggestedLayers[i].name);
		} else {
			DCD_MSGF(WARNING, "Instance layer: '%s'.", suggestedLayers[i].name);
			DCD_MSGF(WARNING, "  `- not available!", suggestedLayers[i].name);
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

	free(enabledExtensions);
	free(enabledLayers);
	free(extensionProperties);
	free(layerProperties);
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
	VkQueueFamilyProperties *queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
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
}

static bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice, size_t extCount, const char **exts) {
	uint32_t propertiesCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &propertiesCount, NULL);
	VkExtensionProperties *properties = malloc(sizeof(VkExtensionProperties) * propertiesCount);
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
	
	return notFound == 0;
}

static void selectPhysicalDevice(DCgState *state) {
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(state->instance, &deviceCount, NULL);
	VkPhysicalDevice *devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
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
			DCD_WARNING("Required extensions not supported.");
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
	uint32_t *queueFamilies = malloc(sizeof(uint32_t) * 2); // maximum possible number of queue families.
	size_t queueFamilyCount = 0;
	queueFamilyCount = addUniqueQueueFamily(queueFamilies, queueFamilyCount, state->graphicsQueueFamily);
	queueFamilyCount = addUniqueQueueFamily(queueFamilies, queueFamilyCount, state->computeQueueFamily);
	queueFamilyCount = addUniqueQueueFamily(queueFamilies, queueFamilyCount, state->presentQueueFamily);

	VkDeviceQueueCreateInfo *queues = malloc(sizeof(VkDeviceQueueCreateInfo) * queueFamilyCount);
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
	VkExtensionProperties *properties = malloc(sizeof(VkExtensionProperties) * propertiesCount);
	vkEnumerateDeviceExtensionProperties(state->physicalDevice, NULL, &propertiesCount, properties);
	const char **enabledExtensions = malloc(sizeof(const char *) * propertiesCount); // TODO: this is the maximum number of names

	size_t enabledExtensionCount = 0;
	for(uint32_t i = 0; i < propertiesCount; ++i) {
		if(strcmp(properties[i].extensionName, "VK_KHR_portability_subset") == 0)
			enabledExtensions[enabledExtensionCount++] = properties[i].extensionName;
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

	free(properties);
	free(enabledExtensions);
	free(queueFamilies);
	free(queues);
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
	state->renderPasses = malloc(sizeof(VkRenderPass) * state->renderPassCount);
}

DCgState *dcgNewState() {
	DCgState *state = malloc(sizeof(DCgState));
	state->allocator = NULL;
	state->instance = NULL;
	state->physicalDevice = NULL;
	return state;
}

void dcgFreeState(DCgState *state) {
	free(state);
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
}

void dcgDeinit(DCgState *state) {
	// TODO: Assert stuff here.
	for(size_t i = 0; i < state->renderPassCount; ++i)
		vkDestroyRenderPass(state->device, state->renderPasses[i], state->allocator);
	vkDestroyDevice(state->device, state->allocator);
	vkDestroySurfaceKHR(state->instance, state->surface, state->allocator);
	vkDestroyInstance(state->instance, state->allocator);
	glfwDestroyWindow(state->window);
	glfwTerminate();
}

void dcgiPrintGlfwErrors() {
	const char *error;
	if(glfwGetError(&error)) {
		DCD_MSGF(ERROR, "Glfw erorr: '%s'", error);
	}
}
