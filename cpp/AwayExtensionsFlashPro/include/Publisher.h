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

#include "OutputWriter.h"
#include "awd.h"

/* -------------------------------------------------- Forward Decl */

using namespace FCM;
using namespace Publisher;
using namespace Exporter::Service;

namespace AwayJS
{
    class CPublisher;
    class ResourcePalette;
    class TimelineBuilder;
    class TimelineBuilderFactory;
    class IOutputWriter;
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


#define OUTPUT_FILE_EXTENSION       "awd"


/* -------------------------------------------------- Enums */

/* -------------------------------------------------- Macros / Constants */

namespace AwayJS
{
    // {835B2A74-9646-43AD-BA86-A35F4E0ECD1B}//original from adobe
	// {D6863765-2532-47E6-83DA-BA7B575EAD27}
    const FCMCLSID CLSID_Publisher =
		{ 0xd6863765, 0x2532, 0x47e6, { 0x83, 0xda, 0xba, 0x7b, 0x57, 0x5e, 0xad, 0x27 } };
       // {0x835b2a74, 0x9646, 0x43ad, {0xba, 0x86, 0xa3, 0x5f, 0x4e, 0xe, 0xcd, 0x1b}};//original from adobe
    
    // {D5830903-02D6-4133-A1F1-F272D29A1802}//original from adobe
	// {1D69F2D7-4CAA-4B17-81D6-6CACF09BBE2C}
    const FCM::FCMCLSID CLSID_ResourcePalette =
		{ 0x1d69f2d7, 0x4caa, 0x4b17, { 0x81, 0xd6, 0x6c, 0xac, 0xf0, 0x9b, 0xbe, 0x2c } };
       // {0xd5830903, 0x2d6, 0x4133, {0xa1, 0xf1, 0xf2, 0x72, 0xd2, 0x9a, 0x18, 0x2}};//original from adobe

    // {70E17C74-34DF-4C46-8F9D-900D2EC045E3}//original from adobe
	// {E07B04A6-435E-4113-B902-B42A88A80AC4}
    const FCMCLSID CLSID_TimelineBuilder =
		{ 0xe07b04a6, 0x435e, 0x4113, { 0xb9, 0x2, 0xb4, 0x2a, 0x88, 0xa8, 0xa, 0xc4 } };
       // {0x70e17c74, 0x34df, 0x4c46, {0x8f, 0x9d, 0x90, 0xd, 0x2e, 0xc0, 0x45, 0xe3}};//original from adobe

    // {442305C9-F02F-43A4-B259-D1CC39666A94}//original from adobe
	// {0AA1EBCE-E6EA-4138-B45A-3EB58594A9CC}
    const FCMCLSID CLSID_TimelineBuilderFactory =
		{ 0xaa1ebce, 0xe6ea, 0x4138, { 0xb4, 0x5a, 0x3e, 0xb5, 0x85, 0x94, 0xa9, 0xcc } };
       // {0x442305c9, 0xf02f, 0x43a4, {0xb2, 0x59, 0xd1, 0xcc, 0x39, 0x66, 0x6a, 0x94}};//original from adobe

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
		
		void GenerateFonts(DOM::PIFLADocument pFlaDocument, ResourcePalette* pResPalette);
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

        FCM::Result Init();

        void LaunchBrowser(const std::string& outputFileName);
		
        FCM::Result ExportLibraryItems(FCM::FCMListPtr pLibraryItemList);

    private:

        AutoPtr<IFrameCommandGenerator> m_frameCmdGeneratorService;
        AutoPtr<IResourcePalette> m_pResourcePalette;
    };


    class ResourcePalette : public IResourcePalette, public FCMObjectBase
    {
    public:

        BEGIN_INTERFACE_MAP(ResourcePalette, AWAYJS_PLUGIN_VERSION)    
            INTERFACE_ENTRY(IResourcePalette)            
        END_INTERFACE_MAP    
		
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

        virtual FCM::Result _FCMCALL AddMovieClip(
            FCM::U_Int32 resourceId, 
            FCM::StringRep16 pName, 
            Exporter::Service::PITimelineBuilder pTimelineBuilder);

        virtual FCM::Result _FCMCALL AddGraphic(
            FCM::U_Int32 resourceId, 
            Exporter::Service::PITimelineBuilder pTimelineBuilder);


        virtual FCM::Result _FCMCALL AddBitmap(
            FCM::U_Int32 resourceId, 
            DOM::LibraryItem::PIMediaItem pLibItem);

        virtual FCM::Result _FCMCALL AddClassicText(
            FCM::U_Int32 resourceId, 
            DOM::FrameElement::PIClassicText pClassicText);
		
        virtual FCM::Result _FCMCALL HasResource(
            FCM::U_Int32 resourceId, 
            FCM::Boolean& hasResource);
		

        FCM::Result ExportFill(DOM::FrameElement::PIShape pIShape);

        FCM::Result ExportStroke(DOM::FrameElement::PIShape pIShape);

        FCM::Result ExportStrokeForFont(DOM::FrameElement::PIShape pIShape);

		bool PointInPolygon(double x, double y,  std::vector<AWDPathSegment*> );

		std::vector<FontPathShape*> arrangeFontPathes(std::vector<FontPathShape*> fontPathes);


        ResourcePalette();

        ~ResourcePalette();

        void Init(IOutputWriter* pOutputWriter);

        void Clear();
		
        FCM::Result HasResource(
            const std::string& name, 
            FCM::Boolean& hasResource);

    private:

        FCM::Result ExportStrokeStyle(FCM::PIFCMUnknown pStrokeStyle);

        FCM::Result ExportSolidStrokeStyle(DOM::StrokeStyle::ISolidStrokeStyle* pSolidStrokeStyle);

        FCM::Result ExportFillStyle(FCM::PIFCMUnknown pFillStyle);

        FCM::Result ExportFillBoundary(DOM::Service::Shape::PIPath pPath);

        FCM::Result ExportHole(DOM::Service::Shape::PIPath pPath);

        FCM::Result ExportPath(DOM::Service::Shape::PIPath pPath);

        FCM::Result ExportSolidFillStyle(
            DOM::FillStyle::ISolidFillStyle* pSolidFillStyle);

        FCM::Result ExportRadialGradientFillStyle(
            DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportLinearGradientFillStyle(
            DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportBitmapFillStyle(
            DOM::FillStyle::IBitmapFillStyle* pBitmapFillStyle);

        FCM::Result CreateImageFileName(DOM::ILibraryItem* pLibItem, std::string& name);

        FCM::Result CreateSoundFileName(DOM::ILibraryItem* pLibItem, std::string& name);

		FCM::Result GetFontInfo(DOM::FrameElement::ITextStyle* pTextStyleItem, std::string& name,FCM::U_Int16 fontSize);

        FCM::Result HasFancyStrokes(DOM::FrameElement::PIShape pShape, FCM::Boolean& hasFancy); 

        FCM::Result ConvertStrokeToFill(
            DOM::FrameElement::PIShape pShape,
            DOM::FrameElement::PIShape& pNewShape);

    private:
		
        IOutputWriter* m_pOutputWriter;

        std::vector<FCM::U_Int32> m_resourceList;
        FCM::U_Int32 m_imageFileNameLabel;

        FCM::U_Int32 m_soundFileNameLabel;

        std::vector<std::string> m_resourceNames;
    };
	


	class TimelineBuilder : public ITimelineBuilder, public FCMObjectBase
    {
    public:

		BEGIN_INTERFACE_MAP(TimelineBuilder, AWAYJS_PLUGIN_VERSION)    
            INTERFACE_ENTRY(ITimelineBuilder)              
        END_INTERFACE_MAP    

        virtual FCM::Result _FCMCALL AddShape(
            FCM::U_Int32 objectId, 
            SHAPE_INFO* pShapeInfo);

        virtual FCM::Result _FCMCALL AddClassicText(
            FCM::U_Int32 objectId, 
            CLASSIC_TEXT_INFO* pClassicTextInfo);
            
        virtual FCM::Result _FCMCALL AddBitmap(
            FCM::U_Int32 objectId, 
            BITMAP_INFO* pBitmapInfo);
            
        virtual FCM::Result _FCMCALL AddMovieClip(
            FCM::U_Int32 objectId, 
            MOVIE_CLIP_INFO* pMovieClipInfo, 
            DOM::FrameElement::PIMovieClip pMovieClip);
            
        virtual FCM::Result _FCMCALL AddGraphic(
            FCM::U_Int32 objectId, 
            GRAPHIC_INFO* pGraphicInfo);

        virtual FCM::Result _FCMCALL AddSound(
            FCM::U_Int32 objectId, 
            SOUND_INFO* pSoundInfo, 
            DOM::FrameElement::PISound pSound);

        virtual FCM::Result _FCMCALL UpdateZOrder(
            FCM::U_Int32 objectId, 
            FCM::U_Int32 placeAfterObjectId);

        virtual FCM::Result _FCMCALL Remove(FCM::U_Int32 objectId);
            
        virtual FCM::Result _FCMCALL UpdateBlendMode(
            FCM::U_Int32 objectId, 
            DOM::FrameElement::BlendMode blendMode);

        virtual FCM::Result _FCMCALL UpdateVisibility(
            FCM::U_Int32 objectId, 
            FCM::Boolean visible);

        virtual FCM::Result _FCMCALL UpdateGraphicFilter(
            FCM::U_Int32 objectId, 
            PIFCMList pFilterable);
            
        virtual FCM::Result _FCMCALL UpdateDisplayTransform(
            FCM::U_Int32 objectId, 
            const DOM::Utils::MATRIX2D& matrix);
            
        virtual FCM::Result _FCMCALL UpdateColorTransform(
            FCM::U_Int32 objectId, 
            const DOM::Utils::COLOR_MATRIX& colorMatrix);

        virtual FCM::Result _FCMCALL ShowFrame();        

        virtual FCM::Result _FCMCALL AddFrameScript(FCM::CStringRep16 pScript, FCM::U_Int32 layerNum);

        virtual FCM::Result _FCMCALL RemoveFrameScript(FCM::U_Int32 layerNum);

        virtual FCM::Result _FCMCALL SetFrameLabel(FCM::StringRep16 pLabel, DOM::KeyFrameLabelType labelType);

        TimelineBuilder();

        ~TimelineBuilder();

        virtual FCM::Result Build(
            FCM::U_Int32 resourceId, 
            FCM::StringRep16 pName,
            ITimelineWriter** ppTimelineWriter);

        void Init(IOutputWriter* pOutputWriter, AWD *);

    private:

        AWDOutputWriter* m_pOutputWriter;

        AWDTimelineWriter* m_pTimelineWriter;

        FCM::U_Int32 m_frameIndex;
    };


    class TimelineBuilderFactory : public ITimelineBuilderFactory, public FCMObjectBase
    {
    public:

        BEGIN_INTERFACE_MAP(TimelineBuilderFactory, AWAYJS_PLUGIN_VERSION)    
            INTERFACE_ENTRY(ITimelineBuilderFactory)            
        END_INTERFACE_MAP    

        virtual FCM::Result _FCMCALL CreateTimelineBuilder(PITimelineBuilder& pTimelineBuilder);

        TimelineBuilderFactory();

        ~TimelineBuilderFactory();

        void Init(IOutputWriter* pOutputWriter);

    private:

        AWDOutputWriter* m_pOutputWriter;
    };

    FCM::Result RegisterPublisher(PIFCMDictionary pPlugins, FCM::FCMCLSID docId);
};

#endif // PUBLISHER_H_

