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

#include "FlashToAWDEncoder.h"
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
#include "TimelineEncoder.h"
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
		FlashToAWDEncoder* flash_to_awd_encoder;
		AWDProject* awd_project;
		DOM::PIFLADocument fla_document;

		FCM::PIFCMDictionary _pDict;

        bool ReadString(
            const FCM::PIFCMDictionary pDict, 
            FCM::StringRep8 key, 
            std::string& retString);

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

        FCM::Boolean IsPreviewNeeded(const PIFCMDictionary pDictConfig);

		AWD::result ExportLibraryItems(FCM::FCMListPtr pLibraryItemList, bool, bool, bool, bool, TimelineEncoder*);
		
    };
	
    FCM::Result RegisterPublisher(PIFCMDictionary pPlugins, FCM::FCMCLSID docId);
};

#endif // PUBLISHER_H_

