#ifndef DCORE_GRAPHICS_INTERNAL_H
#define DCORE_GRAPHICS_INTERNAL_H
#include <dcore/graphics.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

typedef struct { const char *name; bool enabled; } DCgiSuggestedExtension;
typedef struct { const char *name; bool enabled; } DCgiSuggestedLayer;

struct DCgState {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surfaceFormat;

    struct {
        size_t count;
        DCgiSuggestedExtension *extensions;
    } suggestedExtensions;

    struct {
        size_t count;
        DCgiSuggestedLayer *layers;
    } suggestedLayers;

    VkPresentModeKHR presentMode;

    size_t renderPassCount;
    VkRenderPass *renderPasses;

    VkSwapchainKHR swapchain;

    VkAllocationCallbacks *allocator;

    uint32_t graphicsQueueFamily, computeQueueFamily, presentQueueFamily;

    GLFWwindow *window;

    size_t pushConstantRangesCount;
    struct {
        size_t count;
        VkPushConstantRange ranges[];
    } *pushConstantRanges;

    size_t descriptorSetLayoutsCount;
    struct {
        size_t count;
        VkDescriptorSetLayout layouts[];
    } *descriptorSetLayouts;

    size_t vertexBindingsCount;
    struct {
        size_t count;
        VkVertexInputBindingDescription bindings[];
    } *vertexBindings;

    size_t vertexAttributesCount;
    struct {
        size_t count;
        VkVertexInputAttributeDescription attributes[];
    } *vertexAttributes;
};

struct DCgMaterial {
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

VkRenderPass dcgiAddRenderPass(
    DCgState *state,
    size_t attachmentCount,
    VkAttachmentDescription *attachments,
    size_t subpassCount,
    VkSubpassDescription *subpasses,
	size_t dependencyCount,
	VkSubpassDependency *dependencies
);

VkPushConstantRange *dcgiAddPushConstantRanges(DCgState *state, size_t count);
VkDescriptorSetLayout *dcgiAddDescriptorSetLayouts(DCgState *state, size_t count);
VkVertexInputBindingDescription *dcgiAddVertexBindings(DCgState *state, size_t count);
VkVertexInputAttributeDescription *dcgiAddVertexAttributes(DCgState *state, size_t count);

VkRenderPass dcgiGetRenderPass(DCgState *state, size_t index);
size_t dcgiGetPushConstantRanges(DCgState *state, int index, const VkPushConstantRange **ranges);
size_t dcgiGetSetLayouts(DCgState *state, int index, const VkDescriptorSetLayout **layouts);
size_t dcgiGetVertexBindings(DCgState *state, int index, const VkVertexInputBindingDescription **descriptions);
size_t dcgiGetVertexAttributes(DCgState *state, int index, const VkVertexInputAttributeDescription **descriptions);
VkQueue dcgiGetQueue(DCgState *state, uint32_t index);

enum DCgiSuggestedExtensionIndex {
    DCGI_SUGGESTED_EXTENSION_DEBUG_REPORT,
    DCGI_SUGGESTED_EXTENSION_GET_PHYSICAL_DEVICE_PROPERTIES2,
};

enum DCgiSuggestedLayerIndex {
    DCGI_SUGGESTED_LAYER_VALIDATION,
};

void dcgiPrintGlfwErrors();

#endif
