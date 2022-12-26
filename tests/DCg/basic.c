#include <dcore/common.h>
#include <dcore/debug.h>
#include <dcore/graphics.h>
#include <dcore/renderers/basic.h>
#include <tests/test.h>

#if defined(__unix) || defined(__unix__) || defined(__APPLE__)
#	include <sys/stat.h>
#	include <sys/types.h>

off_t fsize(const char *filename) {
	struct stat st;
	if(stat(filename, &st) == 0) return st.st_size;
	return -1;
}
#endif

void loadShaderModuleFromFile(DCgState *state, DCgShaderModule *module, const char *filename, DCgShaderStage stage) {
	FILE *fin = fopen(filename, "rb");
	DC_ASSERT(fin != NULL, "Failed to open file to load a shader module from.");
	off_t fsz = fsize(filename);

	size_t codeSize = fsz;
	uint32_t *code = dcmemAllocate(fsz);
	fread(code, 1, fsz, fin);

	DCD_DEBUG("Read shader module, codeSize: %zu", codeSize);
	dcgInitShaderModule(state, module, stage, codeSize, code, "main");

	dcmemDeallocate(code);
	fclose(fin);
}

DCT_TEST(basicRendererInit, "basic renderer init test") {
	DCD_MSGF(DEBUG, "Creating State");
	DCgState *state = dcgNewState();
	DCD_MSGF(DEBUG, "Initializing State");
	dcgInit(state, 1, "DCE Tests");

	DCD_DEBUG("dcgBasicRendererCreateInfo");
	dcgBasicRendererCreateInfo(state);

	size_t swapchainImageCount = dcgGetSwapchainImageCount(state);
	DCgImageView **swapchainImageViews = dcmemAllocate(sizeof(DCgImageView *) * swapchainImageCount);
	DCgFramebuffer **swapchainFramebuffers = dcmemAllocate(sizeof(DCgFramebuffer *) * swapchainImageCount);

	for(size_t i = 0; i < swapchainImageCount; ++i) {
		swapchainImageViews[i] = dcgNewImageViewFromSwapchain(state, i);
	}

	for(size_t i = 0; i < swapchainImageCount; ++i) {
		swapchainFramebuffers[i] = dcgNewFramebuffer(
		  state, &(DcgFramebufferInfo){ .attachmentCount = 1, .attachments = &swapchainImageViews[i], .renderPassIndex = 0, .size = { 640, 480, 1 } }
		);
	}

	DCgCmdPool *pool = dcgNewCmdPool(state, DCG_CMD_POOL_TYPE_GRAPHICS);
	DCgCmdBuffer *buffer = dcgGetNewCmdBuffer(state, pool);

	DCgShaderModule modules[2];
	loadShaderModuleFromFile(state, &modules[0], "data/shaders/basic.vert.spirv", DCG_SHADER_STAGE_VERTEX);
	loadShaderModuleFromFile(state, &modules[1], "data/shaders/basic.frag.spirv", DCG_SHADER_STAGE_FRAGMENT);

	DCgMaterial *material = dcgNewMaterial(
	  state, 2, modules,
	  &(DCgMaterialInfo){
	    .scissorOffset = { 0, 0 },
	    .scissorExtent = { 0, 0 },
	    .cullMode = DCG_CULL_MODE_NONE,
	    .lineWidth = 1.0f,
	    .polygonMode = DCG_POLYGON_MODE_FILL,
	    .enableDiscard = false,
	    .enableDepthTest = false,
	    .enableDepthWrite = false,
	    .depthCompareOp = DCG_COMAPRE_OP_ALWAYS,
	    .enableDepthBoundsTest = false,
	    .minDepthBound = 0.0f,
	    .maxDepthBound = 1.0f,
	    .enableStencilTest = false,
	    .viewportExtent = { 640, 480 },
	    .pushConstantsIndex = 0,
	    .descriptorSetsIndex = 0,
	    .vertexInputIndex = 0,
	  },
	  NULL
	);

	DCgSemaphore *imageAvailableSemaphore = dcgNewSemaphore(state);
	DCgSemaphore *renderFinishedSemaphore = dcgNewSemaphore(state);
	DCgFence *inFlightFence = dcgNewFence(state, true);

	while(!dcgShouldClose(state)) {
		dcgUpdate(state);

		dcgWaitForFence(state, inFlightFence);
		uint32_t imageIndex = dcgAcquireNextSwapchainImage(state, imageAvailableSemaphore, NULL);

		dcgCmdBegin(state, buffer);
		dcgCmdBeginRenderPass(
		  state, buffer,
		  &(DCgCmdBeginRenderPassInfo){
		    .renderPassIndex = 0,
		    .renderArea = { .offset = { 0, 0 }, .extent = { 640, 480 } },
		    .clearValueCount = 1,
		    .clearValues = (DCgClearValue[2]){
					{ .color = { 0, 0, 0, 0 } },
					{ .color = { 0, 1, 1, 0 } },
				},
				.framebuffer = swapchainFramebuffers[imageIndex]
		  }
		);
		dcgCmdBindMat(state, buffer, material);
		dcgCmdEndRenderPass(state, buffer);
		dcgCmdEnd(state, buffer);
		dcgSubmit(
		  state,
		  &(DCgSubmitInfo){
		    .commandBuffer = buffer,
		    .queueIndex = 0,
		    .waitSemaphoreCount = 1,
		    .waitSemaphores = &imageAvailableSemaphore,
		    .waitPipelineStages = (enum DCgPipelineStage[1]){ DCG_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
		    .signalSemaphoreCount = 1,
		    .signalSemaphores = &renderFinishedSemaphore,
		  }
		);

		dcgPresent(
		  state,
		  &(DCgPresentInfo){
		    .queueIndex = 0,
		    .waitSemaphoreCount = 1,
		    .waitSemaphores = &renderFinishedSemaphore,
		    .imageIndex = imageIndex,
		  }
		);
	}

	dcgWaitForQueue(state, 0);

	DCD_MSGF(DEBUG, "DeInitializing State");
	dcgFreeFence(state, inFlightFence);
	dcgFreeSemaphore(state, renderFinishedSemaphore);
	dcgFreeSemaphore(state, imageAvailableSemaphore);
	dcgFreeMaterial(state, material);
	dcgFreeShaderModule(state, &modules[0]);
	dcgFreeShaderModule(state, &modules[1]);
	dcgFreeCmdPool(state, pool);
	for(size_t i = 0; i < swapchainImageCount; ++i) {
		dcgFreeFramebuffer(state, swapchainFramebuffers[i]);
		dcgFreeImageView(state, swapchainImageViews[i]);
	}
	dcgDeinit(state);
	DCD_MSGF(DEBUG, "Freeing State");
	dcgFreeState(state);

	return 0;
}
