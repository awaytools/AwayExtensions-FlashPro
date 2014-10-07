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
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
        FCM::U_Int32 regionCount;
		
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

	
    FCM::Result ResourcePalette::ExportStrokeForFont(DOM::FrameElement::PIShape pIShape)
    {
		
        AutoPtr<FCM::IFCMUnknown> pGrad;
		
		
        FCM::FCMListPtr pStrokeGroupList;
        FCM::U_Int32 strokeStyleCount;
        FCM::Result res;
		/*
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

            //res = m_pOutputWriter->StartDefineStrokeGroup();
            //ASSERT(FCM_SUCCESS_CODE(res));

            //AutoPtr<FCM::IFCMUnknown> pStrokeStyle;
            //pStrokeGroup->GetStrokeStyle(pStrokeStyle.m_Ptr);

            //DOM::Utils::COLOR color = {};

            FCMListPtr pPathList;
            FCM::U_Int32 pathCount;

            res = pStrokeGroup->GetPaths(pPathList.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = pPathList->Count(pathCount);
            ASSERT(FCM_SUCCESS_CODE(res));
			
			//Utils::Trace(GetCallback(), "Stroke-path-Count = %d.\n", pathCount);
			bool windingBool_start=true;
            if(pathCount>200){
				Utils::Trace(GetCallback(), "Skipping glyph because Stroke-pathCount count is to big.");
			}
			else{
				*/
				/*
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

					res = pEdgeList->Count(edgeCount);
				
					//Utils::Trace(GetCallback(), "Export edgeCount-pathCount count = %d.\n", edgeCount);
					ASSERT(FCM_SUCCESS_CODE(res));	
					if(edgeCount>0){
					
						FCM::Result res;
		
						FCM::U_Int32 edgeCount;
						FCM::FCMListPtr pEdgeList;
						res = pPath->GetEdges(pEdgeList.m_Ptr);
						ASSERT(FCM_SUCCESS_CODE(res));

						res = pEdgeList->Count(edgeCount);
						ASSERT(FCM_SUCCESS_CODE(res));

					
						double startx=0;
						double starty=0;
						double endx=0;
						double endy=0;
						std::vector<AWDPathSegment*> newSegs;
						for (FCM::U_Int32 l = 0; l < edgeCount; l++)
						{
			
							DOM::Utils::SEGMENT segment;
							segment.structSize = sizeof(DOM::Utils::SEGMENT);
							FCM::AutoPtr<DOM::Service::Shape::IEdge> pEdge = pEdgeList[l];			
							res = pEdge->GetSegment(segment);
							AWDPathSegment* newSeg=new AWDPathSegment();
							if (segment.segmentType == DOM::Utils::LINE_SEGMENT)
								{
									if(l==0){
										startx=segment.line.endPoint1.x;
										starty=segment.line.endPoint1.y;
									}
									if(l==edgeCount-1){
										endx=segment.line.endPoint2.x;
										endy=segment.line.endPoint2.y;
									}
									newSeg->set_startPoint(new SimplePoint(segment.line.endPoint1.x, segment.line.endPoint1.y));	
									newSeg->set_endPoint(new SimplePoint(segment.line.endPoint2.x, segment.line.endPoint2.y));	
								}
							else{
									if(l==0){
										startx=segment.quadBezierCurve.anchor1.x;
										starty=segment.quadBezierCurve.anchor1.y;
									}
									if(l==edgeCount-1){
										endx=segment.quadBezierCurve.anchor2.x;
										endy=segment.quadBezierCurve.anchor2.y;
									}
									newSeg->set_startPoint(new SimplePoint(segment.quadBezierCurve.anchor1.x, segment.quadBezierCurve.anchor1.y));	
									newSeg->set_endPoint(new SimplePoint(segment.quadBezierCurve.anchor2.x, segment.quadBezierCurve.anchor2.y));	
									newSeg->set_controlPoint(new SimplePoint(segment.quadBezierCurve.control.x, segment.quadBezierCurve.control.y));	
									newSeg->set_edgeType(CURVED_EDGE);
							}
							newSegs.push_back(newSeg);
						}
						if((startx!=endx)||(starty!=endy)){
							Utils::Trace(GetCallback(), "PATH IS NOT CLOSED: TODO: Convert unClosed pathes into shapes and export those\n");
						}
						else{		
							FCM::PIFCMCallback pCallback = GetCallback();
							FontPathShape* newFontshape=new FontPathShape(&pCallback, newSegs, k);
							vector<vector<AWDPathSegment*> > newShapePathes = newFontshape->remove_intersecting();
							if(newShapePathes.size() > 0){
								newFontPathes.push_back(newFontshape);
								for (FCM::U_Int32 l = 1; l < newShapePathes.size(); l++){
									vector<AWDPathSegment*> shapePath = newShapePathes[l];
									if(shapePath.size()>2){
										//Utils::Trace(GetCallback(), "Add segment\n");
										FontPathShape* newFontshape2=new FontPathShape(&pCallback, shapePath, newFontPathes.size());
										newFontPathes.push_back(newFontshape2);
									}
									else{
										//Utils::Trace(GetCallback(), "Skipped invalide Path (segmentcount smaller 3)\n");
									}
								}
							}
							else{
								Utils::Trace(GetCallback(), "PATH IS EMPTY\n");
							}

						}

					}*/
			//	}

				// combine FontPathShapes (put holes into fills...)
	/*
					res = m_pOutputWriter->StartDefineStroke();
					ASSERT(FCM_SUCCESS_CODE(res));

					res = ExportStrokeStyle(pStrokeStyle);
					ASSERT(FCM_SUCCESS_CODE(res));
				
					AutoPtr<DOM::FillStyle::ISolidFillStyle> pSolidFillStyle;
	*/
			/*
				std::vector<FontPathShape*> newFontPathes2=arrangeFontPathes(newFontPathes);	
				for (FCM::U_Int32 k = 0; k < newFontPathes2.size(); k++)
				{	
					std::vector<AWDPathSegment*> newSegs = newFontPathes2[k]->get_main_path();
				
					for (AWDPathSegment* oneSeg : newSegs)
					{
						m_pOutputWriter->SetAWDSegment(oneSeg);
					}
					std::vector<FontPathShape*> holes=newFontPathes2[k]->get_holes();
					for (FontPathShape* fontShape : holes)
					{		
						m_pOutputWriter->StartDefineHole();
						std::vector<AWDPathSegment*> newSegs2 = fontShape->get_main_path();
						for (AWDPathSegment* oneSeg2 : newSegs2)
						{
							m_pOutputWriter->SetAWDSegment(oneSeg2);
						}
					}
					m_pOutputWriter->EndDefineFontStroke();
				
				}
				m_pOutputWriter->EndDefineShapeLetter();
				*/
		//	}
		/*
				double xNumber=0.0;
				double yNumber=0.0;
				double oneLine=0.0;
				for (FCM::U_Int32 l = 0; l < edgeCount; l++)
				{
				FCM::U_Int32 edgeCount;
				FCM::FCMListPtr pEdgeList;

				res = pPath->GetEdges(pEdgeList.m_Ptr);
				ASSERT(FCM_SUCCESS_CODE(res));

				res = pEdgeList->Count(edgeCount);
				ASSERT(FCM_SUCCESS_CODE(res));
				ExportPath(pPath);
					DOM::Utils::SEGMENT segment;

					segment.structSize = sizeof(DOM::Utils::SEGMENT);

					FCM::AutoPtr<DOM::Service::Shape::IEdge> pEdge = pEdgeList[l];

					res = pEdge->GetSegment(segment);
					if (segment.segmentType == DOM::Utils::LINE_SEGMENT)
					{
						xNumber=segment.line.endPoint1.x;
						yNumber=segment.line.endPoint1.y;
						oneLine+=(segment.line.endPoint2.x-segment.line.endPoint1.x)*(segment.line.endPoint2.y+segment.line.endPoint1.y);	
					}
					else{
						xNumber=segment.quadBezierCurve.anchor1.x;
						yNumber=segment.quadBezierCurve.anchor1.y;
						oneLine+=(segment.quadBezierCurve.anchor2.x-segment.quadBezierCurve.anchor1.x)*(segment.quadBezierCurve.anchor2.y+segment.quadBezierCurve.anchor1.y);	
					}
				}
				bool windingBool=true;
				if(oneLine<0){
					windingBool=false;
				}
				Utils::Trace(GetCallback(), "WINDING ORDER = %f\n", oneLine);
				if(k==0){
					Utils::Trace(GetCallback(), "NEW filled Path\n");
					//prevPath=pPath;
					//windingBool_start=windingBool;
					ExportPath(pPath);
				}
				else{*/
					/*
					//prevPath = pPathList[k-1];
					bool isHole=PointInPolygon(xNumber, yNumber, prevPath);
					if(isHole){
						Utils::Trace(GetCallback(), "Hole in previous Path\n");
						m_pOutputWriter->StartDefineHole();
						ExportPath(pPath);
					}
					else{
						prevPath=pPath;
						m_pOutputWriter->EndDefineFill();// finalize the previous path. 
						ExportPath(pPath);
						Utils::Trace(GetCallback(), "NEW filled Path\n");
					}
					*/
					/*
					// same winding as first path, so this must be a new shape-element
					if(windingBool_start==windingBool){
						m_pOutputWriter->EndDefineFill();// finalize the previous path. 
						ExportPath(pPath);
						Utils::Trace(GetCallback(), "NEW filled Path\n");
					}
					// not same winding as first path, so this must be a hole in the previous path
					else{
					}
					*/
				

              //  res = m_pOutputWriter->EndDefineStroke();
               // ASSERT(FCM_SUCCESS_CODE(res));
            

           // res = m_pOutputWriter->EndDefineStrokeGroup();
           // ASSERT(FCM_SUCCESS_CODE(res));
       // }
		
		return FCM_SUCCESS;//res;
		
	}
	
    std::vector<FontPathShape*> ResourcePalette::arrangeFontPathes(std::vector<FontPathShape*> fontPathes)
    {
		FontPathShape* thisFontshapetest;
		FontPathShape* thisFontshape;
		int pathesCount=fontPathes.size();
        for (FCM::U_Int32 k = 0; k < pathesCount; k++)
        {
			thisFontshape=fontPathes[k];
			std::vector<AWDPathSegment*> newSegs = fontPathes[k]->get_main_path();
			if(thisFontshape->get_parentID()<0){
				// TO DO: we are testing each path against each path. This can be optimizted (?)
				for (FCM::U_Int32 tcnt = 0; tcnt < pathesCount; tcnt++){
					if(tcnt!=k){
						thisFontshapetest=fontPathes[tcnt];
						std::vector<AWDPathSegment*> pPath = fontPathes[tcnt]->get_main_path();		
						bool isInPath=PointInPolygon(thisFontshape->get_startX(), thisFontshape->get_startY(), pPath);
						if(isInPath){
							bool isInPath2=PointInPolygon(thisFontshape->get_endX(), thisFontshape->get_endY(), pPath);
							if(isInPath2){
								thisFontshape->set_parentID(thisFontshapetest->get_pathID());
								thisFontshapetest->add_hole(thisFontshape);
								tcnt=pathesCount;
							}
						}
					}
				}
			}
		}
		vector<FontPathShape*> return_Shapes;
        for (FCM::U_Int32 k = 0; k < pathesCount; k++)
        {
			thisFontshape=fontPathes[k];
			if(thisFontshape->get_parentID()<0){
				return_Shapes.push_back(thisFontshape);
			}
		}
        for (FCM::U_Int32 k = 0; k < return_Shapes.size(); k++)
        {
			thisFontshape=return_Shapes[k];
			std::vector<AWDPathSegment*> pPath = return_Shapes[k]->get_main_path();		
			for (FCM::U_Int32 fsCnt = 0; fsCnt <  thisFontshape->get_holes().size(); fsCnt++){
				FontPathShape* fs=thisFontshape->get_holes()[fsCnt];
				for (FCM::U_Int32 fsChildCnt = 0; fsChildCnt <  fs->get_holes().size(); fsChildCnt++){
					FontPathShape* fsChild=fs->get_holes()[fsChildCnt];
						fsChild->set_parentID(-1);
						return_Shapes.push_back(fsChild);
					
				}
				fs->get_holes().clear();
			}
		}
		pathesCount=return_Shapes.size();
        for (FCM::U_Int32 k = 0; k < pathesCount; k++)
        {
			thisFontshape=return_Shapes[k];
			std::vector<AWDPathSegment*> newSegs = return_Shapes[k]->get_main_path();
			if(thisFontshape->get_parentID()<0){
				// TO DO: we are testing each path against each path. This can be optimizted (?)
				for (FCM::U_Int32 tcnt = 0; tcnt < pathesCount; tcnt++){
					if(tcnt!=k){
						thisFontshapetest=return_Shapes[tcnt];
						std::vector<AWDPathSegment*> pPath = return_Shapes[tcnt]->get_main_path();		
						bool isInPath=PointInPolygon(thisFontshape->get_startX(), thisFontshape->get_startY(), pPath);
						if(isInPath){
							bool isInPath2=PointInPolygon(thisFontshape->get_endX(), thisFontshape->get_endY(), pPath);
							if(isInPath2){
								bool isInHole=false;
								for (FCM::U_Int32 fsCnt = 0; fsCnt <  thisFontshapetest->get_holes().size(); fsCnt++){
									FontPathShape* fs=thisFontshapetest->get_holes()[fsCnt];
									std::vector<AWDPathSegment*> pPath2 = fs->get_main_path();	
									bool isInPathHole=PointInPolygon(thisFontshape->get_startX(), thisFontshape->get_startY(), pPath2);
									if(isInPathHole){
										isInHole=true;
										fsCnt=thisFontshapetest->get_holes().size();
									}
								}
								if(!isInHole){
									thisFontshape->set_parentID(thisFontshapetest->get_pathID());
									thisFontshapetest->add_hole(thisFontshape);
									tcnt=pathesCount;
								}
							}
						}
					}
				}
			}
		}
		vector<FontPathShape*> return_Shapes_final;
        for (FCM::U_Int32 k = 0; k < pathesCount; k++)
        {
			thisFontshape=return_Shapes[k];
			if(thisFontshape->get_parentID()<0){
				return_Shapes_final.push_back(thisFontshape);
			}
		}
		return return_Shapes_final;

	}
    bool ResourcePalette::PointInPolygon(double x, double y,  std::vector<AWDPathSegment*> segments)
    {
		
        bool oddNodes = false;
		/*

		std::vector<SimplePoint*> newPoints;
        for (AWDPathSegment* seg : segments)
        {
			newPoints.push_back(seg->get_startPoint());
		}

        int j = newPoints.size() - 1;

        for (int i = 0; i < newPoints.size(); i++)
        {
			//Utils::Trace(GetCallback(), "check points %f vs %f\n", newPoints[i]->x, newPoints[i]->y);
            if (((newPoints[i]->y < y) && (newPoints[j]->y >= y))||
                ((newPoints[j]->y < y) && (newPoints[i]->y >= y)))
            {
                if ((newPoints[i]->x + (y - newPoints[i]->y)/(newPoints[j]->y - newPoints[i]->y)*(newPoints[j]->x - newPoints[i]->x)) < x)
                {
                    oddNodes = !oddNodes;
                }
            }
            j = i;
        }
		*/
        return oddNodes;
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
