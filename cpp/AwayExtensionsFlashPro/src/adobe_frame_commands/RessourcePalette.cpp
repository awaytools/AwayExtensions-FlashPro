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

#include "AWDTimelineWriter.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

#include "RessourcePalette.h"
#include "TimelineBuilder.h"

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
	
    ResourcePalette::ResourcePalette()
	{
		this->awd_converter = NULL;
	};
	


    ResourcePalette::~ResourcePalette()
    {
    }


    void ResourcePalette::Init(FlashToAWDEncoder* awd_converter)
    {
		this->awd_converter = awd_converter;
    }

    void ResourcePalette::Clear()
    {
        m_resourceList.clear();
    }
    /* ----------------------------------------------------- Resource Palette */

	
    FCM::Result ResourcePalette::AddSymbol(
        FCM::U_Int32 resourceId, 
        FCM::StringRep16 pName, 
        Exporter::Service::PITimelineBuilder pTimelineBuilder)
    {	
        m_resourceList.push_back(resourceId);

        TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);		
        ITimelineWriter* pTimelineWriter;
        return pTimeline->Build(resourceId, pName, &pTimelineWriter);
    }


    FCM::Result ResourcePalette::AddShape(
        FCM::U_Int32 resourceId, 
        DOM::FrameElement::PIShape pShape)
    {
        m_resourceList.push_back(resourceId);
		//	this will create a new geom for this shape, add it to the AWDProject, and fill it with the path-data.
		//	the geometries will be processed later.
		//	we do not check for errors here. the created geometry_blocks can be queried for warning / error messages.
		std::string geom_res_id = std::to_string(resourceId);
		this->awd_converter->get_geom_for_shape(pShape, geom_res_id, true);
        return FCM_SUCCESS;
    }


    FCM::Result ResourcePalette::AddSound(FCM::U_Int32 resourceId, DOM::LibraryItem::PIMediaItem pMediaItem)
    {
        m_resourceList.push_back(resourceId);
		this->awd_converter->ExportSound(pMediaItem, NULL, std::to_string(resourceId));  
        return FCM_SUCCESS;
    }
	
    FCM::Result ResourcePalette::AddBitmap(FCM::U_Int32 resourceId, DOM::LibraryItem::PIMediaItem pMediaItem)
    {		
        m_resourceList.push_back(resourceId);
		this->awd_converter->ExportBitmap(pMediaItem, NULL, std::to_string(resourceId));
        return FCM_SUCCESS;
    }

    FCM::Result ResourcePalette::CreateImageFileName(DOM::ILibraryItem* pLibItem, std::string& name)
    {
        return FCM_SUCCESS;
    }

    FCM::Result ResourcePalette::CreateSoundFileName(DOM::ILibraryItem* pLibItem, std::string& name)
    {
        return FCM_SUCCESS;
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
		BASE::AWDBlock* newTextBlock;
		this->awd_converter->ExportText(pClassicText, &newTextBlock);
		return FCM_SUCCESS;
		
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

	FCM::Result ResourcePalette::HasResource(const std::string& name, FCM::Boolean& hasResource)
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
	



	/*
    FCM::Result ResourcePalette::ExportStrokeForFont(DOM::FrameElement::PIShape pIShape, AWD::FONT::FontShape* fontShape)
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
	*/

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
		/*
        FCM::Result res;
        FCM::Double thickness;
        AutoPtr<DOM::IFCMUnknown> pFillStyle;
        DOM::StrokeStyle::CAP_STYLE capStyle;
        DOM::StrokeStyle::JOIN_STYLE joinStyle;
        DOM::Utils::ScaleType scaleType;
        FCM::Boolean strokeHinting;
		
        res = pSolidStrokeStyle->GetCapStyle(capStyle);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pSolidStrokeStyle->GetJoinStyle(joinStyle);
        ASSERT(FCM_SUCCESS_CODE(res));
		*/
		/*
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
		*/
        return FCM_SUCCESS;
    }
	




};
