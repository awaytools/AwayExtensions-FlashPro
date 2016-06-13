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

#ifndef RESSOURCE_PALLETTE_H_
#define RESSOURCE_PALLETTE_H_

#include <vector>
#include "Version.h"
#include "FCMTypes.h"
#include "FCMPluginInterface.h"
#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"
#include "Publisher/IPublisher.h"
#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"
#include "FrameElement/IClassicText.h"
#include "FrameElement/ITextStyle.h"
#include "Exporter/Service/IFrameCommandGenerator.h"
#include "AWDTimelineWriter.h"
#include "awd_project.h"
/* -------------------------------------------------- Forward Decl */

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG
using namespace FCM;
using namespace Publisher;
using namespace Exporter::Service;

namespace AwayJS
{
    class CPublisher;
    class ResourcePalette;
    class TimelineBuilder;
    class TimelineBuilderFactory;
    class ITimelineWriter;
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


namespace AwayJS
{
    class ResourcePalette : public IResourcePalette, public FCMObjectBase
    {
    public:

        BEGIN_INTERFACE_MAP(ResourcePalette, AWAYJS_PLUGIN_VERSION)    
            INTERFACE_ENTRY(IResourcePalette)            
        END_INTERFACE_MAP    
		
		void* test_shape;

        virtual FCM::Result _FCMCALL AddSymbol(
            FCM::U_Int32 resourceId, 
            FCM::StringRep16 pName, 
            Exporter::Service::PITimelineBuilder pTimelineBuilder); 
		
        virtual FCM::Result _FCMCALL AddShape(
            FCM::U_Int32 resourceId, 
            DOM::FrameElement::PIShape pShape);
		
        virtual FCM::Result _FCMCALL AddSound(
            FCM::U_Int32 resourceId, 
            DOM::LibraryItem::PIMediaItem pLibItem);
		
        virtual FCM::Result _FCMCALL AddBitmap(
            FCM::U_Int32 resourceId, 
            DOM::LibraryItem::PIMediaItem pLibItem);

        virtual FCM::Result _FCMCALL AddClassicText(
            FCM::U_Int32 resourceId, 
            DOM::FrameElement::PIClassicText pClassicText);
		
        virtual FCM::Result _FCMCALL HasResource(
            FCM::U_Int32 resourceId, 
            FCM::Boolean& hasResource);
		
		

        ResourcePalette();

        ~ResourcePalette();

        void Init(AnimateToAWDEncoder* awd_encoder);

        void Clear();
		
        FCM::Result HasResource(
            const std::string& name, 
            FCM::Boolean& hasResource);

    private:

        FCM::Result ExportStrokeStyle(FCM::PIFCMUnknown pStrokeStyle);

        FCM::Result ExportSolidStrokeStyle(DOM::StrokeStyle::ISolidStrokeStyle* pSolidStrokeStyle);
		
        FCM::Result CreateImageFileName(DOM::ILibraryItem* pLibItem, std::string& name);

        FCM::Result CreateSoundFileName(DOM::ILibraryItem* pLibItem, std::string& name);

		FCM::Result GetFontInfo(DOM::FrameElement::ITextStyle* pTextStyleItem, std::string& name,FCM::U_Int16 fontSize);

        FCM::Result HasFancyStrokes(DOM::FrameElement::PIShape pShape, FCM::Boolean& hasFancy); 

        FCM::Result ConvertStrokeToFill(
            DOM::FrameElement::PIShape pShape,
            DOM::FrameElement::PIShape& pNewShape);

    private:
		
		AnimateToAWDEncoder* awd_converter;

        std::vector<FCM::U_Int32> m_resourceList;

        FCM::U_Int32 m_imageFileNameLabel;

        FCM::U_Int32 m_soundFileNameLabel;

        std::vector<std::string> m_resourceNames;
    };
	
};

#endif // PUBLISHER_H_

