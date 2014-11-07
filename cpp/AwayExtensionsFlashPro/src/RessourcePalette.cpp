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

	
    /* ----------------------------------------------------- Resource Palette */

	
    FCM::Result ResourcePalette::AddSymbol(
        FCM::U_Int32 resourceId, 
        FCM::StringRep16 pName, 
        Exporter::Service::PITimelineBuilder pTimelineBuilder)
    {
        FCM::Result res;
        ITimelineWriter* pTimelineWriter;

        LOG(("[EndSymbol] ResId: %d\n", resourceId));

        m_resourceList.push_back(resourceId);

        if (pName != NULL)
        {
            m_resourceNames.push_back(Utils::ToString(pName, GetCallback()));
        }

        TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);

        res = pTimeline->Build(resourceId, pName, &pTimelineWriter);

        return res;
    }


    FCM::Result ResourcePalette::AddMovieClip(
        FCM::U_Int32 resourceId, 
        FCM::StringRep16 pName, 
        Exporter::Service::PITimelineBuilder pTimelineBuilder)
    {
        FCM::Result res;
        ITimelineWriter* pTimelineWriter;

        m_resourceList.push_back(resourceId);

        TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);

        res = pTimeline->Build(resourceId, pName, &pTimelineWriter);

        return res;
    }

    FCM::Result ResourcePalette::AddGraphic(
        FCM::U_Int32 resourceId, 
        Exporter::Service::PITimelineBuilder pTimelineBuilder)
    {
        FCM::Result res;
        ITimelineWriter* pTimelineWriter;

        m_resourceList.push_back(resourceId);

        TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);

        res = pTimeline->Build(resourceId, NULL, &pTimelineWriter);

        return res;
    }


    FCM::Result ResourcePalette::AddShape(
        FCM::U_Int32 resourceId, 
        DOM::FrameElement::PIShape pShape)
    {
        m_resourceList.push_back(resourceId);

		// the awd shape gets created
        m_pOutputWriter->StartDefineShape();
		// awd shape gets filled (subshapes are getting calculated / added)
        if (pShape)
        {
            ExportFill(pShape);
            ExportStroke(pShape);
        }

		// awd-shape is added to awd-object
        m_pOutputWriter->EndDefineShape(resourceId);

        return FCM_SUCCESS;
    }


    FCM::Result ResourcePalette::AddSound(FCM::U_Int32 resourceId, DOM::LibraryItem::PIMediaItem pMediaItem)
    {
        FCM::Result res;
        std::string name;
        DOM::AutoPtr<DOM::ILibraryItem> pLibItem;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnknown;
        FCM::StringRep16 pName;

        LOG(("[DefineSound] ResId: %d\n", resourceId));
		
        m_resourceList.push_back(resourceId);


        res = pMediaItem->GetMediaInfo(pUnknown.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        AutoPtr<DOM::MediaInfo::ISoundInfo> pSoundInfo = pUnknown;
        ASSERT(pSoundInfo);
        pLibItem = pMediaItem;
        
        // Store the resource name
        res = pLibItem->GetName(&pName);
        ASSERT(FCM_SUCCESS_CODE(res));
        m_resourceNames.push_back(Utils::ToString(pName, GetCallback()));
        CreateSoundFileName(pLibItem, name);
        
		//Utils::Trace(GetCallback(), "The Sound-file: '%s' encountered.\n", name.c_str());

        m_pOutputWriter->DefineSound(resourceId, name, pMediaItem);

        // Free the name
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        res = GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

        callocService->Free((FCM::PVoid)pName);

        return FCM_SUCCESS;
    }


    FCM::Result ResourcePalette::AddBitmap(FCM::U_Int32 resourceId, DOM::LibraryItem::PIMediaItem pMediaItem)
    {
        DOM::AutoPtr<DOM::ILibraryItem> pLibItem;
        FCM::StringRep16 pName;
        FCM::Result res;

        m_resourceList.push_back(resourceId);

        // Get name
        pLibItem = pMediaItem;
        pLibItem->GetName(&pName);

        AutoPtr<FCM::IFCMUnknown> medInfo;
        pMediaItem->GetMediaInfo(medInfo.m_Ptr);

        AutoPtr<DOM::MediaInfo::IBitmapInfo> bitsInfo = medInfo;
        ASSERT(bitsInfo);

        // Get image height
        FCM::S_Int32 height;
        res = bitsInfo->GetHeight(height);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Get image width
        FCM::S_Int32 width;
        res = bitsInfo->GetWidth(width);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Dump the definition of a bitmap
        res = m_pOutputWriter->DefineBitmap(resourceId, height, width, pName, pMediaItem);

        // Free the name
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

        callocService->Free((FCM::PVoid)pName);

        return res;
    }
    FCM::Result ResourcePalette::CreateImageFileName(DOM::ILibraryItem* pLibItem, std::string& name)
    {
        FCM::StringRep16 pLibName;
        FCM::Result res;
        std::string str;
        std::size_t pos;
        std::string fileLabel;

        res = pLibItem->GetName(&pLibName);
        ASSERT(FCM_SUCCESS_CODE(res));

        str = Utils::ToString(pLibName, GetCallback());

        fileLabel = Utils::ToString(m_imageFileNameLabel);
        name = "Image" + fileLabel;
        m_imageFileNameLabel++;

        // DOM APIs do not provide a way to get the compression of the image.
        // For time being, we will use the extension of the library item name.
        pos = str.rfind(".");
        if (pos != std::string::npos)
        {
            if (str.substr(pos + 1) == "jpg")
            {
                name += ".jpg";
            }
            else if (str.substr(pos + 1) == "png")
            {
                name += ".png";
            }
            else
            {
                name += ".png";
            }
        }
        else
        {
            name += ".png";
        }

         // Free the name
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        res = GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

        callocService->Free((FCM::PVoid)pLibName);

        return res;
    }

    FCM::Result ResourcePalette::CreateSoundFileName(DOM::ILibraryItem* pLibItem, std::string& name)
    {
        FCM::StringRep16 pLibName;
        FCM::Result res;
        std::string str;
        std::string fileLabel;
        std::size_t pos;

        res = pLibItem->GetName(&pLibName);
        ASSERT(FCM_SUCCESS_CODE(res));
        
        str = Utils::ToString(pLibName, GetCallback());
        fileLabel = Utils::ToString(m_soundFileNameLabel);
        
        name = "Sound" + fileLabel;
        m_soundFileNameLabel++;


        // DOM APIs do not provide a way to get the compression of the sound.
        // For time being, we will use the extension of the library item name.
        pos = str.rfind(".");
        if (pos != std::string::npos)
        {
            if (str.substr(pos + 1) == "wav")
            {
                name += ".WAV";
            }
            else if (str.substr(pos + 1) == "mp3")
            {
                name += ".MP3";
            }
            else
            {
                name += ".MP3";
            }
        }
        else
        {
            name += ".MP3";
        }
        
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        res = GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;
        callocService->Free((FCM::PVoid)pLibName);
        return res;
    }
	FCM::Result ResourcePalette::GetFontInfo(DOM::FrameElement::ITextStyle* pTextStyleItem, std::string& name, FCM::U_Int16 fontSize)
    {
        FCM::StringRep16 pFontName;
        FCM::StringRep8 pFontStyle;
        FCM::Result res;
        std::string str;
        std::string sizeStr;
        std::string styleStr;

        res = pTextStyleItem->GetFontName(&pFontName);
        ASSERT(FCM_SUCCESS_CODE(res));
		
        res = pTextStyleItem->GetFontStyle(&pFontStyle);
        ASSERT(FCM_SUCCESS_CODE(res));

        styleStr = pFontStyle;
        if(styleStr == "BoldItalicStyle")
            styleStr = "italic bold";
        else if(styleStr == "BoldStyle")
            styleStr = "bold";
        else if(styleStr == "ItalicStyle")
            styleStr = "italic";
        else if(styleStr == "RegularStyle")
            styleStr = "";

        sizeStr = Utils::ToString(fontSize);
        str = Utils::ToString(pFontName,GetCallback());
        //name = styleStr+" "+sizeStr + "px" + " " + "'" + str + "'" ;
        name = styleStr+"-'" + str + "'" ;

        // Free the name
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        res = GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

        callocService->Free((FCM::PVoid)pFontName);

        return res;
    }


    FCM::Result ResourcePalette::AddClassicText(FCM::U_Int32 resourceId, DOM::FrameElement::PIClassicText pClassicText)
    {
        FCM::Result res;     
		res = m_pOutputWriter->DefineText(resourceId,pClassicText); 
        return res;
		
    }
	

    FCM::Result ResourcePalette::HasResource(FCM::U_Int32 resourceId, FCM::Boolean& hasResource)
    {
        hasResource = false;

        for (std::vector<FCM::U_Int32>::iterator listIter = m_resourceList.begin();
                listIter != m_resourceList.end(); listIter++)
        {
            if (*listIter == resourceId)
            {
                hasResource = true;
                break;
            }
        }

        //LOG(("[HasResource] ResId: %d HasResource: %d\n", resourceId, hasResource));

        return FCM_SUCCESS;
    }

	
    FCM::Result ResourcePalette::HasResource(
            const std::string& name, 
            FCM::Boolean& hasResource)
    {
        hasResource = false;
        for (FCM::U_Int32 index = 0; index < m_resourceNames.size(); index++)
        {
            if (m_resourceNames[index] == name)
            {
                hasResource = true;
                break;
            }
        }

        return FCM_SUCCESS;
    }


    ResourcePalette::ResourcePalette()
    {
        m_pOutputWriter = NULL;
    }


    ResourcePalette::~ResourcePalette()
    {
    }


    void ResourcePalette::Init(IOutputWriter* pOutputWriter)
    {
        m_pOutputWriter = pOutputWriter;
    }

    void ResourcePalette::Clear()
    {
        m_resourceList.clear();
    }

    FCM::Result ResourcePalette::ExportFill(DOM::FrameElement::PIShape pIShape)
    {
        FCM::Result res;
        FCM::FCMListPtr pFilledRegionList;
        FCM::U_Int32 regionCount;
		
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
        GetCallback()->GetService(DOM::FLA_REGION_GENERATOR_SERVICE,pUnkSRVReg.m_Ptr);
        AutoPtr<DOM::Service::Shape::IRegionGeneratorService> pIRegionGeneratorService(pUnkSRVReg);
		//Utils::Trace(GetCallback(), "Fill1\n");
        ASSERT(pIRegionGeneratorService);

        res = pIRegionGeneratorService->GetFilledRegions(pIShape, pFilledRegionList.m_Ptr);
		//Utils::Trace(GetCallback(), "Fill2\n");
        ASSERT(FCM_SUCCESS_CODE(res));

        pFilledRegionList->Count(regionCount);
		//Utils::Trace(GetCallback(), "Fill_regions = %d\n", regionCount);
		
        for (FCM::U_Int32 j = 0; j < regionCount; j++)
        {
            FCM::AutoPtr<DOM::Service::Shape::IFilledRegion> pFilledRegion = pFilledRegionList[j];
            FCM::AutoPtr<DOM::Service::Shape::IPath> pPath;

            m_pOutputWriter->StartDefineFill();

            // Fill Style
            FCM::AutoPtr<DOM::IFCMUnknown> fillStyle;

            res = pFilledRegion->GetFillStyle(fillStyle.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            ExportFillStyle(fillStyle);

            // Boundary
            res = pFilledRegion->GetBoundary(pPath.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = ExportFillBoundary(pPath);
            ASSERT(FCM_SUCCESS_CODE(res));

            // Hole List
			
            FCMListPtr pHoleList;
            FCM::U_Int32 holeCount;

            res = pFilledRegion->GetHoles(pHoleList.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = pHoleList->Count(holeCount);
            ASSERT(FCM_SUCCESS_CODE(res));

            for (FCM::U_Int32 k = 0; k < holeCount; k++)
            {
                FCM::FCMListPtr pEdgeList;
                FCM::AutoPtr<DOM::Service::Shape::IPath> pPath = pHoleList[k];

                res = ExportHole(pPath);
            }
			

            m_pOutputWriter->EndDefineFill();
        }

        return res;
    }


    FCM::Result ResourcePalette::ExportFillBoundary(DOM::Service::Shape::PIPath pPath)
    {
        FCM::Result res;

        m_pOutputWriter->StartDefineBoundary();

        res = ExportPath(pPath);
        ASSERT(FCM_SUCCESS_CODE(res));

        m_pOutputWriter->EndDefineBoundary();

        return res;
    }


    FCM::Result ResourcePalette::ExportHole(DOM::Service::Shape::PIPath pPath)
    {
        FCM::Result res;

        m_pOutputWriter->StartDefineHole();

        res = ExportPath(pPath);
        ASSERT(FCM_SUCCESS_CODE(res));

        m_pOutputWriter->EndDefineHole();

        return res;
    }


    FCM::Result ResourcePalette::ExportPath(DOM::Service::Shape::PIPath pPath)
    {
        FCM::Result res;
        FCM::U_Int32 edgeCount;
        FCM::FCMListPtr pEdgeList;

        res = pPath->GetEdges(pEdgeList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pEdgeList->Count(edgeCount);
        ASSERT(FCM_SUCCESS_CODE(res));
		
		int oneLine=0;
        for (FCM::U_Int32 l = 0; l < edgeCount; l++)
        {
            DOM::Utils::SEGMENT segment;

            segment.structSize = sizeof(DOM::Utils::SEGMENT);

            FCM::AutoPtr<DOM::Service::Shape::IEdge> pEdge = pEdgeList[l];

            res = pEdge->GetSegment(segment);
            m_pOutputWriter->SetSegment(segment);
        }
        return res;
    }

    FCM::Result ResourcePalette::ExportFillStyle(FCM::PIFCMUnknown pFillStyle)
    {
        FCM::Result res = FCM_SUCCESS;

        AutoPtr<DOM::FillStyle::ISolidFillStyle> pSolidFillStyle;
        AutoPtr<DOM::FillStyle::IGradientFillStyle> pGradientFillStyle;
        AutoPtr<DOM::FillStyle::IBitmapFillStyle> pBitmapFillStyle;

        // Check for solid fill color
        pSolidFillStyle = pFillStyle;
        if (pSolidFillStyle)
        {
            res = ExportSolidFillStyle(pSolidFillStyle);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        // Check for Gradient Fill
        pGradientFillStyle = pFillStyle;
        AutoPtr<FCM::IFCMUnknown> pGrad;

        if (pGradientFillStyle)
        {
            pGradientFillStyle->GetColorGradient(pGrad.m_Ptr);

            if (AutoPtr<DOM::Utils::IRadialColorGradient>(pGrad))
            {
                res = ExportRadialGradientFillStyle(pGradientFillStyle);
                ASSERT(FCM_SUCCESS_CODE(res));
            }
            else if (AutoPtr<DOM::Utils::ILinearColorGradient>(pGrad))
            {
                res = ExportLinearGradientFillStyle(pGradientFillStyle);
                ASSERT(FCM_SUCCESS_CODE(res));
            }
        }

        pBitmapFillStyle = pFillStyle;
        if (pBitmapFillStyle)
        {
            res = ExportBitmapFillStyle(pBitmapFillStyle);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        return res;
    }

	
    FCM::Result ResourcePalette::ExportStrokeForFont(DOM::FrameElement::PIShape pIShape, AWDFontShape* fontShape)
    {
		
        AutoPtr<FCM::IFCMUnknown> pGrad;
		
		
        FCM::FCMListPtr pStrokeGroupList;
        FCM::U_Int32 strokeStyleCount;
        FCM::Result res;
		
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
        GetCallback()->GetService(DOM::FLA_REGION_GENERATOR_SERVICE,pUnkSRVReg.m_Ptr);
        AutoPtr<DOM::Service::Shape::IRegionGeneratorService> pIRegionGeneratorService(pUnkSRVReg);
        ASSERT(pIRegionGeneratorService);
		
		
        res = pIRegionGeneratorService->GetStrokeGroups(pIShape, pStrokeGroupList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));
		
        res = pStrokeGroupList->Count(strokeStyleCount);
        ASSERT(FCM_SUCCESS_CODE(res));
		//Utils::Trace(GetCallback(), "Export Stroke-groups count = %d.\n", strokeStyleCount);
		
        for (FCM::U_Int32 j = 0; j < strokeStyleCount; j++)
        {
            AutoPtr<DOM::Service::Shape::IStrokeGroup> pStrokeGroup = pStrokeGroupList[j];
            ASSERT(pStrokeGroup);

            FCMListPtr pPathList;
            FCM::U_Int32 pathCount;

            res = pStrokeGroup->GetPaths(pPathList.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = pPathList->Count(pathCount);
            ASSERT(FCM_SUCCESS_CODE(res));
			
			m_pOutputWriter->StartDefineShape();
			std::vector<FontPathShape*> newFontPathes;
			
			for (FCM::U_Int32 k = 0; k < pathCount; k++)
			{
				DOM::Service::Shape::IPath* pPath = ( DOM::Service::Shape::IPath*)pPathList[k];
				ASSERT(pPath);			
				FCM::U_Int32 edgeCount;
				FCM::FCMListPtr pEdgeList;

				res = pPath->GetEdges(pEdgeList.m_Ptr);
				ASSERT(FCM_SUCCESS_CODE(res));
				if(k>0){
					m_pOutputWriter->StartDefineHole();}
				res = pEdgeList->Count(edgeCount);
				ExportPath(pPath);
				//Utils::Trace(GetCallback(), "Export edgeCount-pathCount count = %d.\n", edgeCount);
				ASSERT(FCM_SUCCESS_CODE(res));	
			}
			m_pOutputWriter->EndDefineFontStroke(fontShape);
			
		}

		return FCM_SUCCESS;//res;
		
	}
	
    FCM::Result ResourcePalette::ExportStroke(DOM::FrameElement::PIShape pIShape)
    {
		
        FCM::FCMListPtr pStrokeGroupList;
        FCM::U_Int32 strokeStyleCount;
        FCM::Result res;

        FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
        GetCallback()->GetService(DOM::FLA_REGION_GENERATOR_SERVICE,pUnkSRVReg.m_Ptr);
        AutoPtr<DOM::Service::Shape::IRegionGeneratorService> pIRegionGeneratorService(pUnkSRVReg);
        ASSERT(pIRegionGeneratorService);
		
		
        res = pIRegionGeneratorService->GetStrokeGroups(pIShape, pStrokeGroupList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));
		
        res = pStrokeGroupList->Count(strokeStyleCount);
        ASSERT(FCM_SUCCESS_CODE(res));
		//Utils::Trace(GetCallback(), "Export Stroke-groups count = %d.\n", strokeStyleCount);

        AutoPtr<DOM::FillStyle::ISolidFillStyle> pSolidFillStyle = NULL;
        AutoPtr<DOM::FillStyle::IGradientFillStyle> pGradientFillStyle = NULL;
        AutoPtr<DOM::FillStyle::IBitmapFillStyle> pBitmapFillStyle = NULL;

		// use the shapeservice to convert stroke to shape
		// than handle the new shape as normal shape (exportFILL())		

		FCM::AutoPtr<FCM::IFCMUnknown> pUnkShapeService;
		//FCM::AutoPtr<FCM::IFCMUnknown> outShape;
		GetCallback()->GetService(DOM::FLA_SHAPE_SERVICE,pUnkShapeService.m_Ptr);
		AutoPtr<DOM::Service::Shape::IShapeService > pIShapeService(pUnkShapeService);
		
		ASSERT(pIShapeService);
			
		DOM::FrameElement::PIShape outShape;
		res=pIShapeService->ConvertStrokeToFill(pIShape, outShape);
		//Utils::Trace(GetCallback(), "Export Stroke count = %d .\n", res);
		if(res==FCM_SUCCESS){	
			//Utils::Trace(GetCallback(), "Export STROKE.\n");			
			ExportFill(outShape);
		}
        return res;
    }


    FCM::Result ResourcePalette::ExportStrokeStyle(FCM::PIFCMUnknown pStrokeStyle)
    {
        FCM::Result res = FCM_SUCCESS;
        AutoPtr<DOM::StrokeStyle::ISolidStrokeStyle> pSolidStrokeStyle;

        pSolidStrokeStyle = pStrokeStyle;

        if (pSolidStrokeStyle)
        {
            res = ExportSolidStrokeStyle(pSolidStrokeStyle);
        }
        else
        {
            // Other stroke styles are not tested yet.
        }

        return res;
    }


    FCM::Result ResourcePalette::ExportSolidStrokeStyle(DOM::StrokeStyle::ISolidStrokeStyle* pSolidStrokeStyle)
    {
        FCM::Result res;
        FCM::Double thickness;
        AutoPtr<DOM::IFCMUnknown> pFillStyle;
        DOM::StrokeStyle::CAP_STYLE capStyle;
        DOM::StrokeStyle::JOIN_STYLE joinStyle;
        DOM::Utils::ScaleType scaleType;
        FCM::Boolean strokeHinting;
		/*
        res = pSolidStrokeStyle->GetCapStyle(capStyle);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pSolidStrokeStyle->GetJoinStyle(joinStyle);
        ASSERT(FCM_SUCCESS_CODE(res));
		*/
        res = pSolidStrokeStyle->GetThickness(thickness);
        ASSERT(FCM_SUCCESS_CODE(res));

        if (thickness < 0.1)
        {
			//AwayJS::Utils::Trace(GetCallback(),"Thickness is smaller 0.1 !\n");
            thickness = 0.1;
        }
        res = pSolidStrokeStyle->GetScaleType(scaleType);
        ASSERT(FCM_SUCCESS_CODE(res));
		//AwayJS::Utils::Trace(GetCallback(),AwayJS::Utils::ToString(scaleType).c_str());

        res = pSolidStrokeStyle->GetStrokeHinting(strokeHinting);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = m_pOutputWriter->StartDefineSolidStrokeStyle(
            thickness, 
            joinStyle, 
            capStyle, 
            scaleType, 
            strokeHinting);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Stroke fill styles
        res = pSolidStrokeStyle->GetFillStyle(pFillStyle.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));
		
        AutoPtr<DOM::FillStyle::ISolidFillStyle> pSolidFillStyle=pFillStyle;
		if(pSolidFillStyle){
			DOM::Utils::COLOR color;
			pSolidFillStyle->GetColor(color);
			//AwayJS::Utils::Trace(GetCallback(),AwayJS::Utils::ToString(color).c_str());
			//AwayJS::Utils::Trace(GetCallback(),"newFIll\n");
		}
       // res = ExportFillStyle(pFillStyle);
       // ASSERT(FCM_SUCCESS_CODE(res));

        res = m_pOutputWriter->EndDefineSolidStrokeStyle();
        ASSERT(FCM_SUCCESS_CODE(res));

        return res;
    }


    FCM::Result ResourcePalette::ExportSolidFillStyle(DOM::FillStyle::ISolidFillStyle* pSolidFillStyle)
    {
        FCM::Result res;
        DOM::Utils::COLOR color;

        AutoPtr<DOM::FillStyle::ISolidFillStyle> solidFill = pSolidFillStyle;
        ASSERT(solidFill);

        res = solidFill->GetColor(color);
        ASSERT(FCM_SUCCESS_CODE(res));

        m_pOutputWriter->DefineSolidFillStyle(color);

        return res;
    }


    FCM::Result ResourcePalette::ExportRadialGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle)
    {
        DOM::FillStyle::GradientSpread spread;

        AutoPtr<FCM::IFCMUnknown> pGrad;

        AutoPtr<DOM::FillStyle::IGradientFillStyle> gradientFill = pGradientFillStyle;
        FCM::Result res = gradientFill->GetSpread(spread);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = gradientFill->GetColorGradient(pGrad.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        AutoPtr<DOM::Utils::IRadialColorGradient> radialColorGradient = pGrad;
        ASSERT(radialColorGradient);

        DOM::Utils::MATRIX2D matrix;
        res = gradientFill->GetMatrix(matrix);
        ASSERT(FCM_SUCCESS_CODE(res));

        FCM::S_Int32 focalPoint = 0;
        res = radialColorGradient->GetFocalPoint(focalPoint);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = m_pOutputWriter->StartDefineRadialGradientFillStyle(spread, matrix, focalPoint);
        ASSERT(FCM_SUCCESS_CODE(res));

        FCM::U_Int8 nColors;
        res = radialColorGradient->GetKeyColorCount(nColors);
        ASSERT(FCM_SUCCESS_CODE(res));

        for (FCM::U_Int8 i = 0; i < nColors; i++)
        {
            DOM::Utils::GRADIENT_COLOR_POINT point;

            res = radialColorGradient->GetKeyColorAtIndex(i, point);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = m_pOutputWriter->SetKeyColorPoint(point);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        res = m_pOutputWriter->EndDefineRadialGradientFillStyle();
        ASSERT(FCM_SUCCESS_CODE(res));

        return res;
    }


    FCM::Result ResourcePalette::ExportLinearGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle)
    {
        DOM::FillStyle::GradientSpread spread;
        AutoPtr<FCM::IFCMUnknown> pGrad;

        AutoPtr<DOM::FillStyle::IGradientFillStyle> gradientFill = pGradientFillStyle;
        FCM::Result res = gradientFill->GetSpread(spread);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = gradientFill->GetColorGradient(pGrad.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        AutoPtr<DOM::Utils::ILinearColorGradient> linearColorGradient = pGrad;
        ASSERT(linearColorGradient);

        DOM::Utils::MATRIX2D matrix;
        res = gradientFill->GetMatrix(matrix);
        ASSERT(FCM_SUCCESS_CODE(res));
		// TODO: add this gradient on the SubShape that is currently parsed

        res = m_pOutputWriter->StartDefineLinearGradientFillStyle(spread, matrix);
        ASSERT(FCM_SUCCESS_CODE(res));

        FCM::U_Int8 nColors;
        res = linearColorGradient->GetKeyColorCount(nColors);
        ASSERT(FCM_SUCCESS_CODE(res));

        for (FCM::U_Int8 i = 0; i < nColors; i++)
        {
            DOM::Utils::GRADIENT_COLOR_POINT point;

            res = linearColorGradient->GetKeyColorAtIndex(i, point);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = m_pOutputWriter->SetKeyColorPoint(point);
            ASSERT(FCM_SUCCESS_CODE(res));
        }

        res = m_pOutputWriter->EndDefineLinearGradientFillStyle();
        ASSERT(FCM_SUCCESS_CODE(res));

        return res;
    }


    FCM::Result ResourcePalette::ExportBitmapFillStyle(DOM::FillStyle::IBitmapFillStyle* pBitmapFillStyle)
    {
        DOM::AutoPtr<DOM::ILibraryItem> pLibItem;
        DOM::AutoPtr<DOM::LibraryItem::IMediaItem> pMediaItem;
        FCM::StringRep16 pName;
        FCM::Result res;
        FCM::Boolean isClipped;
        DOM::Utils::MATRIX2D matrix;

        // IsClipped ?
        res = pBitmapFillStyle->IsClipped(isClipped);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Matrix
		// this is the uv ? apply to shape2d block.
        res = pBitmapFillStyle->GetMatrix(matrix);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Get name
        res = pBitmapFillStyle->GetBitmap(pMediaItem.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        pLibItem = pMediaItem;
        pLibItem->GetName(&pName);

        AutoPtr<FCM::IFCMUnknown> medInfo;
        pMediaItem->GetMediaInfo(medInfo.m_Ptr);

        AutoPtr<DOM::MediaInfo::IBitmapInfo> bitsInfo = medInfo;
        ASSERT(bitsInfo);

        // Get image height
        FCM::S_Int32 height;
        res = bitsInfo->GetHeight(height);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Get image width
        FCM::S_Int32 width;
        res = bitsInfo->GetWidth(width);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Dump the definition of a bitmap fill style
        res = m_pOutputWriter->DefineBitmapFillStyle(
            isClipped, 
            matrix, 
            height, 
            width, 
            pName, 
            pMediaItem);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Free the name
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

        callocService->Free((FCM::PVoid)pName);

        return res;
    }




};
