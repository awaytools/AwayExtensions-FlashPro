#ifndef _LIBAWD_AWD_H
#define _LIBAWD_AWD_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
using namespace std;
#include "poly2tri.h"
using namespace p2t;
#include "FileWriter.h"
#include "awd_types.h"
#include "ns.h"
#include "block.h"
#include "attr.h"
#include "texture.h"
#include "meta.h"
#include "util.h"
#include "message.h"

#include "shape2D.h"
#include "shape2Dfill.h"
#include "shape2Dtimeline.h"
#include "audio.h"
#include "fonts.h"

#define AWD_STREAMING               0x1
#define AWD_TEXTUREATLAS_LIMIT              8192

class AWD
{
    private:
        // File header fields
        awd_uint8 major_version;
        awd_uint8 minor_version;
        awd_uint16 flags;
        AWD_compression compression;
        bool splitByRootObjs;
        bool exportEmtpyContainers;
        string outPath;
        BlockSettings * thisBlockSettings;
        AWDMetaData *metadata;
		FILE * file;
		AWDFileWriter * fileWriter;
		
        AWDBlockList * root_timelines;
        AWDBlockList * namespace_blocks;
        AWDBlockList * texture_blocks;
        AWDBlockList * command_blocks;
        AWDBlockList * message_blocks;
        AWDBlockList * text_blocks;
        AWDBlockList * font_blocks;
        AWDBlockList * audio_blocks;

        AWDBlockList * shape2Dblocks;
        AWDBlockList * shapeFillBlocks;
        AWDBlockList * timelineBlocks;
		

        // Flags and misc
        awd_baddr last_used_baddr;
        awd_nsid last_used_nsid;
        awd_bool header_written;

        void write_header(awd_uint32);
        size_t write_blocks(AWDBlockList *);
        void re_order_blocks(AWDBlockList *, AWDBlockList *);
        void reset_blocks(AWDBlockList *);
        void reset_all_blocks();
        void reset_blocks2(AWDBlockList *);
        void reset_all_blocks2();
        int get_root_objs_count(AWDBlockList *);
        void check_exported_blocks(AWDBlockList *);

    public:
        AWD(BlockSettings *, string&);
        ~AWD();
        awd_uint32 flush();
        awd_uint32 write_blocks_to_file(AWDBlockList *);

        bool has_flag(int);

        static const int VERSION_MAJOR;
        static const int VERSION_MINOR;
        static const int VERSION_BUILD;
        static const char VERSION_RELEASE;

        void set_metadata(AWDMetaData *);

        int count_all_valid_blocks();
        void set_out_path(string& outputPath);
		
        void add_root_scene(AWDBlock *);
        void add_texture(AWDBitmapTexture *);
		void add_shape2Dblock(AWDShape2D *);
		void add_shapeFillBlock(AWDShape2DFill *);
		void add_timelineBlock(AWDShape2DTimeline *);
		void add_textBlock(AWDTextElement *);
		void add_fontBlock(AWDFontShapes *);
		void add_audioBlock(AWDAudio *);
		
		void get_awd_blocks_for_objIDs();
        void add_namespace(AWDNamespace *);
        AWDNamespace * get_namespace(string&);
        AWDBlockList * get_message_blocks();
        AWDBlockList * get_texture_blocks();
        AWDBlockList * get_shape2D_blocks();
        AWDBlockList * get_shapeFill_blocks();
        AWDBlockList * get_timeline_blocks();
        AWDBlockList * get_text_blocks();
        AWDBlockList * get_font_blocks();
        AWDBlockList * get_audio_blocks();
		AWDBitmapTexture * get_texture(string& );
		AWDAudio *		get_audio(string& );
		AWDFontShapes *	get_font_shapes(string& );
		AWDTextElement * get_text(string& );
        AWDShape2DFill * get_solid_fill(awd_color);
        AWDShape2DFill * get_bitmap_fill(string& );
        AWDShape2DFill * get_linear_gradient_fill();
        AWDShape2DFill * get_radial_gradient_fill();
        
};

#endif
