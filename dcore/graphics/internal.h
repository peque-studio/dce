#ifndef DCORE_GRAPHICS_INTERNAL_H
#define DCORE_GRAPHICS_INTERNAL_H
#include <dcore/graphics.h>
#include <vulkan/vulkan.h>

struct DCgState {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
};

struct DCgMaterial {
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

#endif
