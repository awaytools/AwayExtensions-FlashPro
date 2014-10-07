#ifndef _LIBAWD_PRIMITIVE_H
#define _LIBAWD_PRIMITIVE_H

#include "scene.h"

#define PROP_PRIM_NUMBER1 101
#define PROP_PRIM_NUMBER2 102
#define PROP_PRIM_NUMBER3 103
#define PROP_PRIM_NUMBER4 104
#define PROP_PRIM_NUMBER5 105
#define PROP_PRIM_NUMBER6 106

#define PROP_PRIM_SCALEU 110
#define PROP_PRIM_SCALEV 111

#define PROP_PRIM_INT1 301
#define PROP_PRIM_INT2 302
#define PROP_PRIM_INT3 303
#define PROP_PRIM_INT4 304
#define PROP_PRIM_INT5 305
#define PROP_PRIM_INT6 306

#define PROP_PRIM_BOOL1 701
#define PROP_PRIM_BOOL2 702
#define PROP_PRIM_BOOL3 703
#define PROP_PRIM_BOOL4 704
#define PROP_PRIM_BOOL5 705
#define PROP_PRIM_BOOL6 706

typedef enum {
    AWD_PRIMITIVE_UNDEFINED=0,
    AWD_PRIMITIVE_PLANE=1,
    AWD_PRIMITIVE_CUBE,
    AWD_PRIMITIVE_SPHERE,
    AWD_PRIMITIVE_CYLINDER,
    AWD_PRIMITIVE_CONE,
    AWD_PRIMITIVE_CAPSULE,
    AWD_PRIMITIVE_TORUS
} AWD_primitive_type;

class AWDPrimitive :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWD_primitive_type prim_type;
        double yoffset;
    protected:
        void add_dependencies(AWDBlockList *);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDPrimitive(const char *, awd_uint16, AWD_primitive_type);
        ~AWDPrimitive();

        double get_Yoffset();
        void set_Yoffset(double);
        void add_int_property(int, int, int);
        void add_number_property(int, float, float);
        void add_bool_property(int, bool, bool);
};

#endif
