#ifndef DCORE_RENDERERS_BASIC_H
#define DCORE_RENDERERS_BASIC_H
#include <dcore/common.h>
#include <dcore/graphics.h>
#include <dcore/math.h>

/** creates basic rendering privitives and sets basic settings. */
void dcgBasicRendererCreateInfo(DCgState *state);

typedef enum DCgBasicRendererVertexAttribute {
	DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_POSITION,
	DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_NORMAL,
	DCG_BASIC_RENDERER_VERTEX_ATTRIBUTE_TEXCOORDS
} DCgBasicRendererVertexAttribute;

typedef struct DCgBasicRendererVertex {
	DCmVector3 position;
	DCmVector3 normal;
	DCmVector2 texcoords;
} DCgBasicRendererVertex;

typedef enum DCgBasicRendererPushConstantRange {
	DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_BASE = 0,
	DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_TRANSFORM,
	DCG_BASIC_RENDERER_PUSH_CONSTANT_RANGE_ENUM_MAX
} DCgBasicRendererPushConstantRange;

typedef struct DCgBasicRendererTransformPushConstant {
	DCmMatrix4x4 transform;
} DCgBasicRendererTransformPushConstant;

typedef struct DCgBasicRendererUniformBuffer {
	DCmVector4 sunDirectionAndIntensity;
} DCgBasicRendererUniformBuffer;

typedef struct DCgBasicRendererTransformUniformBuffer {
	DCgBasicRendererUniformBuffer base;
	DCmMatrix4x4 world;
	DCmMatrix4x4 view;
} DCgBasicRendererTransformUniformBuffer;

#endif
