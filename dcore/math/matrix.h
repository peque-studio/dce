#ifndef DCORE_MATH_MATRIX_H
#define DCORE_MATH_MATRIX_H
#include <dcore/math/common.h>

#define DCM__M22_DEF(N, T) typedef T DCmMatrix2x2##N[2][2]
#define DCM__M33_DEF(N, T) typedef T DCmMatrix3x3##N[3][3]
#define DCM__M44_DEF(N, T) typedef T DCmMatrix4x4##N[4][4]

DCM__T_FOR(DCM__M22_DEF, ;);
DCM__T_FOR(DCM__M33_DEF, ;);
DCM__T_FOR(DCM__M44_DEF, ;);

typedef DCmMatrix2x2f DCmMatrix2x2;
typedef DCmMatrix3x3f DCmMatrix3x3;
typedef DCmMatrix4x4f DCmMatrix4x4;

#endif
