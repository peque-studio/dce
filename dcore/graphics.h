#ifndef DCORE_GRAPHICS_H
#define DCORE_GRAPHICS_H
#include <dcore/common.h>
#include <dcore/math.h>
#include <stddef.h>

/** Global state. */
typedef struct DCgState DCgState;

/** Allocator. */
typedef struct DCgAlloc DCgAlloc;

/** Data storage on the GPU. */
typedef struct DCgBuffer DCgBuffer;

/** Combines a pipeline and a pipeline layout. */
typedef struct DCgMaterial DCgMaterial;

/** Stores commands. */
typedef void DCgCmdBuffer;

/** Allocates and stores command buffers. @see DCgCmdBuffer */
typedef void DCgCmdPool;

/** Combines multiple image views (attachments). */
typedef void DCgFramebuffer;

/** View into an image. */
typedef void DCgImageView;

/** Stores image data, basically a buffer. */
typedef void DCgImage;

/** Same as a `DCgBuffer`, but with semantic meaning. @see DCgBuffer */
typedef DCgBuffer DCgVertexBuffer;

/** Same as a `DCgBuffer`, but with semantic meaning. @see DCgBuffer */
typedef DCgBuffer DCgIndexBuffer;

/** Creates and allocates a graphics state */
DCgState *dcgNewState();

/**
 * Initializes a graphics state.
 * @param appVersion client version.
 * @param appName client name.
 **/
void dcgInit(DCgState *s, uint32_t appVersion, const char *appName);

/** Deinitializes a graphics state. */
void dcgDeinit(DCgState *s);

/** Frees a graphics state. */
void dcgFreeState(DCgState *state);

/** Returns whether the window should close or not. */
bool dcgShouldClose(DCgState *state);

/** Closes the window. (shouldClose flag is set) */
void dcgClose(DCgState *state);

/** Retries the position of the mouse relative to the window. */
void dcgGetMousePosition(DCgState *state, DCmVector2i mousePosition);

/** Updates the window. (polls for new events) */
void dcgUpdate(DCgState *state);

typedef struct DcgFramebufferInfo {
	size_t attachmentCount;    /** Number of attachments in the framebuffer. */
	DCgImageView *attachments; /** Pointer to vector of attachments. */
	size_t renderPassIndex;    /** Render pass the framebuffer should be compatible with. */
	DCmExtent3 size;           /** Dimensions of the framebuffer. (width, height, layers) */
} DcgFramebufferInfo;

/** Create new framebuffer. */
DCgFramebuffer *dcgNewFramebuffer(DCgState *state, const DcgFramebufferInfo *info);

/** Free created framebuffer. */
void dcgFreeFramebuffer(DCgState *state, DCgFramebuffer *framebuffer);

enum DCgImageFormat {
	/** Undefined format/ */
	DCG_IMAGE_FORMAT_UNDEFINED = 0,
	/** Unsigned 8-Bit red color channel, normalized float. */
	DCG_IMAGE_FORMAT_R8_UNORM = 9,
	/** Signed 8-Bit red color channel, normalized float. */
	DCG_IMAGE_FORMAT_R8_SNORM = 10,
	/** Unsigned 8-Bit red color channel, integer. */
	DCG_IMAGE_FORMAT_R8_UINT = 13,
	/** Signed 8-Bit red color channel, integer. */
	DCG_IMAGE_FORMAT_R8_SINT = 14,
	/** SRGB 8-Bit red color channel. */
	DCG_IMAGE_FORMAT_R8_SRGB = 15,
	/** Unsigned 8-Bit red and green color channels, normalized float. */
	DCG_IMAGE_FORMAT_R8G8_UNORM = 16,
	/** Signed 8-Bit red and green color channels, normalized float. */
	DCG_IMAGE_FORMAT_R8G8_SNORM = 17,
	/** Unsigned 8-Bit red and green color channels, integer. */
	DCG_IMAGE_FORMAT_R8G8_UINT = 20,
	/** Signed 8-Bit red and green color channels, integer. */
	DCG_IMAGE_FORMAT_R8G8_SINT = 21,
	/** SRGB 8-Bit red and green color channels. */
	DCG_IMAGE_FORMAT_R8G8_SRGB = 22,
	/** Unsigned 8-Bit red, green and blue color channels, normalized float. */
	DCG_IMAGE_FORMAT_R8G8B8_UNORM = 23,
	/** Signed 8-Bit red, green and blue color channels, normalized float. */
	DCG_IMAGE_FORMAT_R8G8B8_SNORM = 24,
	/** Unsigned 8-Bit red, green and blue color channels, integer. */
	DCG_IMAGE_FORMAT_R8G8B8_UINT = 27,
	/** Signed 8-Bit red, green and blue color channels, integer. */
	DCG_IMAGE_FORMAT_R8G8B8_SINT = 28,
	/** SRGB 8-Bit red, green and blue color channels. */
	DCG_IMAGE_FORMAT_R8G8B8_SRGB = 29,
	/** Unsigned 8-Bit blue, green and red color channels, normalized float. */
	DCG_IMAGE_FORMAT_B8G8R8_UNORM = 30,
	/** Signed 8-Bit blue, green and red color channels, normalized float. */
	DCG_IMAGE_FORMAT_B8G8R8_SNORM = 31,
	/** Unsigned 8-Bit blue, green and red color channels, integer. */
	DCG_IMAGE_FORMAT_B8G8R8_UINT = 34,
	/** Signed 8-Bit blue, green and red color channels, integer. */
	DCG_IMAGE_FORMAT_B8G8R8_SINT = 35,
	/** SRGB 8-Bit blue, green and red color channels. */
	DCG_IMAGE_FORMAT_B8G8R8_SRGB = 36,
	/** Unsigned 8-Bit red, green, blue and alpha color channels, normalized float. */
	DCG_IMAGE_FORMAT_R8G8B8A8_UNORM = 37,
	/** Signed 8-Bit red, green, blue and alpha color channels, normalized float. */
	DCG_IMAGE_FORMAT_R8G8B8A8_SNORM = 38,
	/** Unsigned 8-Bit red, green, blue and alpha color channels, integer. */
	DCG_IMAGE_FORMAT_R8G8B8A8_UINT = 41,
	/** Signed 8-Bit red, green, blue and alpha color channels, integer. */
	DCG_IMAGE_FORMAT_R8G8B8A8_SINT = 42,
	/** SRGB 8-Bit red, green, blue and alpha color channels. */
	DCG_IMAGE_FORMAT_R8G8B8A8_SRGB = 43,
	/** Unsigned 8-Bit blue, green, red and alpha color channels, normalized float. */
	DCG_IMAGE_FORMAT_B8G8R8A8_UNORM = 44,
	/** Signed 8-Bit blue, green, red and alpha color channels, normalized float. */
	DCG_IMAGE_FORMAT_B8G8R8A8_SNORM = 45,
	/** Unsigned 8-Bit blue, green, red and alpha color channels, integer. */
	DCG_IMAGE_FORMAT_B8G8R8A8_UINT = 48,
	/** Signed 8-Bit blue, green, red and alpha color channels, integer. */
	DCG_IMAGE_FORMAT_B8G8R8A8_SINT = 49,
	/** SRGB 8-Bit blue, green, red and alpha color channels. */
	DCG_IMAGE_FORMAT_B8G8R8A8_SRGB = 50,
};

enum DCgImageViewType {
	DCG_IMAGE_VIEW_TYPE_1D = 0,
	DCG_IMAGE_VIEW_TYPE_2D = 1,
	DCG_IMAGE_VIEW_TYPE_3D = 2,
	DCG_IMAGE_VIEW_TYPE_CUBEMAP = 3,
	DCG_IMAGE_VIEW_TYPE_1D_ARRAY = 4,
	DCG_IMAGE_VIEW_TYPE_2D_ARRAY = 5,
	DCG_IMAGE_VIEW_TYPE_CUBEMAP_ARRAY = 6,
};

enum DCgImageAspectBit {
	DCG_IMAGE_ASPECT_COLOR_BIT = 0x00000001,
	DCG_IMAGE_ASPECT_DEPTH_BIT = 0x00000002,
	DCG_IMAGE_ASPECT_STENCIL_BIT = 0x00000004,
	DCG_IMAGE_ASPECT_METADATA_BIT = 0x00000008,
	DCG_IMAGE_ASPECT_PLANE_0_BIT = 0x00000010,
	DCG_IMAGE_ASPECT_PLANE_1_BIT = 0x00000020,
	DCG_IMAGE_ASPECT_PLANE_2_BIT = 0x00000040,
	DCG_IMAGE_ASPECT_NONE = 0,
};

typedef struct DCgImageSubresourceRange {
	enum DCgImageAspectBit imageAspect;
	uint32_t baseMipLevel;
	uint32_t mipLevelCount;
	uint32_t baseArrayLayer;
	uint32_t arrayLayerCount;
} DCgImageSubresourceRange;

typedef struct DCgImageViewInfo {
	DCgImage *image;
	enum DCgImageViewType viewType;
	enum DCgImageFormat format;
	DCgImageSubresourceRange subresourceRange;
} DCgImageViewInfo;

DCgImageView *dcgNewImageView(DCgState *state, const DCgImageViewInfo *info);
DCgImageView *dcgNewImageViewFromSwapchain(DCgState *state, size_t index);
void dcgFreeImageView(DCgState *state, DCgImageView *imageView);

typedef enum DCgQueueFamilyType {
	DCG_CMD_POOL_TYPE_GRAPHICS,
	DCG_CMD_POOL_TYPE_COMPUTE,
} DCgCmdPoolType;

DCgCmdPool *dcgNewCmdPool(DCgState *s, DCgCmdPoolType type);
void dcgFreeCmdPool(DCgState *s, DCgCmdPool *pool);

/** Creates a new command buffer */
DCgCmdBuffer *dcgGetNewCmdBuffer(DCgState *s, DCgCmdPool *pool);

/** Begin command buffer. */
void dcgCmdBegin(DCgState *s, DCgCmdBuffer *cmds);

/** End command buffer. */
void dcgCmdEnd(DCgState *s, DCgCmdBuffer *cmds);

/** Binds a vertex buffer. */
void dcgCmdBindVertexBuf(DCgState *s, DCgCmdBuffer *cmds, const DCgVertexBuffer *vbuf);

enum DCgIndexType {
	DCG_INDEX_TYPE_UINT16 = 0,
	DCG_INDEX_TYPE_UINT32 = 1,
};

/** Binds an index buffer. */
void dcgCmdBindIndexBuf(DCgState *s, DCgCmdBuffer *cmds, const DCgIndexBuffer *ibuf, enum DCgIndexType type);

/** Binds a material (pipelines + stuff). */
void dcgCmdBindMat(DCgState *s, DCgCmdBuffer *cmds, DCgMaterial *mat);

typedef union {
	DCmVector4f color;
	union {
		float depth;
		uint32_t stencil;
	} depthStencil;
} DCgClearValue;

typedef struct DCgCmdBeginRenderPassInfo {
	size_t renderPassIndex;
	DCmRect2 renderArea;
	size_t clearValueCount;
	const DCgClearValue *clearValues;
	DCgFramebuffer *framebuffer;
} DCgCmdBeginRenderPassInfo;

/** Begins a render pass with index `index`. */
void dcgCmdBeginRenderPass(DCgState *s, DCgCmdBuffer *cmds, const DCgCmdBeginRenderPassInfo *info);

/** Ends the current render pass. */
void dcgCmdEndRenderPass(DCgState *s, DCgCmdBuffer *cmds);

/** Draws some instances with the specified
 * number indices from the bound vertex/index buffers.
 * @param indices number of indices to draw per instance.
 * @param instances number of instances to draw. */
void dcgCmdDraw(DCgState *s, DCgCmdBuffer *cmds, size_t indices, size_t instances);

/** Submit a command buffer into a queue. */
void dcgSubmit(DCgState *s, DCgCmdBuffer *cmds, int queue);

typedef enum DCgPipelineStage {
	DCG_SHADER_STAGE_VERTEX = 0x01,
	DCG_SHADER_STAGE_GEOMETRY = 0x08,
	DCG_SHADER_STAGE_FRAGMENT = 0x10,
	DCG_SHADER_STAGE_COMPUTE = 0x20,
} DCgShaderStage;

typedef struct DCgShaderModule {
	DCgShaderStage stage;
	void *module;
	const char *name;
} DCgShaderModule;

typedef enum DCgCullMode { DCG_CULL_MODE_NONE, DCG_CULL_MODE_FRONT, DCG_CULL_MODE_BACK, DCG_CULL_MODE_BOTH } DCgCullMode;

typedef enum DCgPolygonMode {
	DCG_POLYGON_MODE_FILL,
	DCG_POLYGON_MODE_LINE,
	DCG_POLYGON_MODE_POINT,
} DCgPolygonMode;

typedef enum DCgCompareOp {
	DCG_COMAPRE_OP_NEVER,
	DCG_COMAPRE_OP_LESS,
	DCG_COMAPRE_OP_EQUAL,
	DCG_COMAPRE_OP_LESS_EQUAL,
	DCG_COMAPRE_OP_GREATER,
	DCG_COMAPRE_OP_NOT_EQUAL,
	DCG_COMAPRE_OP_GREATER_OR_EQUAL,
	DCG_COMAPRE_OP_ALWAYS,
} DCgCompareOp;

typedef struct DCgMaterialInfo {
	DCmOffset2 scissorOffset;
	DCmExtent2 scissorExtent;
	DCgCullMode cullMode;
	float lineWidth;
	DCgPolygonMode polygonMode;
	bool enableDiscard;
	bool enableDepthTest;
	bool enableDepthWrite;
	DCgCompareOp depthCompareOp;
	bool enableDepthBoundsTest;
	float minDepthBound, maxDepthBound;
	bool enableStencilTest;
	DCmExtent2 viewportExtent;
	int pushConstantsIndex, descriptorSetsIndex, vertexInputIndex;
} DCgMaterialInfo;

typedef struct DCgMaterialCache DCgMaterialCache;

DCgMaterial *dcgNewMaterial(
  DCgState *state, size_t moduleCount, DCgShaderModule *modules, const DCgMaterialInfo *options, const DCgMaterialCache *cache /* = NULL */
);

DCgMaterialCache *dcgGetMaterialCache(DCgState *state, DCgMaterial *material);
void dcgFreeMaterial(DCgState *state, DCgMaterial *material);

void dcgInitShaderModule(DCgState *state, DCgShaderModule *module, DCgShaderStage stage, size_t size, uint32_t *code, const char *name);
void dcgFreeShaderModule(DCgState *state, DCgShaderModule *module);

#endif
