#include <dcore/graphics/internal.h>
#include <dcore/renderers/basic.h>

struct test {
	int x, y, z;
	float xx, yy, zz;
};

void dcgBasicRendererCreateInfo(DCgState *state) {
	VkAttachmentDescription attachments[2] = {
		(VkAttachmentDescription){
		  .format = state->surfaceFormat.format,
		  .flags = 0,
		  .samples = VK_SAMPLE_COUNT_1_BIT,
		  .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		  .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		  .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		  .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		  .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		  .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		},
		(VkAttachmentDescription){
		  .format = VK_FORMAT_D32_SFLOAT, // TODO: find best format, this one may not be supported
		  .flags = 0,
		  .samples = VK_SAMPLE_COUNT_1_BIT,
		  .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		  .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		  .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		  .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		  .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		  .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		},
	};

	VkAttachmentReference attachmentReferences[2] = {
		(VkAttachmentReference){
		  .attachment = 0,
		  .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		},
		(VkAttachmentReference){
		  .attachment = 1,
		  .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		},
	};

	VkSubpassDescription subpasses[1] = {
		(VkSubpassDescription){
		  .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		  .flags = 0,
		  .colorAttachmentCount = 1,
		  .pColorAttachments = &attachmentReferences[0],
		  .inputAttachmentCount = 0,
		  .pInputAttachments = NULL,
		  .preserveAttachmentCount = 0,
		  .pPreserveAttachments = NULL,
		  .pResolveAttachments = NULL,
		  .pDepthStencilAttachment = &attachmentReferences[1],
		},
	};

	VkSubpassDependency dependencies[1] = {
		(VkSubpassDependency){
		  .srcSubpass = VK_SUBPASS_EXTERNAL,
		  .dstSubpass = 0,
		  .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		  .srcAccessMask = 0,
		  .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		  .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		  .dependencyFlags = 0,
		},
	};

	dcgiAddRenderPass(state, /* 2 */ 1, attachments, 1, subpasses, 1, dependencies);

	VkDescriptorSetLayout *setLayouts = dcgiAddDescriptorSetLayouts(state, 2);

	VkDescriptorSetLayoutBinding setLayoutBindings[] = {
		(VkDescriptorSetLayoutBinding){
		  .binding = 0,
		  .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		  .descriptorCount = 1,
		  .stageFlags = VK_SHADER_STAGE_ALL,
		  .pImmutableSamplers = NULL,
		},
		(VkDescriptorSetLayoutBinding){
		  .binding = 0,
		  .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		  .descriptorCount = 1,
		  .stageFlags = VK_SHADER_STAGE_ALL,
		  .pImmutableSamplers = NULL,
		},
	};

	{
		VkDescriptorSetLayoutCreateInfo createInfo = { 0 };
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = 1;
		createInfo.pBindings = &setLayoutBindings[0];
		DC_RASSERT(
		  vkCreateDescriptorSetLayout(state->device, &createInfo, state->allocator, &setLayouts[0]) == VK_SUCCESS,
		  "Failed to create descriptor set layout #0"
		);
	}

	{
		VkDescriptorSetLayoutCreateInfo createInfo = { 0 };
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = 1;
		createInfo.pBindings = &setLayoutBindings[1];
		DC_RASSERT(
		  vkCreateDescriptorSetLayout(state->device, &createInfo, state->allocator, &setLayouts[1]) == VK_SUCCESS,
		  "Failed to create descriptor set layout #1"
		);
	}

	VkPushConstantRange *ranges = dcgiAddPushConstantRanges(state, DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_ENUM_MAX);
	ranges[DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_BASE].offset = 0;
	ranges[DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_BASE].size = sizeof(DCgBasicRendererUniformBuffer);
	ranges[DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_BASE].stageFlags = VK_SHADER_STAGE_ALL & ~VK_SHADER_STAGE_VERTEX_BIT;
	ranges[DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_TRANSFORM].offset = 0;
	ranges[DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_TRANSFORM].size = sizeof(DCgBasicRendererTransformUniformBuffer);
	ranges[DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_TRANSFORM].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkVertexInputAttributeDescription *attributes = dcgiAddVertexAttributes(state, 3);
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_POSITION].binding = 0;
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_POSITION].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_POSITION].location = 0;
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_POSITION].offset = 0;

	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_NORMAL].binding = 0;
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_NORMAL].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_NORMAL].location = 1;
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_NORMAL].offset = sizeof(DCmVector3f);

	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_TEXCOORDS].binding = 0;
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_TEXCOORDS].format = VK_FORMAT_R32G32_SFLOAT;
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_TEXCOORDS].location = 2;
	attributes[DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_TEXCOORDS].offset = sizeof(DCmVector3f) + sizeof(DCmVector3f);

	VkVertexInputBindingDescription *bindings = dcgiAddVertexBindings(state, 1);
	bindings[0].binding = 0;
	bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindings[0].stride = sizeof(DCgBasicRendererVertex);
}
