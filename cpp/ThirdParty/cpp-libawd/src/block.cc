#include "awd_types.h"
#include "block.h"
#include "util.h"
#include <vector>


AWDTextureAtlasItem::AWDTextureAtlasItem(awd_color color) 
{
	this->colors.push_back(color);
	this->distributions.push_back(0);
	this->item_type=TEXTURE_ATLAS_COLOR;
    this->width = 1;
    this->height = 1;
}
AWDTextureAtlasItem::AWDTextureAtlasItem(string& source_url, int width, int height) 
{
    this->source_url = source_url;
	this->item_type=TEXTURE_ATLAS_TEXTURE;
    this->width = width;
    this->height = height;
}

AWDTextureAtlasItem::~AWDTextureAtlasItem()
{
}
void AWDTextureAtlasItem::add_color(awd_color color, double distribution){
	if (this->item_type==TEXTURE_ATLAS_COLOR){
		this->item_type=TEXTURE_ATLAS_GRADIENT;
		this->width = 256;
		this->height = 1;
	}
	this->colors.push_back(color);
	this->distributions.push_back(distribution);
}
bool AWDTextureAtlasItem::compare(AWDTextureAtlasItem* atlas_item){
	if (atlas_item->get_item_type()!=item_type)
		return false;
	vector<awd_color> newColors = atlas_item->get_colors();
	if (colors.size()!=atlas_item->get_colors().size())
		return false;
	int cnt=0;
	vector<double> newDistributions = atlas_item->get_distribution();
	for (awd_color color : colors){
		if(color!=newColors[cnt])
			return false;
		if(newDistributions[cnt] != distributions[cnt])
			return false;
		cnt++;
	}
	return true;
}

 vector<awd_color> AWDTextureAtlasItem::get_colors(){
    return this->colors;
}
 vector<double> AWDTextureAtlasItem::get_distribution(){
    return this->distributions;
}
void AWDTextureAtlasItem::set_width(int width){
    this->width = width;
}
int AWDTextureAtlasItem::get_width(){
    return this->width;
}
void AWDTextureAtlasItem::set_height(int height){
    this->height = height;
}
int AWDTextureAtlasItem::get_height(){
    return this->height;
}
void AWDTextureAtlasItem::set_source_url(string& source_url){
    this->source_url = source_url;
}
string& AWDTextureAtlasItem::get_source_url(){
    return this->source_url;
}
AWD_tex_atlas_item_type AWDTextureAtlasItem::get_item_type(){
    return this->item_type;
}
void AWDTextureAtlasItem::set_item_type(AWD_tex_atlas_item_type item_type){
    this->item_type = item_type;
}

BlockSettings::BlockSettings(bool wideMatrix, bool wideGeom, bool wideProps, bool wideAttributes, double scale)
{
        this->wideMatrix=wideMatrix;
        this->wideGeom=wideGeom;
        this->wideProps=wideProps;
        this->wideAttributes=wideAttributes;
        this->scale=scale;
}

BlockSettings::~BlockSettings()
{
}

bool
BlockSettings::get_wide_matrix()
{
    return this->wideMatrix;
}
void
BlockSettings::set_wide_matrix(bool wideMatrix)
{
    this->wideMatrix=wideMatrix;
}
bool
BlockSettings::get_wide_geom()
{
    return this->wideGeom;
}
void
BlockSettings::set_wide_geom(bool wideGeom)
{
    this->wideGeom=wideGeom;
}
bool
BlockSettings::get_wide_props()
{
    return this->wideProps;
}
void
BlockSettings::set_wide_props(bool wideProps)
{
    this->wideProps=wideProps;
}
bool
BlockSettings::get_wide_attributes()
{
    return this->wideAttributes;
}
void
BlockSettings::set_wide_attributes(bool wideAttributes)
{
    this->wideAttributes=wideAttributes;
}
double
BlockSettings::get_scale()
{
    return this->scale;
}
void
BlockSettings::set_scale(double scale)
{
    this->scale=scale;
}

AWDBlock::AWDBlock(AWD_block_type type)
{
    this->type = type;

    // TODO: Allow setting flags
    this->flags = 0;
    this->addr = 0;
    this->isValid =true; //true as long as the block is valid and should get exported
    this->isExported =false; //true if block was exported by the export process (for all files)
    this->isExportedToFile =false; //true if block was exported for one file (gets reset befor exporting one file)
}

AWDBlock::~AWDBlock()
{
}

bool
AWDBlock::get_isValid()
{
    return this->isValid;
}
void
AWDBlock::make_invalide()
{
    this->isValid = false;
}

void
AWDBlock::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    // Does nothing by default. Can be optionally
    // overriden by sub-classes to take any actions
    // that need to happen before length is calculated
}

void
AWDBlock::prepare_and_add_with_dependencies( AWDBlockList *target_list)
{
    if(this->isValid){
        if (!this->isExportedToFile){
            this->isExportedToFile=true;
            this->isExported=true;
            this->prepare_and_add_dependencies(target_list);
            this->addr = target_list->get_num_blocks();
            target_list->append(this);
        }
    }
    else{
        this->addr = 0;
    }
}
size_t
AWDBlock::write_block(AWDFileWriter * fileWriter, BlockSettings *curBlockSettings)
{
    if(!this->isValid)
        return 0;
    awd_uint8 ns_addr;
    awd_uint32 length;
    awd_uint32 length_be;
    awd_baddr block_addr_be;

    this->isExported=true;

    length = this->calc_body_length(curBlockSettings);

    //TODO: Get addr of actual namespace
    ns_addr = 0;

    // Convert to big-endian if necessary
    block_addr_be = this->addr;
    length_be = length;
    if (curBlockSettings->get_wide_matrix())//bit1 = storagePrecision Matrix
        this->flags |= 0x01;
    if (curBlockSettings->get_wide_geom())//bit2 = storagePrecision Geo
        this->flags |= 0x02;
    if (curBlockSettings->get_wide_props())//bit3 = storagePrecision Props
        this->flags |= 0x04;
    // Write 
	fileWriter->writeUINT32(awd_uint32(block_addr_be));
    //(fd, &block_addr_be, sizeof(awd_baddr));
	fileWriter->writeUINT8(awd_uint8(ns_addr));
    //write(fd, &ns_addr, sizeof(awd_uint8));
	fileWriter->writeUINT8(awd_uint8(this->type));
    //write(fd, &this->type, sizeof(awd_uint8));
	fileWriter->writeUINT8(awd_uint8(this->flags));
    //write(fd, &this->flags, sizeof(awd_uint8));
	fileWriter->writeUINT32(awd_uint32(length));
    //write(fd, &length_be, sizeof(awd_uint32));

    // Write body using concrete implementation
    // in block sub-classes
    this->write_body(fileWriter, curBlockSettings);

    return (size_t)length + 11;
}
awd_baddr
AWDBlock::get_addr()
{
    return this->addr;
}

string&
AWDBlock::get_objectID()
{
    return this->objectID;
}
void
AWDBlock::set_objectID(string& objectID)
{
    this->objectID=objectID;
}

AWD_block_type
AWDBlock::get_type()
{
    return this->type;
}

AWDBlockList::AWDBlockList(bool weakReference)
{
    this->first_block = NULL;
    this->last_block = NULL;
    this->num_blocks = 0;
    this->weakReference=weakReference;
}

AWDBlockList::~AWDBlockList()
{
    list_block *cur;
    cur = this->first_block;
    while(cur) {
        list_block *next = cur->next;
        cur->next = NULL;
        if(!weakReference){
            if(cur->block->get_type()==BITMAP_TEXTURE){
                AWDBitmapTexture * thisTex=(AWDBitmapTexture*)cur->block;
                if (thisTex!=NULL)
                    delete thisTex;
            }
            else if(cur->block->get_type()==NAMESPACE){
                AWDNamespace * thisNS=(AWDNamespace*)cur->block;
                if (thisNS!=NULL)
                    delete thisNS;
            }
            else if(cur->block->get_type()==MESSAGE){
                AWDMessageBlock * thisMessage=(AWDMessageBlock*)cur->block;
                if (thisMessage!=NULL)
                    delete thisMessage;
            }
			else if(cur->block->get_type()==SHAPE2D_GEOM){
				AWDShape2D * thisShape=(AWDShape2D*)cur->block;
                if (thisShape!=NULL)
                    delete thisShape;
            }
			else if(cur->block->get_type()==SHAPE2D_TIMELINE){
				AWDShape2DTimeline * thisTimeline=(AWDShape2DTimeline*)cur->block;
                if (thisTimeline!=NULL)
                    delete thisTimeline;
            }
			else if(cur->block->get_type()==SHAPE2D_FILL){
				AWDShape2DFill * thisFill=(AWDShape2DFill*)cur->block;
                if (thisFill!=NULL)
                    delete thisFill;
            }
            // the Metadata-block never gets stored in a BlockList, so it doenst need to be deleted here
        }
        free(cur);
        cur = next;
    }

    // Already deleted as part
    // of above loop
    this->first_block = NULL;
    this->last_block = NULL;
}

bool
AWDBlockList::append(AWDBlock *block)
{
    if (!this->contains(block)) {
        list_block *ctr = (list_block *)malloc(sizeof(list_block));
        ctr->block = block;
        if (this->first_block == NULL) {
            this->first_block = ctr;
        }
        else {
            this->last_block->next = ctr;
        }

        this->last_block = ctr;
        this->last_block->next = NULL;
        this->num_blocks++;

        return true;
    }
    else {
        return false;
    }
}

bool
AWDBlockList::replace(AWDBlock *block, AWDBlock *oldBlock)
{
    list_block *cur;

    cur = this->first_block;
    while (cur) {
        if (cur->block == block){
            cur->block=oldBlock;
            return true;
        }

        cur = cur->next;
    }

    return false;
}

void
AWDBlockList::force_append(AWDBlock *block)
{
    list_block *ctr = (list_block *)malloc(sizeof(list_block));
    ctr->block = block;
    ctr->blockIdx = this->num_blocks;
    if (this->first_block == NULL) {
        this->first_block = ctr;
    }
    else {
        this->last_block->next = ctr;
    }

    this->last_block = ctr;
    this->last_block->next = NULL;
    this->num_blocks++;
}

AWDBlock *
AWDBlockList::getByIndex(int idx)
{
    if (idx>=this->num_blocks)
        return NULL;
    list_block *cur;
    cur = this->first_block;
    int i;
    for (i=0;i<idx;i++){
        cur = cur->next;
    }
    return cur->block;
}

AWDBlock *
AWDBlockList::get_block_by_ID(string& objID)
{
    list_block *cur;
    cur = this->first_block;
    while (cur) {
        if (cur->block->get_objectID() == objID)
            return cur->block;

        cur = cur->next;
    }
    return NULL;
}
        AWDBlock * get_block_by_ID(string&);
bool
AWDBlockList::contains(AWDBlock *block)
{
    list_block *cur;

    cur = this->first_block;
    while (cur) {
        if (cur->block == block)
            return true;

        cur = cur->next;
    }

    return false;
}

int
AWDBlockList::get_num_blocks()
{
    return this->num_blocks;
}
int
AWDBlockList::get_num_blocks_valid()
{
    int num_blocks_valid=0;
    list_block *cur;
    cur = this->first_block;
    while (cur) {
        if (cur->block->get_isValid())
            num_blocks_valid++;
        cur = cur->next;
    }
    return num_blocks_valid;
}

AWDBlockIterator::AWDBlockIterator(AWDBlockList *list)
{
    this->list = list;
    this->cur_block = NULL;
    this->reset();
}

AWDBlockIterator::~AWDBlockIterator()
{
    this->list = NULL;
    this->cur_block = NULL;
    this->reset();
}

void
AWDBlockIterator::reset()
{
    if (this->list != NULL)
        this->cur_block = this->list->first_block;
}

AWDBlock *
AWDBlockIterator::next()
{
    list_block *tmp;

    tmp = this->cur_block;
    if (this->cur_block != NULL)
        this->cur_block = this->cur_block->next;

    if (tmp)
        return tmp->block;
    else return NULL;
}
