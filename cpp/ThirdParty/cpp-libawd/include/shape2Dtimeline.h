#ifndef _LIBAWD_SHAPE2DTIMELINE_H
#define _LIBAWD_SHAPE2DTIMELINE_H

#include "awd_types.h"
#include "stream.h"
#include "block.h"
#include "name.h"
#include "attr.h"
#include <vector>

#include "texture.h"
#include "block.h"
#include "attr.h"

typedef enum {
    AWD_FRAME_COMMAND_UPDATE_OBJECT=1,
    AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT=2,
    AWD_FRAME_COMMAND_REMOVE_OBJECT=3,
    AWD_FRAME_COMMAND_SOUND=4
} AWD_Frame_Command_type;
typedef enum {
    AWD_FRAME_LABEL_NONE=1,
    AWD_FRAME_LABEL_NAME=2,
    AWD_FRAME_LABEL_COMMENT=3,
    AWD_FRAME_LABEL_ANCOR=4
} AWD_Frame_Label_type;

#define CMD_PROPS_OBJID 1
#define CMD_PROPS_RESID 2
#define CMD_PROPS_PLACEAFTER_OBJID 3
#define CMD_PROPS_MATRIX2X3 4
#define CMD_PROPS_VISIBILTIY 5
#define CMD_PROPS_INSTANCENAME 6
#define CMD_PROPS_RESID_FILL 7
class AWDFrameCommand :
    public AWDAttrElement
{
    private:
        AWD_Frame_Command_type command_type;
        AWDNumAttrList * command_properties;
		AWDFrameCommand * filterFrameCommand;// the filter command is special, in that it has a typesd list itself.
        AWDBlock * object_block;
		awd_uint32 objID;
		string resID;
		string instanceName;
		awd_float64* color_matrix;
		awd_float64* display_matrix;
		bool visible;
		awd_uint32 place_after_objectID;
		awd_uint32 clip_depth;
		awd_uint8 blendMode;

		bool hasRessource;// if true than the command will be "add" not "update"
		bool hasDisplayMatrix;
		bool hasColorMatrix;
		bool hasDepthChange;
		bool hasFilterChange;
		bool hasBlendModeChange;
		bool hasDepthClipChange;
		bool hasVisiblitiyChange;


    public:
        AWDFrameCommand(awd_uint32 objID);
        ~AWDFrameCommand();
        void prepare_and_add_dependencies(AWDBlockList *export_list);

		string& get_resID();
		void set_resID(string&);
		awd_uint32 get_objID();
		void set_objID(awd_uint32);
		awd_uint32 get_place_after_objectID();
		void set_place_after_objectID(awd_uint32);
		string& get_instanceName();
		void set_instanceName(string&);
		awd_float64* get_display_matrix();
		void set_display_matrix(awd_float64*);
		awd_float64* get_color_matrix();
		void set_color_matrix(awd_float64*);
		bool get_visible();
		void set_visible(bool);

        void set_object_block(AWDBlock *);
        AWD_Frame_Command_type get_command_type();
		void set_command_type(AWD_Frame_Command_type);
        AWDNumAttrList * get_command_properties();
        awd_uint32 calc_command_length(BlockSettings *);
        void write_command(AWDFileWriter * , BlockSettings *);
};

class AWDTimeLineFrame :public AWDNamedElement,
    public AWDAttrElement
{
    protected:
		vector<AWDFrameCommand*> commands;
		string frameID;
		string frame_code;
		awd_uint32 frame_duration;
		vector<AWD_Frame_Label_type> label_types;
		vector<string> labels;

        AWD_Frame_Command_type command_type;
        AWDNumAttrList * command_properties;

    public:
        AWDTimeLineFrame(string&);
        ~AWDTimeLineFrame();
        void prepare_and_add_dependencies(AWDBlockList *export_list);
		
		void set_frame_duration(awd_uint32);
		awd_uint32 get_frame_duration();
		
		void set_frame_code(string&);
		string& get_frame_code();
		void add_command(AWDFrameCommand*);
		AWDFrameCommand* get_command(awd_uint32 objectID);
		void add_label(AWD_Frame_Label_type, string&);
		
		vector<AWDFrameCommand*> get_commands();

        awd_uint32 calc_frame_length(BlockSettings *);
        void write_frame(AWDFileWriter * , BlockSettings *);
};

class AWDShape2DTimeline : public AWDNamedElement,
    public AWDAttrElement, public AWDBlock
{
    private:
        bool is_processed;
		bool is_scene;
		vector<AWDTimeLineFrame*> frames;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(AWDFileWriter * ,  BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);

    public:
        AWDShape2DTimeline(string& name);
        ~AWDShape2DTimeline();
		
		void set_is_scene(bool);
		bool get_is_scene();
		void add_frame(AWDTimeLineFrame*);
		AWDTimeLineFrame* get_frame();
		vector<AWDTimeLineFrame*>  get_frames();
        bool get_is_processed();
        void set_is_processed(bool);
};

#endif