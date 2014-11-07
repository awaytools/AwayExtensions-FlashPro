#include <cstdio>
#include <stdlib.h>
#include <string.h>

#include "OutputWriter.h"
#include "awd.h"
#include "util.h"
#include "message.h"

const int AWD::VERSION_MAJOR = 3;
const int AWD::VERSION_MINOR = 0;
const int AWD::VERSION_BUILD = 0;
const char AWD::VERSION_RELEASE = 'a';
#include "texturepacker.h"
#include "lodepng.h"

//#include <crtdbg.h>
AWD::AWD()
{

    this->major_version = VERSION_MAJOR;
    this->minor_version = VERSION_MINOR;
	this->compression = UNCOMPRESSED;
    this->flags = 0;
    //this->splitByRootObjs = splitByRootObjs;
    //this->exportEmtpyContainers=exportEmtpyContainers;
    // all this block-lists should have non-weak reference set, so they will delete the blocks
    // this means, that every awdblock should only occur one time in only one of this lists:
    this->texture_blocks = new AWDBlockList(false);
    this->namespace_blocks = new AWDBlockList(false);
    this->command_blocks = new AWDBlockList(false);
    this->message_blocks = new AWDBlockList(false);
	this->audio_blocks = new AWDBlockList(false);
	this->font_blocks = new AWDBlockList(false);
	this->text_blocks = new AWDBlockList(false);
	this->exporterSettings= new AWDExporterSettings();
    this->shape2Dblocks = new AWDBlockList(false);
    this->shapeFillBlocks = new AWDBlockList(false);
    this->textFormat_blocks = new AWDBlockList(false);
    this->timelineBlocks = new AWDBlockList(false);
	this->root_timelines = new AWDBlockList(true);
    this->metadata = NULL;
    this->last_used_nsid = 0;
    this->last_used_baddr = 0;
    this->header_written = AWD_FALSE;
}

AWD::~AWD()
{
    delete this->texture_blocks;
    delete this->namespace_blocks;
    delete this->command_blocks;
    delete this->message_blocks;

    delete this->shape2Dblocks;
    delete this->shapeFillBlocks;
    delete this->timelineBlocks;
    delete this->audio_blocks;
    delete this->font_blocks;
    delete this->text_blocks;
    delete this->root_timelines;
    delete this->textFormat_blocks;
	

    if (this->metadata){
        delete this->metadata;}
    this->metadata=NULL;
    delete this->exporterSettings;
}
AWDExporterSettings* AWD::getExporterSettings(){
	return this->exporterSettings;
}
void
AWD::create_TextureAtlas( string& url)
{
	int maxTexSize=512;
	int maxTexSizeQuad=512*512;
	bool force_quad=true;
	AWDShape2DFill* fill;
	AWDBlockIterator it(shapeFillBlocks);
	vector<vector<vector<AWDTextureAtlasItem*> > > colorAtlasItems;
	vector<vector<AWDTextureAtlasItem*> >bitmapAtlasItems;
	int bitmapSizeCnt=0;
	int colorsSize=0;
	vector<vector<AWDTextureAtlasItem*> > first_colors_items;
	vector<AWDTextureAtlasItem*> first_solid_items;
	vector<AWDTextureAtlasItem*> first_gradient_items;
	first_colors_items.push_back(first_solid_items);
	first_colors_items.push_back(first_gradient_items);
	colorAtlasItems.push_back(first_colors_items);
	vector<AWDTextureAtlasItem*> first_bitmap_items;
	bitmapAtlasItems.push_back(first_bitmap_items);
	int atlas_col_cnt=0;
	int atlas_tex_cnt=0;
	
    while ((fill = (AWDShape2DFill *)it.next()) != NULL) {
		AWDTextureAtlasItem* newAtlasitem;
		if(fill->get_fill_type()==AWD_FILLTYPE_SOLID){			
			colorsSize++;
			if(colorsSize>maxTexSizeQuad){
				atlas_col_cnt++;
				vector<vector<AWDTextureAtlasItem*> > next_colors_items;
				vector<AWDTextureAtlasItem*> next_solid_items;
				vector<AWDTextureAtlasItem*> next_gradient_items;
				next_colors_items.push_back(next_solid_items);
				next_colors_items.push_back(next_gradient_items);
				colorAtlasItems.push_back(next_colors_items);
			}
			newAtlasitem=new AWDTextureAtlasItem(fill->get_color());
			colorAtlasItems[atlas_col_cnt][0].push_back(newAtlasitem);
		}
		else if((fill->get_fill_type()==AWD_FILLTYPE_GRADIENT_LINEAR)||(fill->get_fill_type()==AWD_FILLTYPE_GRADIENT_RADIAL)){
			colorsSize+=256;
			if(colorsSize>maxTexSizeQuad){
				atlas_col_cnt++;
				vector<vector<AWDTextureAtlasItem*> > next_colors_items;
				vector<AWDTextureAtlasItem*> next_solid_items;
				vector<AWDTextureAtlasItem*> next_gradient_items;
				next_colors_items.push_back(next_solid_items);
				next_colors_items.push_back(next_gradient_items);
				colorAtlasItems.push_back(next_colors_items);
			}
			newAtlasitem=new AWDTextureAtlasItem(fill->get_gradient());
			colorAtlasItems[atlas_col_cnt][1].push_back(newAtlasitem);
		}
		else if(fill->get_fill_type()==AWD_FILLTYPE_TEXTURE){
			if(fill->get_tex_width()>maxTexSize){
			}
			else if(fill->get_tex_height()>maxTexSize){
			}
			else if(fill->get_tex_width()*fill->get_tex_height()>maxTexSizeQuad){
			}
			else {
				bitmapSizeCnt+=fill->get_tex_width()*fill->get_tex_height();
				if(bitmapSizeCnt+colorsSize>maxTexSizeQuad){
					atlas_tex_cnt++;
					bitmapSizeCnt=fill->get_tex_width()*fill->get_tex_height();
					vector<AWDTextureAtlasItem*> next_bitmap_items;
					bitmapAtlasItems.push_back(next_bitmap_items);
				}
				newAtlasitem=new AWDTextureAtlasItem(fill->get_texture_url(),  fill->get_tex_width(), fill->get_tex_height());
				bitmapAtlasItems[atlas_tex_cnt].push_back(newAtlasitem);
			}
		}
	}
	string url_start=url+"/ColorAtlas_";
	string url_extens=".png";
	int bitmapCnter=0;
	vector<AWDTextureAtlasItem*> atlasToMerge;
	/*
	TEXTURE_PACKER::TexturePacker *tp = TEXTURE_PACKER::createTexturePacker();
	tp->setTextureCount(10);	
	tp->addTexture(512,1);
	int width;
	int height;
	int unused_area = tp->packTextures(width,height,true,true);
	int texX;
	int texY;
	int texWidth;
	int texHeight;
	bool isflipped=tp->getTextureLocation(0, texX, texY, texWidth, texHeight);
	*/
	/*
	for(vector<AWDTextureAtlasItem*> bitTexAtlasList:bitmapAtlasItems){
		bool exportThis=true;
		if(bitmapCnter<bitmapAtlasItems.size()-1){
			if(!bitmapSizeCnt+colorsSize>maxTexSizeQuad){
				atlasToMerge=bitTexAtlasList;
				exportThis=false;
			}
		}
		if(exportThis){
			// export this ínto a textureAtlas
		}
		bitmapCnter++;
	}*/
	int colorAtlasCnter=0;
	for(vector<vector<AWDTextureAtlasItem*> > bitTexAtlasList:colorAtlasItems){
		vector<AWDTextureAtlasItem*> solidColors=bitTexAtlasList[0];
		vector<AWDTextureAtlasItem*> gradientColors=bitTexAtlasList[1];
		if((colorAtlasCnter==colorAtlasItems.size()-1)&&(atlasToMerge.size()>0)){
			// merge textureatlas into solidAtlas
		}		
		int texHeight=2;
		int texWidth=2;
		bool findSize=true;
		int thisSize=512*512;
		/*
		while (findSize){
			if((texHeight*texWidth)>thisSize){
			}
		}
		*/
		string url_final=url_start+AwayJS::Utils::ToString(colorAtlasCnter)+url_extens;//
		const char* filename = url_final.c_str();
		unsigned width = 512, height = 512;
		std::vector<unsigned char> image;
		image.resize(width * height * 4);
		int colorCnt=0;
		for(AWDTextureAtlasItem* texAtlas:solidColors){
			awd_color color=texAtlas->get_color();
			for(int tester=0; tester<4000; tester++){
				if((4 * colorCnt + 4)<(width * height * 4)){
					image[4 * colorCnt] = (color >> 16);
					image[4 * colorCnt + 1] =(color >> 8);
					image[4 * colorCnt + 2] = (color >> 0);
					image[4 * colorCnt + 3] = (color >> 24);
					colorCnt++;
				}
			}
		}
		/*
		for(unsigned y = 0; y < height; y++)
		for(unsigned x = 0; x < width; x++)
		{
			image[4 * width * y + 4 * x + 0] = 255 * !(x & y);
			image[4 * width * y + 4 * x + 1] = x ^ y;
			image[4 * width * y + 4 * x + 2] = x | y;
			image[4 * width * y + 4 * x + 3] = 255;
		}	*/
		unsigned error = lodepng::encode(filename, image, width, height);
	}
	//for(AWDTextureAtlasItem* bitTexAtlas:
	/*string atlasName="atlas_";
	int atlascnt=0;//todo:add atlas nr to name
	AWDBitmapTexture* newTextureAtlas=new AWDBitmapTexture(atlasName);*/
	//NOTE: this sample will overwrite the file or test.png without warning!
	//generate some image
}

int
AWD::count_all_valid_blocks()
{
    int blockCnt=0;
    blockCnt+=this->texture_blocks->get_num_blocks_valid();
    blockCnt+=this->namespace_blocks->get_num_blocks_valid();
    blockCnt+=this->command_blocks->get_num_blocks_valid();
    blockCnt+=this->shape2Dblocks->get_num_blocks_valid();
    blockCnt+=this->shapeFillBlocks->get_num_blocks_valid();
    blockCnt+=this->timelineBlocks->get_num_blocks_valid();
    return blockCnt;
}
bool
AWD::has_flag(int flag)
{
    return ((this->flags & flag) > 0);
}


		
AWDTextFormat *
AWD::get_text_format(string& newtexFormat)
{
	AWDTextFormat *texFormat;
	AWDBlockIterator it(textFormat_blocks);
    while ((texFormat = (AWDTextFormat *)it.next()) != NULL) {
		if(texFormat->get_name()==newtexFormat){
			return texFormat;
		}
    }
	AWDTextFormat* newAWDTextFormat = new AWDTextFormat(newtexFormat);
	add_textFormatBlock(newAWDTextFormat);
	return newAWDTextFormat;
}
AWDTextElement *
AWD::get_text(string& newText)
{
	AWDTextElement *tex;
	AWDBlockIterator it(text_blocks);
    while ((tex = (AWDTextElement *)it.next()) != NULL) {
		if(tex->get_name()==newText){
			return tex;
		}
    }
	AWDTextElement* newTextElement = new AWDTextElement(newText);
	add_textBlock(newTextElement);
	return newTextElement;
}
AWDFont *
AWD::get_font_shapes(string& newFontName)
{
	AWDFont *font_shapes;
	AWDBlockIterator it(font_blocks);
    while ((font_shapes = (AWDFont *)it.next()) != NULL) {
		if(font_shapes->get_name()==newFontName){
			return font_shapes;
		}
    }
	AWDFont* newFontShapes = new AWDFont(newFontName);
	add_fontBlock(newFontShapes);
	return newFontShapes;
}

AWDShape2DTimeline *
	AWD::get_timeline(string& newAudioPath)
{
	AWDShape2DTimeline *tex;
	AWDBlockIterator it(timelineBlocks);
    while ((tex = (AWDShape2DTimeline *)it.next()) != NULL) {
		if(tex->get_name()==newAudioPath){
			return tex;
		}
    }
	string newAudioName(newAudioPath);
	AWDShape2DTimeline* newAudio = new AWDShape2DTimeline(newAudioName);
	add_timelineBlock(newAudio);
	return newAudio;
}
AWDAudio *
AWD::get_audio(string& newAudioPath)
{
	AWDAudio *tex;
	AWDBlockIterator it(audio_blocks);
    while ((tex = (AWDAudio *)it.next()) != NULL) {
		if(tex->get_name()==newAudioPath){
			return tex;
		}
    }
	string newAudioName(newAudioPath);
	AWDAudio* newAudio = new AWDAudio(newAudioName);
	add_audioBlock(newAudio);
	return newAudio;
}

AWDBitmapTexture *
AWD::get_texture(string& newTexturePath)
{
	AWDBitmapTexture *tex;
	AWDBlockIterator it(texture_blocks);
    while ((tex = (AWDBitmapTexture *)it.next()) != NULL) {
		if(tex->get_url()==newTexturePath){
			return tex;
		}
    }
	string newTexName("Texture");
	AWDBitmapTexture* newTexture = new AWDBitmapTexture(newTexName);
	newTexture->set_url(newTexturePath);
	add_texture(newTexture);
	return newTexture;
}
AWDShape2DFill *
AWD::get_solid_fill(awd_color color)
{
	
	AWDShape2DFill *fill;
	AWDBlockIterator it(shapeFillBlocks);

    while ((fill = (AWDShape2DFill *)it.next()) != NULL) {
		if(fill->get_fill_type()==AWD_FILLTYPE_SOLID){
			if(fill->get_color()==color){
				return fill;
			}
		}
    }	
	string newName("SolidFill");
	AWDShape2DFill* newFill = new AWDShape2DFill(newName, AWD_FILLTYPE_SOLID);
	newFill->set_color(color);
	add_shapeFillBlock(newFill);
	
	
    return newFill;
}
AWDShape2DFill *
AWD::get_bitmap_fill(string& newTexturePath)
{
	
	AWDShape2DFill *fill;
	AWDBlockIterator it(shapeFillBlocks);

    while ((fill = (AWDShape2DFill *)it.next()) != NULL) {
		if(fill->get_fill_type()==AWD_FILLTYPE_TEXTURE){
			if(fill->get_texture()!=NULL){
				if(fill->get_texture()->get_url()==newTexturePath){
					return fill;
				}
			}
		}
    }		
	string newName("BitmapFill");
	AWDShape2DFill* newFill = new AWDShape2DFill(newName, AWD_FILLTYPE_TEXTURE);
	AWDBitmapTexture* newTexture=get_texture(newTexturePath);
	newFill->set_texture(newTexture);
	add_shapeFillBlock(newFill);
    return newFill;
}
AWDShape2DFill *
AWD::get_linear_gradient_fill()
{
	string newName("new Name");
    return new AWDShape2DFill(newName, AWD_FILLTYPE_GRADIENT_LINEAR);
}
AWDShape2DFill *
AWD::get_radial_gradient_fill()
{
	string newName("new Name");
    return new AWDShape2DFill(newName, AWD_FILLTYPE_GRADIENT_RADIAL);
}
void
AWD::set_metadata(AWDMetaData *block)
{
    this->metadata = block;
}

void
AWD::add_texture(AWDBitmapTexture *block)
{
    this->texture_blocks->append(block);
}

void
AWD::add_shape2Dblock(AWDShape2D *block)
{
	this->shape2Dblocks->append(block);
}
void
AWD::add_shapeFillBlock(AWDShape2DFill *block)
{
	this->shapeFillBlocks->append(block);
}
void
AWD::add_timelineBlock(AWDShape2DTimeline *block)
{
	this->timelineBlocks->append(block);
}

void
AWD::add_textBlock(AWDTextElement *block)
{
	this->text_blocks->append(block);
}
void
AWD::add_fontBlock(AWDFont *block)
{
	this->font_blocks->append(block);
}
void
AWD::add_textFormatBlock(AWDTextFormat *block)
{
	this->textFormat_blocks->append(block);
}
void
AWD::add_audioBlock(AWDAudio *block)
{
	this->audio_blocks->append(block);
}
AWDBlockList *
AWD::get_message_blocks()
{
    return this->message_blocks;
}
AWDBlockList *
AWD::get_texture_blocks()
{
    return this->texture_blocks;
}
AWDBlockList *
AWD::get_shape2D_blocks()
{
    return this->shape2Dblocks;
}
AWDBlockList *
AWD::get_shapeFill_blocks()
{
    return this->shapeFillBlocks;
}
AWDBlockList *
AWD::get_timeline_blocks()
{
    return this->timelineBlocks;
}
AWDBlockList *
AWD::get_text_blocks()
{
    return this->text_blocks;
}
AWDBlockList *
AWD::get_font_blocks()
{
    return this->font_blocks;
}
AWDBlockList *
AWD::get_audio_blocks()
{
    return this->audio_blocks;
}
AWDBlockList *
AWD::get_text_format_blocks()
{
    return this->textFormat_blocks;
}


void
AWD::add_namespace(AWDNamespace *block)
{
    if (this->namespace_blocks->append(block)) {
        this->last_used_nsid++;
        block->set_handle(this->last_used_nsid);
    }
}

AWDNamespace *
AWD::get_namespace(string& uri)
{
    AWDNamespace *ns;
    AWDBlockIterator it(this->namespace_blocks);

    while ((ns = (AWDNamespace *)it.next()) != NULL) {
        string ns_uri;
        ns_uri = ns->get_uri();

        if (ns_uri == uri) {
            return ns;
        }
    }

    return NULL;
}

void
AWD::write_header(awd_uint32 body_length)
{
    awd_uint16 flags_be;

    // Convert to big-endian if necessary
    flags_be = this->flags;
    body_length = body_length;
    string awdstring("AWD");
	this->fileWriter->writeSTRING(awdstring, 0);
	
    //write(fd, "AWD", 3);
	this->fileWriter->writeUINT8(this->major_version);
    //write(fd, &this->major_version, sizeof(awd_uint8));
	this->fileWriter->writeUINT8(this->minor_version);
    //write(fd, &this->minor_version, sizeof(awd_uint8));
	this->fileWriter->writeUINT16(flags_be);
    //write(fd, &flags_be, sizeof(awd_uint16));
	
    awd_uint8 compression=this->compression;
	this->fileWriter->writeUINT8(compression);
    //write(fd, (awd_uint8*)&this->compression, sizeof(awd_uint8));
	this->fileWriter->writeUINT32(body_length);
    //write(fd, &body_length, sizeof(awd_uint32));
	
}

void
AWD::check_exported_blocks(AWDBlockList * blocks)
{
    size_t len;
    AWDBlock *block;
    AWDBlockIterator it(blocks);
    len = 0;
    while ((block = it.next()) != NULL) {
        if(block->get_isValid()){
            if(!block->isExported){
                string message_str_1="n.a.";
                string message_str_2="Block not exported because not used in the AWDScene";
                AWDMessageBlock * newWarning = new AWDMessageBlock(message_str_1,message_str_2);
                this->message_blocks->append(newWarning);
            }
        }
    }
}

void
AWD::re_order_blocks(AWDBlockList *blocks, AWDBlockList *targetList)
{
    size_t len;
    AWDBlock *block;
    AWDBlockIterator it(blocks);
    len = 0;
    while ((block = it.next()) != NULL) {
       // if(block->get_isValid())
        block->prepare_and_add_with_dependencies(targetList);
    }
}
size_t
AWD::write_blocks(AWDBlockList *blocks)
{
    size_t len;
    AWDBlock *block;
    AWDBlockIterator it(blocks);

    len = 0;
    while ((block = it.next()) != NULL) {
		len += block->write_block(this->fileWriter, this->exporterSettings->get_blockSettings());
    }

    return len;
}

void
AWD::reset_blocks(AWDBlockList *blocks)
{
    AWDBlock *block;
    AWDBlockIterator it(blocks);

    while ((block = it.next()) != NULL) {
        block->isExportedToFile=false;
    }
}
void
AWD::reset_all_blocks()
{
    this->reset_blocks(this->namespace_blocks);
    this->reset_blocks(this->texture_blocks);
}
void
AWD::reset_blocks2(AWDBlockList *blocks)
{
    AWDBlock *block;
    AWDBlockIterator it(blocks);

    while ((block = it.next()) != NULL) {
        block->isExportedToFile=block->isExported;
    }
}
void
AWD::reset_all_blocks2()
{
    this->reset_blocks2(this->namespace_blocks);
    this->reset_blocks2(this->texture_blocks);
	this->reset_blocks2(this->shapeFillBlocks);
	this->reset_blocks2(this->timelineBlocks);
	this->reset_blocks2(this->shape2Dblocks);
}


void
AWD::get_awd_blocks_for_objIDs()
{
	
	AWDShape2DTimeline *block;
    AWDBlockIterator it(timelineBlocks);
    while ((block = (AWDShape2DTimeline*)it.next()) != NULL) {
		vector<AWDTimeLineFrame*> thisFrames=block->get_frames();
		for (AWDTimeLineFrame * f : thisFrames) 
		{
			vector<AWDFrameCommandBase*> thisCommands=f->get_commands();
			for (AWDFrameCommandBase * c : thisCommands) 
			{
				if((c->get_command_type()==AWD_FRAME_COMMAND_UPDATE_OBJECT)||(c->get_command_type()==AWD_FRAME_COMMAND_SOUND)){
					string resID=c->get_resID();
					if (resID.c_str()!=NULL) {
						AWDBlock * thisBlock;
						thisBlock=texture_blocks->get_block_by_ID(resID);
						if(thisBlock==NULL){
							thisBlock=shape2Dblocks->get_block_by_ID(resID);
						}
						if(thisBlock==NULL){
							thisBlock=timelineBlocks->get_block_by_ID(resID);
						}
						if(thisBlock==NULL){
							thisBlock=text_blocks->get_block_by_ID(resID);
						}
						if(thisBlock==NULL){
							thisBlock=audio_blocks->get_block_by_ID(resID);
						}
						c->set_object_block(thisBlock);
					}
				}
			}
		}
	}
	
}

void
AWD::add_root_scene(AWDBlock* root)
{
	this->root_timelines->append(root);
}
awd_uint32
AWD::flush(std::string& outfile)
{
	
	this->file = fopen (outfile.c_str(), "wb");
	if(this->file==NULL){
		return AWD_FALSE;
	}
	this->fileWriter=new AWDFileWriter(this->file);
    //contain all blocks that are going to be written in the main AWD file.
    AWDBlockList * blocks_mainAWD = new AWDBlockList();


    //make shure the metadata block exists, because no other awdblock should have the id=0
    if (this->metadata==NULL)
        this->metadata=new AWDMetaData();
    this->metadata->prepare_and_add_with_dependencies(blocks_mainAWD);

    char * cur_filename=NULL;
    
    this->re_order_blocks(this->namespace_blocks, blocks_mainAWD);

	
    //this->reset_all_blocks2();
	
	this->re_order_blocks(this->root_timelines, blocks_mainAWD);
	
    this->re_order_blocks(this->texture_blocks, blocks_mainAWD);
    this->re_order_blocks(this->shapeFillBlocks, blocks_mainAWD);
    this->re_order_blocks(this->shape2Dblocks, blocks_mainAWD);
	this->re_order_blocks(this->timelineBlocks, blocks_mainAWD);
	this->re_order_blocks(this->audio_blocks, blocks_mainAWD);
	//this->re_order_blocks(this->font_blocks, blocks_mainAWD);
	//this->re_order_blocks(this->text_blocks, blocks_mainAWD);
	
    // write the main file
   // if (blocks_mainAWD->get_num_blocks()>=1){
		this->write_blocks_to_file(blocks_mainAWD);
   // }

    this->check_exported_blocks(this->texture_blocks);

    delete blocks_mainAWD;
    //open the last stored file with the default appliction (e.g. Awaybuilder)
	
    return AWD_TRUE;
}



awd_uint32
AWD::write_blocks_to_file(AWDBlockList *blocks)
{
	this->fileWriter->set_file(tmpfile());
	/*
    int tmp_fd;
    char *tmp_path=NULL;

    tmp_len = 0;
    tmp_fd = awdutil_mktmp(&tmp_path);
    if (tmp_fd < 0) {
        extern int errno;
        printf("Could not open temporary file necessary for writing, errno=%d\n", errno);
        return AWD_FALSE;
    }
	*/
    size_t tmp_len = 0;
    awd_uint8 *tmp_buf;

    awd_uint8 *body_buf;
    awd_uint32 body_len;
    int sds=(int)this->write_blocks(blocks);
    tmp_len += sds;

    tmp_buf = (awd_uint8 *) malloc(tmp_len);
	rewind(this->fileWriter->get_file());
    fread(tmp_buf, 1, tmp_len, this->fileWriter->get_file());

    // Temp file no longer needed
    fclose(this->fileWriter->get_file());
    //unlink((const char *)tmp_path);

    body_len = awd_uint32(tmp_len);
    body_buf = tmp_buf;
    // Write header and then body from possibly
    // compressed buffer
	this->fileWriter->set_file(this->file);
    this->write_header(body_len);
    fwrite(body_buf, body_len, 1, this->file);
	fflush(this->file);
    //write(out_fd, body_buf, body_len);
    if (body_buf!=tmp_buf)
        free(tmp_buf);
    free (body_buf);
	fclose(this->file);
    //if (tmp_path!=NULL)
        //free(tmp_path);
    return AWD_TRUE;
}