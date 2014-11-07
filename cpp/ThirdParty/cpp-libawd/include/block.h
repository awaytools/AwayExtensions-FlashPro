#ifndef _LIBAWD_BLOCK_H
#define _LIBAWD_BLOCK_H

#include <stdlib.h>
#include "settings.h"
#include "FileWriter.h"
//#include "awd.h"
#include "awd_types.h"
#include <vector>
//#include <crtdbg.h>

class AWDGradientitem
{
    private:
		vector<awd_color> colors;
		vector<double> distributions;

    public:
        AWDGradientitem();
        ~AWDGradientitem();
		bool compare(AWDGradientitem* atlas_item);
        void add_color(awd_color, double);
        vector<awd_color> get_colors();
        vector<double> get_distribution();
};
typedef enum {
    TEXTURE_ATLAS_COLOR=0,
    TEXTURE_ATLAS_GRADIENT=1,
    TEXTURE_ATLAS_TEXTURE=2// not supported yet
} AWD_tex_atlas_item_type;
class AWDTextureAtlasItem
{
    private:
		int width;
		int height;
		AWDGradientitem* gradient;
		awd_color color;
		string source_url;
		AWD_tex_atlas_item_type item_type;


    public:
        AWDTextureAtlasItem(AWDGradientitem*);
        AWDTextureAtlasItem(awd_color);
		AWDTextureAtlasItem(string&, int, int);
        ~AWDTextureAtlasItem();

        void set_source_url(string&);
        string& get_source_url();
        void set_width(int);
        int get_width();
        void set_height(int);
        int get_height();
        AWDGradientitem* get_gradient();
        awd_color get_color();
        void set_item_type(AWD_tex_atlas_item_type);
        AWD_tex_atlas_item_type get_item_type();
};


class AWDBlockList;
class AWDBlock
{
    private:
        awd_baddr addr;
        awd_uint8 flags;
        bool isValid;
        bool isProcessed;
		string objectID;

    protected:
        AWD_block_type type;
        virtual void prepare_and_add_dependencies(AWDBlockList *export_list);
        virtual awd_uint32 calc_body_length(BlockSettings *)=0;
        virtual void write_body(AWDFileWriter *, BlockSettings *)=0;

    public:
        AWDBlock(AWD_block_type);
        ~AWDBlock();
        bool isExported;
        bool isExportedToFile;
		
        string& get_objectID();
        void set_objectID(string&);
        awd_baddr get_addr();
        AWD_block_type get_type();
        bool get_isValid();
        void make_invalide();
        bool get_isProcessed();
        void set_isProcessed(bool);

        void prepare_and_add_with_dependencies(AWDBlockList *);

        size_t write_block(AWDFileWriter *, BlockSettings *);
};

typedef struct _list_block
{
    AWDBlock *block;
    int blockIdx;
    struct _list_block *next;
} list_block;

class AWDBlockList
{
    private:
        int num_blocks;
        bool weakReference;

    public:
        list_block *first_block;
        list_block *last_block;

        AWDBlockList(bool weakReference=true);
        ~AWDBlockList();

        bool append(AWDBlock *);
        void force_append(AWDBlock *);
        bool contains(AWDBlock *);
        bool replace(AWDBlock *, AWDBlock *oldBlock);
        AWDBlock * get_block_by_ID(string&);

        AWDBlock* getByIndex(int);

        int get_num_blocks();
        int get_num_blocks_valid();
};

class AWDBlockIterator
{
    private:
        AWDBlockList * list;
        list_block * cur_block;

    public:
        AWDBlockIterator(AWDBlockList *);
        ~AWDBlockIterator();
        AWDBlock * next();
        void reset();
};

#endif
