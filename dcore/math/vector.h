#ifndef DCORE_MATH_VECTOR_H
#define DCORE_MATH_VECTOR_H
#include <dcore/math/common.h>

#define DCM__V2_DEF(N, T) typedef T DCmVector2##N[2]
#define DCM__V3_DEF(N, T) typedef T DCmVector3##N[3]
#define DCM__V4_DEF(N, T) typedef T DCmVector4##N[4]

DCM__T_FOR(DCM__V2_DEF, ;);
DCM__T_FOR(DCM__V3_DEF, ;);
DCM__T_FOR(DCM__V4_DEF, ;);

typedef DCmVector2f DCmVector2;
typedef DCmVector2i32 DCmOffset2;
typedef DCmVector2u32 DCmExtent2;
typedef DCmVector3f DCmVector3;
typedef DCmVector3i32 DCmOffset3;
typedef DCmVector3u32 DCmExtent3;
typedef DCmVector4f DCmVector4;
typedef DCmVector4i32 DCmOffset4;
typedef DCmVector4u32 DCmExtent4;

#define DCM__V2_OP_FOR(O, SEP, ...) O(0, __VA_ARGS__) SEP O(1, __VA_ARGS__)

#define DCM__V3_OP_FOR(O, SEP, ...) \
	O(0, __VA_ARGS__) SEP O(1, __VA_ARGS__) \
	SEP O(2, __VA_ARGS__)

#define DCM__V4_OP_FOR(O, SEP, ...) \
	O(0, __VA_ARGS__) SEP O(1, __VA_ARGS__) \
	SEP O(2, __VA_ARGS__) \
	SEP O(3, __VA_ARGS__)

#define DCM__VV_OP(I, OP) dst[I] OP src[I]
#define DCM__VS_OP(I, OP) vector[I] OP scalar
#define DCM__VV_F(S, _, N, K, BODY) \
	static inline void DCmVector##K##S##N(DCmVector##K##S dst, const DCmVector##K##S src) { BODY }
#define DCM__SVV_F(S, T, N, K, BODY) \
	static inline T DCmVector##K##S##N(const DCmVector##K##S a, const DCmVector##K##S b) { BODY }
#define DCM__VS_F(S, T, N, K, BODY) \
	static inline void DCmVector##K##S##N(DCmVector##K##S vector, T scalar) { BODY }

DCM__T_FOR(DCM__VV_F, , Addv, 2, DCM__V2_OP_FOR(DCM__VV_OP, ;, +=);)
DCM__T_FOR(DCM__VV_F, , Subv, 2, DCM__V2_OP_FOR(DCM__VV_OP, ;, -=);)
DCM__T_FOR(DCM__VV_F, , Mulv, 2, DCM__V2_OP_FOR(DCM__VV_OP, ;, *=);)
DCM__T_FOR(DCM__VV_F, , Divv, 2, DCM__V2_OP_FOR(DCM__VV_OP, ;, /=);)
DCM__T_FOR(DCM__VS_F, , Adds, 2, DCM__V2_OP_FOR(DCM__VS_OP, ;, +=);)
DCM__T_FOR(DCM__VS_F, , Subs, 2, DCM__V2_OP_FOR(DCM__VS_OP, ;, -=);)
DCM__T_FOR(DCM__VS_F, , Muls, 2, DCM__V2_OP_FOR(DCM__VS_OP, ;, *=);)
DCM__T_FOR(DCM__VS_F, , Divs, 2, DCM__V2_OP_FOR(DCM__VS_OP, ;, /=);)
DCM__T_FOR(DCM__SVV_F, , Dot, 2, return a[0] * b[0] + a[1] * b[1];)
DCM__T_FOR(DCM__VV_F, , Addv, 3, DCM__V3_OP_FOR(DCM__VV_OP, ;, +=);)
DCM__T_FOR(DCM__VV_F, , Subv, 3, DCM__V3_OP_FOR(DCM__VV_OP, ;, -=);)
DCM__T_FOR(DCM__VV_F, , Mulv, 3, DCM__V3_OP_FOR(DCM__VV_OP, ;, *=);)
DCM__T_FOR(DCM__VV_F, , Divv, 3, DCM__V3_OP_FOR(DCM__VV_OP, ;, /=);)
DCM__T_FOR(DCM__VS_F, , Adds, 3, DCM__V3_OP_FOR(DCM__VS_OP, ;, +=);)
DCM__T_FOR(DCM__VS_F, , Subs, 3, DCM__V3_OP_FOR(DCM__VS_OP, ;, -=);)
DCM__T_FOR(DCM__VS_F, , Muls, 3, DCM__V3_OP_FOR(DCM__VS_OP, ;, *=);)
DCM__T_FOR(DCM__VS_F, , Divs, 3, DCM__V3_OP_FOR(DCM__VS_OP, ;, /=);)
DCM__T_FOR(DCM__SVV_F, , Dot, 3, return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];)
DCM__T_FOR(DCM__VV_F, , Cross, 3, dst[0] = dst[1] * src[2] - dst[2] * src[1]; dst[1] = dst[2] * src[0] - dst[0] * src[2];
           dst[2] = dst[0] * src[1] - dst[1] * src[0];)

/* void test() {
    DCmVector2i16 a = { 12, -48 };
    DCmVector2i16 b = { -93, 11 };
    DCmVector2i16Divs(a, 12);
    DCmVector2i16Addv(b, a);
    // -> a = { -92, 7 };
} */

#endif
