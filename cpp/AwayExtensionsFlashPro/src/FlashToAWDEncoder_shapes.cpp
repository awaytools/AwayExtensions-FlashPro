#ifdef _WINDOWS
#include <Windows.h>
#include "ShellApi.h"
#endif
#include <vector>
#include <fstream>
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

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

#include "FlashToAWDEncoder.h"
#include "FCMPluginInterface.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG



/**
\brief Get a BLOCKS::Geometry for a DOM::FrameElement::IShape.\n
If adobeFrameCommands is false, the function will compare exising BLOCKS::Geometry and their connected IShapes to the IShape, using the TestShapeSimilarity provided by XDK.\n
If no Geometry exists for this shape, it will create one, and fill it with the shape-data (creating Materials on the way).
*/

BLOCKS::Geometry*
FlashToAWDEncoder::get_geom_for_shape(DOM::FrameElement::IShape* thisShape, const std::string& timeline_name, bool adobeFrameCommands)
{
	if(this->scene_geoms.size()==0)
		this->prepare_new_scene();
	if(this->scene_geoms.back().size()==0)
		this->prepare_new_timeline();
	if(this->scene_geoms.back().back().size()==0)
		this->prepare_new_frame();
	// if called with adobeFrameCommands = true, this will not contain the name, but the object_id
	std::string shape_name = timeline_name;	
	shape_name = "_frame_" + std::to_string(this->scene_geoms.back().back().size()) + "_shape_" + std::to_string(this->scene_geoms.back().back().back().size());
	BLOCKS::Geometry* newGeometry = NULL;

	if(!adobeFrameCommands){
		FCM::FCMListPtr pFilledRegionList;
		FCM::U_Int32 regionCount;
		FCM::Result res;	
		res = this->pIRegionGeneratorService->GetFilledRegions(thisShape, pFilledRegionList.m_Ptr);
		ASSERT(FCM_SUCCESS_CODE(res));
	
		pFilledRegionList->Count(regionCount);
		//AwayJS::Utils::Trace(this->m_pCallback, "						Fill_regions = %d\n", regionCount);
	
		/*
		std::vector<BLOCKS::Geometry*> same_frame_geos = this->scene_geoms.back().back().back();

		// check if the same shape can be found on same frame.
		for(BLOCKS::Geometry* this_geom : same_frame_geos){		
			DOM::FrameElement::IShape* thisTestShape=reinterpret_cast<DOM::FrameElement::IShape*>(this_geom->get_external_object());
			if(thisTestShape==NULL)
				AwayJS::Utils::Trace(this->m_pCallback, "ERROR!!! could not get IShape* from awd_geom!");
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
						return this_geom;
					}
				}
			}
		}

		// check if the same shape can be found on previous frame (if any previous frame exists)
		if(this->scene_geoms.back().back().size()>1){
			std::vector<BLOCKS::Geometry*> last_frame_geoms = this->scene_geoms.back().back()[this->scene_geoms.back().back().size()-2];
			for(BLOCKS::Geometry* this_geom : last_frame_geoms){			
				DOM::FrameElement::IShape* thisTestShape=reinterpret_cast<DOM::FrameElement::IShape*>(this_geom->get_external_object());					
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
							return this_geom;
						}
					}
				}
			}
		}
		if(false){ //todo: add option for checking against all geometries/shapes of current timeline.
			std::vector<std::vector<BLOCKS::Geometry*> > timline_geoms = this->scene_geoms.back().back();
			// we already checked against all geos within the last two frames of this timeline
			if(timline_geoms.size()>2){
				for(int i = 0; i< timline_geoms.size()-2; i++){
					std::vector<BLOCKS::Geometry*> last_frame_geoms = timline_geoms[i];
					for(BLOCKS::Geometry* this_geom : last_frame_geoms){			
						DOM::FrameElement::IShape* thisTestShape=reinterpret_cast<DOM::FrameElement::IShape*>(this_geom->get_external_object());					
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
									return this_geom;
								}
							}
						}
					}
				}
			}
		}
		if(false){ //todo: add option for checking against all timelines within this scene.		
			std::vector<std::vector<std::vector<BLOCKS::Geometry*> > > all_timelines = this->scene_geoms.back();
			if(all_timelines.size()>1){
				for(int i = 0; i< all_timelines.size()-1; i++){
					std::vector<std::vector<BLOCKS::Geometry*> > timline_geoms = all_timelines[i];
					for(std::vector<BLOCKS::Geometry*> frame_geoms : timline_geoms){		
						for(BLOCKS::Geometry* this_geom : frame_geoms){			
							DOM::FrameElement::IShape* thisTestShape=reinterpret_cast<DOM::FrameElement::IShape*>(this_geom->get_external_object());					
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
										return this_geom;
									}
								}
							}
						}
					}
				}
			}
		}
		if(false){ //todo: add option for checking against all scenes within this document.		
			std::vector<std::vector<std::vector<BLOCKS::Geometry*> > > all_timelines = this->scene_geoms.back();
			if(this->scene_geoms.size()>1){
				for(int i = 0; i< this->scene_geoms.size()-1; i++){
					std::vector<std::vector<std::vector<BLOCKS::Geometry*> > > scene_geoms = this->scene_geoms[i];
					for(std::vector<std::vector<BLOCKS::Geometry*> > timline_geoms : scene_geoms){	
						for(std::vector<BLOCKS::Geometry*> frame_geoms : timline_geoms){		
							for(BLOCKS::Geometry* this_geom : frame_geoms){			
								DOM::FrameElement::IShape* thisTestShape=reinterpret_cast<DOM::FrameElement::IShape*>(this_geom->get_external_object());					
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
											return this_geom;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		*/
		//	if the shape made it here, no matching shape was found.
		//	so we create a geometry for it.
		shape_name = timeline_name + shape_name;
		newGeometry = reinterpret_cast<BLOCKS::Geometry*>(this->awd_project->get_block_by_name_and_type(shape_name, BLOCK::block_type::TRI_GEOM, true));
	
		this->scene_geoms.back().back().back().push_back(newGeometry);
		// set the IShape as external object on geomtry
		newGeometry->set_external_object(thisShape);
	}

	else{
		// if we use adobe-frame-commands, shape_name will be the geometry_object_id
		newGeometry = reinterpret_cast<BLOCKS::Geometry*>(this->awd_project->get_block_by_external_id_and_type(timeline_name, BLOCK::block_type::TRI_GEOM, true));
		newGeometry->set_name(shape_name);
	}


	this->current_geom=newGeometry;

	// if we come to here, we need to fill the geometry with data:
	this->convert_shape_to_geometry(thisShape, newGeometry);

	return newGeometry;
}

void
FlashToAWDEncoder::convert_shape_to_geometry(DOM::FrameElement::IShape* thisShape, BLOCKS::Geometry* shapeBlock)
{	
	ExportFilledShape(thisShape, TYPES::filled_region_type::STANDART_FILL);
    ExportStroke(thisShape);
}

FCM::Result FlashToAWDEncoder::ExportFilledShape(DOM::FrameElement::PIShape pIShape, TYPES::filled_region_type path_type)
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

		this->current_path_shape=new GEOM::FilledRegion(path_type);
		this->current_geom->add_path_shape(this->current_path_shape);

        // Fill Style
        FCM::AutoPtr<DOM::IFCMUnknown> fillStyle;

        res = pFilledRegion->GetFillStyle(fillStyle.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = ExportFillStyle(fillStyle);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Boundary
        res = pFilledRegion->GetBoundary(pPath.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = ExportPath(pPath);
        ASSERT(FCM_SUCCESS_CODE(res));

        // Hole List
			
        FCM::FCMListPtr pHoleList;
        FCM::U_Int32 holeCount;

        res = pFilledRegion->GetHoles(pHoleList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pHoleList->Count(holeCount);
        ASSERT(FCM_SUCCESS_CODE(res));

        for (FCM::U_Int32 k = 0; k < holeCount; k++)
        {
            FCM::AutoPtr<DOM::Service::Shape::IPath> pPath = pHoleList[k];
            res = ExportPath(pPath);
			ASSERT(FCM_SUCCESS_CODE(res));
        }
    }				
	return res;
}

FCM::Result FlashToAWDEncoder::ExportPath(DOM::Service::Shape::PIPath pPath)
{
	this->current_path_shape->add_path();

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
}// Sets a segment of a path (Used for boundary, holes)

FCM::Result FlashToAWDEncoder::SetSegment(const DOM::Utils::SEGMENT& segment)
{
	int flipx=1;
	if(this->awd_project->get_settings()->get_flipXaxis())
		flipx=-1;
	int flipy=1;
	if(this->awd_project->get_settings()->get_flipYaxis())
		flipy=-1;
	GEOM::PathSegment*	awdPathSeg	= new GEOM::PathSegment();
    if (segment.segmentType == DOM::Utils::LINE_SEGMENT)
    {			
		//AwayJS::Utils::Trace(m_pCallback, "Line\n");
		awdPathSeg->set_startPoint(GEOM::VECTOR2D(segment.line.endPoint1.x*flipx , segment.line.endPoint1.y*flipy));
		awdPathSeg->set_endPoint(GEOM::VECTOR2D(segment.line.endPoint2.x*flipx, segment.line.endPoint2.y*flipy));
	}
    else{
		//AwayJS::Utils::Trace(m_pCallback, "Curve\n");
		awdPathSeg->set_startPoint(GEOM::VECTOR2D(segment.quadBezierCurve.anchor1.x*flipx, segment.quadBezierCurve.anchor1.y*flipy));
		awdPathSeg->set_endPoint(GEOM::VECTOR2D(segment.quadBezierCurve.anchor2.x*flipx, segment.quadBezierCurve.anchor2.y*flipy));		

		awdPathSeg->set_controlPoint(GEOM::VECTOR2D(segment.quadBezierCurve.control.x*flipx, segment.quadBezierCurve.control.y*flipy));

		awdPathSeg->set_edgeType(GEOM::edge_type::CURVED_EDGE);
	}	

	this->current_path_shape->add_segment(awdPathSeg);
	
    return FCM_SUCCESS;
}

FCM::Result FlashToAWDEncoder::ExportStrokeForFont(DOM::FrameElement::PIShape pIShape, AWD::FONT::FontShape* fontShape)
{
	this->current_path_shape=new GEOM::FilledRegion(TYPES::filled_region_type::GENERATED_FONT_OUTLINES);
	fontShape->set_subShape(this->current_path_shape);

    FCM::AutoPtr<FCM::IFCMUnknown> pGrad;
		
		
    FCM::FCMListPtr pStrokeGroupList;
    FCM::U_Int32 strokeStyleCount;
    FCM::Result res;
		
    FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
    this->m_pCallback->GetService(DOM::FLA_REGION_GENERATOR_SERVICE,pUnkSRVReg.m_Ptr);
    FCM::AutoPtr<DOM::Service::Shape::IRegionGeneratorService> pIRegionGeneratorService(pUnkSRVReg);
    ASSERT(pIRegionGeneratorService);
		
		
    res = pIRegionGeneratorService->GetStrokeGroups(pIShape, pStrokeGroupList.m_Ptr);
    ASSERT(FCM_SUCCESS_CODE(res));
		
    res = pStrokeGroupList->Count(strokeStyleCount);
    ASSERT(FCM_SUCCESS_CODE(res));
	//Utils::Trace(GetCallback(), "Export Stroke-groups count = %d.\n", strokeStyleCount);
		
    for (FCM::U_Int32 j = 0; j < strokeStyleCount; j++)
    {
        FCM::AutoPtr<DOM::Service::Shape::IStrokeGroup> pStrokeGroup = pStrokeGroupList[j];
        ASSERT(pStrokeGroup);

        FCM::FCMListPtr pPathList;
        FCM::U_Int32 pathCount;

        res = pStrokeGroup->GetPaths(pPathList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pPathList->Count(pathCount);
        ASSERT(FCM_SUCCESS_CODE(res));
						
		for (FCM::U_Int32 k = 0; k < pathCount; k++)
		{
			DOM::Service::Shape::IPath* pPath = ( DOM::Service::Shape::IPath*)pPathList[k];
			ASSERT(pPath);			

			res=ExportPath(pPath);
			ASSERT(FCM_SUCCESS_CODE(res));	

		}
			
	}

	return FCM_SUCCESS;
		
}
	
FCM::Result FlashToAWDEncoder::ExportStroke(DOM::FrameElement::PIShape pIShape)
{
		
    FCM::FCMListPtr pStrokeGroupList;
    FCM::U_Int32 strokeStyleCount;
    FCM::Result res;
		
    res = this->pIRegionGeneratorService->GetStrokeGroups(pIShape, pStrokeGroupList.m_Ptr);
    ASSERT(FCM_SUCCESS_CODE(res));
		
    res = pStrokeGroupList->Count(strokeStyleCount);
    ASSERT(FCM_SUCCESS_CODE(res));
	//Utils::Trace(GetCallback(), "Export Stroke-groups count = %d.\n", strokeStyleCount);

    FCM::AutoPtr<DOM::FillStyle::ISolidFillStyle> pSolidFillStyle = NULL;
	FCM::AutoPtr<DOM::FillStyle::IGradientFillStyle> pGradientFillStyle = NULL;
    FCM::AutoPtr<DOM::FillStyle::IBitmapFillStyle> pBitmapFillStyle = NULL;

	// use the shapeservice to convert stroke to shape
	// than handle the new shape as normal shape (exportFILL())		
					
	DOM::FrameElement::PIShape outShape;
	res=this->pIShapeService->ConvertStrokeToFill(pIShape, outShape);
	//Utils::Trace(GetCallback(), "Export Stroke count = %d .\n", res);
	if(res==FCM_SUCCESS){	
		//Utils::Trace(GetCallback(), "Export STROKE.\n");			
		ExportFilledShape(outShape, TYPES::filled_region_type::FILL_CONVERTED_FROM_STROKE);
	}
	
    return res;
}

