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

#include <iostream>
#include <fstream>
#include "Publisher.h"
#include "Utils.h"
#include "FlashFCMPublicIDs.h"
#include "awd_project.h"
#include "Utils/DOMTypes.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

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

#include "Utils/ILinearColorGradient.h"
#include "Utils/IRadialColorGradient.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder2.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

#include "AWDTimelineWriter.h"
#include "RessourcePalette.h"
#include "TimelineBuilder.h"
#include "blocks/mesh_library.h"
#include "blocks/billboard.h"

namespace AwayJS
{
	using namespace AWD;
    /* ----------------------------------------------------- CPublisher */
	
    CPublisher::CPublisher()
    {
    }

    CPublisher::~CPublisher()
    {

    }
	

    FCM::Result CPublisher::Publish(
        DOM::PIFLADocument pFlaDocument, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
        return Export(pFlaDocument, NULL, NULL, pDictPublishSettings, pDictConfig);
    }

    // This function will be currently called in "Test-Scene" workflow. 
    // In future, it might be called in other workflows as well. 
    FCM::Result CPublisher::Publish(
        DOM::PIFLADocument pFlaDocument, 
        DOM::PITimeline pTimeline, 
        const Exporter::Service::RANGE &frameRange, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
        return Export(pFlaDocument, pTimeline, &frameRange, pDictPublishSettings, pDictConfig);
    }
	

    FCM::Result CPublisher::Export(
        DOM::PIFLADocument pFlaDocument, 
        DOM::PITimeline pTimeline, 
        const Exporter::Service::RANGE* pFrameRange, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
		this->fla_document = pFlaDocument;
		this->timeline=pTimeline;
		this->frameRange=pFrameRange;
		this->pDict=pDictPublishSettings;
		this->pDictConfig=pDictConfig;

        this->pCalloc = AwayJS::Utils::GetCallocService(GetCallback());
		ASSERT(pCalloc.m_Ptr != NULL);

        Utils::Trace(GetCallback(), "Publishing AwayJS-document (AWD export)\n");

        res = GetOutputFileName(this->fla_document, this->timeline, this->pDict, this->outFile);
        if (FCM_FAILURE_CODE(res))
        {
            // FLA is untitled. Ideally, we should use a temporary location for output generation.
            // However, for now, we report an error.
            Utils::Trace(GetCallback(), "\nFailed to publish. Either save the AwayJS-Document or provide a output path in publish settings.\n");
            return res;
        }

		AWD::result awd_result;

		awd_result = this->CreateAWDProject();
		if(awd_result!=AWD::result::AWD_SUCCESS) {
			Utils::Trace(GetCallback(), "\nFailed to create a AWD project !\n"); 
			Utils::Trace(GetCallback(), "\nErrorMessage: %s\n", AWD::get_string_for_result(awd_result).c_str());
			return FCM_EXPORT_FAILED;
		}
		
		awd_result = this->CreateAWDSettings();
		if(awd_result!=AWD::result::AWD_SUCCESS) {
			Utils::Trace(GetCallback(), "\nFailed to create AWD settings !\n"); 
			Utils::Trace(GetCallback(), "\nErrorMessage: %s\n", AWD::get_string_for_result(awd_result).c_str());
			return FCM_EXPORT_FAILED;
		}

		awd_result = this->CollectDocumentData();
		if(awd_result!=AWD::result::AWD_SUCCESS) {
			Utils::Trace(GetCallback(), "\nFailed to collect document data !\n");
			Utils::Trace(GetCallback(), "\nErrorMessage: %s\n", AWD::get_string_for_result(awd_result).c_str());
			return FCM_EXPORT_FAILED;
		}

		awd_result = this->ProcessData();
		if(awd_result!=AWD::result::AWD_SUCCESS) {
			Utils::Trace(GetCallback(), "\nFailed to process the data !\n");
			Utils::Trace(GetCallback(), "\nErrorMessage: %s\n", AWD::get_string_for_result(awd_result).c_str());
			return FCM_EXPORT_FAILED;
		}
		
		awd_result = this->awd_project->export_file();
		if(awd_result!=AWD::result::AWD_SUCCESS) {
			Utils::Trace(GetCallback(), "\nFailed to export the data to file !\n");
			Utils::Trace(GetCallback(), "\nErrorMessage: %s\n", AWD::get_string_for_result(awd_result).c_str());
			return FCM_EXPORT_FAILED;
		}

		awd_result = this->PrintStatistics();
		if(awd_result!=AWD::result::AWD_SUCCESS) {
			Utils::Trace(GetCallback(), "\nFailed to print statistics for the exported data !\n");
			Utils::Trace(GetCallback(), "\nErrorMessage: %s\n", AWD::get_string_for_result(awd_result).c_str());
			return FCM_EXPORT_FAILED;
		}
		
		awd_result = this->OpenPreview();
		if(awd_result!=AWD::result::AWD_SUCCESS) {
			Utils::Trace(GetCallback(), "\nFailed to open the exported file !\n");
			Utils::Trace(GetCallback(), "\nErrorMessage: %s\n", AWD::get_string_for_result(awd_result).c_str());
			return FCM_EXPORT_FAILED;
		}
		
			

		// copy the Preview files to output directory
		
		Utils::Trace(GetCallback(), "\nAwayJS-Document-Export Complete. ");
		delete this->awd_project;
		delete this->animate_to_awd_encode;
					
		//_CrtDumpMemoryLeaks();
        return FCM_SUCCESS;
    }

    FCM::Result RegisterPublisher(PIFCMDictionary pPlugins, FCM::FCMCLSID docId)
    {
        FCM::Result res;
        /*
         * Dictionary structure for a Publisher plugin is as follows:
         *
         *  Level 0 :    
         *              --------------------------------
         *             | Application.Component |  ----- | -----------------------------
         *              --------------------------------                               |
         *                                                                             |
         *  Level 1:                                   <-------------------------------
         *              ------------------------------  
         *             | CLSID_Publisher_GUID | ----- | -------------------------------
         *              ------------------------------                                 |
         *                                                                             |
         *  Level 2:                                      <---------------------------- 
         *              ---------------------------------------------------
         *             | Application.Component.Category.Publisher |  ----- |-----------
         *              ---------------------------------------------------            |
         *                                                                             |
         *  Level 3:                                                           <-------
         *              -------------------------------------------------------------------------
         *             | Application.Component.Category.Name           | PUBLISHER_NAME          |
         *              -------------------------------------------------------------------------|
         *             | Application.Component.Category.UniversalName  | PUBLISHER_UNIVERSAL_NAME|
         *              -------------------------------------------------------------------------|
         *             | Application.Component.Publisher.UI            | PUBLISH_SETTINGS_UI_ID  |
         *              -------------------------------------------------------------------------|
         *             | Application.Component.Publisher.TargetDocs    |              -----------|--
         *              -------------------------------------------------------------------------| |
         *                                                                                         |
         *  Level 4:                                                    <--------------------------
         *              -----------------------------------------------
         *             | CLSID_DocType   |  Empty String               |
         *              -----------------------------------------------
		 */

        {
            // Level 1 Dictionary
            AutoPtr<IFCMDictionary> pPlugin;
            res = pPlugins->AddLevel(
                (const FCM::StringRep8)Utils::ToString(CLSID_Publisher).c_str(), 
                pPlugin.m_Ptr);

            {
                // Level 2 Dictionary
                AutoPtr<IFCMDictionary> pCategory;
                res = pPlugin->AddLevel(
                    (const FCM::StringRep8)kFlashCategoryKey_Publisher, //kApplicationCategoryKey_Publisher
                    pCategory.m_Ptr);

                {
                    // Level 3 Dictionary
                    std::string str_name = PUBLISHER_NAME;
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_Name, //kApplicationCategoryKey_Name
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)str_name.c_str(),
                        (FCM::U_Int32)str_name.length() + 1);

                    std::string str_uniname = PUBLISHER_UNIVERSAL_NAME;
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_UniversalName, //kApplicationCategoryKey_UniversalName
                        kFCMDictType_StringRep8,
                        (FCM::PVoid)str_uniname.c_str(),
                        (FCM::U_Int32)str_uniname.length() + 1);

                    std::string str_ui = PUBLISH_SETTINGS_UI_ID;
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_UI,		//kApplicationPublisherKey_UI
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)str_ui.c_str(),
                        (FCM::U_Int32)str_ui.length() + 1);
					
                    AutoPtr<IFCMDictionary> pDocs;
                    res = pCategory->AddLevel((const FCM::StringRep8)kFlashPublisherKey_TargetDocs, pDocs.m_Ptr);//kApplicationPublisherKey_TargetDocs

                    {
                        // Level 4 Dictionary
                        std::string empytString = "";   
                        res = pDocs->Add(
                            (const FCM::StringRep8)Utils::ToString(docId).c_str(), 
                            kFCMDictType_StringRep8, 
                            (FCM::PVoid)empytString.c_str(),
                            (FCM::U_Int32)empytString.length() + 1);
                    }
                }
            }
        }

        return res;
    }

};
