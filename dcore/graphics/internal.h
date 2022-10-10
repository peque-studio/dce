#ifndef DCORE_GRAPHICS_INTERNAL_H
#define DCORE_GRAPHICS_INTERNAL_H
#include <dcore/graphics.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

struct DCgState {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;

    size_t renderPassCount;
    VkRenderPass *renderPasses;

    VkAllocationCallbacks *allocator;

    uint32_t graphicsQueueFamily, computeQueueFamily, presentQueueFamily;

    GLFWwindow *window;
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
VkQueue dcgiGetQueue(DCgState *state, uint32_t index);

typedef struct { const char *name; bool available; } DCgiSuggestedExtension;
typedef struct { const char *name; bool available; } DCgiSuggestedLayer;

extern DCgiSuggestedExtension dcgiSuggestedExtensions[]; // extensions that we might need.
extern DCgiSuggestedLayer dcgiSuggestedLayers[]; // layers that we might need.

enum DCgiSuggestedExtensionIndex {
    DCGI_SUGGESTED_EXTENSION_DEBUG_REPORT,
    DCGI_SUGGESTED_EXTENSION_GET_PHYSICAL_DEVICE_PROPERTIES2,
};

enum DCgiSuggestedLayerIndex {
    DCGI_SUGGESTED_LAYER_VALIDATION,
};

void dcgiPrintGlfwErrors();

#endif
