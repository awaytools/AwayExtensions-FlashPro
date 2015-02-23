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
	
	AWD::result CPublisher::ExportLibraryItems(bool export_lib_bitmaps, bool export_lib_sounds)
	{		
		AWD::result awd_res=result::AWD_SUCCESS;		
        FCM::Result res;
		FCM::FCMListPtr pLibraryItems;
		res = this->fla_document->GetLibraryItems(pLibraryItems.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));
        FCM::U_Int32 libCnt;
        res = pLibraryItems->Count(libCnt);
        for (FCM::U_Int32 l = 0; l < libCnt; l++)        
		{		
			DOM::AutoPtr<DOM::ILibraryItem> plibrary_item = pLibraryItems[l];
			awd_res = ExportLibraryItem(plibrary_item, export_lib_bitmaps, export_lib_sounds);
			if(awd_res!=result::AWD_SUCCESS)
				return awd_res;
		}
		return awd_res;
	}
	AWD::result CPublisher::ExportLibraryItem(DOM::ILibraryItem* plibrary_item, bool export_lib_bitmaps, bool export_lib_sounds)
	{
		FCM::Result res;
		AWD::result awd_res=result::AWD_SUCCESS;

		DOM::AutoPtr<DOM::LibraryItem::IFontItem> pFontItem = plibrary_item;
		if(pFontItem){
			// export a font item from library
			//AWD::result awd_res = flash_awd_cencoder->ExportFont(pFontItem, awd);
			//if(awd_res!=result::AWD_SUCCESS)
			//	return awd_res;
		}

		DOM::AutoPtr<DOM::LibraryItem::IFolderItem> pFolder_item = plibrary_item;
		if(pFolder_item){
			// export a folder item from library. just call this function for all its children.			
			FCM::FCMListPtr pLibraryItems;
			res = pFolder_item->GetChildren(pLibraryItems.m_Ptr);
			ASSERT(FCM_SUCCESS_CODE(res));

			FCM::U_Int32 libCnt;
			res = pLibraryItems->Count(libCnt);
			for (FCM::U_Int32 l = 0; l < libCnt; l++)        
			{		
				DOM::AutoPtr<DOM::ILibraryItem> plibrary_item = pLibraryItems[l];
				ExportLibraryItem(plibrary_item, export_lib_bitmaps, export_lib_sounds);
			}
			if(awd_res!=result::AWD_SUCCESS)
				return awd_res;
		}
		DOM::AutoPtr<DOM::LibraryItem::ISymbolItem> pSymbol_item = plibrary_item;
		if(pSymbol_item){	
			// export a symbol (timeline). If the timeline has already been encoded, nothing will happen. 
			AutoPtr<DOM::ITimeline> timeline;
			res = pSymbol_item->GetTimeLine(timeline.m_Ptr);
			ASSERT(FCM_SUCCESS_CODE(res));
		/*	TimelineEncoder* newTimeLineEncoder = new TimelineEncoder(GetCallback(), timeline, awd, flash_awd_cencoder, 0);
			awd_res = newTimeLineEncoder->encode();	
			if(awd_res!=result::AWD_SUCCESS)
				return awd_res;
				*/
	
		}
		DOM::AutoPtr<DOM::LibraryItem::IMediaItem> pMedia_item = plibrary_item;
		if(pMedia_item){	
			// export a media item. 
			FCM::PIFCMUnknown unknownMediaInfo;
			res = pMedia_item->GetMediaInfo(unknownMediaInfo); 
			ASSERT(FCM_SUCCESS_CODE(res));
			AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmapInfo = unknownMediaInfo;
			if((bitmapInfo)&&(export_lib_bitmaps)){
				flash_to_awd_encoder->ExportBitmap(pMedia_item, NULL, "");
				// export a bitmap. 
			}
			FCM::AutoPtr<DOM::MediaInfo::ISoundInfo> SoundInfo = unknownMediaInfo;
			if((SoundInfo)&&(export_lib_sounds)){
				flash_to_awd_encoder->ExportSound(pMedia_item, NULL, "");
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
