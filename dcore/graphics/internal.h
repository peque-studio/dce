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
};

struct DCgMaterial {
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

void DCgiAddRenderPass(
    DCgState *state,
    size_t attachmentCount,
    VkAttachmentDescription *attachments,
    size_t subpassCount,
    VkSubpassDescription *subpasses
);

VkRenderPass DCgiGetRenderPass(DCgState *state, int index);
size_t DCgiGetPushConstantRanges(DCgState *state, int index, const VkPushConstantRange **ranges);
size_t DCgiGetSetLayouts(DCgState *state, int index, const VkDescriptorSetLayout **layouts);
size_t DCgiGetVertexBindings(DCgState *state, int index, const VkVertexInputBindingDescription **descriptions);
size_t DCgiGetVertexAttributes(DCgState *state, int index, const VkVertexInputAttributeDescription **descriptions);

#endif
