#ifndef _LIBAWD_AWDUTIL_H
#define _LIBAWD_AWDUTIL_H

#include "awd.h"
#include "awd_types.h"


// Macros to calculate matrix size depending on width (optimized for size or accuracy)
#define VEC2_SIZE(wide) (wide? (2*sizeof(awd_float64)):(2*sizeof(awd_float32)))
#define VEC3_SIZE(wide) (wide? (3*sizeof(awd_float64)):(3*sizeof(awd_float32)))
#define VEC4_SIZE(wide) (wide? (4*sizeof(awd_float64)):(4*sizeof(awd_float32)))
#define MTX32_SIZE(wide) (wide? (6*sizeof(awd_float64)):(6*sizeof(awd_float32)))
#define MTX33_SIZE(wide) (wide? (9*sizeof(awd_float64)):(9*sizeof(awd_float32)))
#define MTX43_SIZE(wide) (wide? (12*sizeof(awd_float64)):(12*sizeof(awd_float32)))
#define MTX44_SIZE(wide) (wide? (16*sizeof(awd_float64)):(16*sizeof(awd_float32)))


// Utility functions
awd_float64 *   awdutil_id_mtx4x4(awd_float64 *);

size_t          awdutil_get_type_size(AWD_field_type, bool);

awd_uint32      awdutil_write_floats(AWDFileWriter*, awd_float64 *, int, bool);

awd_color       awdutil_float_color(double, double, double, double);
awd_color       awdutil_int_color(int, int, int, int);

awd_uint16      awdutil_swapui16(awd_uint16);
awd_uint32      awdutil_swapui32(awd_uint32);
awd_float32     awdutil_swapf32(awd_float32);
awd_float64     awdutil_swapf64(awd_float64);

#endif
