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

#include "TimelineEncoder.h"
#include "FlashFCMPublicIDs.h"



TimelineEncoder::TimelineEncoder(FCM::PIFCMCallback pCallback, DOM::ITimeline* timeline, AWDProject* awd, FlashToAWDEncoder* flash_to_awd, int sceneID)
{
    this->m_pCallback=pCallback;
	this->timeline=timeline;
	this->flash_to_awd=flash_to_awd;
	this->awd=awd;
	this->curFrame=NULL;


	FCM::StringRep16 thisName16;
	this->timeline->GetName(&thisName16);
    std::string this_name=AwayJS::Utils::ToString(thisName16, this->m_pCallback);

	this->awd_timeline = reinterpret_cast<BLOCKS::Timeline*>(awd->get_block_by_external_id_and_type(this_name, BLOCK::block_type::TIMELINE, true));
	this->awd_timeline->set_name(this_name);
	if(this->awd_timeline ==NULL)
		return;
	
	if(sceneID>=0){
		std::string sceneName = "Scene "+std::to_string(sceneID);
		this->awd_timeline->set_name(sceneName);
		this->awd_timeline->set_scene_id(sceneID);
	}
	this->layerIdx=0;	
	

}

TimelineEncoder::~TimelineEncoder()
{
}

BLOCKS::Timeline* 
TimelineEncoder::get_awd_timeLine(){
	return this->awd_timeline;
}

AWD::result
TimelineEncoder::encode(){
	
    FCM::Result res;
	int duration = 1;
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
	
		this->flash_to_awd->prepare_new_timeline();		
		this->flash_to_awd->prepare_new_frame();
	
		FCM::FCMListPtr pLayerList;
		this->timeline->GetLayers(pLayerList.m_Ptr);
		FCM::U_Int32 layerCount;
		pLayerList->Count(layerCount);
		//AwayJS::Utils::Trace(this->m_pCallback, "Parse TimeLine '%s', sceneID: %d, duration: %d, layers: %d\n", this->awd_timeline->get_name().c_str(), this->awd_timeline->get_scene_id(), maxFrameCount, layerCount);
		for(int i=0; i<maxFrameCount;i++){
			//AwayJS::Utils::Trace(this->m_pCallback, "Parse Frame nr: %d\n\n",i);
			this->mask_range_max=0;
			this->mask_range_min=0;
			this->curFrame=new AWD::ANIM::TimelineFrame();
			this->curFrame->set_frame_duration(duration);

			// run over layers from top to bottom = collect framescript and labels
			for(int lc=0; lc<layerCount;lc++){
				FCM::AutoPtr<DOM::ILayer> layer = pLayerList[lc];
				this->collectFrameScripts(i, layer);
			}

			int l=layerCount;
			this->layerIdx=0;	

			// run over layers from bottom to top = collect frameCommands and display objects
			while(l>0){
				l--;
				FCM::AutoPtr<DOM::ILayer> layer = pLayerList[l];
				this->collectFrameCommands(i, layer, false);
			}

			// if any layers of this frame has been changed, we add the frame to the timeline.

			if(this->curFrame->get_dirty_layers().size()>0){
				this->awd_timeline->add_frame(this->curFrame, true);
				this->flash_to_awd->prepare_new_frame();
			}
			// if no layers of this frame has been changed, we add to the duration of the last frame.
			else{
				if(this->awd_timeline->get_frame()){
					this->awd_timeline->get_frame()->set_frame_duration(this->awd_timeline->get_frame()->get_frame_duration()+duration);						
					this->flash_to_awd->prepare_new_frame();
				}
				//AwayJS::Utils::Trace(this->m_pCallback, "EMPTY FRAME\n");
				delete this->curFrame;
			}
		}
		// set the process state for this timeline to processed, so we dont process it again
		this->awd_timeline->set_process_state(AWD::TYPES::process_state::PROCESSED);
	}
	return result::AWD_SUCCESS;
}

AWD::result
TimelineEncoder::collectFrameScripts(int frameIdx, DOM::ILayer* iLayer)
{
	
    FCM::Result res;
	FCM::StringRep16 layerName16;
	res = iLayer->GetName(&layerName16);
	FCM::Boolean isVisible;
	iLayer->IsVisible(isVisible);
	
	FCM::AutoPtr<DOM::Layer::ILayerFolder> pLayerFolder;
	FCM::AutoPtr<DOM::Layer::ILayerMask> pLayerMask;
	FCM::AutoPtr<DOM::Layer::ILayerGuide> pLayerGuide;
	FCM::AutoPtr<DOM::Layer::ILayerNormal> pLayerNormal;

	FCM::PIFCMUnknown pLayerType;
	res = iLayer->GetLayerType(pLayerType);
	if(res==FCM_SUCCESS){
		pLayerNormal=pLayerType;
		if(pLayerNormal){
			if(isVisible){
				FCM::U_Int32 totalDuration;
				pLayerNormal->GetTotalDuration(totalDuration);
				if(totalDuration>=frameIdx){
					
					//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Folder\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str());	
					FCM::AutoPtr<DOM::IFrame> frame;
					pLayerNormal->GetFrameAtIndex(frameIdx, frame.m_Ptr);
					if(frame){
						FCM::U_Int32 startFrame;
						frame->GetStartFrameIndex(startFrame);
						if(startFrame!=frameIdx){
							//AwayJS::Utils::Trace(this->m_pCallback, "		Frame Should already be parsed\n");	
						}
						else{											
							FCM::U_Int32 frameDuration;
							frame->GetDuration(frameDuration);
							//AwayJS::Utils::Trace(this->m_pCallback, "				duration: %d\n", frameDuration);
							DOM::KeyFrameLabelType thisLabelType;
							frame->GetLabelType(thisLabelType);
							ANIM::frame_label_type thisAWDLabelType=ANIM::frame_label_type::AWD_FRAME_LABEL_NONE;
							//AwayJS::Utils::Trace(this->m_pCallback, "			thisLabelType: %d\n", thisLabelType);
							if(thisLabelType == 1)
								thisAWDLabelType=ANIM::frame_label_type::AWD_FRAME_LABEL_NAME;
							else if(thisLabelType == 2)//COMMENTS DONT SEAM TO GET EXPORTED BY IEXPORTERSERVICE
								thisAWDLabelType=ANIM::frame_label_type::AWD_FRAME_LABEL_COMMENT;
							else if(thisLabelType == 3)
								thisAWDLabelType=ANIM::frame_label_type::AWD_FRAME_LABEL_ANCOR;
							if(thisLabelType>0){
								FCM::StringRep16 thisLabel;
								res=frame->GetLabel(&thisLabel);
								if(res==FCM_SUCCESS){
									if(thisLabel){
										//AwayJS::Utils::Trace(this->m_pCallback, "				found a Label: %s\n", AwayJS::Utils::ToString(thisLabel,this->m_pCallback).c_str());
                                        std::string thisLabel2=AwayJS::Utils::ToString(thisLabel,this->m_pCallback);
										curFrame->add_label(thisAWDLabelType, thisLabel2);
									}
								}
							}
							FCM::StringRep16 thisScript;
							frame->GetScript(&thisScript);
							if(res==FCM_SUCCESS){
								if(thisScript){
									//AwayJS::Utils::Trace(this->m_pCallback, "				found a script: %s\n", AwayJS::Utils::ToString(thisScript,this->m_pCallback).c_str());
                                    std::string newFrameCode=AwayJS::Utils::ToString(thisScript,this->m_pCallback);
									curFrame->set_frame_code( newFrameCode);
								}
							}
						}						
					}
				}
				else{
					//AwayJS::Utils::Trace(this->m_pCallback, "		Layer has no Frames \n");	
				}

			}
		}
		pLayerFolder=pLayerType;
		if(pLayerFolder){
			FCM::FCMListPtr pChildList;
			pLayerFolder->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			for(int ccount=0; ccount<childCount; ccount++){
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameScripts(frameIdx, layer);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Folder, masked=%\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);			
		}				
		pLayerMask=pLayerType;
		if(pLayerMask){
			FCM::FCMListPtr pChildList;
			pLayerMask->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			for(int ccount=0; ccount<childCount; ccount++){
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameScripts(frameIdx, layer);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Mask, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(),isMasked);
		}
		pLayerGuide=pLayerType;
		if(pLayerGuide){
			FCM::FCMListPtr pChildList;
			pLayerGuide->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			for(int ccount=0; ccount<childCount; ccount++){
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameScripts(frameIdx, layer);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Guide, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);
		}
	}
	return result::AWD_SUCCESS;

}

AWD::result
TimelineEncoder::collectFrameCommands(int frameIdx, DOM::ILayer* iLayer, bool isMasked)
{
	//  this is excuted per frame and per layer. starting with first frame, and bottom layer.
	//	if isMasked is true, the Display-Objects found on this layer will be considered to be masked by the objects with 
	
	this->layerIdx++;

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
		
		// check if layer is affected by masks.
		// if the function was called with isMasked==true, all objects in this layer should be masked by parent layer
		TYPES::display_object_mask_type isMask = TYPES::display_object_mask_type::NO_MASKING;
		if(isMasked)
			isMask = TYPES::display_object_mask_type::OBJECTS_ARE_MASKED;

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
				this->collectFrameCommands(frameIdx, layer, isMasked);
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
			this->mask_range_max=0;
			this->mask_range_min=0;
			while(ccount>0){
				ccount--;
				FCM::AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameCommands(frameIdx, layer, true);
			}
			if(isMask == TYPES::display_object_mask_type::OBJECTS_ARE_MASKED){
				// should not happen. a mask-layer should never be masked by another layer
			}
			isMask = TYPES::display_object_mask_type::OBJECTS_ARE_MASKS;
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
				this->collectFrameCommands(frameIdx, layer, isMasked);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Guide, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);
		}
		pLayerNormal=pLayerType;
		if(pLayerNormal){	

			// this is a normal layer only consider if it is visible. todo: get setting to consider visible layers or not make this as settings.
			if(isVisible){
				FCM::U_Int32 totalDuration;
				pLayerNormal->GetTotalDuration(totalDuration);
				if(totalDuration>=frameIdx){
					
					//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Folder, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);	
					FCM::AutoPtr<DOM::IFrame> frame;
					pLayerNormal->GetFrameAtIndex(frameIdx, frame.m_Ptr);
					if(frame){
						DOM::PITween thisTween;
						frame->GetTween(thisTween);
						FCM::U_Int32 startFrame;
						frame->GetStartFrameIndex(startFrame);
						if((startFrame!=frameIdx)&&(thisTween==NULL)){
							//AwayJS::Utils::Trace(this->m_pCallback, "		Frame Should already be parsed\n");	
						}
						else{		
							curFrame->add_dirty_layer(this->layerIdx);					
							FCM::U_Int32 frameDuration;
							frame->GetDuration(frameDuration);
							//AwayJS::Utils::Trace(this->m_pCallback, "				duration: %d\n", frameDuration);

							DOM::PITween thisTween;
							frame->GetTween(thisTween);
							if(thisTween){
									//AwayJS::Utils::Trace(this->m_pCallback, "				HASTWEEN\n");
							//AutoPtr<DOM::Isha> frame = keyFrameList[f];
									//thisTween
							}
							FCM::FCMListPtr allPrevObjects;

							if(startFrame!=0){
								pLayerNormal->GetFrameAtIndex(frameIdx, frame.m_Ptr);

							}
							// get all frame-elements and loop over them.

							FCM::FCMListPtr allFrameElements;
							frame->GetFrameElements(allFrameElements.m_Ptr);
							FCM::U_Int32 objCount;
							allFrameElements->Count(objCount);
							int frameObjCnt=objCount;
							while(frameObjCnt>0){
								frameObjCnt--;
								// find the object in the last frame (check on all layers)
								FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> frameDisplayElement = allFrameElements[frameObjCnt];
								if(frameDisplayElement){
									DOM::Utils::MATRIX2D groupMatrix=DOM::Utils::MATRIX2D();
									this->collectFrameDisplayElement(frameDisplayElement, groupMatrix, this->layerIdx, isMask, false);
								}

								FCM::AutoPtr<DOM::FrameElement::ISound> soundElement = allFrameElements[frameObjCnt];
								
							}

						}
						
					}
				}
				else{
					//AwayJS::Utils::Trace(this->m_pCallback, "		Layer has no Frames \n");	
				}

			}
		}
	}
	return result::AWD_SUCCESS;
};

AWD::result
TimelineEncoder::collectFrameDisplayElement(DOM::FrameElement::IFrameDisplayElement* frameDisplayElement, DOM::Utils::MATRIX2D groupMatrix, int layerCnt, TYPES::display_object_mask_type mask_type, bool group_member=false)
{
	

	// To get all the properties for a Display-object, we need to cast it to different objects.
	
	// first get the common properties
	
	// available are:

	//	-	Matrix				(display-transform of the object)
	//	-	depth				(the z-position)
	//	-	pivot-point			(do we need it ?)
	//	-	bounds				(we dont need it, so we dont get it)
	//	-	Object-space-bounds	(we dont need it, so we dont get it)

	//	Get depth
	FCM::U_Int32 depth;
	frameDisplayElement->GetDepth(depth);

	//	Get Matrix
	DOM::Utils::MATRIX2D thisMatrix;
	frameDisplayElement->GetMatrix(thisMatrix);

	if(group_member){
		//	This object is a GroupMember of a Group.
		//	\todo	If we want to disolve groups on export, we need to convert the matrix from local-space to global-space (we got the parent matrix)
		//			If we do not want to resolve Groups, we need to put te FrameCommand 
	}

	// If this

	// Get Pivot Point
	DOM::Utils::POINT2D pivotPoint;
	frameDisplayElement->GetPivot(pivotPoint);

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
		for(int gm=0; gm< groupCntCount; gm++){
			FCM::AutoPtr<DOM::FrameElement::IFrameDisplayElement> groupItem = pGroupMembers[gm];
			this->collectFrameDisplayElement(groupItem, groupMatrix, layerCnt, mask_type, true);
		}
	}	

	else{
		
		//	This was no Group. It must be a display-object coontaining actual data.

		// now we know that we must create a Frame-commadn for this object

		AWD::ANIM::FrameCommandDisplayObject* newFrameCommand=new AWD::ANIM::FrameCommandDisplayObject();
		
		// set the index of the current layer. we need it later.
		//std::string resid="abc";
		//newFrameCommand->set_resID(resid);

		// set the index of the current layer. we need it later.
		newFrameCommand->set_layerID(this->layerIdx);
		
		// set has-ressource to true - we will merge commands later (has-ressource might becomme false again)
		newFrameCommand->set_hasRessource(true);

		// fill the command with the info that we have so far 
		newFrameCommand->set_display_matrix(this->flash_to_awd->convert_matrix2x3(thisMatrix));
		newFrameCommand->set_depth(depth);
		
		// check if this object is affected by masking.

		if(mask_type==TYPES::display_object_mask_type::OBJECTS_ARE_MASKS){

			// if the object should mask any other objects, we already know the range of depth on which objects should be masked.
			// All objects on a mask-layer should share depth, and mask a range of dpeth directly beneath its depth.
			// so for 
			// all objects found on the same layer-mask should share the same mask-depth
			// we just set the mask-range +1, so we are sure both restrictions are matched

			if(this->mask_range_max==0){
				// fallback if no depth-range has been set (should not happen)
				this->mask_range_max=depth;
				this->mask_range_min=depth;
			}
			newFrameCommand->set_depth(this->mask_range_max+1);		// making sure all objects of a mask layer share the same depth
			newFrameCommand->set_clipDepth(this->mask_range_min);	// set the min_range - the starting depth for objects to mask
		}

		else if(mask_type==TYPES::display_object_mask_type::OBJECTS_ARE_MASKED){

			// if the object should be masked by objects located in a mask layer above this layer, 
			// we set the mask-range min and max values by checking agains the objects depth.

			if(this->mask_range_min>depth)
				this->mask_range_min=depth;
			if(this->mask_range_max<depth)
				this->mask_range_max=depth;
		}
		else{
			
			// if this object should not be affected by mask layers, 
			// we reset the mask-range min and max values

			this->mask_range_min=0;
			this->mask_range_max=0;
		}

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
		//	we directly try to get the librarItem for this instance object.
		//	this is done for all objects that are IInstance

		FCM::AutoPtr<DOM::FrameElement::IInstance> instanceObject = frameDisplayElement;
		if(instanceObject){
			FCM::AutoPtr<DOM::ILibraryItem> libraryItem;
			instanceObject->GetLibraryItem(libraryItem.m_Ptr);
			if(libraryItem){	
				
				//	If this is a ILibraryItem, we get the object-name in the libary and its properties.
				//	we dont do anything really with the properties yet
				
				FCM::StringRep16 libName;
				libraryItem->GetName(&libName);

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
				}
				
				//	Try if this is a symbol instance.
				// if it is a symbol-instance, we can get this properties:
				//	-	ColorMatrix
				//	-	RegisrationPoint	(Not used)

				// still ISymbolInstance is not a final object-type, so we need to keep checking if it is movieclip or graphic.

				FCM::AutoPtr<DOM::FrameElement::ISymbolInstance> symbolInstance = frameDisplayElement;
				if(symbolInstance){

					DOM::Utils::COLOR_MATRIX colorMatrix;
					symbolInstance->GetColorMatrix(colorMatrix);
					DOM::Utils::POINT2D registrationPoint;
					symbolInstance->GetRegistrationPoint(registrationPoint);
					
					// try to get a ISymbolItem for this libraryItem. this is the class that will give use acces for the timeline of the movieclip or graphic
					FCM::AutoPtr<DOM::LibraryItem::ISymbolItem> symbolItem = libraryItem;
					if(symbolItem){

						//	try to get the timeline, and if we can get it, we create a new TimeLineEncoder, 
						//	and encode the Timeline, if it doesnt exists already.
						//	ATTENTION: Major recursion happening here.
						DOM::PITimeline new_timeline;
						symbolItem->GetTimeLine(new_timeline);
						TimelineEncoder newTimeLineEncoder = TimelineEncoder(m_pCallback, new_timeline, awd, flash_to_awd, -1);	
						result res = newTimeLineEncoder.encode();
						if(res!=result::AWD_SUCCESS)
							return res;
						// get the timeline from the timeline_encoder, and set it as source for the frame-command
						newFrameCommand->set_object_block(newTimeLineEncoder.get_awd_timeLine());
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

				FCM::AutoPtr<DOM::LibraryItem::IMediaItem> media_item = frameDisplayElement;
				if(media_item){
					FCM::AutoPtr<DOM::FrameElement::IBitmapInstance> bitmap = frameDisplayElement;
					if(bitmap){
						//	if it is a bitmap. we have a final object-type reached.				
						thisType="bitmap";	

						//	we call the flash_to_awd->ExportBitmap, and hopefully get back a AWDBlock for this bitmap.

						AWDBlock* new_bitmap; 
						FCM::Result res = this->flash_to_awd->ExportBitmap(media_item, &new_bitmap, "");					
						ASSERT(FCM_SUCCESS_CODE(res));

						// set the bitmap as source for the frame-command
						newFrameCommand->set_object_block(new_bitmap);

					}
				}
			}
		}

		//	if this is no instance-object, it must be a Classic-text, or a shape.

		// first check if it is text.

		FCM::AutoPtr<DOM::FrameElement::IClassicText> textField = frameDisplayElement;
		if(textField){
			//	if it is a textField. we have a final object-type reached.	
			thisType="textField";
			AWDBlock* new_textfield; 
			AWD::result res = this->flash_to_awd->ExportText(textField, &new_textfield);	
			if(res!=result::AWD_SUCCESS)
				return res;
			
			// set the textField as source for the frame-command
			newFrameCommand->set_object_block(new_textfield);

		}
		
		//	last but not least: check if the object is a shape.
		// if it is, we call the get_geom_for_shape() on the flash_to_awd class, and set it directly to the command.
		FCM::AutoPtr<DOM::FrameElement::IShape> shape = frameDisplayElement;
		if(shape){
			thisType="shape";
			newFrameCommand->set_object_block(this->flash_to_awd->get_geom_for_shape(shape, this->awd_timeline->get_name(), false));
			//AwayJS::Utils::Trace(this->m_pCallback, "					SHAPE\n");	
		}

		// thats it. the object should have been converted into awd-data.
		// the thisType should have been filled
		AwayJS::Utils::Trace(this->m_pCallback, "					-- found a %s on timeline %s\n", thisType.c_str(), this->awd_timeline->get_name().c_str());

		// now add the frame to the current output list:

		this->curFrame->add_command(newFrameCommand);
	}
	
	return result::AWD_SUCCESS;
};
