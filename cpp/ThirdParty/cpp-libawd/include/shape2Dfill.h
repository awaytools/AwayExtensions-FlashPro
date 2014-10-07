#ifndef _LIBAWD_SHAPE2DFILL_H
#define _LIBAWD_SHAPE2DFILL_H

#include "name.h"
#include "texture.h"
#include "block.h"
#include "attr.h"
//#include "anim.h"

typedef enum {
    AWD_FILLTYPE_SOLID=0,
    AWD_FILLTYPE_TEXTURE=1,
    AWD_FILLTYPE_GRADIENT_LINEAR=2,
    AWD_FILLTYPE_GRADIENT_RADIAL=3
} AWD_fill_type;

#define PROP_MAT_COLOR 1
#define PROP_MAT_TEXTURE 2


class AWDShape2DFill :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWD_fill_type fill_type;
        // vars for material properties
		double red;
		double green;
		double blue;
		double alpha;
        awd_color color;
        AWDBitmapTexture *texture;
        awd_float64 * uv_transform_mtx;
		AWDTextureAtlasItem* textureAtlasItem;// for solid and texture, size shoudl be 1

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        void write_body(AWDFileWriter * , BlockSettings *);

    public:
        AWDShape2DFill(string& name, AWD_fill_type);
        ~AWDShape2DFill();

		void set_color_components(double red, double green, double blue, double alpha);
        void set_fill_type(AWD_fill_type);
        AWD_fill_type get_fill_type();
		
        void set_color(awd_color);
        awd_color get_color();
        double get_color_red();
        double get_color_green();
        double get_color_blue();
        double get_color_alpha();
        void add_color(awd_color, double);

        void set_texture(AWDBitmapTexture *);
        AWDBitmapTexture *get_texture();


        void set_uv_transform_mtx(awd_float64 *);
        awd_float64 *get_uv_transform_mtx();

};

#endif
