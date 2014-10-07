#include "util.h"
#include "shape2Dtimeline.h"

#include <stdlib.h>
#include <cstdio>
#include <string>
using namespace std;


AWDFrameCommand::AWDFrameCommand(awd_uint32 objectID) :
    AWDAttrElement()
{
    this->command_properties=new AWDNumAttrList();
    this->command_type = command_type;
	this->objID = objectID;
	this->object_block = NULL;
	this->hasRessource=false;// if true than the command will be "add" not "update"
	this->hasDisplayMatrix=false;
	this->hasColorMatrix=false;
	this->hasDepthChange=false;
	this->hasFilterChange=false;
	this->hasBlendModeChange=false;
	this->hasDepthClipChange=false;
	this->hasVisiblitiyChange=false;
	this->clip_depth=0;
	this->blendMode=0;
}

AWDFrameCommand::~AWDFrameCommand()
{
    delete this->command_properties;
	this->object_block = NULL;
	if(hasDisplayMatrix){
		free(display_matrix);
		display_matrix=NULL;
		hasDisplayMatrix=false;
	}
	if(hasColorMatrix){
		free(color_matrix);
		color_matrix=NULL;
		hasColorMatrix=false;
	}
}

string&
AWDFrameCommand::get_resID()
{
	return this->resID;
}
void
AWDFrameCommand::set_resID(string& new_res)
{
	this->hasRessource=true;
    this->resID = new_res;
}
void
AWDFrameCommand::set_display_matrix(awd_float64* display_matrix)
{
	this->hasDisplayMatrix=true;
    this->display_matrix = display_matrix;
}
awd_float64*
AWDFrameCommand::get_display_matrix()
{
	return this->display_matrix;
}
void
AWDFrameCommand::set_color_matrix(awd_float64* color_matrix)
{
	this->hasColorMatrix=true;
    this->color_matrix = color_matrix;
}
awd_float64*
AWDFrameCommand::get_color_matrix()
{
	return this->color_matrix;
}
string&
AWDFrameCommand::get_instanceName()
{
	return this->instanceName;
}
void
AWDFrameCommand::set_instanceName(string& instanceName)
{
    this->instanceName = instanceName;
}
void
AWDFrameCommand::set_visible(bool visible)
{
	this->hasVisiblitiyChange=true;
	this->visible = visible;
}
bool
AWDFrameCommand::get_visible()
{
	return this->visible;
}
void
AWDFrameCommand::set_place_after_objectID(awd_uint32 place_after_objectID)
{
	this->hasDepthChange=true;
    this->place_after_objectID = place_after_objectID;
}
awd_uint32
AWDFrameCommand::get_place_after_objectID()
{
	return this->place_after_objectID;
}
awd_uint32
AWDFrameCommand::get_objID()
{
	return this->objID;
}
void
AWDFrameCommand::set_objID(awd_uint32 objID)
{
    this->objID = objID;
}
void
AWDFrameCommand::set_object_block(AWDBlock * object_block)
{
    this->object_block = object_block;
}
AWDNumAttrList *
AWDFrameCommand::get_command_properties()
{
    return this->command_properties;
}
AWD_Frame_Command_type
AWDFrameCommand::get_command_type()
{
    return this->command_type;
}
void
AWDFrameCommand::set_command_type(AWD_Frame_Command_type command_type)
{
    this->command_type=command_type;
}



void
AWDFrameCommand::prepare_and_add_dependencies(AWDBlockList *export_list)
{
	if (this->object_block!=NULL) {		
		if(this->object_block->get_type()==TEXT_ELEMENT){
			AWDTextElement* thisText = (AWDTextElement*)this->object_block;
			if(thisText!=NULL){
				if(thisText->get_isLocalized()){
					this->command_type=AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT;
				}
			}
		}
		if(this->command_type!=AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT){
			this->object_block->prepare_and_add_with_dependencies(export_list);
		}
		if(this->object_block->get_type()==SHAPE2D_GEOM){
			AWDShape2D* thisShape = (AWDShape2D*)this->object_block;
			if(thisShape!=NULL){
				AWDSubShape2D *sub = thisShape->get_first_sub();
				while (sub) {
					AWDBlock * thisFill=sub->get_fill();
					if(thisFill!=NULL){
						//thisFill->prepare_and_add_with_dependencies(export_list);
					}
					sub = sub->next;
				}
			}
		}
    }
}

awd_uint32
AWDFrameCommand::calc_command_length(BlockSettings * blockSettings)
{
    int len;
    len = sizeof(awd_uint16); // command_type
	if((this->command_type==AWD_FRAME_COMMAND_UPDATE_OBJECT)||(this->command_type==AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT)){
		
		len+=sizeof(bool);// add vs update
		len+=sizeof(bool);// no display matrix / display matrix
		len+=sizeof(bool);// no color matrix / color matrix
		len+=sizeof(bool);// no depth-change / depth-change
		len+=sizeof(bool);// no change filter / change filter
		len+=sizeof(bool);// no change visible / change visible
		len+=sizeof(bool);// no change BlendMode / change BlendMode
		len+=sizeof(bool);// no change clip-depth / change clip-depth

		len+=sizeof(awd_uint32);// objectID
		if(hasRessource){
			if(this->command_type==AWD_FRAME_COMMAND_UPDATE_OBJECT){
				len+=sizeof(awd_uint32);
			}
			else if(this->command_type==AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT){
				len+=sizeof(awd_uint32);
				AWDTextElement* thisText = (AWDTextElement*)this->object_block;
				if(thisText!=NULL){
					len += sizeof(awd_uint16) +  awd_uint16(thisText->get_name().size());//
				}
				else{
					len+=sizeof(awd_uint16);
				}
			}
		}
		if(hasDisplayMatrix){
			len+=sizeof(awd_float32)*6;
		}
		if(hasColorMatrix){
			len+=sizeof(awd_float32)*20;
		}
		if(hasDepthChange){
			len+=sizeof(awd_uint32);
		}
		if(hasFilterChange){
			len+=0;// no to do
		}
		if(hasBlendModeChange){
			len+=sizeof(awd_uint8);/// no to do
		}
		if(hasDepthClipChange){
			len+=sizeof(awd_uint32);// no to do
		}
		if(hasVisiblitiyChange){
			len+=sizeof(bool);
		}
		
		len+=sizeof(awd_uint16);// num shapes
		AWDBlock * thisRefObj = this->object_block;
		if(thisRefObj!=NULL){
			if(thisRefObj->get_type()==SHAPE2D_GEOM){
				AWDShape2D* thisShape = (AWDShape2D*)this->object_block;
				if(thisShape!=NULL){
					//len+=thisShape->get_num_subs()*sizeof(awd_uint32);
				}
			}
		}
		if(this->get_instanceName().size()!=0){
			len += sizeof(awd_uint16) +  awd_uint16(this->get_instanceName().size());//
		}
		else{
			len += sizeof(awd_uint16);
		}
	
	}
	else if(this->command_type==AWD_FRAME_COMMAND_SOUND){
		len+=sizeof(awd_uint32);// objectID
		len+=sizeof(awd_uint32);// resID
	}
		
	else{
		len+=sizeof(awd_uint32);// objectID
	}
    //len += this->command_properties->calc_length(blockSettings); 
    return len;
}

void
AWDFrameCommand::write_command(AWDFileWriter * fileWriter, BlockSettings * blockSettings)
{
	fileWriter->writeUINT16(this->command_type);// command type
	if((this->command_type==AWD_FRAME_COMMAND_UPDATE_OBJECT)||(this->command_type==AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT)){
		
		fileWriter->writeBOOL(hasRessource);
		fileWriter->writeBOOL(hasDisplayMatrix);
		fileWriter->writeBOOL(hasColorMatrix);
		fileWriter->writeBOOL(hasDepthChange);
		fileWriter->writeBOOL(hasFilterChange);
		fileWriter->writeBOOL(hasBlendModeChange);
		fileWriter->writeBOOL(hasDepthClipChange);
		fileWriter->writeBOOL(hasVisiblitiyChange);
		
		fileWriter->writeUINT32(this->objID);
		if(hasRessource){
			if(this->command_type==AWD_FRAME_COMMAND_UPDATE_OBJECT){
				fileWriter->writeUINT32(this->object_block->get_addr());
			}
			else if(this->command_type==AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT){
				AWDTextElement* thisText = (AWDTextElement*)this->object_block;
				if(thisText!=NULL){
					fileWriter->writeSTRING(thisText->get_name(), 1);
				}
				else{
					fileWriter->writeUINT16(0); // no id found (error)
				}
			}
		}
		if(hasDisplayMatrix){
			int i=0;
			for(i=0; i<6;i++){
				fileWriter->writeFLOAT32(this->display_matrix[i]);
			}
		}
		if(hasColorMatrix){
			int i=0;
			for(i=0; i<20;i++){
				fileWriter->writeFLOAT32(this->color_matrix[i]);
			}
		}
		if(hasDepthChange){
			fileWriter->writeUINT32(this->place_after_objectID);
		}
		if(hasFilterChange){
			//TODO:ADD FILTER
		}
		if(hasBlendModeChange){
			fileWriter->writeUINT8(this->blendMode);
		}
		if(hasDepthClipChange){
			fileWriter->writeUINT32(this->clip_depth);
		}
		if(hasVisiblitiyChange){
			fileWriter->writeBOOL(this->visible);
		}
		
		AWDBlock * thisRefObj = this->object_block;
		if(thisRefObj!=NULL){
			if(thisRefObj->get_type()==SHAPE2D_GEOM){
				AWDShape2D* thisShape = (AWDShape2D*)this->object_block;
				if(false){//thisShape!=NULL){
					fileWriter->writeUINT16(thisShape->get_num_subs()); // num fills (subShape)
					AWDSubShape2D *sub = thisShape->get_first_sub();
					while (sub) {
						AWDBlock * thisFill=sub->get_fill();
						if(thisFill!=NULL){
							fileWriter->writeUINT32(thisFill->get_addr());// fill id
						}
						else{
							fileWriter->writeUINT32(0); // num fills (subShape)
						}
						sub = sub->next;
					}
				}
				else{
					fileWriter->writeUINT16(0); // num fills (subShape)
				}
			}
			// this is no shape (it is timeline or sound or something)
			else{
				fileWriter->writeUINT16(0); // num fills (subShape)
			}
		}
		else{//ERROR OCCURED, write 0 for subshapes 
			fileWriter->writeUINT16(0); // num fills (subShape)
		}
		if(this->instanceName.size()>0){
			string instanceName2 = instanceName;
			fileWriter->writeSTRING(instanceName2, 1);
		}
		else{
			fileWriter->writeUINT16(0); // num fills (subShape)
		}
	}
	else if(this->command_type==AWD_FRAME_COMMAND_SOUND){
		fileWriter->writeUINT32(this->objID);
		fileWriter->writeUINT32(this->object_block->get_addr());
	}
		
	else{
		fileWriter->writeUINT32(this->objID);
	}	
    //this->command_properties->write_attributes(fileWriter, blockSettings);
}



AWDTimeLineFrame::AWDTimeLineFrame(string& name) :
    AWDNamedElement(name),
    AWDAttrElement()
{
	this->frame_duration=0;
    this->command_properties=new AWDNumAttrList();
}

AWDTimeLineFrame::~AWDTimeLineFrame()
{
    delete this->command_properties;
	for (AWDFrameCommand* fc :  this->commands){
		delete fc;
	}
}

awd_uint32
AWDTimeLineFrame::get_frame_duration()
{
	return this->frame_duration;
}

void
AWDTimeLineFrame::set_frame_duration(awd_uint32 frame_duration)
{
	this->frame_duration=frame_duration;
}
void
AWDTimeLineFrame::add_label(AWD_Frame_Label_type label_type, string& label)
{
	this->label_types.push_back(label_type);
	this->labels.push_back(label);
}

vector<AWDFrameCommand*>
AWDTimeLineFrame::get_commands()
{
	return this->commands;
}
void
AWDTimeLineFrame::set_frame_code(string& frame_code)
{
	this->frame_code=this->frame_code+frame_code;
}

string&
AWDTimeLineFrame::get_frame_code()
{
    return this->frame_code;
}
void
AWDTimeLineFrame::add_command(AWDFrameCommand* newFrame)
{
	this->commands.push_back(newFrame);
}

AWDFrameCommand*
AWDTimeLineFrame::get_command(awd_uint32 objectID)
{
	for (AWDFrameCommand* fc :  this->commands){
		if((fc->get_command_type()==AWD_FRAME_COMMAND_UPDATE_OBJECT)&&
			(fc->get_objID()==objectID)){
				return fc;
		}
	}
	
	AWDFrameCommand* newFrameCommand=  new AWDFrameCommand(objectID);
	newFrameCommand->set_command_type(AWD_FRAME_COMMAND_UPDATE_OBJECT);
	this->commands.push_back(newFrameCommand);
	return newFrameCommand;
}

void
AWDTimeLineFrame::prepare_and_add_dependencies(AWDBlockList *export_list)
{
	for (AWDFrameCommand * f : this->commands) 
	{
		f->prepare_and_add_dependencies(export_list);
	}

}

awd_uint32
AWDTimeLineFrame::calc_frame_length(BlockSettings * blockSettings)
{
    int len;
    len = sizeof(awd_uint32); // frame duration
	len += sizeof(awd_uint16);//labels-cnt
	for (string s : this->labels) 
	{
		len += sizeof(awd_uint8);
		len += sizeof(awd_uint16) + s.size();
	}

	len += sizeof(awd_uint16);// number of frame commands

	for (AWDFrameCommand * f : this->commands) 
	{
		len+=f->calc_command_length(blockSettings);
	}
	
    len += sizeof(awd_uint32) +  awd_uint16(this->frame_code.size()); 

    len += this->calc_attr_length(false,true, blockSettings); 
    
	return len;
}

void
AWDTimeLineFrame::write_frame(AWDFileWriter * fileWriter, BlockSettings * blockSettings)
{
	fileWriter->writeUINT32(1);// frameduration (whole frames / default = 1)
	fileWriter->writeUINT16(this->labels.size());// frameduration (whole frames / default = 1)
	int labelCnt=0;
	for (string s : this->labels) 
	{
		awd_uint8 labelType=this->label_types[labelCnt];
		fileWriter->writeUINT8(labelType);
		labelCnt++;
		fileWriter->writeSTRING(s, 1);
	}
	fileWriter->writeUINT16(this->commands.size());// num of frames	
	int cmdCnt=this->commands.size()-1;
	while(cmdCnt>=0){
		AWDFrameCommand * f = this->commands[cmdCnt]; 
		f->write_command(fileWriter, blockSettings);
		cmdCnt--;
	}
	fileWriter->writeSTRING(this->get_frame_code(), 2);// frame code	
	this->user_attributes->write_attributes(fileWriter, blockSettings);
}


AWDShape2DTimeline::AWDShape2DTimeline(string& name) :
    AWDBlock(SHAPE2D_TIMELINE),
    AWDNamedElement(name),
    AWDAttrElement()
{
	
	this->frames.clear();//all frames
    this->is_processed=false;
}
AWDShape2DTimeline::~AWDShape2DTimeline()
{
	for (AWDTimeLineFrame * f : this->frames) 
	{
		delete f;
	}
}

void
AWDShape2DTimeline::add_frame(AWDTimeLineFrame* newFrame)
{
	this->frames.push_back(newFrame);
}

AWDTimeLineFrame*
AWDShape2DTimeline::get_frame()
{
    return this->frames.back();
}

vector<AWDTimeLineFrame*>
AWDShape2DTimeline::get_frames()
{
	return this->frames;
}
bool
AWDShape2DTimeline::get_is_processed()
{
    return this->is_processed;
}

void
AWDShape2DTimeline::set_is_processed(bool isProcessed)
{
    this->is_processed = isProcessed;
}

bool
AWDShape2DTimeline::get_is_scene()
{
    return this->is_scene;
}

void
AWDShape2DTimeline::set_is_scene(bool is_scene)
{
    this->is_scene = is_scene;
}


awd_uint32
AWDShape2DTimeline::calc_body_length(BlockSettings *curBlockSettings)
{
    if(!this->get_isValid())
        return 0;

    awd_uint32 len;

    len = sizeof(awd_uint16) +  awd_uint16(this->get_name().size());//name
	len += sizeof(awd_bool);// is scene
	len += sizeof(awd_uint8);// sceneID
    len += sizeof(awd_uint16);// num of frames
	for (AWDTimeLineFrame * f : this->frames) 
	{
		len+=f->calc_frame_length(curBlockSettings);
	}
    len += this->calc_attr_length(true,true, curBlockSettings);
	
    return len;
}

void
AWDShape2DTimeline::prepare_and_add_dependencies(AWDBlockList *export_list)

{
	int fps=12;//TODO: DO NOT HARDCODE THIS
	int duration = 60000 / fps;
	vector<AWDTimeLineFrame*> newFramesList;
	int extraduration=0;
	AWDTimeLineFrame* lastTimeLine=NULL;
	for (AWDTimeLineFrame * f : this->frames) 
	{
		f->set_frame_duration(duration+extraduration);
		extraduration=0;
		if(f->get_commands().size()>0){
			newFramesList.push_back(f);
			lastTimeLine=f;
		}
		else{			
			if(lastTimeLine!=NULL){
				lastTimeLine->set_frame_duration(f->get_frame_duration()+lastTimeLine->get_frame_duration());
			}
			else{
				extraduration=f->get_frame_duration();
			}
			delete f;
		}
	}
	this->frames.clear();
	for (AWDTimeLineFrame * f : newFramesList) 
	{
		this->frames.push_back(f);
		f->prepare_and_add_dependencies(export_list);
	}
}
void
AWDShape2DTimeline::write_body(AWDFileWriter * fileWriter, BlockSettings *curBlockSettings)
{
	fileWriter->writeSTRING(this->get_name(), 1);// name	
	fileWriter->writeBOOL(this->is_scene);// is scene
	fileWriter->writeUINT8(0);// sceneID //TODO
	fileWriter->writeUINT16(awd_uint16(this->frames.size()));// num of frames	
	for (AWDTimeLineFrame * f : this->frames) 
	{
		f->write_frame(fileWriter, curBlockSettings);
	}
    this->properties->write_attributes(fileWriter, curBlockSettings);
    this->user_attributes->write_attributes(fileWriter, curBlockSettings);
}
