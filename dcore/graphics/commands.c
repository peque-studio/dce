#include <dcore/debug.h>
#include <dcore/graphics.h>
#include <dcore/graphics/internal.h>

DCgCmdPool *dcgNewCmdPool(DCgState *s, DCgCmdPoolType type) {
	VkCommandPool commandPool;
	VkCommandPoolCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	switch(type) {
	case DCG_CMD_POOL_TYPE_COMPUTE: createInfo.queueFamilyIndex = s->computeQueueFamily; break;
	case DCG_CMD_POOL_TYPE_GRAPHICS: createInfo.queueFamilyIndex = s->graphicsQueueFamily; break;
	default: DCD_WARNING("Bad DCgCmdPoolType: %d", type); break;
	}
	DC_ASSERT(vkCreateCommandPool(s->device, &createInfo, s->allocator, &commandPool) == VK_SUCCESS, "Failed to create command pool!");
	return (void *)commandPool;
}

void dcgFreeCmdPool(DCgState *s, DCgCmdPool *pool) {
	DEBUGIF(pool == NULL) {
		DCD_MSGF(ERROR, "Tried to free NULL command pool");
		return;
	}

	vkDestroyCommandPool(s->device, (void *)pool, s->allocator);
}
