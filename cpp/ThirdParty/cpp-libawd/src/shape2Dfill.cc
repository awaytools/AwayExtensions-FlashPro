#include "shape2Dfill.h"
#include "util.h"



AWDShape2DFill::AWDShape2DFill(string& name, AWD_fill_type fill_type) :
    AWDBlock(SHAPE2D_FILL),
    AWDNamedElement(name),
    AWDAttrElement()
{
	this->fill_type=fill_type;
    this->color = 0xffffff;

    this->texture = NULL;

    this->uv_transform_mtx = NULL;
	this->red=0.5;
	this->green=0.5;
	this->blue=0.5;
	this->alpha=1;
	this->tex_width=0;
	this->tex_height=0;

}

AWDShape2DFill::~AWDShape2DFill()
{
    this->texture = NULL;
    if (this->uv_transform_mtx!=NULL) {
        free(this->uv_transform_mtx);
        this->uv_transform_mtx = NULL;
    }
}

awd_float64 *
AWDShape2DFill::get_uv_transform_mtx()
{
    return this->uv_transform_mtx;
}
void
AWDShape2DFill::set_uv_transform_mtx(awd_float64 * new_uv_transform_mtx)
{
    if (this->uv_transform_mtx!=NULL)
        free(this->uv_transform_mtx);
    this->uv_transform_mtx = (double *)malloc(6*sizeof(double));
    this->uv_transform_mtx[0] = new_uv_transform_mtx[0];
    this->uv_transform_mtx[1] = new_uv_transform_mtx[1];
    this->uv_transform_mtx[2] = new_uv_transform_mtx[2];
    this->uv_transform_mtx[3] = new_uv_transform_mtx[3];
    this->uv_transform_mtx[4] = new_uv_transform_mtx[4];
    this->uv_transform_mtx[5] = new_uv_transform_mtx[5];
}

double
AWDShape2DFill::get_color_red()
{
    return this->red;
}
double
AWDShape2DFill::get_color_green()
{
    return this->green;
}
double
AWDShape2DFill::get_color_blue()
{
    return this->blue;
}
double
AWDShape2DFill::get_color_alpha()
{
    return this->alpha;
}
int
AWDShape2DFill::get_tex_width()
{
	return this->tex_width;
}
void
AWDShape2DFill::set_tex_width( int  tex_width)
{
    this->tex_width = tex_width;
}
int
AWDShape2DFill::get_tex_height()
{
	return this->tex_height;
}
void
AWDShape2DFill::set_tex_height( int  tex_height)
{
    this->tex_height = tex_height;
}
string&
AWDShape2DFill::get_texture_url()
{
	return this->texture_url;
}
void
AWDShape2DFill::set_texture_url( string&  texture_url)
{
    this->texture_url = texture_url;
}
 AWDGradientitem* 
AWDShape2DFill::get_gradient()
{
	return this->gradient_item;
}
void
AWDShape2DFill::set_gradient( AWDGradientitem*  gradient)
{
    this->gradient_item = gradient;
}
awd_color
AWDShape2DFill::get_color()
{
    return this->color;
}
void
AWDShape2DFill::set_color(awd_color color)
{
    this->color = color;
}

void
AWDShape2DFill::set_color_components(double red, double green, double blue, double alpha)
{
	this->red=red/255;
	this->green=green/255;
	this->blue=blue/255;
	this->alpha=alpha/255;
}
AWD_fill_type
AWDShape2DFill::get_fill_type()
{
    return this->fill_type;
}
void
AWDShape2DFill::set_fill_type(AWD_fill_type fill_type)
{
    this->fill_type = fill_type;
}

AWDBitmapTexture *
AWDShape2DFill::get_texture()
{
    return this->texture;
}
void
AWDShape2DFill::set_texture(AWDBitmapTexture *texture)
{
    this->texture = texture;
}

void
AWDShape2DFill::prepare_and_add_dependencies(AWDBlockList *export_list)
{
	
	if (this->fill_type == AWD_FILLTYPE_SOLID) {
		this->add_color_property(PROP_MAT_COLOR,this->color,0xffffff);
    }
	// todo: get the textureatlas information from the TextureAtlasItem
	/*
    if (this->texture) {
            if (this->texture->get_tex_type()!=UNDEFINEDTEXTYPE){
                this->texture->prepare_and_add_with_dependencies(export_list);
                AWD_field_ptr tex_val;
                tex_val.v = malloc(sizeof(awd_baddr));
                *tex_val.addr = this->texture->get_addr();
                this->properties->set(PROP_MAT_TEXTURE, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
            }
        }   
	*/

}

awd_uint32
AWDShape2DFill::calc_body_length(BlockSettings * curBlockSettings)
{
	
    awd_uint32 len;

    len = sizeof(awd_uint16) + this->get_name().size(); //name
    len += sizeof(awd_uint8); // type

    len += this->calc_attr_length(true, true, curBlockSettings);
	

    return len;
}

void
AWDShape2DFill::write_body(AWDFileWriter * fileWriter,  BlockSettings * curBlockSettings)
{
	
	fileWriter->writeSTRING( this->get_name(), 1);
	fileWriter->writeUINT8( this->fill_type);
    this->properties->write_attributes(fileWriter, curBlockSettings);
    this->user_attributes->write_attributes(fileWriter, curBlockSettings);
	
}