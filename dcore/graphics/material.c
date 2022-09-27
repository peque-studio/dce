#include <dcore/graphics.h>
#include <dcore/graphics/internal.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

DCgMaterial *dcgNewMaterial(
    DCgState *state,
    size_t moduleCount,
    DCgShaderModule *modules,
    DCgMaterialOptions *options,
    DCgMaterialCache *cache
) {
    DCgMaterial *material = malloc(sizeof(DCgMaterial));

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	// vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)options->bindingDescriptions.count;
	// vertexInputInfo.pVertexBindingDescriptions = info.vertexInputInfo.bindingDescriptions.values;
	// vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)info.vertexInputInfo.attributeDescriptions.count;
	// vertexInputInfo.pVertexAttributeDescriptions = info.vertexInputInfo.attributeDescriptions.values;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkRect2D scissor = {0};
	scissor.offset = (VkOffset2D){ options->scissorOffset.x, options->scissorOffset.y };
	scissor.extent = (VkExtent2D){ options->scissorExtent.x, options->scissorExtent.y };

	VkViewport viewport = {0};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)options->viewportExtent.x;
	viewport.height = (float)options->viewportExtent.y;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkPipelineViewportStateCreateInfo viewportState = {0};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor; 

	VkPipelineRasterizationStateCreateInfo rasterizer = {0};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = options->enableDiscard;
	rasterizer.polygonMode = (VkPolygonMode)options->polygonMode;
	rasterizer.lineWidth = options->lineWidth;
	rasterizer.cullMode = (VkCullModeFlags)options->cullMode;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling = {0};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = NULL;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {0};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineDynamicStateCreateInfo dynamicState = {0};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 0;
	dynamicState.pDynamicStates = NULL;

	VkPipelineDepthStencilStateCreateInfo depthStencilState = {0};
	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.depthTestEnable = options->enableDepthTest;
	depthStencilState.depthWriteEnable = options->enableDepthWrite;
	depthStencilState.depthCompareOp = (VkCompareOp)options->depthCompareOp;
	depthStencilState.depthBoundsTestEnable = options->enableDepthBoundsTest;
	depthStencilState.minDepthBounds = options->minDepthBound;
	depthStencilState.maxDepthBounds = options->maxDepthBound;
	depthStencilState.stencilTestEnable = options->enableStencilTest;

	VkGraphicsPipelineCreateInfo createInfo = {0};

	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = (uint32_t)moduleCount;
	createInfo.pStages = NULL; // TODO: convert `modules` to pStages
	createInfo.pVertexInputState = &vertexInputInfo;
	createInfo.pInputAssemblyState = &inputAssembly;
	createInfo.pViewportState = &viewportState;
	createInfo.pRasterizationState = &rasterizer;
	createInfo.pMultisampleState = &multisampling;
	createInfo.pDepthStencilState = &depthStencilState;
	createInfo.pColorBlendState = &colorBlending;
	createInfo.pDynamicState = NULL;
	createInfo.layout = material->layout; // TODO: create the actual layout
	createInfo.renderPass = NULL; // TODO: get the render pass (probably from DCgState)
	createInfo.subpass = 0;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = -1;

    // TODO: error handling.
    vkCreateGraphicsPipelines(state->device, (VkPipelineCache)cache, 1, &createInfo, NULL, &material->pipeline);

    return material;
}

void dcgFreeMaterial(DCgState *state, DCgMaterial *material) {
    if(material == NULL) DCD_MSGF(DEBUG, "Tried to free NULL material.");
    free(material);
}

DCgMaterialCache *dcgGetMaterialCache(DCgState *state, DCgMaterial *material) {
    // TODO: material/pipeline cache
    
    // VkPipelineCache cache;
    // VkPipelineCacheCreateInfo createInfo = {0};
    // createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    // createInfo.flags
    // vkCreatePipelineCache(state->device, &createInfo, NULL, &cache);
    // vkGetPipelineCacheData(state->device, VkPipelineCache pipelineCache, size_t *pDataSize, void *pData)
    return NULL;
}
