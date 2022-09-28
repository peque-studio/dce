#ifndef DCORE_MATH_VECTOR_H
#define DCORE_MATH_VECTOR_H
#include <dcore/common.h>

#define DCM_MK_VECTOR2_DEF__(N,T) typedef T N[2]

DCM_MK_VECTOR2_DEF__(DCmVector2u8 ,  uint8_t);
DCM_MK_VECTOR2_DEF__(DCmVector2u16, uint16_t);
DCM_MK_VECTOR2_DEF__(DCmVector2u32, uint32_t);
DCM_MK_VECTOR2_DEF__(DCmVector2u64, uint64_t);
DCM_MK_VECTOR2_DEF__(DCmVector2i8 ,  int8_t);
DCM_MK_VECTOR2_DEF__(DCmVector2i16, int16_t);
DCM_MK_VECTOR2_DEF__(DCmVector2i32, int32_t);
DCM_MK_VECTOR2_DEF__(DCmVector2i64, int64_t);
DCM_MK_VECTOR2_DEF__(DCmVector2f, float);
DCM_MK_VECTOR2_DEF__(DCmVector2d, double);

typedef DCmVector2f DCmVector2;
typedef DCmVector2i32 DCmOffset2;
typedef DCmVector2u32 DCmExtent2;

#endif
