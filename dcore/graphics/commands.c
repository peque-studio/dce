#include <dcore/debug.h>
#include <dcore/graphics.h>
#include <dcore/graphics/internal.h>
#include <vulkan/vulkan_core.h>

DCgCmdPool *dcgNewCmdPool(DCgState *s, DCgCmdPoolType type) {
    VkCommandPool commandPool;
    VkCommandPoolCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = dcgiGetQueueFamily(s, type);
    DC_ASSERT(vkCreateCommandPool(s->device, &createInfo, s->allocator, &commandPool) == VK_SUCCESS,
        "Failed to create command pool!");
    return (void*)commandPool;
}

void dcgFreeCmdPool(DCgState *s, DCgCmdPool *pool) {
    DEBUGIF(pool == NULL) {
        DCD_MSGF(ERROR, "Tried to free NULL command pool");
        return;
    }
    
    vkDestroyCommandPool(s->device, (void*)pool, s->allocator);
}
