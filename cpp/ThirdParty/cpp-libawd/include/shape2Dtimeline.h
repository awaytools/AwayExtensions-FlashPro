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
class AWDFrameCommandBase
{
    private:
        AWD_Frame_Command_type command_type;
        AWDNumAttrList * command_properties;
		string objectType;
        AWDBlock * object_block;
		awd_uint32 objID;
		string resID;
		awd_uint32 layerID;
		bool hasRessource;// if true than the command will be "add" not "update"

		
    public:
        AWDFrameCommandBase();
        ~AWDFrameCommandBase();

        void set_object_block(AWDBlock *);
        AWDBlock* get_object_block();
		awd_uint32 get_layerID();
		void set_layerID(awd_uint32);
		string& get_objectType();
		void set_objectType(string&);
		string& get_resID();
		void set_resID(string&);
		awd_uint32 get_objID();
		void set_objID(awd_uint32);
		bool get_hasRessource();
		void set_hasRessource(bool hasRessource);
        AWD_Frame_Command_type get_command_type();
		void set_command_type(AWD_Frame_Command_type);
        AWDNumAttrList * get_command_properties();

        virtual void prepare_and_add_dependencies(AWDBlockList *export_list);
        virtual awd_uint32 calc_command_length(BlockSettings *)=0;
        virtual void write_command(AWDFileWriter *, BlockSettings *)=0;
		virtual double compare_to_command(AWDFrameCommandBase *)=0;
		virtual void update_by_command(AWDFrameCommandBase *)=0;
};
class AWDFrameCommandDisplayObject :
	public AWDFrameCommandBase
{
    private:

		//AWDFrameCommandDisplayObject * filterFrameCommand;// the filter command is special, in that it has a typesd list itself.
		string instanceName;
		awd_float64* color_matrix;
		awd_float64* display_matrix;
		bool visible;
		awd_uint32 depth;
		awd_uint32 clip_depth;
		awd_uint8 blendMode;

		bool hasDisplayMatrix;
		bool hasColorMatrix;
		bool hasDepthChange;
		bool hasFilterChange;
		bool hasBlendModeChange;
		bool hasDepthClipChange;
		bool hasVisiblitiyChange;
		

    public:
        AWDFrameCommandDisplayObject();
        ~AWDFrameCommandDisplayObject();

		double compareColorMatrix(awd_float64* color_matrix);		
		double comparedisplaMatrix(awd_float64* display_matrix);
		bool get_hasDisplayMatrix();
		bool get_hasColorMatrix();
		bool get_hasDepthChange();
		bool get_hasFilterChange();
		bool get_hasBlendModeChange();
		bool get_hasDepthClipChange();
		bool get_hasVisiblitiyChange();
		int get_blendmode();
		void set_blendmode(int);
		awd_uint32 get_depth();
		void set_depth(awd_uint32);
		awd_uint32 get_clipDepth();
		void set_clipDepth(awd_uint32);
		string& get_instanceName();
		void set_instanceName(string&);
		awd_float64* get_display_matrix();
		void set_display_matrix(awd_float64*);
		awd_float64* get_color_matrix();
		void set_color_matrix(awd_float64*);
		bool get_visible();
		void set_visible(bool);

        awd_uint32 calc_command_length(BlockSettings *);
        void write_command(AWDFileWriter * ,  BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
		double compare_to_command(AWDFrameCommandBase *);
		void update_by_command(AWDFrameCommandBase *);

};

class AWDTimeLineFrame :
    public AWDAttrElement
{
protected:
		vector<AWDFrameCommandBase*> commands;
		vector<AWDFrameCommandBase*> inActivecommands;
		string frameID;
		string frame_code;
		awd_uint32 frame_duration;
		vector<int> dirty_layer_idx;
		vector<AWD_Frame_Label_type> label_types;
		vector<string> labels;

        AWDNumAttrList * command_properties;

    public:
        AWDTimeLineFrame();
        ~AWDTimeLineFrame();
        void prepare_and_add_dependencies(AWDBlockList *export_list);
		
		void set_frame_duration(awd_uint32);
		awd_uint32 get_frame_duration();
		
		void set_frame_code(string&);
		string& get_frame_code();
		void add_command(AWDFrameCommandBase*);
		void clear_commands();
		void add_inActivecommand(AWDFrameCommandBase*);
		AWDFrameCommandBase* get_command(awd_uint32 objectID);
		void add_label(AWD_Frame_Label_type, string&);
		void add_dirty_layer(int);
		vector<int> get_dirty_layers();

		vector<AWDFrameCommandBase*> get_commands();
		vector<AWDFrameCommandBase*> get_inActivecommands();	

        awd_uint32 calc_frame_length(BlockSettings *);
        void write_frame(AWDFileWriter * , BlockSettings *);
        void finalizeCommands();
};

class AWDShape2DTimeline : public AWDNamedElement,
    public AWDAttrElement, public AWDBlock
{
    private:
		int scene_id;
		int obj_id_cnt;
		vector<AWDTimeLineFrame*> frames;
		vector<AWDBlock*> usedRessourceBlocks;
		vector<bool> usedReccourseBlocksUsage;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(AWDFileWriter * ,  BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);

    public:
        AWDShape2DTimeline(string& name);
        ~AWDShape2DTimeline();
		
		void set_scene_id(int);
		int get_scene_id();
		void reset_ressourceBlockUsage();
		void reset_ressourceBlockUsageForID(int thisID);
		int get_ressourceBlockID(AWDBlock* resBlock);
		void add_frame(AWDTimeLineFrame*, bool modifyCommands);
		AWDTimeLineFrame* get_frame();
		vector<AWDTimeLineFrame*>  get_frames();
};

#endif