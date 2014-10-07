#ifndef _LIBAWD_BLOCK_H
#define _LIBAWD_BLOCK_H

#include <stdlib.h>

#include "FileWriter.h"
//#include "awd.h"
#include "awd_types.h"
#include <vector>

typedef enum {
    TEXTURE_ATLAS_COLOR=0,
    TEXTURE_ATLAS_GRADIENT=1,
    TEXTURE_ATLAS_TEXTURE=2// not supported yet
} AWD_tex_atlas_item_type;
class AWDTextureAtlasItem
{
    private:
		vector<awd_color> colors;
		vector<double> distributions;
		string source_url;// only for texture items
		int width;
		int height;
		AWD_tex_atlas_item_type item_type;


    public:
        AWDTextureAtlasItem(awd_color);
		AWDTextureAtlasItem(string&, int, int);
        ~AWDTextureAtlasItem();
        void add_color(awd_color, double);
        bool compare(AWDTextureAtlasItem* atlas_item);
        vector<awd_color> get_colors();
        vector<double> get_distribution();

        void set_source_url(string&);
        string& get_source_url();
        void set_width(int);
        int get_width();
        void set_height(int);
        int get_height();
        void set_item_type(AWD_tex_atlas_item_type);
        AWD_tex_atlas_item_type get_item_type();
};

class BlockSettings
{
    private:
        // File header fields
        bool wideMatrix;
        bool wideGeom;
        bool wideProps;
        bool wideAttributes;


        double scale;

    public:
        BlockSettings(bool,bool, bool, bool, double);
        ~BlockSettings();
		
        bool get_wide_matrix();
        void set_wide_matrix(bool);
        bool get_wide_geom();
        void set_wide_geom(bool);
        bool get_wide_props();
        void set_wide_props(bool);
        bool get_wide_attributes();
        void set_wide_attributes(bool);
        double get_scale();
        void set_scale(double);
};

class AWDBlockList;
class AWDBlock
{
    private:
        awd_baddr addr;
        awd_uint8 flags;
        bool isValid;
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
