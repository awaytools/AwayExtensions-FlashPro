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
#include <fstream>
#include "Publisher.h"
#include "Utils.h"

#include "FlashFCMPublicIDs.h"

/*
#include "FrameElement/IClassicText.h"
#include "FrameElement/IParagraph.h"
#include "FrameElement/ITextRun.h"
#include "FrameElement/ITextStyle.h"
#include "FrameElement/IShape.h"
#include "FrameElement/ISound.h"
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

#include "Utils/DOMTypes.h"
#include "Utils/ILinearColorGradient.h"
#include "Utils/IRadialColorGradient.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"
*/

#include "AWDTimelineWriter.h"
#include "RessourcePalette.h"
#include "TimelineBuilder.h"
#include "TimelineEncoder.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG
namespace AwayJS
{


    FCM::Result CPublisher::ClearCache()
    {
        return FCM_SUCCESS;
    }

	
    FCM::Result CPublisher::GetOutputFileName(        
        DOM::PIFLADocument pFlaDocument, 
        DOM::PITimeline pTimeline, 
        const PIFCMDictionary pDictPublishSettings,
        std::string& outFile)
    {
        FCM::Result res = FCM_SUCCESS;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
        FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;

        pCalloc = AwayJS::Utils::GetCallocService(GetCallback());
        ASSERT(pCalloc.m_Ptr != NULL);

        // Read the output file name from the publish settings
        ReadString(pDictPublishSettings, (FCM::StringRep8)"out_file", outFile);
        if (outFile.empty())
        {
            FCM::StringRep16 path;

            res = pFlaDocument->GetPath(&path);
            ASSERT(FCM_SUCCESS_CODE(res));

            if (path)
            {
                std::string parent;
                std::string ext;
                std::string filePath = Utils::ToString(path, GetCallback());

                Utils::GetFileNameWithoutExtension(filePath, outFile);

                if (pTimeline)
                {
                    FCM::StringRep16 pSceneName;
                    std::string sceneName;

                    res = pTimeline->GetName(&pSceneName);
                    ASSERT(FCM_SUCCESS_CODE(res));

                    sceneName = Utils::ToString(pSceneName, GetCallback());

                    outFile += "_";
                    outFile += sceneName;
					pCalloc->Free(pSceneName);
                }

                outFile += ".";
                outFile += OUTPUT_FILE_EXTENSION;

                Utils::GetFileExtension(filePath, ext);
                
                // TODO: Convert "ext" to lower case and then compare
                if (ext.compare("xfl") == 0)
                {
                    std::string immParent;
                    Utils::GetParent(filePath, immParent);
                    Utils::GetParent(immParent, parent);
                }
                else
                {
                    // FLA
                    Utils::GetParent(filePath, parent);
                }

                // Extract the extension and append output file extension.
                outFile = parent + outFile;

                pCalloc->Free(path);

                res = FCM_SUCCESS;
            }
            else
            {
                res = FCM_INVALID_PARAM;
            }
        }

        return res;
    }
	
	AWD::result CPublisher::ExportLibraryItems(FCM::FCMListPtr pLibraryItemList, bool export_lib_bitmaps, bool export_lib_sounds, bool export_fonts, bool use_adobe_frame_cmds, TimelineEncoder* timelineEncoder)
	{
        FCM::U_Int32 count = 0;
        FCM::Result res;

        ASSERT(pLibraryItemList);
        res = pLibraryItemList->Count(count);
        ASSERT(FCM_SUCCESS_CODE(res));
		
		Utils::Trace(GetCallback(), "Encountered %d library items\n", count);
        FCM::AutoPtr<FCM::IFCMCalloc> pCalloc = AwayJS::Utils::GetCallocService(GetCallback());
        ASSERT(pCalloc.m_Ptr != NULL);

		AWD::result awd_res=result::AWD_SUCCESS;
		
        for (FCM::U_Int32 index = 0; index < count ; index++)
        {
            AutoPtr<DOM::ILibraryItem> plibrary_item = pLibraryItemList[index];
		
			DOM::AutoPtr<DOM::LibraryItem::IFolderItem> pFolder_item = plibrary_item;
			if(pFolder_item){
				//Utils::Trace(GetCallback(), "\n	->	Found Folder");
				// export a folder item from library. just call this function for all its children.			
				FCM::FCMListPtr pLibraryItems;
				res = pFolder_item->GetChildren(pLibraryItems.m_Ptr);
				ASSERT(FCM_SUCCESS_CODE(res));
				awd_res=ExportLibraryItems(pLibraryItems, export_lib_bitmaps, export_lib_sounds, export_fonts, use_adobe_frame_cmds, timelineEncoder);
				if(awd_res!=result::AWD_SUCCESS)
					return awd_res;
				continue;
			}
			
			AutoPtr<IFCMDictionary> pDict;
			FCM::FCMDictRecTypeID link_type;
			FCM::U_Int32 valLen;
			std::string script_name="";

			res = plibrary_item->GetProperties(pDict.m_Ptr);
			ASSERT(FCM_SUCCESS_CODE(res));
			res = pDict->GetInfo(kLibProp_LinkageClass_DictKey, link_type, valLen);
			if (FCM_SUCCESS_CODE(res))
			{
				FCM::StringRep8 script_name_8 = new char[valLen];
				res = pDict->Get(kLibProp_LinkageClass_DictKey, link_type, script_name_8, valLen);
				ASSERT(FCM_SUCCESS_CODE(res));
				script_name+=script_name_8;
				delete []script_name_8;
			}
			if(export_fonts){
				DOM::AutoPtr<DOM::LibraryItem::IFontItem> pFontItem = plibrary_item;
				if(pFontItem){
					//Utils::Trace(GetCallback(), "\n	->	Found Font");
					// export a font item from library
					AWDBlock* new_font = this->flash_to_awd_encoder->ExportFont(pFontItem);
					if(new_font==NULL)
                		return awd_res;
					if(script_name.size()>0){
						new_font->set_name(script_name);
						new_font->add_scene_name("script-linkage");
					}
					continue;
				}
			}
			DOM::AutoPtr<DOM::LibraryItem::ISymbolItem> pSymbol_item = plibrary_item;
			if(pSymbol_item){	
				if((script_name.size()>0)||(!use_adobe_frame_cmds)){
					FCM::StringRep16 symbol_name=NULL;
					plibrary_item->GetName(&symbol_name);
					std::string symbol_name_str=AwayJS::Utils::ToString(symbol_name, GetCallback());
					// export a symbol (timeline). If the timeline has already been encoded, nothing will happen. 
					//Utils::Trace(GetCallback(), "\n	->	Found Symbol %s | %s", symbol_name_str.c_str(), timeline_name_str.c_str());
					if(use_adobe_frame_cmds){
						BLOCKS::Timeline* this_timeline = this->awd_project->get_timeline_by_symbol_name(symbol_name_str);
						if(this_timeline!=NULL){
							this_timeline->add_scene_name("script-linkage");
							continue;
						}
						else{				
							// Generate frame commands				
							FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
							AutoPtr<IFrameCommandGenerator> m_frameCmdGeneratorService;		
							AutoPtr<IResourcePalette> m_pResourcePalette;		
							AwayJS::ResourcePalette* pResPalette;

							res = GetCallback()->GetService(Exporter::Service::EXPORTER_FRAME_CMD_GENERATOR_SERVICE, pUnk.m_Ptr);
							ASSERT(FCM_SUCCESS_CODE(res));
							m_frameCmdGeneratorService = pUnk;
					
							res = GetCallback()->CreateInstance(NULL, CLSID_ResourcePalette, IID_IResourcePalette, (void**)&m_pResourcePalette);
							ASSERT(FCM_SUCCESS_CODE(res));
			
							pResPalette = static_cast<ResourcePalette*>(m_pResourcePalette.m_Ptr);
							pResPalette->Init(this->flash_to_awd_encoder);	
							// Create a Timeline Builder Factory for the root timelines of the document
							AutoPtr<ITimelineBuilderFactory> pTimelineBuilderFactory;
							res = GetCallback()->CreateInstance(NULL, CLSID_TimelineBuilderFactory, IID_ITimelineBuilderFactory, (void**)&pTimelineBuilderFactory);
							ASSERT(FCM_SUCCESS_CODE(res));
							(static_cast<TimelineBuilderFactory*>(pTimelineBuilderFactory.m_Ptr))->Init(this->flash_to_awd_encoder);
				
							AutoPtr<DOM::ITimeline> timeline;
							res = pSymbol_item->GetTimeLine(timeline.m_Ptr);

							AutoPtr<ITimelineBuilder> pTimelineBuilder;
							ITimelineWriter* pTimelineWriter;
							Exporter::Service::RANGE range;		
							range.min = 0;
							res = timeline->GetMaxFrameCount(range.max);
							range.max--;
				
							FCM::StringRep16 scene_name_str = NULL;
							timeline->GetName(&scene_name_str);
							std::string scene_name=AwayJS::Utils::ToString(scene_name_str, GetCallback());

							// Generate frame commands		
							res = m_frameCmdGeneratorService->GenerateFrameCommands(timeline, range, _pDict,	m_pResourcePalette, pTimelineBuilderFactory, pTimelineBuilder.m_Ptr);
							ASSERT(FCM_SUCCESS_CODE(res));
							((TimelineBuilder*)pTimelineBuilder.m_Ptr)->Build(0, scene_name_str, &pTimelineWriter);
				
							this_timeline = this->awd_project->get_timeline_by_symbol_name(scene_name);
							this_timeline->set_script_name(script_name);
							this_timeline->add_scene_name("script-linkage");

							if(this->awd_project->get_blocks_for_external_ids()!=result::AWD_SUCCESS)
								Utils::Trace(GetCallback(), "PROBLEM IN CONVERTING RESSOURCE_ID TO AWDBLOCKS FOR FRAMECOMMANDS!!!\nEXPORT STILL CONTINUES !!!\n");
							pResPalette->Clear();
							if(scene_name_str){
								pCalloc->Free((FCM::PVoid)scene_name_str);
							}
						}
					}
					else{
						BLOCKS::Timeline* this_timeline = this->awd_project->get_timeline_by_symbol_name(symbol_name_str);
						if(this_timeline==NULL){
							this_timeline=new BLOCKS::Timeline();
							this_timeline->set_fps(this->awd_project->get_settings()->get_fps());
							this->awd_project->add_block(this_timeline);
						}
						this_timeline->set_name(symbol_name_str);
						this_timeline->set_symbol_name(symbol_name_str);
						if(script_name.size()>0){
							this_timeline->add_scene_name("script-linkage");
							this_timeline->set_name(script_name);
							this_timeline->set_script_name(script_name);
						}	
						AutoPtr<DOM::ITimeline> timeline;
						res = pSymbol_item->GetTimeLine(timeline.m_Ptr);
						timelineEncoder->encode(timeline, this_timeline);

					}
					if(symbol_name)
						pCalloc->Free((FCM::PVoid)symbol_name);
					continue;
				}	
			}
			if((export_lib_sounds)||(export_lib_bitmaps)){
				DOM::AutoPtr<DOM::LibraryItem::IMediaItem> pMedia_item = plibrary_item;
				if(pMedia_item){	
					//Utils::Trace(GetCallback(), "\n	->	Found Media");
					// export a media item. 
					FCM::PIFCMUnknown unknownMediaInfo;
					res = pMedia_item->GetMediaInfo(unknownMediaInfo); 
					ASSERT(FCM_SUCCESS_CODE(res));
					FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmapInfo = unknownMediaInfo;
					FCM::AutoPtr<DOM::MediaInfo::ISoundInfo> SoundInfo = unknownMediaInfo;
					if((bitmapInfo)&&(export_lib_bitmaps)){
						//AwayJS::Utils::Trace(GetCallback(), "	ENCOUNTERED LIB BITMAP 1!!");
						if(script_name.size()>0){
							AWDBlock* new_bitmap=NULL; 
							flash_to_awd_encoder->ExportBitmap(pMedia_item, &new_bitmap);
							if(new_bitmap==NULL)
								continue;
							//AwayJS::Utils::Trace(GetCallback(), "	ENCOUNTERED LIB BITMAP 2!!");
							new_bitmap->set_name(script_name);
							new_bitmap->add_scene_name("script-linkage");
						}
					}
					else if((SoundInfo)&&(export_lib_sounds)){
						if(script_name.size()>0){
							AWDBlock* new_sound = flash_to_awd_encoder->ExportSound(pMedia_item, NULL, "");
							if(new_sound==NULL)
								continue;
							new_sound->set_name(script_name);
							new_sound->add_scene_name("script-linkage");
						}
					}
					continue;
				}
			}
		}
		return awd_res;
	}

    bool CPublisher::ReadString(
        const FCM::PIFCMDictionary pDict,
        FCM::StringRep8 key, 
        std::string &retString)
    {
        FCM::U_Int32 valueLen;
        FCM::FCMDictRecTypeID type;

        FCM::Result res = pDict->GetInfo(key, type, valueLen);
        if (FCM_FAILURE_CODE(res))
        {
            return false;
        }

        FCM::StringRep8 strValue = new char[valueLen];
        res = pDict->Get(key, type, (FCM::PVoid)strValue, valueLen);
        if (FCM_FAILURE_CODE(res))
        {
            delete [] strValue;
            return false;
        }

        retString = strValue;

        delete [] strValue;
        return true;
    }


    FCM::Boolean CPublisher::IsPreviewNeeded(const PIFCMDictionary pDictConfig)
    {
        FCM::Boolean found;
        std::string previewNeeded;

        found = ReadString(pDictConfig, (FCM::StringRep8)kPublishSettingsKey_PreviewNeeded, previewNeeded);
        ASSERT(found);

        if (previewNeeded == "true")
        {
            return true;
        }
        return false;
    }




};
