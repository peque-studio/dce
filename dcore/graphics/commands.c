#include <dcore/debug.h>
#include <dcore/graphics.h>
#include <dcore/graphics/internal.h>
#include <string.h>

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

DCgCmdBuffer *dcgGetNewCmdBuffer(DCgState *s, DCgCmdPool *pool) {
	VkCommandBufferAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = (void *)pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer buffer;
	DC_ASSERT(vkAllocateCommandBuffers(s->device, &allocInfo, &buffer) == VK_SUCCESS, "Failed to allocate command buffers!");
	return (void *)buffer;
}

void dcgCmdBegin(DCgState *s, DCgCmdBuffer *cmds) {
	VkCommandBufferBeginInfo beginInfo = { 0 };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	DC_ASSERT(vkBeginCommandBuffer((VkCommandBuffer)cmds, &beginInfo) == VK_SUCCESS, "Failed to begin command buffer.");
}

void dcgCmdEnd(DCgState *s, DCgCmdBuffer *cmds) {
	DC_ASSERT(vkEndCommandBuffer((VkCommandBuffer)cmds) == VK_SUCCESS, "Failed to end command buffer.");
}

void dcgCmdBindVertexBuf(DCgState *s, DCgCmdBuffer *cmds, const DCgVertexBuffer *vbuf) {
	VkBuffer vertexBuffers[] = { vbuf->buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers((VkCommandBuffer)cmds, 0, 1, vertexBuffers, offsets);
}

void dcgCmdBindIndexBuf(DCgState *s, DCgCmdBuffer *cmds, const DCgIndexBuffer *ibuf, enum DCgIndexType type) {
	vkCmdBindIndexBuffer((VkCommandBuffer)cmds, ibuf->buffer, 0, (VkIndexType)type);
}

void dcgCmdBindMat(DCgState *s, DCgCmdBuffer *cmds, DCgMaterial *mat) {
	vkCmdBindPipeline((VkCommandBuffer)cmds, VK_PIPELINE_BIND_POINT_GRAPHICS, mat->pipeline);
}

void dcgCmdBeginRenderPass(DCgState *s, DCgCmdBuffer *cmds, const DCgCmdBeginRenderPassInfo *info) {
	DC_RASSERT(info != NULL, "Passed null info into dcgCmdBeginRenderPass");
	DC_RASSERT(info->renderPassIndex < s->renderPassCount, "Render pass index out of bounds.");
	VkRenderPassBeginInfo beginInfo = { 0 };
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.renderPass = s->renderPasses[info->renderPassIndex];
	beginInfo.renderArea.offset = (VkOffset2D){ info->renderArea.offset[0], info->renderArea.offset[1] };
	beginInfo.renderArea.extent = (VkExtent2D){ info->renderArea.extent[0], info->renderArea.extent[1] };
	beginInfo.clearValueCount = info->clearValueCount;
	beginInfo.pClearValues = dcmemAllocate(sizeof(VkClearValue) * info->clearValueCount);
	beginInfo.framebuffer = (VkFramebuffer)info->framebuffer;
	for(size_t i = 0; i < info->clearValueCount; ++i) {
		memcpy(
		  (void *)&beginInfo.pClearValues[i], &info->clearValues[i],
		  sizeof(info->clearValues[i]) > sizeof(beginInfo.pClearValues[i]) ? sizeof(beginInfo.pClearValues[i]) : sizeof(info->clearValues[i])
		);
	}

	vkCmdBeginRenderPass((VkCommandBuffer)cmds, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

	dcmemDeallocate((void *)beginInfo.pClearValues);
}

void dcgCmdEndRenderPass(DCgState *s, DCgCmdBuffer *cmds) { vkCmdEndRenderPass((VkCommandBuffer)cmds); }

void dcgSubmit(DCgState *s, const DCgSubmitInfo *info) {
	VkSubmitInfo submitInfo = { 0 };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = info->waitSemaphoreCount;
	submitInfo.pWaitSemaphores = (const VkSemaphore *)info->waitSemaphores;
	submitInfo.pWaitDstStageMask = (const VkPipelineStageFlags *)info->waitPipelineStages;
	submitInfo.signalSemaphoreCount = info->signalSemaphoreCount;
	submitInfo.pSignalSemaphores = (const VkSemaphore *)info->signalSemaphores;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = (VkCommandBuffer *)&info->commandBuffer;
	DC_ASSERT(vkQueueSubmit(dcgiGetQueue(s, info->queueIndex), 1, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS, "Failed to submit queue.");
}
