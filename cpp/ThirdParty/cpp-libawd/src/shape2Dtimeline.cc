#include "util.h"
#include "shape2Dtimeline.h"

#include <stdlib.h>
#include <cstdio>
#include <string>
using namespace std;
//#include <crtdbg.h>


AWDFrameCommandBase::AWDFrameCommandBase() 
{
    this->command_properties=new AWDNumAttrList();
	this->command_type = AWD_FRAME_COMMAND_UPDATE_OBJECT;
	this->hasRessource=false;// if true than the command will be "add" not "update"
	this->objID = -1;
	this->object_block = NULL;
	this->hasRessource=false;// if true than the command will be "add" not "update"
}

AWDFrameCommandBase::~AWDFrameCommandBase()
{
    delete this->command_properties;
	this->object_block = NULL;
}

void
AWDFrameCommandBase::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    // Does nothing by default. Can be optionally
    // overriden by sub-classes to take any actions
    // that need to happen before length is calculated
}
double
AWDFrameCommandBase::compare_to_command(AWDFrameCommandBase * )
{
    // Does nothing by default. Can be optionally
    // overriden by sub-classes to take any actions
    // that need to happen before length is calculated
	return 0;
}
void
AWDFrameCommandBase::update_by_command(AWDFrameCommandBase *)
{
    // Does nothing by default. Can be optionally
    // overriden by sub-classes to take any actions
    // that need to happen before length is calculated
}
void
AWDFrameCommandBase::write_command(AWDFileWriter * ,  BlockSettings *)
{
    // Does nothing by default. Can be optionally
    // overriden by sub-classes to take any actions
    // that need to happen before length is calculated
}
awd_uint32
AWDFrameCommandBase::calc_command_length(BlockSettings * blockSettings)
{
	return 0;
}
string&
AWDFrameCommandBase::get_objectType()
{
	return this->objectType;
}
void
AWDFrameCommandBase::set_objectType(string& objectType)
{
    this->objectType = objectType;
}
string&
AWDFrameCommandBase::get_resID()
{
	return this->resID;
}
void
AWDFrameCommandBase::set_resID(string& new_res)
{
	this->hasRessource=true;
    this->resID = new_res;
}
awd_uint32
AWDFrameCommandBase::get_objID()
{
	return this->objID;
}
void
AWDFrameCommandBase::set_objID(awd_uint32 objID)
{
    this->objID = objID;
}
void
AWDFrameCommandBase::set_object_block(AWDBlock * object_block)
{
    this->object_block = object_block;
}
AWDBlock*
AWDFrameCommandBase::get_object_block()
{
    return this->object_block;
}
AWDNumAttrList *
AWDFrameCommandBase::get_command_properties()
{
    return this->command_properties;
}
AWD_Frame_Command_type
AWDFrameCommandBase::get_command_type()
{
    return this->command_type;
}
void
AWDFrameCommandBase::set_layerID(awd_uint32 layerID)
{
	this->layerID = layerID;
}
awd_uint32
AWDFrameCommandBase::get_layerID()
{
	return this->layerID;
}
void
AWDFrameCommandBase::set_command_type(AWD_Frame_Command_type command_type)
{
    this->command_type=command_type;
}
bool
AWDFrameCommandBase::get_hasRessource()
{
    return this->hasRessource;
}
void
AWDFrameCommandBase::set_hasRessource(bool hasRessource)
{
    this->hasRessource=hasRessource;
}

AWDFrameCommandDisplayObject::AWDFrameCommandDisplayObject() :
	AWDFrameCommandBase()
{
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

AWDFrameCommandDisplayObject::~AWDFrameCommandDisplayObject()
{
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
bool
AWDFrameCommandDisplayObject::get_hasDisplayMatrix(){
	return this->hasDisplayMatrix;
}
bool
AWDFrameCommandDisplayObject::get_hasColorMatrix(){
	return this->hasColorMatrix;
}
bool
AWDFrameCommandDisplayObject::get_hasDepthChange(){
	return this->hasDepthChange;
}
bool
AWDFrameCommandDisplayObject::get_hasFilterChange(){
	return this->hasFilterChange;
}
bool
AWDFrameCommandDisplayObject::get_hasBlendModeChange(){
	return this->hasBlendModeChange;
}
bool
AWDFrameCommandDisplayObject::get_hasDepthClipChange(){
	return this->hasDepthClipChange;
}
bool
AWDFrameCommandDisplayObject::get_hasVisiblitiyChange(){
	return this->hasVisiblitiyChange;
}
void
AWDFrameCommandDisplayObject::set_display_matrix(awd_float64* display_matrix)
{
	this->hasDisplayMatrix=true;
    this->display_matrix = display_matrix;
}
awd_float64*
AWDFrameCommandDisplayObject::get_display_matrix()
{
	return this->display_matrix;
}
void
AWDFrameCommandDisplayObject::set_color_matrix(awd_float64* color_matrix)
{
	this->hasColorMatrix=true;
    this->color_matrix = color_matrix;
}
awd_float64*
AWDFrameCommandDisplayObject::get_color_matrix()
{
	return this->color_matrix;
}
string&
AWDFrameCommandDisplayObject::get_instanceName()
{
	return this->instanceName;
}
void
AWDFrameCommandDisplayObject::set_instanceName(string& instanceName)
{
    this->instanceName = instanceName;
}
void
AWDFrameCommandDisplayObject::set_visible(bool visible)
{
	this->hasVisiblitiyChange=true;
	this->visible = visible;
}
bool
AWDFrameCommandDisplayObject::get_visible()
{
	return this->visible;
}
int
AWDFrameCommandDisplayObject::get_blendmode()
{
	return this->blendMode;
}
void
AWDFrameCommandDisplayObject::set_blendmode(int blendMode)
{
	this->blendMode=blendMode;
}
void
AWDFrameCommandDisplayObject::set_clipDepth(awd_uint32 clipDepth)
{
	this->hasDepthClipChange=true;
	this->clip_depth = clipDepth;
}
awd_uint32
AWDFrameCommandDisplayObject::get_clipDepth()
{
	return this->clip_depth;
}
void
AWDFrameCommandDisplayObject::set_depth(awd_uint32 depth)
{
	this->hasDepthChange=true;
    this->depth = depth;
}
awd_uint32
AWDFrameCommandDisplayObject::get_depth()
{
	return this->depth;
}
double
AWDFrameCommandDisplayObject::compareColorMatrix(awd_float64* color_matrix)
{
	int countvalid=0;
	for(int i=0; i<20;i++){
		if(this->color_matrix[i]==color_matrix[i]){
			countvalid++;
		}
	}

    return countvalid/20;
}
double
AWDFrameCommandDisplayObject::comparedisplaMatrix(awd_float64* display_matrix)
{
	int countvalid=0;
	for(int i=0; i<6;i++){
		if(this->display_matrix[i]==display_matrix[i]){
			countvalid++;
		}
	}

    return countvalid/6;
}

double
	AWDFrameCommandDisplayObject::compare_to_command(AWDFrameCommandBase* frameCommand)
{
	AWDFrameCommandDisplayObject* thisFC=(AWDFrameCommandDisplayObject*)frameCommand;
	if(thisFC==NULL){
		return 0;
	}
	if(this->get_objectType()!=thisFC->get_objectType()){
		return 0;
	}
	if(this->get_command_type()!=AWD_FRAME_COMMAND_UPDATE_OBJECT){
		return 0;
	}
	if(this->get_object_block()!=thisFC->get_object_block()){
		return 0;
	}
	// the commands 
	int equal=1;
	int equalCnt=1;

	if((this->hasColorMatrix==thisFC->get_hasColorMatrix())&&(this->hasColorMatrix)){
		equal+=this->compareColorMatrix(thisFC->get_color_matrix());}	
	else if((this->hasColorMatrix==thisFC->get_hasColorMatrix())&&(!this->hasColorMatrix)){
		equal+=1;}	
	equalCnt++;

	if((this->hasDisplayMatrix==thisFC->get_hasDisplayMatrix())&&(this->hasDisplayMatrix)){
		equal+=this->comparedisplaMatrix(thisFC->get_display_matrix());}
	else if((this->hasDisplayMatrix==thisFC->get_hasDisplayMatrix())&&(!this->hasDisplayMatrix)){
		equal+=1;}	
	equalCnt++;
	
	if((this->hasDepthChange==thisFC->get_hasDepthChange())&&(this->hasDepthChange)){
		if(this->get_depth()==thisFC->get_depth()){
			equal+=1;
		}	
	}
	else if((this->hasDepthChange==thisFC->get_hasDepthChange())&&(!this->hasDepthChange)){
		equal+=1;}	
	equalCnt++;

	if((this->hasVisiblitiyChange==thisFC->get_hasVisiblitiyChange())&&(this->hasVisiblitiyChange)){
		if(this->get_visible()==thisFC->get_visible()){
			equal+=1;
		}	
	}
	else if((this->hasVisiblitiyChange==thisFC->get_hasVisiblitiyChange())&&(!this->hasVisiblitiyChange)){
		equal+=1;}	
	equalCnt++;

	if(this->get_instanceName()==thisFC->get_instanceName()){
		equal+=1;
	}	
	equalCnt++;
	
	double returnEqual=double(equal)/double(equalCnt);
	return returnEqual;
}
void
AWDFrameCommandDisplayObject::update_by_command(AWDFrameCommandBase* frameCommand)
{
	int equal=0;
}


void
AWDFrameCommandDisplayObject::prepare_and_add_dependencies(AWDBlockList *export_list)
{
	if (this->get_object_block()!=NULL) {		
		if(this->get_object_block()->get_type()==TEXT_ELEMENT){
			AWDTextElement* thisText = (AWDTextElement*)this->get_object_block();
			if(thisText!=NULL){
				if(thisText->get_isLocalized()){
					this->set_command_type(AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT);
				}
			}
		}
		if(this->get_command_type()!=AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT){
			this->get_object_block()->prepare_and_add_with_dependencies(export_list);
		}
		if(this->get_object_block()->get_type()==SHAPE2D_GEOM){
			AWDShape2D* thisShape = (AWDShape2D*)this->get_object_block();
			if(thisShape!=NULL){
				AWDSubShape2D *sub = thisShape->get_first_sub();
				while (sub) {
					AWDBlock * thisFill=sub->get_fill();
					if(thisFill!=NULL){
						thisFill->prepare_and_add_with_dependencies(export_list);
					}
					sub = sub->next;
				}
			}
		}
    }
}

awd_uint32
AWDFrameCommandDisplayObject::calc_command_length(BlockSettings * blockSettings)
{
    int len;
    len = sizeof(awd_uint16); // command_type
	if((this->get_command_type()==AWD_FRAME_COMMAND_UPDATE_OBJECT)||(this->get_command_type()==AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT)){
		
		len+=sizeof(bool);// add vs update
		len+=sizeof(bool);// no display matrix / display matrix
		len+=sizeof(bool);// no color matrix / color matrix
		len+=sizeof(bool);// no depth-change / depth-change
		len+=sizeof(bool);// no change filter / change filter
		len+=sizeof(bool);// no change visible / change visible
		len+=sizeof(bool);// no change BlendMode / change BlendMode
		len+=sizeof(bool);// no change clip-depth / change clip-depth

		len+=sizeof(awd_uint32);// objectID
		if(this->get_hasRessource()){
			if(this->get_command_type()==AWD_FRAME_COMMAND_UPDATE_OBJECT){
				len+=sizeof(awd_uint32);
			}
			else if(this->get_command_type()==AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT){
				len+=sizeof(awd_uint32);
				AWDTextElement* thisText = (AWDTextElement*)this->get_object_block();
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
		AWDBlock * thisRefObj = this->get_object_block();
		if(thisRefObj!=NULL){
			if(thisRefObj->get_type()==SHAPE2D_GEOM){
				AWDShape2D* thisShape = (AWDShape2D*)this->get_object_block();
				if(thisShape!=NULL){
					len+=thisShape->get_num_subs()*sizeof(awd_uint32);
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
	else if(this->get_command_type()==AWD_FRAME_COMMAND_SOUND){
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
AWDFrameCommandDisplayObject::write_command(AWDFileWriter * fileWriter, BlockSettings * blockSettings)
{
	fileWriter->writeUINT16(this->get_command_type());// command type
	if((this->get_command_type()==AWD_FRAME_COMMAND_UPDATE_OBJECT)||(this->get_command_type()==AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT)){
		
		fileWriter->writeBOOL(this->get_hasRessource());
		fileWriter->writeBOOL(hasDisplayMatrix);
		fileWriter->writeBOOL(hasColorMatrix);
		fileWriter->writeBOOL(hasDepthChange);
		fileWriter->writeBOOL(hasFilterChange);
		fileWriter->writeBOOL(hasBlendModeChange);
		fileWriter->writeBOOL(hasDepthClipChange);
		fileWriter->writeBOOL(hasVisiblitiyChange);
		
		fileWriter->writeUINT32(this->get_objID());
		if(this->get_hasRessource()){
			if(this->get_command_type()==AWD_FRAME_COMMAND_UPDATE_OBJECT){
				fileWriter->writeUINT32(this->get_object_block()->get_addr());
			}
			else if(this->get_command_type()==AWD_FRAME_COMMAND_UPDATE_LOCALIZEDTEXT){
				AWDTextElement* thisText = (AWDTextElement*)this->get_object_block();
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
			fileWriter->writeUINT32(this->depth);
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
		
		AWDBlock * thisRefObj = this->get_object_block();
		if(thisRefObj!=NULL){
			if(thisRefObj->get_type()==SHAPE2D_GEOM){
				AWDShape2D* thisShape = (AWDShape2D*)this->get_object_block();
				if(thisShape!=NULL){
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
	else if(this->get_command_type()==AWD_FRAME_COMMAND_SOUND){
		fileWriter->writeUINT32(this->get_objID());
		fileWriter->writeUINT32(this->get_object_block()->get_addr());
	}
		
	else{
		fileWriter->writeUINT32(this->get_objID());
	}	
    //this->command_properties->write_attributes(fileWriter, blockSettings);
}



AWDTimeLineFrame::AWDTimeLineFrame() :
    AWDAttrElement()
{
	this->frame_duration=0;
    this->command_properties=new AWDNumAttrList();
}

AWDTimeLineFrame::~AWDTimeLineFrame()
{
    delete this->command_properties;
	for (AWDFrameCommandBase* fc :  this->commands){
		//delete fc;
	}
}

void
AWDTimeLineFrame::add_dirty_layer(int newDirtyLayer)
{
	for(int i : this->dirty_layer_idx){
		if(i==newDirtyLayer){
			return;
		}
	}
	this->dirty_layer_idx.push_back(newDirtyLayer);
}

vector<int>
AWDTimeLineFrame::get_dirty_layers()
{
	return this->dirty_layer_idx;
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

vector<AWDFrameCommandBase*>
AWDTimeLineFrame::get_inActivecommands()
{
	return this->inActivecommands;
}
vector<AWDFrameCommandBase*>
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
AWDTimeLineFrame::add_command(AWDFrameCommandBase* newFrame)
{
	this->commands.push_back(newFrame);
}
void
AWDTimeLineFrame::add_inActivecommand(AWDFrameCommandBase* newFrame)
{
	this->inActivecommands.push_back(newFrame);
}

AWDFrameCommandBase*
AWDTimeLineFrame::get_command(awd_uint32 objectID)
{
	for (AWDFrameCommandBase* fc :  this->commands){
		if((fc->get_command_type()==AWD_FRAME_COMMAND_UPDATE_OBJECT)&&
			(fc->get_objID()==objectID)){
				return fc;
		}
	}
	
	AWDFrameCommandDisplayObject* newFrameCommand=  new AWDFrameCommandDisplayObject();
	newFrameCommand->set_objID(objectID);
	newFrameCommand->set_command_type(AWD_FRAME_COMMAND_UPDATE_OBJECT);
	this->commands.push_back(newFrameCommand);
	return newFrameCommand;
}

void
AWDTimeLineFrame::prepare_and_add_dependencies(AWDBlockList *export_list)
{
	for (AWDFrameCommandBase * f : this->commands) 
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

	for (AWDFrameCommandBase * f : this->commands) 
	{
		len+=f->calc_command_length(blockSettings);
	}
	
    len += sizeof(awd_uint32) +  awd_uint16(this->frame_code.size()); 

    len += this->calc_attr_length(false,true, blockSettings); 
    
	return len;
}

void
AWDTimeLineFrame::clear_commands()
{
	this->commands.clear();
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
	for(int cmdCnt=0; cmdCnt<this->commands.size(); cmdCnt++){
		AWDFrameCommandBase * f = this->commands[cmdCnt]; 
		f->write_command(fileWriter, blockSettings);
		//cmdCnt--;
	}
	fileWriter->writeSTRING(this->get_frame_code(), 2);// frame code	
	this->user_attributes->write_attributes(fileWriter, blockSettings);
}


AWDShape2DTimeline::AWDShape2DTimeline(string& name) :
    AWDBlock(SHAPE2D_TIMELINE),
    AWDNamedElement(name),
    AWDAttrElement()
{
	
}
AWDShape2DTimeline::~AWDShape2DTimeline()
{
	for (AWDTimeLineFrame * f : this->frames) 
	{
		delete f;
	}
}

void
AWDShape2DTimeline::add_frame(AWDTimeLineFrame* newFrame, bool modifyCommands)
{
	if(modifyCommands){
		if(this->frames.size()>0){
			vector<int> activeLayers=newFrame->get_dirty_layers();
			vector<AWDFrameCommandBase*> newRemoveCommands;
			vector<AWDFrameCommandBase*> newAddCommands;
			vector<AWDFrameCommandBase*> newUpdateCommands;
			vector<AWDFrameCommandBase*> newCommands=newFrame->get_commands();
			vector<AWDFrameCommandBase*> allOldCommands;
			vector<AWDFrameCommandBase*> oldCommands=this->frames.back()->get_commands();
			vector<AWDFrameCommandBase*> oldInActiveCommands=this->frames.back()->get_inActivecommands();
			for(AWDFrameCommandBase* oldCommand: oldCommands){
				allOldCommands.push_back(oldCommand);
			}
			for(AWDFrameCommandBase* oldInActiveCommand: oldInActiveCommands){
				allOldCommands.push_back(oldInActiveCommand);
			}
			for(AWDFrameCommandBase* oldCmd: allOldCommands){
				int thisLayerID=oldCmd->get_layerID();
				bool isActive=false;
				for(int i:activeLayers){
					if(i==thisLayerID){
						isActive=true;
						break;
					}
				}
				if(isActive){
					// check if one of the new cmds is a update command
					double equal=0;
					double testEqual=0;
					int cmdIdx=-1;
					int cmdCnt=0;
					for (AWDFrameCommandBase* newCmd:newCommands){
						if(newCmd->get_hasRessource()){
							// we only need to check if this is still a add-object-command
							testEqual=oldCmd->compare_to_command(newCmd);
							if(equal<testEqual){
								equal=testEqual;
								cmdIdx=cmdCnt;
							}
							cmdCnt++;
						}
					}
					if(cmdIdx<0){
						// no connected command found for this Command. So we need to create a remove command for it!
						reset_ressourceBlockUsageForID(oldCmd->get_objID()-1);
						AWDFrameCommandDisplayObject* newremoveCommand=new AWDFrameCommandDisplayObject();
						newremoveCommand->set_command_type(AWD_FRAME_COMMAND_REMOVE_OBJECT);
						newremoveCommand->set_objID(oldCmd->get_objID());
						newRemoveCommands.push_back(newremoveCommand);
					}
					else{
						// found a connected command. - if equal is 1, there is no change at all,
						// we put the Command in the inactiveCommands-list, because we still need it on next frame.
						if(equal==1){
							newCommands[cmdIdx]->set_hasRessource(false);
						}
						// found a connected command. - if equal is smaller than 1, there was a change
						// we transform the object into a update command
						else{
							newCommands[cmdIdx]->update_by_command(oldCmd);
							newCommands[cmdIdx]->set_objID(oldCmd->get_objID());
							newUpdateCommands.push_back(newCommands[cmdIdx]);
							newCommands[cmdIdx]->set_hasRessource(false);
						}
					}
				}
				else{
					newFrame->add_inActivecommand(oldCmd);
				}
			}
			for(AWDFrameCommandBase* newCmd: newCommands){
				if(newCmd->get_hasRessource()){
					//if the cmd is still a add command, we get a obj id for it
					newCmd->set_objID(this->get_ressourceBlockID(newCmd->get_object_block()));
					newAddCommands.push_back(newCmd);
				}
			}
			newFrame->clear_commands();
			for(AWDFrameCommandBase* newCmd: newRemoveCommands){
				newFrame->add_command(newCmd);
			}
			for(AWDFrameCommandBase* newCmd: newUpdateCommands){
				newFrame->add_command(newCmd);
			}
			for(AWDFrameCommandBase* newCmd: newAddCommands){
				newFrame->add_command(newCmd);
			}

		}
		else{
			vector<AWDFrameCommandBase*> newCommands=newFrame->get_commands();
			for(AWDFrameCommandBase* newCmd: newCommands){
				if(newCmd->get_hasRessource()){
					//if the cmd is still a add command, we get a obj id for it
					newCmd->set_objID(this->get_ressourceBlockID(newCmd->get_object_block()));
				}
			}
		}
	}
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

int
AWDShape2DTimeline::get_scene_id()
{
    return this->scene_id;
}

void
AWDShape2DTimeline::set_scene_id(int scene_id)
{
    this->scene_id = scene_id;
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
AWDShape2DTimeline::reset_ressourceBlockUsage()
{
	for(bool thisusage: usedReccourseBlocksUsage){
		thisusage=false;
	}
}
void 
AWDShape2DTimeline::reset_ressourceBlockUsageForID(int thisID)
{
	usedReccourseBlocksUsage[thisID]=false;
}
int 
AWDShape2DTimeline::get_ressourceBlockID(AWDBlock* resBlock)
{
	int resCnt=0;
	for(AWDBlock* thisBlock: usedRessourceBlocks){
		if((resBlock==thisBlock)&&(!usedReccourseBlocksUsage[resCnt])){
			usedReccourseBlocksUsage[resCnt]=true;
			return resCnt;
		}
		resCnt++;
	}
	usedReccourseBlocksUsage.push_back(true);
	usedRessourceBlocks.push_back(resBlock);
	return usedRessourceBlocks.size();

}
void
AWDShape2DTimeline::prepare_and_add_dependencies(AWDBlockList *export_list)

{
	// get rid of frame that have no duration
	vector<AWDTimeLineFrame*> newFramesList;
	AWDTimeLineFrame* lastTimeLine=NULL;
	for (AWDTimeLineFrame * f : this->frames) 
	{
		if(f->get_commands().size()>0){
			newFramesList.push_back(f);
			lastTimeLine=f;
		}
		else{			
			if(lastTimeLine!=NULL){
				lastTimeLine->set_frame_duration(f->get_frame_duration()+lastTimeLine->get_frame_duration());
			}
			delete f;
		}
	}
	this->frames.clear();
	for (AWDTimeLineFrame * f : newFramesList) 
	{
		f->prepare_and_add_dependencies(export_list);
		this->frames.push_back(f);
	}
}
void
AWDShape2DTimeline::write_body(AWDFileWriter * fileWriter, BlockSettings *curBlockSettings)
{
	fileWriter->writeSTRING(this->get_name(), 1);// name	
	fileWriter->writeUINT8(this->scene_id);// is scene
	fileWriter->writeUINT8(0);// sceneID //TODO
	fileWriter->writeUINT16(awd_uint16(this->frames.size()));// num of frames	
	for (AWDTimeLineFrame * f : this->frames) 
	{
		f->write_frame(fileWriter, curBlockSettings);
	}
    this->properties->write_attributes(fileWriter, curBlockSettings);
    this->user_attributes->write_attributes(fileWriter, curBlockSettings);
}
