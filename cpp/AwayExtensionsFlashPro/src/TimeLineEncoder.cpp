/*
#ifdef _WINDOWS
#include <Windows.h>
#include "ShellApi.h"
#include "ShellApi.h"
#endif
#include <vector>
#include <fstream>
#include "Utils.h"

#include "FlashFCMPublicIDs.h"

#include "Service/Shape/IRegionGeneratorService.h"
#include "Service/Shape/IFilledRegion.h"
#include "Service/Shape/IStrokeGroup.h"
#include "Service/Shape/IPath.h"
#include "Service/Shape/IEdge.h"
#include "Service/Shape/IShapeService.h"
#include "Service/Image/IBitmapExportService.h"

*/
#include "ILayer.h"
#include "IFrame.h"
#include "Utils/DOMTypes.h"
#include "FCMPluginInterface.h"

#include "IFilterable.h"
#include "FrameElement/IClassicText.h"
#include "FrameElement/IParagraph.h"
#include "ITween.h"
#include "FrameElement/ITextRun.h"
#include "FrameElement/ITextStyle.h"
#include "FrameElement/ITextBehaviour.h"
#include "FrameElement/IShape.h"
#include "FrameElement/ISound.h"
#include "FrameElement/IInstance.h"
#include "FrameElement/IGraphic.h"
#include "FrameElement/IGroup.h"
#include "FrameElement/IBitmapInstance.h"
#include "FrameElement/IMovieClip.h"

#include "MediaInfo/IBitmapInfo.h"
#include "MediaInfo/ISoundInfo.h"

#include "Utils.h"
#include "Publisher.h"

#include "TimelineEncoder.h"
#include "FlashFCMPublicIDs.h"

#include "blocks/mesh_library.h"
#include "blocks/billboard.h"

TimelineEncoder::TimelineEncoder(FCM::PIFCMCallback pCallback, AWDProject* awd, FlashToAWDEncoder* flash_to_awd)
{
    this->m_pCallback=pCallback;
	this->flash_to_awd=flash_to_awd;
	this->awd=awd;
	this->curFrame=NULL;
	
	this->layerIdx=0;	
	

}

TimelineEncoder::~TimelineEncoder()
{
}

BLOCKS::Timeline* 
TimelineEncoder::get_awd_timeLine(){
	return this->awd_timeline;
}


#include "StdAfx.h"
using namespace IceCore;
#define SORTER	RadixSort

AWD::result
TimelineEncoder::encode(DOM::ITimeline* timeline, BLOCKS::Timeline* awd_timeline){
	
	this->timeline=timeline;
	this->awd_timeline = awd_timeline;
	
	SORTER RS_segs;
	this->all_timeline_data.clear();
    FCM::Result res;
	int duration = 1;
	bool isDirty=false;
	if(this->awd_timeline->get_process_state()==AWD::TYPES::process_state::PROCESSED){
		//AwayJS::Utils::Trace(this->m_pCallback, "Timeline is already parsed!!!\n");
	}
	else if(this->awd_timeline->get_process_state()==AWD::TYPES::process_state::PROCESS_ACTIV){
		AwayJS::Utils::Trace(this->m_pCallback, "ERROR: Timeline references itself. should never happen!!!\n");
	}
	else{
		this->awd_timeline->set_process_state(AWD::TYPES::process_state::PROCESS_ACTIV);
		FCM::U_Int32 maxFrameCount;
		res=this->timeline->GetMaxFrameCount(maxFrameCount);
		
		FCM::FCMListPtr pLayerList;
		this->timeline->GetLayers(pLayerList.m_Ptr);
		FCM::U_Int32 layerCount;
		pLayerList->Count(layerCount);
		
		

		this->layer_mask_skeleton.clear();

		int lc=layerCount;
		while(lc--){
			FCM::AutoPtr<DOM::ILayer> layer = pLayerList[lc];
			get_layer_skeleton(layer, -2);
		}
		int full_layer_cnt=this->layer_mask_skeleton.size();



		this->cur_layer_data.clear();
		this->all_durations.clear();
		std::vector<int> layer_max_obj_cnts;
		lc=full_layer_cnt;
		while(lc--)
			layer_max_obj_cnts.push_back(0);	
		//AwayJS::Utils::Trace(this->m_pCallback, "Parse TimeLine '%s', sceneID: %d, duration: %d, layers: %d\n", this->awd_timeline->get_name().c_str(), this->awd_timeline->get_scene_id(), maxFrameCount, layerCount);
		for(int i=0; i<maxFrameCount;i++){
			this->is_dirty=false;
			this->layerIdx=0;
			
			this->cur_layer_data.clear();

			this->mask_group=0;

			lc=full_layer_cnt;
			while(lc--){
				this->cur_layer_data.push_back(new LayerData());	
			}

			lc=layerCount;
			while(lc--){
				FCM::AutoPtr<DOM::ILayer> layer = pLayerList[lc];
				this->collectFrameCommands(i, layer);
			}


			// if any layers of this frame has been changed, or if this is the first frame, we consider it a new frame.

			if((this->is_dirty)||((i==0))){
				
				this->all_timeline_data.push_back(std::vector<LayerData*>());		
				this->all_durations.push_back(duration);
				lc=full_layer_cnt;
				while(lc--){
					this->all_timeline_data.back().push_back(new LayerData());	
				}
				int obj_cnt=0;
				lc=0;
				// for each layer we order the commands by depth
				for(LayerData* layer_data:this->cur_layer_data){
					//AwayJS::Utils::Trace(this->m_pCallback, "DIRTY FRAME cmds: %d\n", layer_cmds.size());
					if(layer_data->isDirty){
						this->all_timeline_data.back()[lc]->frame_script = layer_data->frame_script;
						this->all_timeline_data.back()[lc]->frame_label = layer_data->frame_label;
						this->all_timeline_data.back()[lc]->isDirty=true;
						if(layer_data->display_commands.size()>0){
							int cmd_cnt=0;
							udword* InputValues_depth = new udword[ layer_data->display_commands.size()];
							for(ANIM::FrameCommandDisplayObject* f_cmd:layer_data->display_commands){
								InputValues_depth[cmd_cnt++]=(f_cmd->get_depth()*100)+f_cmd->found_nr;
							}
							const udword* Sorted = RS_segs.Sort(InputValues_depth, layer_data->display_commands.size(), RADIX_UNSIGNED).GetRanks();
							int sorted_cnt=0;
							std::string new_infos="";
							for(sorted_cnt = 0; sorted_cnt<layer_data->display_commands.size(); sorted_cnt++){
								ANIM::FrameCommandDisplayObject* f_cmd=layer_data->display_commands[Sorted[sorted_cnt]];
								f_cmd->target_mask_id=this->layer_mask_skeleton[lc];
								this->all_timeline_data.back()[lc]->display_commands.push_back(f_cmd);	
								unsigned int test = InputValues_depth[Sorted[sorted_cnt]];
								new_infos+= " | "+std::to_string(f_cmd->get_depth());
								test=test;
							}
							//AwayJS::Utils::Trace(this->m_pCallback, "new_objects_sorted = %s\n", new_infos.c_str());
							DELETEARRAY(InputValues_depth);
							if(layer_max_obj_cnts[lc]<layer_data->display_commands.size()){
								layer_max_obj_cnts[lc]=layer_data->display_commands.size();
							}
						}
						if(layer_data->audio_commands.size()>0){
							//todo
						}
					}
					lc++;
				}
			}
			// if no layers of this frame has been changed, we add to the duration of the last frame.
			else
				this->all_durations.back() += duration;
		}
		
		// at this point we have a list of frameCommand per layer.
		// all commands are add commands
		// we want to prepare them in a way that they are the same as the adobe frame commands
		int frame_cnt=0;
		int layer_cnt=0;
		int start_depth=0;
		int obj_id_cnt=0;
		std::vector<int> min_id_layer;
		std::vector<int> max_id_layer;
		std::vector<ANIM::FrameCommandDisplayObject*> last_frame_children;
		for(std::vector<LayerData*> frame_cmds:this->all_timeline_data){
			layer_cnt=frame_cmds.size();
			start_depth=0;
			AWD::ANIM::TimelineFrame* new_frame=new AWD::ANIM::TimelineFrame();
			new_frame->set_frame_duration(TYPES::UINT32(this->all_durations[frame_cnt]));
			while (layer_cnt--){
				LayerData* layer_data=frame_cmds[layer_cnt];
				new_frame->set_frame_code(layer_data->frame_script);
				if(layer_data->frame_label!="")
					new_frame->add_label(AWD::ANIM::frame_label_type::AWD_FRAME_LABEL_NAME, layer_data->frame_label);
				if(layer_data->isDirty){
					std::vector<ANIM::FrameCommandRemoveObject*> new_remove_commands;
					std::vector<ANIM::FrameCommandDisplayObject*> new_display_commands;
					if(frame_cnt>0){
						bool has_prev_layer= false;
						int framesearch=frame_cnt-1;
						LayerData* prev_frame_layer_data;
						// we need to get the prev frame of this layer. because of other layers, there might be empty frames inbetween. search for the first frame that is dirty.
						while((has_prev_layer==false)&&(framesearch>=0)){
							prev_frame_layer_data=this->all_timeline_data[framesearch][layer_cnt];
							if(prev_frame_layer_data->isDirty)
								has_prev_layer=true;
							framesearch--;
						}

						// group commands by object they use. FrameCommandGroups is still splittet in prev_frame and this_frame
						std::map<int, FrameCommandGroup> object_command_map;
						for(ANIM::FrameCommandDisplayObject* f_cmd:prev_frame_layer_data->display_commands){
							if(object_command_map.find(f_cmd->get_object_block()->inc_id)==object_command_map.end())
								object_command_map[f_cmd->get_object_block()->inc_id]=FrameCommandGroup();
							object_command_map[f_cmd->get_object_block()->inc_id].pre_frame.push_back(f_cmd);
						}
						for(ANIM::FrameCommandDisplayObject* f_cmd:layer_data->display_commands){
							if(object_command_map.find(f_cmd->get_object_block()->inc_id)==object_command_map.end())
								object_command_map[f_cmd->get_object_block()->inc_id]=FrameCommandGroup();
							object_command_map[f_cmd->get_object_block()->inc_id].cur_frame.push_back(f_cmd);
						}
						
						// for each of the groups, we want to check if the object is used both in prev-frame and cur-frame.
						// if it is used in both, and one ore both frames contains multiple instances of the same object, we need to map the objects from frame to frame.
						// goal is to find the pairs, that are most similair (less properties to update)
						for (std::map<int,FrameCommandGroup>::iterator it=object_command_map.begin(); it!=object_command_map.end(); ++it){
							if((it->second.pre_frame.size()>0)&&(it->second.cur_frame.size()==0)){
								// prev_frame is not empty, but this frame is. remove all.
								for(ANIM::FrameCommandDisplayObject* f_cmd_a:it->second.pre_frame){
									ANIM::FrameCommandRemoveObject* new_remove_cmd = new ANIM::FrameCommandRemoveObject();
									new_remove_cmd->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_REMOVE);
									new_remove_cmd->set_objID(f_cmd_a->get_objID());
									new_remove_commands.push_back(new_remove_cmd);
								 }
							}
							else if((it->second.pre_frame.size()==0)&&(it->second.cur_frame.size()>0)){
								// prev_frame is empty, all commands are add commands
								for(ANIM::FrameCommandDisplayObject* f_cmd_a:it->second.cur_frame){
									//new_frame->add_command(f_cmd_a);
									f_cmd_a->set_objID(obj_id_cnt++);
									new_display_commands.push_back(f_cmd_a);
								}
							}
							else if((it->second.pre_frame.size()==1)&&(it->second.cur_frame.size()==1)){
								int compare_result=it->second.pre_frame[0]->compare_to_command(it->second.cur_frame[0])*1000;
								if(compare_result==1000){
									//new_commands.push_back(it->second.cur_frame[0]);
									ANIM::FrameCommandDisplayObject* new_exists_cmd = new ANIM::FrameCommandDisplayObject();
									new_exists_cmd->set_command_type(ANIM::frame_command_type::AWD_FRAME_COMMAND_OBJECT_EXISTS);
									new_exists_cmd->set_objID(it->second.pre_frame[0]->get_objID());
									new_exists_cmd->set_object_block(it->second.pre_frame[0]->get_object_block());
									new_display_commands.push_back(new_exists_cmd);
								}
								else{
									it->second.cur_frame[0]->update_by_command(it->second.pre_frame[0]);
									//new_frame->add_command(it->second.cur_frame[0]);
									new_display_commands.push_back(it->second.cur_frame[0]);
								}
							}
							// multiple command for same object existed in both frames. most tricky case
							else if((it->second.pre_frame.size()>0)&&(it->second.cur_frame.size()>0)){
								std::vector<ANIM::FrameCommandDisplayObject*> compare_a;
								std::vector<ANIM::FrameCommandDisplayObject*> compare_b;
								// create all possible pairs of frame_commands, and sort them by equalness;
								udword* InputValues_compare = new udword[ it->second.cur_frame.size() * it->second.pre_frame.size()];
								int cnt_comp=0;
								for(ANIM::FrameCommandDisplayObject* f_cmd_a:it->second.cur_frame){
									f_cmd_a->isUsed=false;
									for(ANIM::FrameCommandDisplayObject* f_cmd_b:it->second.pre_frame){
										f_cmd_b->isUsed=false;
										compare_a.push_back(f_cmd_a);
										compare_b.push_back(f_cmd_b);
										InputValues_compare[cnt_comp++]=f_cmd_a->compare_to_command(f_cmd_b)*1000;
									}
								}
								const udword* Sorted = RS_segs.Sort(InputValues_compare, cnt_comp, RADIX_UNSIGNED).GetRanks();
								int sorted_cnt=0;
								int sorted_idx=0;
								int added_cmd_cnt=0;
								int check_up_to=0;
								for(sorted_cnt = 0; sorted_cnt<cnt_comp; sorted_cnt++){
									sorted_idx=Sorted[sorted_cnt];
									if((compare_a[sorted_idx]->isUsed)||(compare_b[sorted_idx]->isUsed))
										continue;
									compare_a[sorted_idx]->isUsed=true;
									compare_b[sorted_idx]->isUsed=true;
									if(InputValues_compare[sorted_idx]==1000){
										//added_cmd_cnt++;
										//ANIM::FrameCommandDisplayObject* new_exists_cmd = new ANIM::FrameCommandDisplayObject();
										//new_exists_cmd->set_command_type(ANIM::frame_command_type::AWD_FRAME_COMMAND_OBJECT_EXISTS);
										//new_display_commands.push_back(compare_a[sorted_idx]);
										ANIM::FrameCommandDisplayObject* new_exists_cmd = new ANIM::FrameCommandDisplayObject();
										new_exists_cmd->set_command_type(ANIM::frame_command_type::AWD_FRAME_COMMAND_OBJECT_EXISTS);
										new_exists_cmd->set_object_block(compare_b[sorted_idx]->get_object_block());
										new_exists_cmd->set_objID(compare_b[sorted_idx]->get_objID());
										new_display_commands.push_back(new_exists_cmd);
									}
									else{
										added_cmd_cnt++;
										compare_a[sorted_idx]->update_by_command(compare_b[sorted_idx]);
										//new_frame->add_command(compare_a[sorted_idx]);
										new_display_commands.push_back(compare_a[sorted_idx]);
									}
									if(added_cmd_cnt>=it->second.cur_frame.size()){
										check_up_to=sorted_cnt;
										break;
									}
								}
								for(sorted_cnt = check_up_to; sorted_cnt<cnt_comp; sorted_cnt++){
									sorted_idx=Sorted[sorted_cnt];
									if(it->second.pre_frame.size()>it->second.cur_frame.size()){
										if(compare_b[sorted_idx]->isUsed)
											continue;
										compare_b[sorted_idx]->isUsed=true;
										ANIM::FrameCommandRemoveObject* new_remove_cmd = new ANIM::FrameCommandRemoveObject();
										new_remove_cmd->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_REMOVE);
										new_remove_cmd->set_objID(compare_b[sorted_idx]->get_objID());
										new_remove_commands.push_back(new_remove_cmd);
									}
									else if(it->second.cur_frame.size()>it->second.pre_frame.size()){
										if(compare_a[sorted_idx]->isUsed)
											continue;
										compare_a[sorted_idx]->isUsed=true;
										compare_a[sorted_idx]->set_objID(obj_id_cnt++);
										new_display_commands.push_back(compare_a[sorted_idx]);

									}
								}
							}
							else if((it->second.pre_frame.size()==0)&&(it->second.cur_frame.size()==0)){
								// this should never happen
							}
						}
						if(new_remove_commands.size()>0){
							int cnt_comp=0;
							udword* InputValues_compare = new udword[ new_remove_commands.size()];
							for(ANIM::FrameCommandRemoveObject* f_cmd:new_remove_commands)
								InputValues_compare[cnt_comp++]=f_cmd->get_depth();

							const udword* Sorted = RS_segs.Sort(InputValues_compare, cnt_comp, RADIX_UNSIGNED).GetRanks();
							for(int sorted_cnt = 0; sorted_cnt<cnt_comp; sorted_cnt++){
								new_frame->add_command(new_remove_commands[Sorted[sorted_cnt]]);
							}
						}
						std::vector<ANIM::FrameCommandDisplayObject*> new_new_display_cmds;
						if(new_display_commands.size()>0){
							int cnt_comp=0;
							udword* InputValues_compare = new udword[ new_display_commands.size()];
							for(ANIM::FrameCommandDisplayObject* f_cmd:new_display_commands)
								InputValues_compare[cnt_comp++]=f_cmd->get_depth();

							const udword* Sorted = RS_segs.Sort(InputValues_compare, cnt_comp, RADIX_UNSIGNED).GetRanks();
							int prevframe_idx=0;
							int this_frame_idx=0;
							for(int sorted_cnt = 0; sorted_cnt<cnt_comp; sorted_cnt++){
								ANIM::FrameCommandDisplayObject* one_cmd = new_display_commands[Sorted[sorted_cnt]];
								if(one_cmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE){
									// todo: check if the order is consistent with previous frame.
									int new_old_idx=0;
									for(ANIM::FrameCommandDisplayObject* one_prev_cmt:prev_frame_layer_data->display_commands){
										if(one_prev_cmt->get_objID()==one_cmd->get_objID()){
											break;
										}
										new_old_idx++;
									}
									if(new_old_idx==prev_frame_layer_data->display_commands.size()){
										int error=0;
										one_cmd->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD);
									}
									else{
										if(new_old_idx>prevframe_idx){
											new_new_display_cmds.push_back(one_cmd);
										}
										else{
											ANIM::FrameCommandRemoveObject* new_remove_cmd = new ANIM::FrameCommandRemoveObject();
											new_remove_cmd->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_REMOVE);
											new_remove_cmd->set_objID(one_cmd->get_objID());
											new_frame->add_command(new_remove_cmd);
											one_cmd->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD);
										}
										prevframe_idx=new_old_idx;
									}
								}
								if(one_cmd->get_command_type()==ANIM::frame_command_type::AWD_FRAME_COMMAND_OBJECT_EXISTS)
									new_new_display_cmds.push_back(one_cmd);
								// if not, we need to add a remove object cmd, and add with new depth.
								else if(one_cmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD){
									one_cmd->set_objID(obj_id_cnt++);
									new_new_display_cmds.push_back(one_cmd);
								}
							}
							layer_data->display_commands.clear();
							for(ANIM::FrameCommandDisplayObject* f_cmd:new_new_display_cmds){
								new_frame->add_command(f_cmd);
								layer_data->display_commands.push_back(f_cmd);
								f_cmd->cur_depth = layer_data->display_commands.size();
							}
						}
					}
					else{
						for(ANIM::FrameCommandDisplayObject* f_cmd:layer_data->display_commands){
							new_frame->add_command(f_cmd);
							f_cmd->set_objID(obj_id_cnt++);
							f_cmd->cur_depth = obj_id_cnt;
						}
					}
				}
			}
			// all layers processed. all commands should have been added to the frame.
			// now lets reproduce the depth that is used by adobe-commands (placeAfterObjectWithID)

			ANIM::FrameCommandDisplayObject* last_cmd=NULL;
			for(ANIM::FrameCommandBase* b_cmd:new_frame->get_commands()){
				ANIM::FrameCommandDisplayObject* f_cmd= reinterpret_cast<ANIM::FrameCommandDisplayObject*>(b_cmd);
				if(f_cmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD){
					if(last_cmd!=NULL){
						f_cmd->set_depth(last_cmd->get_depth());
					}
					else{
						f_cmd->set_depth(0);
					}
				}
				last_cmd=f_cmd;
				/*
				int mask_id=f_cmd->target_mask_id;
				if(mask_id==-1){
					f_cmd->mask_ids.push_back(0);
					f_cmd->set_hasTargetMaskIDs(true);
				}
				else if(mask_id>=0){
					int startidx=min_id_layer[min_id_layer.size()-1-f_cmd->target_mask_id];
					int endidx=max_id_layer[min_id_layer.size()-1-f_cmd->target_mask_id];
					while(startidx<=endidx){
						f_cmd->mask_ids.push_back(startidx);
						startidx++;
					}
					f_cmd->set_hasTargetMaskIDs(true);
				}
				*/
			}
			this->awd_timeline->add_frame(new_frame);
			frame_cnt++;
		}

		this->awd_timeline->set_process_state(AWD::TYPES::process_state::PROCESSED);
	}
	return result::AWD_SUCCESS;
}

void
TimelineEncoder::get_layer_skeleton(DOM::ILayer* iLayer, int isMasked)
{
	//  this is excuted per frame and per layer. starting with first frame, and bottom layer.
	//	if isMasked is true, the Display-Objects found on this layer will be considered to be masked by the objects with 
	
    FCM::Result res;
	
	// check visibility for layer.
	FCM::Boolean isVisible;
	iLayer->IsVisible(isVisible);
	
	FCM::AutoPtr<DOM::Layer::ILayerFolder> pLayerFolder;
	FCM::AutoPtr<DOM::Layer::ILayerMask> pLayerMask;
	FCM::AutoPtr<DOM::Layer::ILayerGuide> pLayerGuide;
	FCM::AutoPtr<DOM::Layer::ILayerNormal> pLayerNormal;

	// get the layer-type
	FCM::PIFCMUnknown pLayerType;
	res = iLayer->GetLayerType(pLayerType);
	if(res==FCM_SUCCESS){		
		
		//layer_mask_skeleton;
		//layer_object_ids;
		pLayerFolder=pLayerType;
		if(pLayerFolder){
			
			// this is a layer folder. it has no Display-Objects itself, but it can contain child layers of all layer-types.
			FCM::FCMListPtr pChildList;
			pLayerFolder->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			int ccount=childCount;			
			
			while(ccount--){
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->get_layer_skeleton(layer, -2);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Folder, masked=%\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);			
		}				
		pLayerMask=pLayerType;
		if(pLayerMask){
			
			// this is a layer mask. all objects found on this layer are considered to be objects that mask objects in the  child-layers of this layer.
			FCM::FCMListPtr pChildList;
			pLayerMask->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			int ccount=childCount;
			while(ccount--){
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->get_layer_skeleton(layer, (layer_mask_skeleton.size()+childCount));
			}
			isMasked=-1;
		}
		pLayerGuide=pLayerType;
		if(pLayerGuide){

			// this is a layer-guide - for now it is handled like a normal layer
			FCM::FCMListPtr pChildList;
			pLayerGuide->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			int ccount=childCount;
			while(ccount--){
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->get_layer_skeleton(layer, -2);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Guide, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);
		}
		pLayerNormal=pLayerType;
		if(pLayerNormal){	
			// this is a normal layer only consider if it is visible. todo: get setting to consider visible layers or not make this as settings.
			if((isVisible)||(this->awd->get_settings()->get_export_invisible_timeline_layer()))
				layer_mask_skeleton.push_back(isMasked);
			
		}
	}
};

#include "Utils/DOMTypes.h"
AWD::result
TimelineEncoder::collectFrameCommands(int frameIdx, DOM::ILayer* iLayer)
{
	//  this is excuted per frame and per layer. starting with first frame, and bottom layer.
	//	if isMasked is true, the Display-Objects found on this layer will be considered to be masked by the objects with 
	
    FCM::Result res;

	// get name of layer
	FCM::StringRep16 layerName16;;
	res = iLayer->GetName(&layerName16);

	// check visibility for layer.
	FCM::Boolean isVisible;
	iLayer->IsVisible(isVisible);
	
	FCM::AutoPtr<DOM::Layer::ILayerFolder> pLayerFolder;
	FCM::AutoPtr<DOM::Layer::ILayerMask> pLayerMask;
	FCM::AutoPtr<DOM::Layer::ILayerGuide> pLayerGuide;
	FCM::AutoPtr<DOM::Layer::ILayerNormal> pLayerNormal;

	// get the layer-type
	FCM::PIFCMUnknown pLayerType;
	res = iLayer->GetLayerType(pLayerType);
	if(res==FCM_SUCCESS){
		
		this->layer_obj_id_cnt=0;
		pLayerFolder=pLayerType;
		if(pLayerFolder){

			// this is a layer folder. it has no Display-Objects itself, but it can contain child layers of all layer-types.
			FCM::FCMListPtr pChildList;
			pLayerFolder->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			int ccount=childCount;
			while(ccount>0){
				ccount--;
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameCommands(frameIdx, layer);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Folder, masked=%\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);			
		}				
		pLayerMask=pLayerType;
		if(pLayerMask){
			
			// this is a layer mask. all objects found on this layer are considered to be objects that mask objects in the  child-layers of this layer.
			FCM::FCMListPtr pChildList;
			pLayerMask->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			int ccount=childCount;
			this->mask_group++;;
			while(ccount>0){
				ccount--;
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameCommands(frameIdx, layer);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Mask, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(),isMasked);
		}
		pLayerGuide=pLayerType;
		if(pLayerGuide){

			// this is a layer-guide - for now it is handled like a normal layer
			FCM::FCMListPtr pChildList;
			pLayerGuide->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			int ccount=childCount;
			while(ccount>0){
				ccount--;
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameCommands(frameIdx, layer);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Guide, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);
		}
		pLayerNormal=pLayerType;
		if(pLayerNormal){	

			// this is a normal layer only consider if it is visible. todo: get setting to consider visible layers or not make this as settings.
			if((isVisible)||(this->awd->get_settings()->get_export_invisible_timeline_layer())){
				FCM::U_Int32 totalDuration;
				pLayerNormal->GetTotalDuration(totalDuration);
				if(totalDuration>=frameIdx){
					
					//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Normal, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);	
					FCM::AutoPtr<DOM::IFrame> frame;
					pLayerNormal->GetFrameAtIndex(frameIdx, frame.m_Ptr);
					if(frame){
						FCM::AutoPtr<DOM::ITween> thisTween;
						frame->GetTween(thisTween.m_Ptr);
						if(thisTween){
							FCM::AutoPtr<IFCMDictionary> pDict;
							FCM::FCMDictRecTypeID link_type;
							thisTween->GetTweenedProperties(pDict.m_Ptr);
						}
						/*
						res = pDict->GetIID(kLibProp_LinkageClass_DictKey, link_type, valLen);
						if (FCM_SUCCESS_CODE(res))
						{
							FCM::StringRep8 script_name_8 = new char[valLen];
							res = pDict->Get(kLibProp_LinkageClass_DictKey, link_type, script_name_8, valLen);
							ASSERT(FCM_SUCCESS_CODE(res));
							script_name+=script_name_8;
							delete []script_name_8;
						}
						*/

						FCM::U_Int32 startFrame;
						frame->GetStartFrameIndex(startFrame);
						bool export_shape_tweens=false;
						if((startFrame==frameIdx)||(export_shape_tweens)){
							this->is_dirty=true;
							this->cur_layer_data[this->layerIdx]->isDirty=true;

							FCM::U_Int32 frameDuration;
							frame->GetDuration(frameDuration);

							//AwayJS::Utils::Trace(this->m_pCallback, "				duration: %d\n", frameDuration);
							DOM::KeyFrameLabelType thisLabelType;
							frame->GetLabelType(thisLabelType);
							ANIM::frame_label_type thisAWDLabelType=ANIM::frame_label_type::AWD_FRAME_LABEL_NONE;
							//AwayJS::Utils::Trace(this->m_pCallback, "			thisLabelType: %d\n", thisLabelType);
							if(thisLabelType == 1){
							
								/*thisAWDLabelType=ANIM::frame_label_type::AWD_FRAME_LABEL_NAME;
							else if(thisLabelType == 2)//COMMENTS DONT SEEM TO GET EXPORTED BY IEXPORTERSERVICE
								thisAWDLabelType=ANIM::frame_label_type::AWD_FRAME_LABEL_COMMENT;
							else if(thisLabelType == 3)
								thisAWDLabelType=ANIM::frame_label_type::AWD_FRAME_LABEL_ANCOR;*/

								FCM::StringRep16 thisLabel;
								res=frame->GetLabel(&thisLabel);
								if(res==FCM_SUCCESS){
									if(thisLabel){
										//AwayJS::Utils::Trace(this->m_pCallback, "				found a Label: %s\n", AwayJS::Utils::ToString(thisLabel,this->m_pCallback).c_str());
                                        std::string thisLabel2=AwayJS::Utils::ToString(thisLabel,this->m_pCallback);										
										this->cur_layer_data[this->layerIdx]->frame_label = thisLabel2;
										//curFrame->add_label(thisAWDLabelType, thisLabel2);
									}
								}
							}
							FCM::StringRep16 thisScript;
							frame->GetScript(&thisScript);
							if(res==FCM_SUCCESS){
								if(thisScript){
									//AwayJS::Utils::Trace(this->m_pCallback, "				found a script: %s\n", AwayJS::Utils::ToString(thisScript,this->m_pCallback).c_str());
                                    std::string newFrameCode=AwayJS::Utils::ToString(thisScript,this->m_pCallback);
									this->cur_layer_data[this->layerIdx]->frame_script = newFrameCode;
								}
							}
							//AwayJS::Utils::Trace(this->m_pCallback, "				duration: %d\n", frameDuration);

							FCM::FCMListPtr allPrevObjects;

							if(startFrame!=0){
								pLayerNormal->GetFrameAtIndex(frameIdx, frame.m_Ptr);

							}
							// get all frame-elements and loop over them.

							FCM::FCMListPtr allFrameElements;
							frame->GetFrameElements(allFrameElements.m_Ptr);
							FCM::U_Int32 objCount;
							allFrameElements->Count(objCount);
							//int frameObjCnt=objCount;
							//while(frameObjCnt--){
							for(int frameObjCnt=0; frameObjCnt<objCount;frameObjCnt++){
								FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> frameDisplayElement = allFrameElements[frameObjCnt];
								if(frameDisplayElement){
									DOM::Utils::MATRIX2D groupMatrix=DOM::Utils::MATRIX2D();
									this->collectFrameDisplayElement(frameDisplayElement, groupMatrix, false, 0);
								}

								FCM::AutoPtr<DOM::FrameElement::ISound> soundElement = allFrameElements[frameObjCnt];
								
							}

						}
						
					}
					else{
						if(frameIdx==0){
							this->is_dirty=true;
							this->cur_layer_data[this->layerIdx]->isDirty=true;
						}
						else{
							if(this->cur_layer_data[this->layerIdx]->display_commands.size()>0){
								this->is_dirty=true;
								this->cur_layer_data[this->layerIdx]->isDirty=true;
							}
						}

					}
				}
				else{
					//AwayJS::Utils::Trace(this->m_pCallback, "		Layer has no Frames \n");	
				}
				this->layerIdx++;

			}
		}
	}
	return result::AWD_SUCCESS;
};

AWD::result
TimelineEncoder::collectFrameDisplayElement(DOM::FrameElement::IFrameDisplayElement* frameDisplayElement, DOM::Utils::MATRIX2D groupMatrix, bool group_member=false, int parent_depth=0)
{

	// To get all the properties for a Display-object, we need to cast it to different objects.
	
	// first get the common properties
	
	// available are:

	//	-	Matrix				(display-transform of the object)
	//	-	depth				(the z-position)
	//	-	pivot-point			(do we need it ?)
	//	-	bounds				(need it for textfields)
	//	-	Object-space-bounds	(we dont need it, so we dont get it)

	//	Get depth
	FCM::U_Int32 depth;
	FCM::Result res = frameDisplayElement->GetDepth(depth);
	if(res!=FCM_SUCCESS)
		bool error=true;

	//	Get Matrix
	DOM::Utils::MATRIX2D thisMatrix;
	frameDisplayElement->GetMatrix(thisMatrix);


	// If this

	// Get Pivot Point
	DOM::Utils::POINT2D pivotPoint;
	frameDisplayElement->GetPivot(pivotPoint);

	DOM::Utils::RECT bounds;
	frameDisplayElement->GetBounds(bounds);

	DOM::Utils::RECT objectSpaceBounds;
	frameDisplayElement->GetObjectSpaceBounds(objectSpaceBounds);

	// we will keep track of the object-type by storing the type as string.
	// if this string is still "undefined" at end of this function, we have a error. (unknown object)

	std::string thisType="undefined";
	
	//	Next we can distingish between a group and all other elements.
	//	If this is a group, the only other implemented properties are GroupMember.
	//	In this case, we will call this function for all the group-members, and we are done.
	//	No FrameCommadn will be created for a Group (The Group will be splittet into the GroupMembers)
	// \todo consider adding option that allows to export Group as (single-frame) Timeline

	FCM::AutoPtr<DOM::FrameElement::IGroup> group =  frameDisplayElement;
	if(group){
		thisType="group";
		FCM::FCMListPtr pGroupMembers;
		group->GetMembers(pGroupMembers.m_Ptr);
		FCM::U_Int32 groupCntCount;
		pGroupMembers->Count(groupCntCount);
		for(int i=0; i<groupCntCount; i++){
		//while(groupCntCount--){
			FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> groupItem = pGroupMembers[i];
			this->collectFrameDisplayElement(groupItem, groupMatrix, true, depth);
		}
	}	

	else{		
		//AwayJS::Utils::Trace(this->m_pCallback, "	ENCOUNTERED object\n!!");
		//	This was no Group. It must be a display-object coontaining actual data.
		// now we know that we must create a Frame-commadn for this object

		AWD::ANIM::FrameCommandDisplayObject* newFrameCommand=new AWD::ANIM::FrameCommandDisplayObject();
		
		//std::string resid="abc";
		//newFrameCommand->set_resID(resid);

		// set the index of the current layer. we need it later.
		newFrameCommand->set_layerID(this->layerIdx);
		
		// set has-ressource to true - we will merge commands later (has-ressource might becomme false again)
		newFrameCommand->set_hasRessource(true);

		// fill the command with the info that we have so far 
		newFrameCommand->set_display_matrix(this->flash_to_awd->convert_matrix2x3(thisMatrix));

		
		if(group_member){
			/*
				TYPES::F64* this_shape_mtx=this->flash_to_awd->convert_matrix2x3(groupMatrix);
				TYPES::F64* this_mesh_mtx=newFrameCommand->get_display_matrix()->get();
				TYPES::F64* new_mesh_mtx=(TYPES::F64*)malloc(6*8);
				TYPES::F64 a1 =  this_mesh_mtx[0];
				TYPES::F64 b1 =  this_mesh_mtx[1];
				TYPES::F64 c1 =  this_mesh_mtx[2];
				TYPES::F64 d1 =  this_mesh_mtx[3];
				TYPES::F64 tx1 =  this_mesh_mtx[4];
				TYPES::F64 ty1 =  this_mesh_mtx[5];

				TYPES::F64 a2 =  this_shape_mtx[0];
				TYPES::F64 b2 =  this_shape_mtx[1];
				TYPES::F64 c2 =  this_shape_mtx[2];
				TYPES::F64 d2 =  this_shape_mtx[3];
				TYPES::F64 tx2 =  this_shape_mtx[4];
				TYPES::F64 ty2 =  this_shape_mtx[5];
				new_mesh_mtx[0]= a1 * a2 + b1 * c2;
				new_mesh_mtx[1]= a1 * b2 + b1 * d2;

				new_mesh_mtx[2]= c1 * a2 + d1 * c2;
				new_mesh_mtx[3]= c1 * b2 + d1 * d2;

				new_mesh_mtx[4]= tx1 * a2 + ty2 * c2 + tx2;
				new_mesh_mtx[5]= tx1 * b2 + ty2 * d2 + ty2;
									
				newFrameCommand->set_display_matrix(new_mesh_mtx);
			*/
			//	This object is a GroupMember of a Group.
			//	\todo	If we want to disolve groups on export, we need to convert the matrix from local-space to global-space (we got the parent matrix)
		}

		newFrameCommand->set_depth(depth+parent_depth);
		newFrameCommand->found_nr=  this->layer_obj_id_cnt++;
		

		// if this is a filterable object, we get the list of filter
		// a filterable object is not the final object-type, so we keep checking.
		// todo: filters is the most untested feature!!!

		FCM::AutoPtr<DOM::IFilterable> filterableObject = frameDisplayElement;
		if(filterableObject){
			FCM::FCMListPtr pFilterList;
			filterableObject->GetGraphicFilters(pFilterList.m_Ptr);
			FCM::U_Int32 pFilterListCount;
			pFilterList->Count(pFilterListCount);
			//AwayJS::Utils::Trace(this->m_pCallback, "					ISFILTERABLE %d\n", pFilterListCount);
			for(int gm=0; gm< pFilterListCount; gm++){
			}
		}
		
		//	we check if this is a instance object, and if it is an instance object, 
		//	we directly try to get the libraryItem for this instance object.
		//	this is done for all objects that are IInstance

		FCM::AutoPtr<DOM::FrameElement::IInstance> instanceObject = frameDisplayElement;
		if(instanceObject){
			//AwayJS::Utils::Trace(this->m_pCallback, "	ENCOUNTERED instanceObject\n");
			FCM::AutoPtr<DOM::ILibraryItem> libraryItem;
			instanceObject->GetLibraryItem(libraryItem.m_Ptr);
			if(libraryItem){	
				
				//AwayJS::Utils::Trace(this->m_pCallback, "	ENCOUNTERED libraryItem\n");
				
				FCM::StringRep16 libName;
				libraryItem->GetName(&libName);

				/*
				FCM::PIFCMDictionary thisProps;
				libraryItem->GetProperties(thisProps);
				FCM::U_Int32 propsCnt;
				thisProps->Count(propsCnt);				
				for(int pcnt=0; pcnt<propsCnt; pcnt++){
					FCM::StringRep8 propKey;
					FCM::FCMDictRecTypeID typeID;
					FCM::U_Int32 typeLength;
					thisProps->GetNth(pcnt, propKey, typeID, typeLength);
					//AwayJS::Utils::Trace(this->m_pCallback, "					propkey %s\n", propKey);
				}*/
				
				//	Try if this is a symbol instance.
				// if it is a symbol-instance, we can get this properties:
				//	-	ColorMatrix
				//	-	RegisrationPoint	(Not used)

				// still ISymbolInstance is not a final object-type, so we need to keep checking if it is movieclip or graphic.

				FCM::AutoPtr<DOM::FrameElement::ISymbolInstance> symbolInstance = frameDisplayElement;
				if(symbolInstance){
					
					//AwayJS::Utils::Trace(this->m_pCallback, "	ENCOUNTERED symbolInstance\n");
					DOM::Utils::COLOR_MATRIX colorMatrix;
					symbolInstance->GetColorMatrix(colorMatrix);
					newFrameCommand->set_color_matrix(this->flash_to_awd->convert_matrix4x5_to_colortransform(colorMatrix));
					DOM::Utils::POINT2D registrationPoint;
					symbolInstance->GetRegistrationPoint(registrationPoint);
					
					// try to get a ISymbolItem for this libraryItem. this is the class that will give use acces for the timeline of the movieclip or graphic
					FCM::AutoPtr<DOM::LibraryItem::ISymbolItem> symbolItem = libraryItem;
					if(symbolItem){
						
						FCM::StringRep16 symbol_name=NULL;
						libraryItem->GetName(&symbol_name);
						std::string symbol_name_str=AwayJS::Utils::ToString(symbol_name, this->m_pCallback);
						// get the timeline from the timeline_encoder, and set it as source for the frame-command
						BLOCKS::Timeline* this_timeline=awd->get_timeline_by_symbol_name(symbol_name_str);
						if(this_timeline==NULL){	
							this_timeline=new BLOCKS::Timeline();						
							this_timeline->set_fps(this->awd->get_settings()->get_fps());
							this_timeline->set_name(symbol_name_str);
							this_timeline->set_symbol_name(symbol_name_str);
							this->awd->add_block(this_timeline);
						}
						this_timeline->instance_cnt++;
						newFrameCommand->set_object_block(awd->get_timeline_by_symbol_name(symbol_name_str));
					}
					
					// check if this is a graphic. 
					FCM::AutoPtr<DOM::FrameElement::IGraphic> graphic =  frameDisplayElement;
					if(graphic){
						//	if it is a grafic. we have a final object-type reached.
						//	the timeline should already have been encodet.
						//	all thats left to get are the proeprties:
						//		-	AnimationLoopMode	(should the timeline loop)
						//		-	firstFrame			(the startframe of the animation 
						//								e.g. a timeline from 0-100 with loop = true and firstframe = 20, 
						//								 will continiously play from frame 20 - frame 100)

						thisType="graphic";
						//AwayJS::Utils::Trace(this->m_pCallback, "					GRAPHIC\n");	
						DOM::FrameElement::AnimationLoopMode loopMode;
						graphic->GetLoopMode(loopMode);	
						FCM::U_Int32 firstFrame;
						graphic->GetFirstFrameIndex(firstFrame);
					}
					
					// check if this is a movieclip.
					FCM::AutoPtr<DOM::FrameElement::IMovieClip> movieclip =  frameDisplayElement;
					if(movieclip){
						//	if it is a movieclip. we have a final object-type reached.
						//	the timeline should already have been encodet.
						//	all thats left to is getting this properties:
						//		-	thisBlendMode	(blendmode of movieclip)
						//		-	instanceName	instance_name of movieclip
						//		-	visiblitly		is the moviclip visible.


						thisType="movieclip";
						//AwayJS::Utils::Trace(this->m_pCallback, "					MOVIECLIP\n");		
						DOM::FrameElement::BlendMode thisBlendMode;
						movieclip->GetBlendMode(thisBlendMode);
						newFrameCommand->set_blendmode(thisBlendMode);

						FCM::StringRep16 mcName;
						movieclip->GetName(&mcName);
                        std::string instanceName=AwayJS::Utils::ToString(mcName, this->m_pCallback);

						newFrameCommand->set_instanceName(instanceName);

						FCM::Boolean isVisible;
						movieclip->IsVisible(isVisible);	
						newFrameCommand->set_visible(bool(isVisible));
					}
				}
				
				//	If this a libraryItem, but no symbolinstance, it should be a media_item
				// still ISymbolInstance is not a final object-type, so we need to keep checking if it is movieclip or graphic.

				FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item = libraryItem;
				if(media_item){
					//AwayJS::Utils::Trace(this->m_pCallback, "	ENCOUNTERED media_item\n");
					FCM::AutoPtr<DOM::FrameElement::IBitmapInstance> bitmap = frameDisplayElement;
					if(bitmap){
						//	if it is a bitmap. we have a final object-type reached.				
						thisType="bitmap";	
						
						//AwayJS::Utils::Trace(this->m_pCallback, "	ENCOUNTERED BITMAP\n");

						//	we call the flash_to_awd->ExportBitmap, and hopefully get back a AWDBlock for this bitmap.

						AWDBlock* new_bitmap; 
						this->flash_to_awd->ExportBitmap(media_item, &new_bitmap);		
						BLOCKS::Billboard* new_billboard=NULL; 
						if(new_bitmap){
							//AwayJS::Utils::Trace(this->m_pCallback, "	ENCOUNTERED BITMAP 2\n");
							BLOCKS::BitmapTexture* new_tex=reinterpret_cast<BLOCKS::BitmapTexture*>(new_bitmap);
							new_tex->add_scene_name(this->flash_to_awd->current_scene_name);
							// get the material-block for this color (create if does not exist)
							std::string matname = "mat_"+new_tex->get_name();
							BLOCKS::Material* new_fill_material=reinterpret_cast<BLOCKS::Material*>(this->flash_to_awd->get_project()->get_block_by_name_and_type(matname,  BLOCK::SIMPLE_MATERIAL, true));
							new_fill_material->add_scene_name(this->flash_to_awd->current_scene_name);
							new_fill_material->set_name(matname);
							new_fill_material->set_material_type(MATERIAL::type::TEXTURE_MATERIAL);
						//	new_fill_material->set_uv_transform_mtx(this->convert_matrix2x3(matrix));
							new_fill_material->set_texture(reinterpret_cast<BLOCKS::BitmapTexture*>(new_tex));
							std::string billboard_name = "billboard_"+new_tex->get_name();
							new_billboard = reinterpret_cast<BLOCKS::Billboard*>(this->awd->get_block_by_name_and_type(billboard_name, BLOCK::block_type::BILLBOARD, true));
							new_billboard->set_name("billboard_"+new_fill_material->get_texture()->get_name());
							new_billboard->set_external_id(new_fill_material->get_external_id());
							new_billboard->set_material(new_fill_material);
						}
						// set the bitmap as source for the frame-command
						newFrameCommand->set_object_block(new_billboard);

					}
				}
			}
		}

		//	if this is no instance-object, it must be a Classic-text, or a shape.

		// first check if it is text.

		FCM::AutoPtr<DOM::FrameElement::IClassicText> textField = frameDisplayElement;
		if(textField){
			//AwayJS::Utils::Trace(this->m_pCallback, "	ENCOUNTERED textField\n");
			//	if it is a textField. we have a final object-type reached.	
			thisType="textField";
			AWDBlock* new_textfield;
			std::string this_res_id=std::to_string(this->flash_to_awd->text_field_cnt);
			this->flash_to_awd->text_field_cnt++;
			AWD::result res = this->flash_to_awd->ExportText(textField, &new_textfield, this_res_id);
			if(res!=result::AWD_SUCCESS)
				return res;
			
			// set the textField as source for the frame-command
			newFrameCommand->set_object_block(new_textfield);

		}
		
		//	last but not least: check if the object is a shape.
		// if it is, we call the get_geom_for_shape() on the flash_to_awd class, and set it directly to the command.
		FCM::AutoPtr<DOM::FrameElement::IShape> shape = frameDisplayElement;
		if(shape){
			//AwayJS::Utils::Trace(this->m_pCallback, "	ENCOUNTERED shape\n");
			thisType="shape";
			this->flash_to_awd->shape_instance_cnt++;
			std::string geom_instance_name=std::to_string(this->flash_to_awd->shape_instance_cnt);
			BLOCKS::Geometry* this_geom = this->flash_to_awd->get_geom_for_shape(shape, geom_instance_name, false);
			GEOM::MATRIX2x3* this_shape_mtx2x3=this_geom->has_res_id_geom(geom_instance_name);
			if(this_shape_mtx2x3!=NULL){

				TYPES::F64* this_shape_mtx=newFrameCommand->get_display_matrix()->get();

				TYPES::F64* this_mesh_mtx= this_shape_mtx2x3->get();
				TYPES::F64 a1 =  this_mesh_mtx[0];
				TYPES::F64 b1 =  this_mesh_mtx[1];
				TYPES::F64 c1 =  this_mesh_mtx[2];
				TYPES::F64 d1 =  this_mesh_mtx[3];
				TYPES::F64 tx1 =  this_mesh_mtx[4];
				TYPES::F64 ty1 =  this_mesh_mtx[5];
				
				TYPES::F64 a_inv =  d1 / (a1*d1- b1*c1);
				TYPES::F64 b_inv =  -b1 / (a1*d1 - b1*c1);
				TYPES::F64 c_inv =  -c1 / (a1*d1 - b1*c1);
				TYPES::F64 d_inv =  a1 / (a1*d1 - b1*c1);
				TYPES::F64 tx_inv =  (c1*ty1 - d1*tx1)/(a1*d1 - b1*c1);
				TYPES::F64 ty_inv =  -(a1*ty1 - b1*tx1)/(a1*d1 - b1*c1);
				TYPES::F64* new_mesh_mtx=(TYPES::F64*)malloc(6*8);
								
				TYPES::F64 a2 =  this_shape_mtx[0];
				TYPES::F64 b2 =  this_shape_mtx[1];
				TYPES::F64 c2 =  this_shape_mtx[2];
				TYPES::F64 d2 =  this_shape_mtx[3];
				TYPES::F64 tx2 =  this_shape_mtx[4];
				TYPES::F64 ty2 =  this_shape_mtx[5];


				new_mesh_mtx[0]= a_inv * a2 + b_inv * c2;
				new_mesh_mtx[1]= a_inv * b2 + b_inv * d2;

				new_mesh_mtx[2]= c_inv * a2 + d_inv * c2;
				new_mesh_mtx[3]= c_inv * b2 + d_inv * d2;

				new_mesh_mtx[4]= tx_inv * a2 + ty_inv * c2 + tx2;
				new_mesh_mtx[5]= tx_inv * b2 + ty_inv * d2 + ty2;
									
				newFrameCommand->set_display_matrix(new_mesh_mtx);
			}
			newFrameCommand->set_object_block(this_geom->get_mesh_instance());
			//AwayJS::Utils::Trace(this->m_pCallback, "					SHAPE\n");	
		}

		// thats it. the object should have been converted into awd-data.
		// the thisType should have been filled
		// AwayJS::Utils::Trace(this->m_pCallback, "					-- found a %s on timeline %s\n", thisType.c_str(), this->awd_timeline->get_name().c_str());

		// now add the frame to the current output list:
		
		if(newFrameCommand->get_object_block()!=NULL)
			this->cur_layer_data[this->layerIdx]->display_commands.push_back(newFrameCommand);
		else{
			delete newFrameCommand;
		}
	}
	
	return result::AWD_SUCCESS;
};
