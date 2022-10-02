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
	
	if(vkCreateInstance(&createInfo, state->allocator, &state->instance) != VK_SUCCESS)
		DCD_FATAL("Failed to create Vulkan Instance");

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
		
		DCD_MSGF(INFO, "Score: %zu", score);
		if(score > maxScrore) state->physicalDevice = devices[i];
	}
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
	selectPhysicalDevice(state);
}

void dcgDeinit(DCgState *state) {
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
