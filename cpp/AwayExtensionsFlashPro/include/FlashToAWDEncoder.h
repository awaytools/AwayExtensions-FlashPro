#ifndef FLASHCONVERTER_H_
#define FLASHCONVERTER_H_

#include <vector>
#include <string>

#include "Utils.h"
#include "Utils/DOMTypes.h"
#include "FlashFCMPublicIDs.h"

#include "FrameElement/IClassicText.h"
#include "FrameElement/IShape.h"
#include "FrameElement/IBitmapInstance.h"
#include "FrameElement/ISound.h"

#include "MediaInfo/ISoundInfo.h"
#include "MediaInfo/IBitmapInfo.h"

#include "LibraryItem/IMediaItem.h"
#include "LibraryItem/IFolderItem.h"
#include "LibraryItem/IFontItem.h"
#include "LibraryItem/ISymbolItem.h"
#include "ILibraryItem.h"


#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"

#include "Service/Shape/IFilledRegion.h"
#include "Service/Shape/IStrokeGroup.h"
#include "Service/Shape/IPath.h"
#include "Service/Shape/IEdge.h"
#include "Service/Shape/IRegionGeneratorService.h"
#include "Service/Shape/IShapeService.h"
#include "IFLADocument.h"
#include "awd_project.h"

using namespace AWD;

/**
This class provides all the functions needed to encode flash-objects into AWD-Data. 
The only Flash-Object that has its dedicaded class is the Timeline. It needed its own Clas
*/

class FlashToAWDEncoder 
{

    private:
		
		// awd-geometry-blocks ordered by scenes timelines and frames
		// each awd-geometry stores a pointer to the Flash DOM::FrameElement::IShape
		// this way, we can compare Geometries by their IShape object.
		std::vector< std::vector<std::vector< std::vector<BLOCKS::Geometry*> > > >	scene_geoms;
		
		// pointer to awd-project needed to add blocks to awd-project
		AWDProject* awd_project;
		
		// needed to communicate with flash
		FCM::PIFCMCallback m_pCallback;
		
		// needed when converting shapes to geometry
		FCM::AutoPtr<DOM::Service::Shape::IRegionGeneratorService> pIRegionGeneratorService;
		FCM::AutoPtr<DOM::Service::Shape::IShapeService>  pIShapeService;
		BLOCKS::Geometry*  current_geom;
		GEOM::FilledRegion* current_path_shape;
		
// FlashToAWDEncoder_geometry.cpp:
		void convert_shape_to_geometry(DOM::FrameElement::IShape* thisShape, BLOCKS::Geometry* shapeBlock);
		
    public:

// FlashToAWDEncoder.cpp:

        FlashToAWDEncoder(FCM::PIFCMCallback pCallback, AWDProject* awd_project);
        ~FlashToAWDEncoder();		 

		/** \brief start converting a new scene - not needed when using adobe frame commands
		*/
		void prepare_new_scene();

		/** \brief start converting a new timeline - not needed when using adobe frame commands
		*/
        void prepare_new_timeline();
		
		/** \brief start converting a new Frame - not needed when using adobe frame commands
		*/
        void prepare_new_frame();
				
		/** \brief Convert a DOM::Utils::MATRIX2D to TYPES::F64*
		*/
        TYPES::F64* convert_matrix2x3(DOM::Utils::MATRIX2D);

		/** \brief Convert a DOM::Utils::COLOR_MATRIX to TYPES::F64*
		*/
		TYPES::F64* convert_matrix4x5(DOM::Utils::COLOR_MATRIX);
		
		/** \brief Get Pointer to the AWD_project
		*/
		AWDProject* get_project();

// FlashToAWDEncoder_geometry.cpp:
		
		BLOCKS::Geometry* get_geom_for_shape(DOM::FrameElement::IShape* thisShape, const std::string& timeline_name, bool adobeFrameCommands);
		
        FCM::Result ExportFilledShape(DOM::FrameElement::PIShape pIShape, TYPES::filled_region_type from_stroke);
        FCM::Result ExportStroke(DOM::FrameElement::PIShape pIShape);
		
        FCM::Result ExportPath(DOM::Service::Shape::PIPath pPath);
        FCM::Result SetSegment(const DOM::Utils::SEGMENT& segment);
        FCM::Result ExportStrokeForFont(DOM::FrameElement::PIShape pIShape, AWD::FONT::FontShape*);

		FCM::Result ExportFillStyle(FCM::PIFCMUnknown pFillStyle);
		
		
// FlashToAWDEncoder_fills.cpp:	

        FCM::Result ExportSolidFillStyle(DOM::FillStyle::ISolidFillStyle* pSolidFillStyle);

        FCM::Result ExportRadialGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportLinearGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportBitmapFillStyle(DOM::FillStyle::IBitmapFillStyle* pBitmapFillStyle);
		
		
// FlashToAWDEncoder_fonts.cpp:	
		
		AWD::result ExportText(DOM::FrameElement::IClassicText*, AWDBlock** awd_block);

		AWD::result ExportFont(DOM::LibraryItem::IFontItem* font);

		AWD::result FinalizeFonts(DOM::PIFLADocument pFlaDocument);
		
// FlashToAWDEncoder_mediaItems.cpp:	
		
		FCM::Result ExportBitmap(DOM::LibraryItem::IMediaItem* , BASE::AWDBlock** output_block, const std::string res_id);
		FCM::Result ExportSound(DOM::LibraryItem::IMediaItem* , BASE::AWDBlock** output_block, const std::string res_id);
};

#endif