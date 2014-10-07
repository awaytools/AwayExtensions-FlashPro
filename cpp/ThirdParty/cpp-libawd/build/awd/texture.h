#ifndef LIBAWD_TEXTURE_H
#define LIBAWD_TEXTURE_H

#include "awd_types.h"
#include "block.h"
#include "name.h"
#include "attr.h"
#include "block.h"
//#include "shape2Dfill.h"
#include <vector>
typedef enum {
    EXTERNAL=0,
    EMBEDDED,
    UNDEFINEDTEXTYPE
} AWD_tex_type;
typedef enum {
    MATDIFFUSE=0,
    MATSPECULAR,
    MATAMBIENT,
    MATNORMAL,
    SHADINGLIGHTMAP,
    SHADINGGRADIENTDIFFUSE,
    SHADINGSIMPLEWATERNORMAL,
    FXENVMAPMASK,
    FXLIGHTMAP,
    FXMASK,
    FORCUBETEXTURE,
    FORTEXTUREPROJECTOR
} AWDTexPurpose_type;

class AWDBitmapTexture :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWD_tex_type saveType;
        string url;

        awd_uint8 *embed_data;
        awd_uint32 embed_data_len;
        int height;
        int width;
		
        int final_colors_count;
        int final_gradients_count;
		
		vector<AWDTextureAtlasItem*> atlas_items;
		vector<AWDTextureAtlasItem*> new_atlas_items;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
		void write_body(AWDFileWriter*, BlockSettings *);

    public:
        AWDBitmapTexture(string& name);
        ~AWDBitmapTexture();
		


		void export_texture_atlas();
		AWDTextureAtlasItem* add_texture_atlas_gradient_item(awd_color, awd_color);
		AWDTextureAtlasItem* add_texture_atlas_color_item(awd_color);
		AWDTextureAtlasItem* add_texture_atlas_bitmap_item(string&, int, int);
        void set_embed_data(awd_uint8 *, awd_uint32);

        string get_url();
        awd_uint16 get_url_length();
        void set_url(string& url);
		void add_shape(AWDBlock*);

        AWD_tex_type get_tex_type();
        void set_tex_type(AWD_tex_type);
        int get_height();
        void set_height(int);
        int get_width();
        void set_width(int);

};

#endif
