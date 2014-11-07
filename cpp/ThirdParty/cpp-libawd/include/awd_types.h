#ifndef _LIBAWD_AWD_TYPES_H
#define _LIBAWD_AWD_TYPES_H

// POD types
typedef char awd_int8;
typedef short awd_int16;
typedef int awd_int32;

typedef unsigned char awd_uint8;
typedef unsigned short awd_uint16;
typedef unsigned int awd_uint32;

typedef float awd_float32;
typedef double awd_float64;

typedef unsigned int awd_baddr;
typedef unsigned char awd_nsid;

typedef unsigned int awd_color;
typedef unsigned char awd_bool;
#define AWD_TRUE 1
#define AWD_FALSE 0
#define AWD_NULL 0
// Byte order conversions
// TODO: Use different depending on platform
/*
#define UI16(x) awdutil_swapui16(x)
#define UI32(x) awdutil_swapui32(x)
#define F32(x)  awdutil_swapf32(x)
#define F64(x)  awdutil_swapf64(x)
#define UI16(x) x
#define UI32(x) x
#define F32(x)  x
#define F64(x)  x
*/

typedef enum {
    // Numeric types
    AWD_FIELD_INT8=1,
    AWD_FIELD_INT16,
    AWD_FIELD_INT32,
    AWD_FIELD_UINT8,
    AWD_FIELD_UINT16,
    AWD_FIELD_UINT32,
    AWD_FIELD_FLOAT32,
    AWD_FIELD_FLOAT64,

    // Derived numeric types
    AWD_FIELD_BOOL=21,
    AWD_FIELD_COLOR,
    AWD_FIELD_BADDR,

    // Aggregate/array types
    AWD_FIELD_STRING=31,
    AWD_FIELD_BYTEARRAY,

    // Mathetmatical types
    AWD_FIELD_VECTOR2x1=41,
    AWD_FIELD_VECTOR3x1,
    AWD_FIELD_VECTOR4x1,
    AWD_FIELD_MTX3x2,
    AWD_FIELD_MTX3x3,
    AWD_FIELD_MTX4x3,
    AWD_FIELD_MTX4x4,
} AWD_field_type;

typedef union {
    void *v;
    awd_bool *b;
    awd_int8 *i8;
    awd_int16 *i16;
    awd_int32 *i32;
    awd_uint8 *ui8;
    awd_uint16 *ui16;
    awd_uint32 *ui32;
    awd_float32 *f32;
    awd_float64 *f64;
    awd_float64 *mtx;
    awd_float64 *vec;
    awd_uint32 *col;
    awd_baddr *addr;
    char *str;
} AWD_field_ptr;

typedef enum {
    NULL_REF=0,

    // Geometry/data
    TRI_GEOM=1,
    PRIM_GEOM=11,

    // Scene objects
    SCENE=21,
    CONTAINER=22,
    MESH_INSTANCE=23,

    SKYBOX=31,

    LIGHT=41,
    CAMERA=42,
    TEXTURE_PROJECTOR=43,
    SOUND_SOURCE=44,

    LIGHTPICKER=51,

    // Partitioning
    BSP_TREE=61,
    OCT_TREE=62,

    // Materials
    SIMPLE_MATERIAL=81,
    BITMAP_TEXTURE=82,
    CUBE_TEXTURE=83,
    CUBE_TEXTURE_ATF=84,

    EFFECT_METHOD=91,
    SHADOW_METHOD=92,
    // Animation
    SKELETON=101,
    SKELETON_POSE=102,
    SKELETON_ANIM=103,

    VERTEX_POSE=111,
    VERTEX_ANIM=112,

    ANIMATION_SET=113,
    ANIMATOR=122,

    UV_ANIM=121,
	
    SHAPE2D_GEOM=131,
    SHAPE2D_FILL=132,
    SHAPE2D_TIMELINE=133,
    TEXT_ELEMENT=134,
    FONT=135,
    FORMAT=136,
    // Misc
    COMMAND=253,
    NAMESPACE=254,
    METADATA=255,

    MESSAGE=500//only used intern to store warnings
} AWD_block_type;

typedef enum {
    UNCOMPRESSED,
    DEFLATE,
    LZMA
} AWD_compression;

#endif
