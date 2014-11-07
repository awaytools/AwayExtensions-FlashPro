/*************************************************************************
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright [2013] Adobe Systems Incorporated
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of Adobe Systems Incorporated and its suppliers,
* if any.  The intellectual and technical concepts contained
* herein are proprietary to Adobe Systems Incorporated and its
* suppliers and are protected by all applicable intellectual 
* property laws, including trade secret and copyright laws.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe Systems Incorporated.
**************************************************************************/

#ifdef _WINDOWS
#include <Windows.h>
#include "ShellApi.h"
#endif
#include <vector>
#include <fstream>
#include "Publisher.h"
#include "Utils.h"

#include "FlashFCMPublicIDs.h"
#include "ILayer.h"
#include "IFilterable.h"
#include "FrameElement/IClassicText.h"
#include "FrameElement/IParagraph.h"
#include "FrameElement/ITextRun.h"
#include "FrameElement/ITextStyle.h"
#include "FrameElement/IShape.h"
#include "FrameElement/ISound.h"
#include "FrameElement/IInstance.h"
#include "FrameElement/IGraphic.h"
#include "FrameElement/IGroup.h"
#include "FrameElement/IBitmapInstance.h"
#include "StrokeStyle/IDashedStrokeStyle.h"
#include "StrokeStyle/IDottedStrokeStyle.h"
#include "StrokeStyle/IHatchedStrokeStyle.h"
#include "StrokeStyle/IRaggedStrokeStyle.h"
#include "StrokeStyle/ISolidStrokeStyle.h"
#include "StrokeStyle/IStippleStrokeStyle.h"

#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"

#include "MediaInfo/IBitmapInfo.h"
#include "FrameElement/IBitmapInstance.h"
#include "FrameElement/ISound.h"
#include "MediaInfo/ISoundInfo.h"
#include "LibraryItem/IMediaItem.h"
#include "LibraryItem/IFolderItem.h"
#include "LibraryItem/IFontItem.h"
#include "LibraryItem/ISymbolItem.h"
#include "ILibraryItem.h"

#include "FrameElement/IClassicText.h"
#include "FrameElement/ITextStyle.h"
#include "FrameElement/IParagraph.h"
#include "FrameElement/ITextRun.h"
#include "FrameElement/ITextBehaviour.h"

#include "Service/Shape/IRegionGeneratorService.h"
#include "Service/Shape/IFilledRegion.h"
#include "Service/Shape/IStrokeGroup.h"
#include "Service/Shape/IPath.h"
#include "Service/Shape/IEdge.h"
#include "Service/Shape/IShapeService.h"
#include "Service/Image/IBitmapExportService.h"
#include "Service/FontTable/IFontTableGeneratorService.h"
#include "Service/FontTable/IFontTable.h"
#include "Service/FontTable/IGlyph.h"
#include "Service/FontTable/IKerningPair.h"

#include "Utils/DOMTypes.h"
#include "Utils/ILinearColorGradient.h"
#include "Utils/IRadialColorGradient.h"

#include "OutputWriter.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

#include "TimeLineEncoder.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG


TimeLineEncoder::TimeLineEncoder(FCM::PIFCMCallback pCallback, DOM::ITimeline* timeline, AWD* awd,ShapePool* shapePool, int sceneID)
{
    this->m_pCallback=pCallback;
	this->timeline=timeline;
	this->curFrame=NULL;
	this->shapePool=shapePool;
	this->awd=awd;
	this->curDepth=0;
	this->clipMask=0;
	FCM::StringRep16 thisName16;
	this->timeline->GetName(&thisName16);
    std::string thisstring=AwayJS::Utils::ToString(thisName16, this->m_pCallback);
	this->awd_timeline=awd->get_timeline(thisstring);
	this->awd_timeline->set_scene_id(sceneID);
	this->layerIdx=0;	

}

TimeLineEncoder::~TimeLineEncoder()
{
}

AWDShape2DTimeline* 
TimeLineEncoder::get_awd_timeLine(){
	return this->awd_timeline;
}
void
TimeLineEncoder::encode(){
    FCM::Result res;
	int duration = 1000 / awd->getExporterSettings()->get_fps();
	if(this->awd_timeline->get_isProcessed()){
		//AwayJS::Utils::Trace(this->m_pCallback, "Timeline is already parsed!!!\n");
	}
	else{
		FCM::U_Int32 maxFrameCount;
		res=this->timeline->GetMaxFrameCount(maxFrameCount);
	
	
		FCM::FCMListPtr pLayerList;
		this->timeline->GetLayers(pLayerList.m_Ptr);
		FCM::U_Int32 layerCount;
		pLayerList->Count(layerCount);
		//AwayJS::Utils::Trace(this->m_pCallback, "Parse TimeLine '%s', sceneID: %d, duration: %d, layers: %d\n", this->awd_timeline->get_name().c_str(), this->awd_timeline->get_scene_id(), maxFrameCount, layerCount);
		for(int i=0; i<maxFrameCount;i++){
			//AwayJS::Utils::Trace(this->m_pCallback, "Parse Frame nr: %d\n\n",i);
			this->curDepth=0;
			this->clipMask=0;
			this->curFrame=new AWDTimeLineFrame();
			this->curFrame->set_frame_duration(duration);
			// run over layers from top to bottom = collect framescript and labels
			for(int lc=0; lc<layerCount;lc++){
				AutoPtr<DOM::ILayer> layer = pLayerList[lc];
				this->collectFrameScripts(i, layer);
			}
			int l=layerCount;
			this->layerIdx=0;	
			// run over layers from bottom to top = collect frameCommands
			while(l>0){
				l--;
				AutoPtr<DOM::ILayer> layer = pLayerList[l];
				this->collectFrameCommands(i, layer, false);
			}
			if(i>=2){
			//	i=maxFrameCount;
			}
			if(this->curFrame->get_dirty_layers().size()>0){
				this->awd_timeline->add_frame(this->curFrame, true);
			}
			else{
				if(this->awd_timeline->get_frame())
					this->awd_timeline->get_frame()->set_frame_duration(this->awd_timeline->get_frame()->get_frame_duration()+duration);
				//AwayJS::Utils::Trace(this->m_pCallback, "EMPTY FRAME\n");
				delete this->curFrame;
			}
		}
		this->awd_timeline->set_isProcessed(true);
	}
}

void
TimeLineEncoder::collectFrameScripts(int frameIdx, DOM::ILayer* iLayer)
{
    FCM::Result res;
	FCM::StringRep16 layerName16;
	res = iLayer->GetName(&layerName16);
	FCM::Boolean isVisible;
	iLayer->IsVisible(isVisible);
	
	AutoPtr<DOM::Layer::ILayerFolder> pLayerFolder;
	AutoPtr<DOM::Layer::ILayerMask> pLayerMask;
	AutoPtr<DOM::Layer::ILayerGuide> pLayerGuide;
	AutoPtr<DOM::Layer::ILayerNormal> pLayerNormal;

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
					AutoPtr<DOM::IFrame> frame;
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
							AWD_Frame_Label_type thisAWDLabelType=AWD_FRAME_LABEL_NONE;
							//AwayJS::Utils::Trace(this->m_pCallback, "			thisLabelType: %d\n", thisLabelType);
							if(thisLabelType == 1)
								thisAWDLabelType=AWD_FRAME_LABEL_NAME;//AwayJS::Utils::Trace(this->m_pCallback, "				thisLabelType: NAME\n");
							else if(thisLabelType == 2)//COMMENTS DONT SEAM TO GET EXPORTED BY IEXPORTERSERVICE
								thisAWDLabelType=AWD_FRAME_LABEL_COMMENT;//AwayJS::Utils::Trace(this->m_pCallback, "				thisLabelType: COMMENT\n");
							else if(thisLabelType == 3)
								thisAWDLabelType=AWD_FRAME_LABEL_ANCOR;//AwayJS::Utils::Trace(this->m_pCallback, "				thisLabelType: ANCHOR\n");
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
				AutoPtr<DOM::ILayer> layer = pChildList[ccount];
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
				AutoPtr<DOM::ILayer> layer = pChildList[ccount];
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
				AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameScripts(frameIdx, layer);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Guide, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);
		}
	}

}

void
TimeLineEncoder::collectFrameCommands(int frameIdx, DOM::ILayer* iLayer, bool isMasked)
{
	this->layerIdx++;
    FCM::Result res;
	FCM::StringRep16 layerName16;;
	res = iLayer->GetName(&layerName16);
	FCM::Boolean isVisible;
	iLayer->IsVisible(isVisible);
	
	AutoPtr<DOM::Layer::ILayerFolder> pLayerFolder;
	AutoPtr<DOM::Layer::ILayerMask> pLayerMask;
	AutoPtr<DOM::Layer::ILayerGuide> pLayerGuide;
	AutoPtr<DOM::Layer::ILayerNormal> pLayerNormal;

	FCM::PIFCMUnknown pLayerType;
	res = iLayer->GetLayerType(pLayerType);
	if(res==FCM_SUCCESS){
		int isMask=0;
		pLayerFolder=pLayerType;
		if(pLayerFolder){
			FCM::FCMListPtr pChildList;
			pLayerFolder->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			int ccount=childCount;
			while(ccount>0){
				ccount--;
				AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameCommands(frameIdx, layer, isMasked);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Folder, masked=%\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);			
		}				
		pLayerMask=pLayerType;
		if(pLayerMask){
			FCM::FCMListPtr pChildList;
			pLayerMask->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			int ccount=childCount;
			this->maskDepth=0;
			this->clipMask=0;
			while(ccount>0){
				ccount--;
				AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameCommands(frameIdx, layer, 2);
			}
			isMask=1;
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Mask, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(),isMasked);
		}
		pLayerGuide=pLayerType;
		if(pLayerGuide){
			FCM::FCMListPtr pChildList;
			pLayerGuide->GetChildren(pChildList.m_Ptr);
			FCM::U_Int32 childCount;
			pChildList->Count(childCount);
			int ccount=childCount;
			while(ccount>0){
				ccount--;
				AutoPtr<DOM::ILayer> layer = pChildList[ccount];
				this->collectFrameCommands(frameIdx, layer, isMasked);
			}
			//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Guide, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);
		}
		pLayerNormal=pLayerType;
		if(pLayerNormal){
			if(isVisible){
				FCM::U_Int32 totalDuration;
				pLayerNormal->GetTotalDuration(totalDuration);
				if(totalDuration>=frameIdx){
					
					//AwayJS::Utils::Trace(this->m_pCallback, "		LayerName: %s, LayerType = Folder, masked=%d\n", AwayJS::Utils::ToString(layerName16,this->m_pCallback).c_str(), isMasked);	
					AutoPtr<DOM::IFrame> frame;
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

							FCM::FCMListPtr allFrameElements;
							frame->GetFrameElements(allFrameElements.m_Ptr);
							FCM::U_Int32 objCount;
							allFrameElements->Count(objCount);
							int frameObjCnt=objCount;
							while(frameObjCnt>0){
								frameObjCnt--;
								// find the object in the last frame (check on all layers)
								AutoPtr<DOM::FrameElement::IFrameDisplayElement> frameDisplayElement = allFrameElements[frameObjCnt];
								if(frameDisplayElement){
									 DOM::Utils::MATRIX2D groupMatrix=DOM::Utils::MATRIX2D();
									this->collectFrameDisplayElement(frameDisplayElement, groupMatrix, this->layerIdx, isMask);
								}

								AutoPtr<DOM::FrameElement::ISound> soundElement = allFrameElements[frameObjCnt];
								
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

};

void
	TimeLineEncoder::collectFrameDisplayElement(DOM::FrameElement::IFrameDisplayElement* frameDisplayElement, DOM::Utils::MATRIX2D groupMatrix, int layerCnt, int isMask)
{
	FCM::U_Int32 depth;
	frameDisplayElement->GetDepth(depth);
	DOM::Utils::MATRIX2D thisMatrix;
	frameDisplayElement->GetMatrix(thisMatrix);
	DOM::Utils::POINT2D pivotPoint;
	frameDisplayElement->GetPivot(pivotPoint);	
	std::string thisType="undefined";
	AutoPtr<DOM::FrameElement::IGroup> group =  frameDisplayElement;
	if(group){
		thisType="group";
		//AwayJS::Utils::Trace(this->m_pCallback, "					GROUP\n");
		FCM::FCMListPtr pGroupMembers;
		group->GetMembers(pGroupMembers.m_Ptr);
		FCM::U_Int32 groupCntCount;
		pGroupMembers->Count(groupCntCount);
		for(int gm=0; gm< groupCntCount; gm++){
			AutoPtr<DOM::FrameElement::IFrameDisplayElement> groupItem = pGroupMembers[gm];
			this->collectFrameDisplayElement(groupItem, groupMatrix, layerCnt, isMask);
		}
	}
	else{
		AWDFrameCommandDisplayObject* newFrameCommand=new AWDFrameCommandDisplayObject();
		newFrameCommand->set_layerID(layerIdx);
        std::string newstring2=std::string("");
		newFrameCommand->set_resID(newstring2);
		newFrameCommand->set_hasRessource(true);
		//newFrameCommand->set_color_matrix(get_color_mtx_4x5());
		newFrameCommand->set_display_matrix(this->get_mtx_2x3(&thisMatrix));
		//newFrameCommand->set_command_type(AWD_FRAME_COMMAND_ADD_OBJECT);
		newFrameCommand->set_depth(depth);
		if(isMask==1){
			if(this->maskDepth==0){
				this->maskDepth=depth;// making sure all objects of a mask layer share the same depth
			}
			newFrameCommand->set_depth(this->maskDepth);
			newFrameCommand->set_clipDepth(this->clipMask);
		}
		else if (isMask==2){
			if(this->clipMask<depth){
				this->clipMask=depth;
			}
		}
		AutoPtr<DOM::IFilterable> filterableObject = frameDisplayElement;
		if(filterableObject){
			FCM::FCMListPtr pFilterList;
			filterableObject->GetGraphicFilters(pFilterList.m_Ptr);
			FCM::U_Int32 pFilterListCount;
			pFilterList->Count(pFilterListCount);
			//AwayJS::Utils::Trace(this->m_pCallback, "					ISFILTERABLE %d\n", pFilterListCount);
			for(int gm=0; gm< pFilterListCount; gm++){
			}
		}
		AutoPtr<DOM::FrameElement::IInstance> instanceObject = frameDisplayElement;
		if(instanceObject){
			//thisType="instanceObject";
			AutoPtr<DOM::ILibraryItem> libraryItem;
			instanceObject->GetLibraryItem(libraryItem.m_Ptr);
			if(libraryItem){	
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
				//AutoPtr<DOM::LibraryItem::ISymbolItem> mcSymbol =libraryItem;
				//instanceObject->GetLibraryItem();
				//AwayJS::Utils::Trace(this->m_pCallback, "					instanceObject\n");
				AutoPtr<DOM::FrameElement::ISymbolInstance> symbolInstance = frameDisplayElement;
				if(symbolInstance){
					thisType="ISymbolInstance";
					DOM::Utils::COLOR_MATRIX colorMatrix;
					symbolInstance->GetColorMatrix(colorMatrix);
					DOM::Utils::POINT2D registrationPoint;
					symbolInstance->GetRegistrationPoint(registrationPoint);	
					AutoPtr<DOM::LibraryItem::ISymbolItem> symbolItem =libraryItem;
					if(symbolItem){
						// RECURSION HAPPENS HERE!!! 
						DOM::PITimeline newTimeline;
						symbolItem->GetTimeLine(newTimeline);
						TimeLineEncoder* newTimeLineEncoder = new TimeLineEncoder(m_pCallback, newTimeline, awd, shapePool, -1);	
						newTimeLineEncoder->encode();
						newFrameCommand->set_object_block(newTimeLineEncoder->get_awd_timeLine());
						delete newTimeLineEncoder;
					}

					AutoPtr<DOM::FrameElement::IGraphic> graphic =  frameDisplayElement;
					if(graphic){
						thisType="graphic";
						//AwayJS::Utils::Trace(this->m_pCallback, "					GRAPHIC\n");	
						DOM::FrameElement::AnimationLoopMode loopMode;
						graphic->GetLoopMode(loopMode);	
						FCM::U_Int32 firstFrame;
						graphic->GetFirstFrameIndex(firstFrame);	
					}

					AutoPtr<DOM::FrameElement::IMovieClip> movieclip =  frameDisplayElement;
					if(movieclip){
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
						newFrameCommand->set_visible(isVisible);

					}

				}
				AutoPtr<DOM::FrameElement::IBitmapInstance> bitmap = frameDisplayElement;
				if(bitmap){
					thisType="bitmap";	
					//AwayJS::Utils::Trace(this->m_pCallback, "					BITMAP\n");								 
					AutoPtr<DOM::LibraryItem::IMediaItem> mediaItem =libraryItem;
					FCM::PIFCMUnknown unknownMediaInfo;
					mediaItem->GetMediaInfo(unknownMediaInfo); 
					AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmapInfo = unknownMediaInfo;
					if(bitmapInfo){
						FCM::S_Int32 bmWidth;
						bitmapInfo->GetWidth(bmWidth);
						FCM::S_Int32 bmheight;
						bitmapInfo->GetHeight(bmheight);
						//AwayJS::Utils::Trace(this->m_pCallback, "					BITMAP %d , %d\n", bmWidth, bmheight);	
						//AwayJS::Utils::Trace(this->m_pCallback, "					BITMAP %s , %d\n", AwayJS::Utils::ToString(libName, m_pCallback).c_str());		
						std::string bitmapPath;
						std::string bitmapName;
						// Form the image path
						bitmapName = AwayJS::Utils::ToString(libName, m_pCallback);
						std::size_t pos = bitmapName.find(".");

						if (pos != std::string::npos)
						{
							FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
							std::string bitmapRelPath;
							std::string bitmapExportPath = "c:/";//m_outputImageFolder + "/";
            
							if ((bitmapName.substr(pos + 1) != "jpg") ||
								(bitmapName.substr(pos + 1) != "png"))
							{
								std::string bitmapNameWithoutExt;

								bitmapNameWithoutExt = bitmapName.substr(0, pos);
								bitmapName = bitmapNameWithoutExt + ".png";
							}
							bitmapExportPath += bitmapName;            
							bitmapRelPath = "./";
							bitmapRelPath += IMAGE_FOLDER;
							bitmapRelPath += "/" + bitmapName;

							FCM::Result res = m_pCallback->GetService(DOM::FLA_BITMAP_SERVICE, pUnk.m_Ptr);
							ASSERT(FCM_SUCCESS_CODE(res));

							FCM::AutoPtr<DOM::Service::Image::IBitmapExportService> bitmapExportService = pUnk;
							if (bitmapExportService)
							{
								res = bitmapExportService->ExportToFile(mediaItem,AwayJS::Utils::ToString16(bitmapExportPath, m_pCallback), 100);
								ASSERT(FCM_SUCCESS_CODE(res));
							}

							newFrameCommand->set_object_block(awd->get_texture(bitmapRelPath));
						}
					}
				}
			}
		}
		AutoPtr<DOM::FrameElement::IClassicText> textField = frameDisplayElement;
		if(textField){
			thisType="textField";
			//AwayJS::Utils::Trace(this->m_pCallback, "					TEXT\n");
			FCM::FCMListPtr pParagraphs;
			textField->GetParagraphs(pParagraphs.m_Ptr);
			FCM::U_Int32 pParagraphsCount;
			pParagraphs->Count(pParagraphsCount);
			DOM::FrameElement::AA_MODE_PROP aamode;
			textField->GetAntiAliasModeProp(aamode);
			FCM::StringRep16 text;
			textField->GetText(&text);
			DOM::FrameElement::PITextBehaviour textBehaviour;
			textField->GetTextBehaviour(textBehaviour);
		}
		AutoPtr<DOM::FrameElement::IShape> shape = frameDisplayElement;
		if(shape){
			thisType="shape";
			newFrameCommand->set_object_block(this->shapePool->getShape(shape));
			//AwayJS::Utils::Trace(this->m_pCallback, "					SHAPE\n");	
		}
		//AwayJS::Utils::Trace(this->m_pCallback, "					----------\n");
		this->curFrame->add_command(newFrameCommand);
	}

};

	awd_float64 *
		TimeLineEncoder::get_color_mtx_4x5(const DOM::Utils::COLOR_MATRIX* mtx)
	{	
		awd_float64* newMtx = (awd_float64*)malloc(20 * sizeof(awd_float64));
		if(mtx!=NULL){
			for(int i=0; i<10; i++){
				newMtx[i]  = mtx->colorArray[i];
			}
		}
		else{
			newMtx[0]  = 1.0; 
			newMtx[1]  = 0.0; 
			newMtx[2]  = 0.0;
			newMtx[3]  = 1.0;
			newMtx[4]  = 0.0; 
			newMtx[5]  = 0.0; 
			newMtx[6]  = 0.0; 
			newMtx[7]  = 0.0; 
			newMtx[8]  = 0.0; 
			newMtx[9]  = 0.0; 
			newMtx[10]  = 0.0; 
			newMtx[11]  = 0.0; 
			newMtx[12]  = 0.0; 
			newMtx[13]  = 0.0; 
			newMtx[14]  = 0.0; 
			newMtx[15]  = 0.0; 
			newMtx[16]  = 0.0; 
			newMtx[17]  = 0.0; 
			newMtx[18]  = 0.0; 
			newMtx[19]  = 0.0; 
		}
		return newMtx;
	} 
	awd_float64 *
	TimeLineEncoder::get_mtx_2x3(const DOM::Utils::MATRIX2D* mtx)
	{	
		awd_float64* newMtx = (awd_float64*)malloc(6 * sizeof(awd_float64));
		if(mtx!=NULL){
			newMtx[0]  = mtx->a; 
			newMtx[1]  = mtx->b; 
			newMtx[2]  = mtx->c;
			newMtx[3]  = mtx->d;
			newMtx[4]  = mtx->tx*-1;
			newMtx[5]  = mtx->ty*-1;
		}
		else{
			newMtx[0]  = 1.0; 
			newMtx[1]  = 0.0; 
			newMtx[2]  = 0.0;
			newMtx[3]  = 1.0;
			newMtx[4]  = 0.0; 
			newMtx[5]  = 0.0; 
		}
		return newMtx;
	} 