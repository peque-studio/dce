#ifndef DCORE_MATH_COMMON_H
#define DCORE_MATH_COMMON_H
#include <dcore/common.h>

#define DCM__T_FOR(O, SEP, ...) \
	O(u8, uint8_t, ##__VA_ARGS__) \
	SEP O(u16, uint16_t, ##__VA_ARGS__) SEP O(u32, uint32_t, ##__VA_ARGS__) SEP O(u64, uint64_t, ##__VA_ARGS__) SEP O(i8, int8_t, ##__VA_ARGS__) SEP \
	O(i16, int16_t, ##__VA_ARGS__) SEP \
	O(i32, int32_t, ##__VA_ARGS__) SEP \
	O(i64, int64_t, ##__VA_ARGS__) SEP \
	O(i, int, ##__VA_ARGS__) SEP \
	O(u, unsigned int, ##__VA_ARGS__) SEP \
	O(s, size_t, ##__VA_ARGS__) SEP \
	O(f, float, ##__VA_ARGS__) SEP \
	O(d, double, ##__VA_ARGS__)

#endif
