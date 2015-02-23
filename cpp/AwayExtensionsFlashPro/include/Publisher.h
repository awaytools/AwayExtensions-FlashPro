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

namespace AwayJS
{
    
	// the ids have been generated specific for this plugin.

	// {D6863765-2532-47E6-83DA-BA7B575EAD27}
    const FCMCLSID CLSID_Publisher =
		{ 0xd6863765, 0x2532, 0x47e6, { 0x83, 0xda, 0xba, 0x7b, 0x57, 0x5e, 0xad, 0x27 } };
	

	// {1D69F2D7-4CAA-4B17-81D6-6CACF09BBE2C}
    const FCM::FCMCLSID CLSID_ResourcePalette =
		{ 0x1d69f2d7, 0x4caa, 0x4b17, { 0x81, 0xd6, 0x6c, 0xac, 0xf0, 0x9b, 0xbe, 0x2c } };

	// {E07B04A6-435E-4113-B902-B42A88A80AC4}
    const FCMCLSID CLSID_TimelineBuilder =
		{ 0xe07b04a6, 0x435e, 0x4113, { 0xb9, 0x2, 0xb4, 0x2a, 0x88, 0xa8, 0xa, 0xc4 } };

	// {0AA1EBCE-E6EA-4138-B45A-3EB58594A9CC}
    const FCMCLSID CLSID_TimelineBuilderFactory =
		{ 0xaa1ebce, 0xe6ea, 0x4138, { 0xb4, 0x5a, 0x3e, 0xb5, 0x85, 0x94, 0xa9, 0xcc } };
		
}


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

		AWD::result ExportLibraryItems(bool, bool);
		AWD::result ExportLibraryItem(DOM::ILibraryItem* pFlaDocument, bool, bool);
		
    };
	
    FCM::Result RegisterPublisher(PIFCMDictionary pPlugins, FCM::FCMCLSID docId);
};

#endif // PUBLISHER_H_

