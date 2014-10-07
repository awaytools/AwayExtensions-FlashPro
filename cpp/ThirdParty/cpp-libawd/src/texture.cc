#include "util.h"
#include "texture.h"
#include <sys/stat.h>

#include <stdlib.h>
#include <cstdio>
#include <string>
#include "lodepng.h"


AWDBitmapTexture::AWDBitmapTexture(string& name) :
    AWDBlock(BITMAP_TEXTURE),
    AWDNamedElement(name),
    AWDAttrElement()
{
    this->saveType = EXTERNAL;
    this->embed_data = NULL;
    this->embed_data_len = 0;
    this->height = 0;
    this->width = 0;
	//this->solidColorFills;
}

AWDBitmapTexture::~AWDBitmapTexture()
{
    if (this->embed_data_len>0) free(this->embed_data);
    this->embed_data = NULL;
    this->embed_data_len = 0;
	//free the url ?
}

AWDTextureAtlasItem*
AWDBitmapTexture::add_texture_atlas_bitmap_item(string& source_url, int width, int height)
{
	for (AWDTextureAtlasItem* atlas_item : atlas_items){
		if((atlas_item->get_item_type()==TEXTURE_ATLAS_TEXTURE)&&(atlas_item->get_source_url()==source_url)){
			return atlas_item;
		}
	}
	AWDTextureAtlasItem* new_atlas_item = new AWDTextureAtlasItem(source_url, width, height);
	atlas_items.push_back(new_atlas_item);
	return new_atlas_item;
}
AWDTextureAtlasItem*
AWDBitmapTexture::add_texture_atlas_color_item(awd_color color)
{
	/*
	for (AWDTextureAtlasItem* atlas_item : atlas_items){
		if((atlas_item->get_item_type()==TEXTURE_ATLAS_COLOR)&&(atlas_item->get_color()==color)){
			return atlas_item;
		}
	}
	AWDTextureAtlasItem* new_atlas_item = new AWDTextureAtlasItem(color);
	atlas_items.push_back(new_atlas_item);
	*/
	return NULL;
}
AWDTextureAtlasItem*
AWDBitmapTexture::add_texture_atlas_gradient_item(awd_color start_color, awd_color end_color)
{
	/*
	for (AWDTextureAtlasItem* atlas_item : atlas_items){
		if(atlas_item->get_item_type()==TEXTURE_ATLAS_GRADIENT){			
			if((atlas_item->get_start_color()==start_color)&&(atlas_item->get_end_color()==end_color)){
				return atlas_item;
			}
		}
	}
	AWDTextureAtlasItem* new_atlas_item = new AWDTextureAtlasItem(start_color, end_color);
	atlas_items.push_back(new_atlas_item);
	*/
	return NULL;
}
void
AWDBitmapTexture::export_texture_atlas()
{
	//NOTE: this sample will overwrite the file or test.png without warning!
	const char* filename = this->url.c_str();
	//generate some image
	unsigned width = 512, height = 512;
	std::vector<unsigned char> image;
	image.resize(width * height * 4);
	for(unsigned y = 0; y < height; y++)
	for(unsigned x = 0; x < width; x++)
	{
		image[4 * width * y + 4 * x + 0] = 255 * !(x & y);
		image[4 * width * y + 4 * x + 1] = x ^ y;
		image[4 * width * y + 4 * x + 2] = x | y;
		image[4 * width * y + 4 * x + 3] = 255;
	}	
	unsigned error = lodepng::encode(filename, image, width, height);
}


int
AWDBitmapTexture::get_width()
{
    return this->width;
}

void
AWDBitmapTexture::set_width(int width)
{
    this->width=width;
}
int
AWDBitmapTexture::get_height()
{
    return this->height;
}
void
AWDBitmapTexture::set_height(int height)
{
    this->height=height;
}
string
AWDBitmapTexture::get_url()
{
    return this->url;
}


void
AWDBitmapTexture::set_url(string& name)
{
    this->url = name;
}

AWD_tex_type
AWDBitmapTexture::get_tex_type()
{
    return this->saveType;
}
void
AWDBitmapTexture::set_tex_type(AWD_tex_type texType)
{
    this->saveType = texType;
}


void
AWDBitmapTexture::set_embed_data(awd_uint8 *buf, awd_uint32 buf_len)
{
    this->embed_data = buf;
    this->embed_data_len = buf_len;
}

void
    AWDBitmapTexture::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    // Do nothing
}


awd_uint32
AWDBitmapTexture::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;

    len = sizeof(awd_uint32); //datalength;
	len += sizeof(awd_uint16) + this->get_name().size(); //name
    len += sizeof(awd_uint8); //save type (external/embbed)

    if (this->saveType == EXTERNAL) {
        len += this->url.size();
    }
    else {
        len += this->embed_data_len;
    }

    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}

void
AWDBitmapTexture::write_body(AWDFileWriter * fileWriter, BlockSettings * curBlockSettings)
{
    awd_uint32 data_len;
	
	fileWriter->writeSTRING( this->get_name(), 1);
	fileWriter->writeUINT8( this->saveType);
	//awdutil_write_varstr(fd, this->get_name().c_str(), this->get_name().size());

    //write(fd, &this->saveType, sizeof(awd_uint8));
    if (this->saveType == EXTERNAL) {
		//fileWriter->writeUINT32(data_len);// 
        //write(fd, &data_len, sizeof(awd_uint32));
        //write(fd, this->url.c_str(), this->url.size());
		fileWriter->writeSTRING(this->url, 2);// frame code	
    }
    else {
		fileWriter->writeUINT32(sizeof(this->embed_data));// 
		awd_uint8* thisEmbeddData=this->embed_data;
		fileWriter->writeBytes(this->embed_data, this->embed_data_len);// frame code	
        //write(fd, &data_len, sizeof(awd_uint32));
        //write(fd, this->embed_data, this->embed_data_len);
    }

    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
}