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
#include <vector>
#include <fstream>
#include "Publisher.h"
#include "Utils.h"

#include "FlashFCMPublicIDs.h"
#include "ILayer.h"
#include "FrameElement/IClassicText.h"
#include "FrameElement/IParagraph.h"
#include "FrameElement/ITextRun.h"
#include "FrameElement/ITextStyle.h"
#include "FrameElement/IShape.h"
#include "FrameElement/ISound.h"
#include "FrameElement/IInstance.h"
#include "FrameElement/IGraphic.h"
#include "FrameElement/IGroup.h"
#include "FrameElement/IBitmapInstance.h"
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
#include "Service/FontTable/IKerningPair.h"

#include "Utils/DOMTypes.h"
#include "Utils/ILinearColorGradient.h"
#include "Utils/IRadialColorGradient.h"

#include "OutputWriter.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

#include "ShapePool.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG


ShapePool::ShapePool(FCM::PIFCMCallback pCallback, AWD* awd)
{
	this->shapeEncoder=new ShapeEncoder(&pCallback, awd);
		//shape_encoder->set_double_subdivide(double_subdivide);
		//shape_encoder->set_outline_threshold(outline_threshold);
		//shape_encoder->set_save_interstect(save_interstect);
    this->m_pCallback=pCallback;
	this->awd=awd;
	FCM::AutoPtr<FCM::IFCMUnknown> pUnkShapeService;
	this->m_pCallback->GetService(DOM::FLA_SHAPE_SERVICE,pUnkShapeService.m_Ptr);
	pIShapeService=pUnkShapeService.m_Ptr;
	FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
	this->m_pCallback->GetService(DOM::FLA_REGION_GENERATOR_SERVICE,pUnkSRVReg.m_Ptr);
	pIRegionGeneratorService=pUnkSRVReg.m_Ptr;

}
ShapePool::~ShapePool()
{
	delete this->shapeEncoder;
}
AWDShape2D*
ShapePool::getShape(DOM::FrameElement::IShape* thisShape)
{
	FCM::FCMListPtr pFilledRegionList;
	FCM::U_Int32 regionCount;
	FCM::Result res;	
	res = this->pIRegionGeneratorService->GetFilledRegions(thisShape, pFilledRegionList.m_Ptr);
	ASSERT(FCM_SUCCESS_CODE(res));
	
	pFilledRegionList->Count(regionCount);
	//AwayJS::Utils::Trace(this->m_pCallback, "						Fill_regions = %d\n", regionCount);
					
	for(int i : this->this_frame_shapes){		
		DOM::FrameElement::IShape* thisTestShape=this->shapes_flash[i];							
		FCM::U_Int32 regionCount_test;
		FCM::Boolean issame;
		DOM::Utils::MATRIX2D matrix;
		res = this->pIRegionGeneratorService->GetFilledRegions(thisTestShape, pFilledRegionList.m_Ptr);
		ASSERT(FCM_SUCCESS_CODE(res));
		pFilledRegionList->Count(regionCount_test);
		// if they have different regions, the shapes to not match
		if(regionCount_test==regionCount){
			res=this->pIShapeService->TestShapeSimilarity(thisTestShape, thisShape, issame, matrix);
			if(res==FCM_SUCCESS){
				if(issame){
					return this->shapes_awd[i];
				}
			}
		}
		i++;
	}
	for(int i : this->last_frame_shapes){		
		DOM::FrameElement::IShape* thisTestShape=this->shapes_flash[i];							
		FCM::U_Int32 regionCount_test;
		FCM::Boolean issame;
		DOM::Utils::MATRIX2D matrix;
		res = this->pIRegionGeneratorService->GetFilledRegions(thisTestShape, pFilledRegionList.m_Ptr);
		ASSERT(FCM_SUCCESS_CODE(res));
		pFilledRegionList->Count(regionCount_test);
		// if they have different regions, the shapes to not match
		if(regionCount_test==regionCount){
			res=this->pIShapeService->TestShapeSimilarity(thisTestShape, thisShape, issame, matrix);
			if(res==FCM_SUCCESS){
				if(issame){
					return this->shapes_awd[i];
				}
			}
		}
		i++;
	}
	string shapeName="";
	AWDShape2D* newAWDShape=new AWDShape2D(shapeName);
	this->this_frame_shapes.push_back(this->shapes_flash.size());
	this->shapes_flash.push_back(thisShape);
	this->shapes_awd.push_back(newAWDShape);
	this->encodeShape(thisShape, newAWDShape);
	return newAWDShape;

}
void ShapePool::newTimeline(){
	this->last_frame_shapes.clear();
	this->this_frame_shapes.clear();
}
void ShapePool::addFrame(){
	this->last_frame_shapes.clear();
	for(int i : this->this_frame_shapes){
		this->last_frame_shapes.push_back(i);
	}
	this->this_frame_shapes.clear();
}
void
ShapePool::encodeShape(DOM::FrameElement::IShape* thisShape, AWDShape2D* shapeBlock)
{
	
	this->shapeEncoder->reset(shapeBlock);
    ExportFill(thisShape);
    ExportStroke(thisShape);
	
    string objID_string="testShape";//AwayJS::Utils::ToString(this->shapes_flash.size());	
	shapeBlock->set_objectID( objID_string);
	this->awd->add_shape2Dblock(shapeBlock);

}


FCM::Result ShapePool::ExportFill(DOM::FrameElement::PIShape pIShape)
    {
        FCM::Result res;
        FCM::FCMListPtr pFilledRegionList;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
        FCM::U_Int32 regionCount;
		
        res = this->pIRegionGeneratorService->GetFilledRegions(pIShape, pFilledRegionList.m_Ptr);
		//Utils::Trace(GetCallback(), "Fill2\n");
        ASSERT(FCM_SUCCESS_CODE(res));

        pFilledRegionList->Count(regionCount);
		//Utils::Trace(GetCallback(), "Fill_regions = %d\n", regionCount);
		
        for (FCM::U_Int32 j = 0; j < regionCount; j++)
        {
            FCM::AutoPtr<DOM::Service::Shape::IFilledRegion> pFilledRegion = pFilledRegionList[j];
            FCM::AutoPtr<DOM::Service::Shape::IPath> pPath;

            // Fill Style
            FCM::AutoPtr<DOM::IFCMUnknown> fillStyle;

            res = pFilledRegion->GetFillStyle(fillStyle.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));
            ExportFillStyle(fillStyle);

            // Boundary
            res = pFilledRegion->GetBoundary(pPath.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            res = ExportPath(pPath, false);
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
                FCM::AutoPtr<DOM::Service::Shape::IPath> pPath = pHoleList[k];
                res = ExportPath(pPath, true);
            }
			this->shapeEncoder->encode_subShape(false);
        }

        return res;
    }

    FCM::Result ShapePool::ExportPath(DOM::Service::Shape::PIPath pPath, bool is_hole)
    {
		if(is_hole){
			this->shapeEncoder->add_hole();
		}
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
            this->SetSegment(segment);
        }
        return res;
    }
	// Sets a segment of a path (Used for boundary, holes)
	FCM::Result ShapePool::SetSegment(const DOM::Utils::SEGMENT& segment)
    {
	
		ShapePoint* startPoint= new ShapePoint();
		ShapePoint* endPoint= new ShapePoint();
		AWDPathSegment* awdPathSeg = new AWDPathSegment();
        if (segment.segmentType == DOM::Utils::LINE_SEGMENT)
        {			
			//AwayJS::Utils::Trace(m_pCallback, "Line\n");
			startPoint->x=segment.line.endPoint1.x;
			startPoint->y=segment.line.endPoint1.y;
			endPoint->x=segment.line.endPoint2.x;
			endPoint->y=segment.line.endPoint2.y;
		}
        else{
			//AwayJS::Utils::Trace(m_pCallback, "Curve\n");
			startPoint->x=segment.quadBezierCurve.anchor1.x;
			startPoint->y=segment.quadBezierCurve.anchor1.y;
			endPoint->x=segment.quadBezierCurve.anchor2.x;
			endPoint->y=segment.quadBezierCurve.anchor2.y;
			ShapePoint* controlPoint= new ShapePoint();
			controlPoint->x=segment.quadBezierCurve.control.x;
			controlPoint->y=segment.quadBezierCurve.control.y;
			awdPathSeg->set_controlPoint(controlPoint);
			awdPathSeg->set_edgeType(CURVED_EDGE);
		}	
		awdPathSeg->set_startPoint(startPoint);
		awdPathSeg->set_endPoint(endPoint);
		int hole_idx=this->shapeEncoder->get_hole_idx();
		awdPathSeg->set_hole_idx(hole_idx);
		this->shapeEncoder->add_path_segment(awdPathSeg);
		
        return FCM_SUCCESS;
    }



    FCM::Result ShapePool::ExportFillStyle(FCM::PIFCMUnknown pFillStyle)
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

	
    FCM::Result ShapePool::ExportStrokeForFont(DOM::FrameElement::PIShape pIShape, AWDFontShape* fontShape)
    {
		
        AutoPtr<FCM::IFCMUnknown> pGrad;
		
		
        FCM::FCMListPtr pStrokeGroupList;
        FCM::U_Int32 strokeStyleCount;
        FCM::Result res;
		
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
        this->m_pCallback->GetService(DOM::FLA_REGION_GENERATOR_SERVICE,pUnkSRVReg.m_Ptr);
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
			
			//m_pOutputWriter->StartDefineShape();
			std::vector<FontPathShape*> newFontPathes;
			
			for (FCM::U_Int32 k = 0; k < pathCount; k++)
			{
				bool isHole=true;
				if(k==0){
					isHole=false;
				}
				DOM::Service::Shape::IPath* pPath = ( DOM::Service::Shape::IPath*)pPathList[k];
				ASSERT(pPath);			
				FCM::U_Int32 edgeCount;
				FCM::FCMListPtr pEdgeList;

				res = pPath->GetEdges(pEdgeList.m_Ptr);
				ASSERT(FCM_SUCCESS_CODE(res));
				//if(k>0){
				//	m_pOutputWriter->StartDefineHole();}
				res = pEdgeList->Count(edgeCount);
				ExportPath(pPath, isHole);
				//Utils::Trace(GetCallback(), "Export edgeCount-pathCount count = %d.\n", edgeCount);
				ASSERT(FCM_SUCCESS_CODE(res));	
			}
			//m_pOutputWriter->EndDefineFontStroke(fontShape);
			
		}

		return FCM_SUCCESS;//res;
		
	}
	
    FCM::Result ShapePool::ExportStroke(DOM::FrameElement::PIShape pIShape)
    {
		
        FCM::FCMListPtr pStrokeGroupList;
        FCM::U_Int32 strokeStyleCount;
        FCM::Result res;
			
		
        res = this->pIRegionGeneratorService->GetStrokeGroups(pIShape, pStrokeGroupList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));
		
        res = pStrokeGroupList->Count(strokeStyleCount);
        ASSERT(FCM_SUCCESS_CODE(res));
		//Utils::Trace(GetCallback(), "Export Stroke-groups count = %d.\n", strokeStyleCount);

        AutoPtr<DOM::FillStyle::ISolidFillStyle> pSolidFillStyle = NULL;
        AutoPtr<DOM::FillStyle::IGradientFillStyle> pGradientFillStyle = NULL;
        AutoPtr<DOM::FillStyle::IBitmapFillStyle> pBitmapFillStyle = NULL;

		// use the shapeservice to convert stroke to shape
		// than handle the new shape as normal shape (exportFILL())		
					
		DOM::FrameElement::PIShape outShape;
		res=this->pIShapeService->ConvertStrokeToFill(pIShape, outShape);
		//Utils::Trace(GetCallback(), "Export Stroke count = %d .\n", res);
		if(res==FCM_SUCCESS){	
			//Utils::Trace(GetCallback(), "Export STROKE.\n");			
			ExportFill(outShape);
		}
        return res;
    }



    FCM::Result ShapePool::ExportSolidFillStyle(DOM::FillStyle::ISolidFillStyle* pSolidFillStyle)
    {
        FCM::Result res;
        DOM::Utils::COLOR color;

        AutoPtr<DOM::FillStyle::ISolidFillStyle> solidFill = pSolidFillStyle;
        ASSERT(solidFill);

        res = solidFill->GetColor(color);
        ASSERT(FCM_SUCCESS_CODE(res));

		awd_color awdColor=awdutil_int_color( color.alpha, color.red, color.green, color.blue );
		AWDShape2DFill* newFill=awd->get_solid_fill(awdColor);
		newFill->set_color_components(color.red, color.green, color.blue, color.alpha);
		this->shapeEncoder->get_shape()->set_fill(newFill);
		this->shapeEncoder->set_color(color.red, color.green, color.blue, color.alpha);

        return res;
    }

    FCM::Result ShapePool::ExportRadialGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle)
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
		/*
        res = m_pOutputWriter->StartDefineRadialGradientFillStyle(spread, matrix, focalPoint);
        ASSERT(FCM_SUCCESS_CODE(res));*/

        FCM::U_Int8 nColors;
        res = radialColorGradient->GetKeyColorCount(nColors);
        ASSERT(FCM_SUCCESS_CODE(res));

        for (FCM::U_Int8 i = 0; i < nColors; i++)
        {
            DOM::Utils::GRADIENT_COLOR_POINT point;

            res = radialColorGradient->GetKeyColorAtIndex(i, point);
            ASSERT(FCM_SUCCESS_CODE(res));
			
           // res = m_pOutputWriter->SetKeyColorPoint(point);
          //  ASSERT(FCM_SUCCESS_CODE(res));
        }

		//res = m_pOutputWriter->EndDefineRadialGradientFillStyle();
		//ASSERT(FCM_SUCCESS_CODE(res));

        return res;
    }


    FCM::Result ShapePool::ExportLinearGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle)
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
		/*
        res = m_pOutputWriter->StartDefineLinearGradientFillStyle(spread, matrix);
        ASSERT(FCM_SUCCESS_CODE(res));*/

        FCM::U_Int8 nColors;
        res = linearColorGradient->GetKeyColorCount(nColors);
        ASSERT(FCM_SUCCESS_CODE(res));

        for (FCM::U_Int8 i = 0; i < nColors; i++)
        {
            DOM::Utils::GRADIENT_COLOR_POINT point;

            res = linearColorGradient->GetKeyColorAtIndex(i, point);
            ASSERT(FCM_SUCCESS_CODE(res));

            //res = m_pOutputWriter->SetKeyColorPoint(point);
            //ASSERT(FCM_SUCCESS_CODE(res));
        }

        //res = m_pOutputWriter->EndDefineLinearGradientFillStyle();
        //ASSERT(FCM_SUCCESS_CODE(res));

        return res;
    }


    FCM::Result ShapePool::ExportBitmapFillStyle(DOM::FillStyle::IBitmapFillStyle* pBitmapFillStyle)
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
		/*
        res = m_pOutputWriter->DefineBitmapFillStyle(
            isClipped, 
            matrix, 
            height, 
            width, 
            pName, 
            pMediaItem);
        ASSERT(FCM_SUCCESS_CODE(res));
		*/

        // Free the name
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
		this->m_pCallback->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

        callocService->Free((FCM::PVoid)pName);

        return res;
    }
