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

#include "OutputWriter.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

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
        if (m_pResourcePalette)
        {
            ResourcePalette* pResPalette = static_cast<ResourcePalette*>(m_pResourcePalette.m_Ptr);

            pResPalette->Clear();
        }
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

    FCM::Result CPublisher::Init()
    {
        FCM::Result res = FCM_SUCCESS;;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;

        if (!m_frameCmdGeneratorService)
        {
            // Get the frame command generator service
            res = GetCallback()->GetService(Exporter::Service::EXPORTER_FRAME_CMD_GENERATOR_SERVICE, pUnk.m_Ptr);
            m_frameCmdGeneratorService = pUnk;
        }

        if (!m_pResourcePalette)
        {
            // Create a Resource Palette
            res = GetCallback()->CreateInstance(NULL, CLSID_ResourcePalette, IID_IResourcePalette, (void**)&m_pResourcePalette);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        return res;
    }


    void CPublisher::LaunchBrowser(const std::string& outputFileName)
    {

#ifdef _WINDOWS

        //std::wstring output = L"http://localhost:8080/";
        std::wstring tail;
        tail.assign(outputFileName.begin(), outputFileName.end());
        //output += tail;
        ShellExecute(NULL, L"open", tail.c_str(), NULL, NULL, SW_SHOWNORMAL);

#else

        std::string output = outputFileName;
        std::string str = "/usr/bin/open " + output;
        system(str.c_str());

#endif // _WINDOWS

    }
	
    //
    // Note: This function is NOT completely implemented but provides guidelines 
    // on how this can be possibly done.      
    //
    FCM::Result CPublisher::ExportLibraryItems(FCM::FCMListPtr pLibraryItemList)
    {
        FCM::U_Int32 count = 0;
        FCM::Result res;


        ASSERT(pLibraryItemList);

        res = pLibraryItemList->Count(count);
        ASSERT(FCM_SUCCESS_CODE(res));

        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        res = GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

        for (FCM::U_Int32 index = 0; index < count ; index++)
        {
            FCM::StringRep16 pLibItemName = NULL;
            std::string libItemName;
            AutoPtr<IFCMDictionary> pDict;
            AutoPtr<DOM::ILibraryItem> pLibItem = pLibraryItemList[index];

            res = pLibItem->GetName(&pLibItemName);
            ASSERT(FCM_SUCCESS_CODE(res));
            libItemName = Utils::ToString(pLibItemName, GetCallback());

            AutoPtr<DOM::LibraryItem::IFolderItem> pFolderItem = pLibItem;
            if (pFolderItem)
            {
                FCM::FCMListPtr pChildren;

                res = pFolderItem->GetChildren(pChildren.m_Ptr);
                ASSERT(FCM_SUCCESS_CODE(res));

                // Export all its children
                res = ExportLibraryItems(pChildren);
                ASSERT(FCM_SUCCESS_CODE(res));
            }
            else
            {
                FCM::FCMDictRecTypeID type;
                FCM::U_Int32 valLen;
                AutoPtr<DOM::LibraryItem::IFontItem> pFontItem = pLibItem;
                AutoPtr<DOM::LibraryItem::ISymbolItem> pSymbolItem = pLibItem;
                AutoPtr<DOM::LibraryItem::IMediaItem> pMediaItem = pLibItem;

                res = pLibItem->GetProperties(pDict.m_Ptr);
                ASSERT(FCM_SUCCESS_CODE(res));

                res = pDict->GetInfo(kLibProp_LinkageIdentifier_DictKey, 
                    type, valLen);

                if (FCM_SUCCESS_CODE(res))
                {
                    FCM::Boolean hasResource;
                    ResourcePalette* pResPalette = static_cast<ResourcePalette*>(m_pResourcePalette.m_Ptr);

                    // Library Item has linkage identifer

                    if (pSymbolItem)
                    {
                        //
                        // Check if it has been exported already by comparing names of resources 
                        // already exported from the timelines.
                        //
                        res = pResPalette->HasResource(libItemName, hasResource);
                        if (!hasResource)
                        {
                            // Resource is not yet exported. Export it using 
                            // FrameCommandGenerator::GenerateFrameCommands
                        }
                    }
                    else if (pMediaItem)
                    {
                        //
                        // Check if it has been exported already by comparing names of resources 
                        // already exported from the timelines.
                        //
                        res = pResPalette->HasResource(libItemName, hasResource);
                        if (!hasResource)
                        {
                            // Resource is not yet exported. Export it.

                            // Depending on bitmap/sound, export it.
                        }
                    }
                    else if (pFontItem)
                    {
                        // Use the font name to check if already exported.
                        
                        // Use IFontTableGeneratorService::CreateFontTableForFontItem() to create a font table
                        // and then export it.
                    }
                }
            }

            callocService->Free((FCM::PVoid)pLibItemName);
        }
        return FCM_SUCCESS;
    }


};
