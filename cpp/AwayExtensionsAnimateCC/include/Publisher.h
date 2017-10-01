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

/**
 * @file  Publisher.h
 *
 * @brief This file contains declarations for a Publisher plugin.
 */

#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include <vector>
#include "Version.h"
#include "FCMTypes.h"
#include "FCMPluginInterface.h"

#include "AnimateToAWDEncoder.h"
#include "awd_project.h"

#include "Publisher/IPublisher.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"
#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"
#include "FrameElement/IClassicText.h"
#include "FrameElement/ITextStyle.h"
#include "Exporter/Service/IFrameCommandGenerator.h"

#include "ILibraryItem.h"
#include "RessourcePalette.h"
#include "PluginConfiguration.h"



using namespace FCM;
using namespace Publisher;
using namespace Exporter::Service;

namespace AwayJS
{
    class CPublisher;
}

namespace DOM
{
    namespace Service
    {
        namespace Shape
        {
            FORWARD_DECLARE_INTERFACE(IPath);
        }
    };
};


#define OUTPUT_FILE_EXTENSION       "awd"


/* -------------------------------------------------- Enums */

/* -------------------------------------------------- Macros / Constants */



/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

namespace AwayJS
{

    class CPublisher : public IPublisher, public FCMObjectBase
    {
        BEGIN_INTERFACE_MAP(CPublisher,AWAYJS_PLUGIN_VERSION)
            INTERFACE_ENTRY(IPublisher)
        END_INTERFACE_MAP
        
    public:
		AWD::result ExportTimeline(const PIFCMDictionary pDictPublishSettings, DOM::ITimeline* pTimeline, int scene_id, std::string& folderName);
        virtual FCM::Result _FCMCALL Publish(
            DOM::PIFLADocument pFlaDocument, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig);

        virtual FCM::Result _FCMCALL Publish(
            DOM::PIFLADocument pFlaDocument, 
            DOM::PITimeline pTimeline, 
            const Exporter::Service::RANGE &frameRange, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig);
        
        virtual FCM::Result _FCMCALL ClearCache();

        CPublisher();

        ~CPublisher();

	private:
		
		DOM::PIFLADocument fla_document;
		DOM::PITimeline timeline;
		const Exporter::Service::RANGE* frameRange;
		FCM::PIFCMDictionary pDict;
		FCM::PIFCMDictionary pDictConfig;
		AnimateToAWDEncoder* animate_to_awd_encode;
		AWDProject* awd_project;
		AWD::SETTINGS::BlockSettings* awd_settings;
        std::string outFile;
        FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;
        FCM::Result res;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk2;
		
		DOM::Utils::COLOR color;
		FCM::U_Int32 stageHeight;
		FCM::U_Int32 stageWidth;
		FCM::Double fps;

        bool ReadString(
            const FCM::PIFCMDictionary pDict, 
            FCM::StringRep8 key, 
            std::string& retString);
		
        bool GetSettingsBool(
            const PIFCMDictionary pDictPublishSettings,
            std::string settingsname,
			bool default_value);
		
        double GetSettingsFloat(
            const PIFCMDictionary pDictPublishSettings,
            std::string settingsname,
			double default_value);
		
        int GetSettingsInt(
            const PIFCMDictionary pDictPublishSettings,
            std::string settingsname,
			int default_value);

        FCM::Result GetOutputFileName(        
            DOM::PIFLADocument pFlaDocument, 
            DOM::PITimeline pITimeline, 
            const PIFCMDictionary pDictPublishSettings,
            std::string& outFile);

        FCM::Result Export(
            DOM::PIFLADocument pFlaDocument, 
            DOM::PITimeline pTimeline, 
            const Exporter::Service::RANGE* pFrameRange, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig);

        AWD::result CreateAWDProject();
		
        AWD::result CreateAWDSettings();

        AWD::result CollectDocumentData();

        AWD::result ProcessData();

        AWD::result OpenFile();
		
        AWD::result PrintStatistics();

        AWD::result OpenPreview();

        FCM::Boolean IsPreviewNeeded(const PIFCMDictionary pDictConfig);

		AWD::result ExportLibraryItems(FCM::FCMListPtr pLibraryItemList, FCM::AutoPtr<FCM::IFCMCalloc> pCalloc, std::string& folderName);
		
    };
	
    FCM::Result RegisterPublisher(PIFCMDictionary pPlugins, FCM::FCMCLSID docId);
};

#endif // PUBLISHER_H_

