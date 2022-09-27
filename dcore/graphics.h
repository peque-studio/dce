#ifndef DCORE_GRAPHICS_H
#define DCORE_GRAPHICS_H
#include <dcore/common.h>
#include <dcore/math/vector.h>
#include <stddef.h>

typedef struct DCgState DCgState;
typedef struct DCgAlloc DCgAlloc;
typedef struct DCgBuffer DCgBuffer;
typedef struct DCgMaterial DCgMaterial;
typedef struct DCgCmdBuffer DCgCmdBuffer;

typedef DCgBuffer DCgVertexBuffer;
typedef DCgBuffer DCgIndexBuffer;

void dcgInit(DCgState *s);
void dcgDeinit(DCgState *s);
void dcgCmdBegin(DCgState *s);
void dcgCmdBindVertexBuf(DCgState *s, DCgCmdBuffer *cmds, DCgVertexBuffer *vbuf);
void dcgCmdBindIndexBuf(DCgState *s, DCgCmdBuffer *cmds, DCgIndexBuffer *ibuf);
void dcgCmdBindMat(DCgState *s, DCgCmdBuffer *cmds, DCgMaterial *mat);
void dcgCmdDraw(DCgState *s, DCgCmdBuffer *cmds, size_t indices, size_t instances);
void dcgSubmit(DCgState *s, DCgCmdBuffer *cmds, int queue);

typedef enum DCgShaderModuleType {
    DCG_SHADER_MODULE_VERTEX,
    DCG_SHADER_MODULE_FRAGMENT,
} DCgShaderModuleType;

typedef struct DCgShaderModule {
    DCgShaderModuleType type;
    uint32_t *code;
    const char *entry;
} DCgShaderModule;

typedef enum DCgCullMode {
    DCG_CULL_MODE_NONE,
    DCG_CULL_MODE_FRONT,
    DCG_CULL_MODE_BACK,
    DCG_CULL_MODE_BOTH
} DCgCullMode;

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

typedef struct DCgMaterialOptions {
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
} DCgMaterialOptions;

typedef struct DCgMaterialCache DCgMaterialCache;

DCgMaterial *dcgNewMaterial(
    DCgState *state,
    size_t moduleCount,
    DCgShaderModule *modules,
    DCgMaterialOptions *options,
    DCgMaterialCache *cache /* = NULL */
);

DCgMaterialCache *dcgGetMaterialCache(DCgState *state, DCgMaterial *material);

void dcgFreeMaterial(DCgState *state, DCgMaterial *material);

#endif
