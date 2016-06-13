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

#include "AnimateToAWDEncoder.h"
#include "FCMPluginInterface.h"

#include "blocks/mesh_library.h"

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
AnimateToAWDEncoder::get_geom_for_shape(DOM::FrameElement::IShape* thisShape, const std::string& ressourceID, bool adobeFrameCommands)
{
	BLOCKS::Geometry* newGeometry=new BLOCKS::Geometry();
	newGeometry->add_res_id_geom(ressourceID, NULL);
	
	// We collect the data for this shape, 
	// At the same time we collect a string that is build from the basic-shape-properties (type, regionCnt, mat, segcnt, mat, segcnt...)
	this->current_geom=newGeometry;
	this->convert_shape_to_geometry(thisShape, newGeometry);
	/*
	if(true){
		// We have a map<string, vector<Geometry> > that is using this string as lookup to group to group shapes(Geometry-object store pointer to shape) on their basic properties
		if(this->geom_cache.find(geom_lookup_str)==this->geom_cache.end()){
			this->geom_cache[geom_lookup_str]=std::vector<BLOCKS::Geometry*>();
		}
		else{
			// To optimize further, we use the TestShapeSimilarity to check if the shape already has been exported.
			// If the shape has not been exported yet, we delete the Geometry that has been build, and retarget the ressourceID, to the existing Geometry.
			FCM::Result res = FCM_SUCCESS;
			for(BLOCKS::Geometry* one_geom : this->geom_cache[geom_lookup_str]){
				DOM::FrameElement::IShape* testshape = reinterpret_cast<DOM::FrameElement::IShape*>(one_geom->get_external_object());
				FCM::Boolean issame;
				DOM::Utils::MATRIX2D matrix;
				res=this->pIShapeService->TestShapeSimilarity(thisShape, testshape, issame, matrix);
				if(issame==1){
					delete newGeometry;
					one_geom->add_res_id_geom(ressourceID, new GEOM::MATRIX2x3(this->convert_matrix2x3(matrix)));
					return one_geom;
				}
			}
		}
		this->geom_cache[geom_lookup_str].push_back(newGeometry);
	}
	*/
	this->geom_cnt++;
	std::string geom_name="g"+std::to_string(this->geom_cnt);//+geom_lookup_str;
	newGeometry->set_name(geom_name);
	thisShape->AddRef();
	newGeometry->set_external_object(thisShape);
	this->awd_project->add_block(newGeometry);
	BLOCKS::MeshLibrary* thisMesh = new BLOCKS::MeshLibrary();
	std::string mesh_name="m"+std::to_string(this->geom_cnt);
	thisMesh->set_name(mesh_name);
	thisMesh->set_geom(newGeometry);
	newGeometry->set_mesh_instance(thisMesh);
	this->awd_project->add_block(thisMesh);
								
	return newGeometry;
}

void
AnimateToAWDEncoder::convert_shape_to_geometry(DOM::FrameElement::IShape* thisShape, BLOCKS::Geometry* shapeBlock)
{	
	ExportFilledShape( thisShape, TYPES::filled_region_type::STANDART_FILL);
    ExportStroke(thisShape);
}

FCM::Result 
AnimateToAWDEncoder::ExportFilledShape(DOM::FrameElement::PIShape pIShape, TYPES::filled_region_type path_type)
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

		this->current_filled_region=new GEOM::FilledRegion(path_type);
		this->current_geom->add_filled_region(this->current_filled_region);

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

FCM::Result AnimateToAWDEncoder::ExportPath(DOM::Service::Shape::PIPath pPath)
{
	this->current_filled_region->add_path();

    FCM::Result res;
    FCM::U_Int32 edgeCount;
    FCM::FCMListPtr pEdgeList;

    res = pPath->GetEdges(pEdgeList.m_Ptr);
    ASSERT(FCM_SUCCESS_CODE(res));

    res = pEdgeList->Count(edgeCount);
    ASSERT(FCM_SUCCESS_CODE(res));
		
	//AwayJS::Utils::Trace(m_pCallback, "edgeCount = %d\n", edgeCount);
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
FCM::Result AnimateToAWDEncoder::ExportPath_font(DOM::Service::Shape::PIPath pPath)
{
	this->current_filled_region->add_path();

    FCM::Result res;
    FCM::U_Int32 edgeCount;
    FCM::FCMListPtr pEdgeList;

    res = pPath->GetEdges(pEdgeList.m_Ptr);
    ASSERT(FCM_SUCCESS_CODE(res));

    res = pEdgeList->Count(edgeCount);
    ASSERT(FCM_SUCCESS_CODE(res));
		
	//AwayJS::Utils::Trace(m_pCallback, "edgeCount = %d\n", edgeCount);
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

FCM::Result AnimateToAWDEncoder::SetSegment(const DOM::Utils::SEGMENT& segment)
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
	
	this->current_filled_region->add_segment(awdPathSeg);
	
    return FCM_SUCCESS;
}

FCM::Result AnimateToAWDEncoder::ExportStrokeForFont(DOM::FrameElement::PIShape pIShape, AWD::FONT::FontShape* fontShape)
{

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
		
	this->current_geom=new BLOCKS::Geometry(false);
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
			
			this->current_filled_region=new GEOM::FilledRegion(TYPES::filled_region_type::GENERATED_FONT_OUTLINES);
			this->current_geom->add_filled_region(this->current_filled_region);
			res=ExportPath_font(pPath);
			ASSERT(FCM_SUCCESS_CODE(res));	

		}			
	}

	// todo: add new Settings that define streamdata specific for fonts
	GEOM::ProcessShapeGeometry(this->current_geom, this->awd_project, this->awd_project->get_settings());
	
	this->current_geom->merge_subgeos();
	std::vector<std::string> messages;
	this->current_geom->get_messages(messages, " -> ");
	for(std::string one_message : messages){
		if(one_message.size()<1024)
			AwayJS::Utils::Trace(this->m_pCallback, "%s\n", one_message.c_str());
	}
	fontShape->set_subShape(this->current_geom->get_sub_at(0));
	delete this->current_geom;
	return FCM_SUCCESS;
		
}

FCM::Result AnimateToAWDEncoder::ExportStroke(DOM::FrameElement::PIShape pIShape)
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

