#ifndef DCORE_GRAPHICS_INTERNAL_H
#define DCORE_GRAPHICS_INTERNAL_H
#include <dcore/graphics.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct DCgState {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;

    size_t renderPassCount;
    VkRenderPass *renderPasses;

    VkAllocationCallbacks *allocator;
};

struct DCgMaterial {
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

void dcgiAddRenderPass(
    DCgState *state,
    size_t attachmentCount,
    VkAttachmentDescription *attachments,
    size_t subpassCount,
    VkSubpassDescription *subpasses
);

VkRenderPass dcgiGetRenderPass(DCgState *state, int index);
size_t dcgiGetPushConstantRanges(DCgState *state, int index, const VkPushConstantRange **ranges);
size_t dcgiGetSetLayouts(DCgState *state, int index, const VkDescriptorSetLayout **layouts);
size_t dcgiGetVertexBindings(DCgState *state, int index, const VkVertexInputBindingDescription **descriptions);
size_t dcgiGetVertexAttributes(DCgState *state, int index, const VkVertexInputAttributeDescription **descriptions);
uint32_t dcgiGetQueueFamily(DCgState *state, DCgQueueFamilyType type);

#endif
