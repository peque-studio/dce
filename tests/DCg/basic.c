#include <dcore/common.h>
#include <dcore/debug.h>
#include <dcore/graphics.h>
#include <dcore/renderers/basic.h>
#include <tests/test.h>

void loadShaderModuleFromFile(DCgState *state, DCgShaderModule *module, const char *filename, DCgShaderStage stage) {
	FILE *fin = fopen(filename, "rb");
	DC_ASSERT(fin != NULL, "Failed to open file to load a shader module from.");

	size_t codeSize = 0, codeAllocated = 256;
	uint32_t *code = dcmemAllocate(codeAllocated);

	size_t actuallyRead = 0;
	while(1) {
		if(codeSize > codeAllocated) {
			codeAllocated += 256;
			code = dcmemReallocate(code, codeAllocated);
		}

		actuallyRead = fread(code + codeSize, 1, codeAllocated - codeSize, fin);
		if(actuallyRead == 0) break;

		codeSize += actuallyRead;
	}

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

	DCgCmdPool *pool = dcgNewCmdPool(state, DCG_CMD_POOL_TYPE_GRAPHICS);
	DCgCmdBuffer *buffer = dcgGetNewCmdBuffer(state, pool);

	DCgShaderModule modules[2];
	loadShaderModuleFromFile(state, &modules[0], "data/shaders/basic.vert.spirv", DCG_SHADER_STAGE_VERTEX);
	loadShaderModuleFromFile(state, &modules[1], "data/shaders/basic.frag.spirv", DCG_SHADER_STAGE_FRAGMENT);

	DCgMaterialOptions materialOptions = {
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
	};

	DCgMaterial *material = dcgNewMaterial(state, 2, modules, &materialOptions, NULL);

	while(!dcgShouldClose(state)) {
		dcgUpdate(state);
		dcgCmdBegin(state, buffer);
		dcgCmdBeginRenderPass(state, buffer, 0);
		dcgCmdBindMat(state, buffer, material);
		dcgCmdEndRenderPass(state, buffer);
		dcgCmdEnd(state, buffer);
		dcgSubmit(state, buffer, 0);
	}

	DCD_MSGF(DEBUG, "DeInitializing State");
	dcgFreeShaderModule(state, &modules[0]);
	dcgFreeShaderModule(state, &modules[1]);
	dcgFreeCmdPool(state, pool);
	dcgDeinit(state);
	DCD_MSGF(DEBUG, "Freeing State");
	dcgFreeState(state);

	return 0;
}
