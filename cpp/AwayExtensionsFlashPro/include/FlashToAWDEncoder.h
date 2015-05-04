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
		
		
		std::map<std::string, std::vector<BLOCKS::Geometry*> >	geom_cache;
		// pointer to awd-project needed to add blocks to awd-project
		AWDProject* awd_project;
		
		// needed to communicate with flash
		FCM::PIFCMCallback m_pCallback;
		
		// needed when converting shapes to geometry
		FCM::AutoPtr<DOM::Service::Shape::IRegionGeneratorService> pIRegionGeneratorService;
		BLOCKS::Geometry*  current_geom;
		GEOM::FilledRegion* current_filled_region;
		
// FlashToAWDEncoder_geometry.cpp:
		void convert_shape_to_geometry(std::string& this_lookup_str, DOM::FrameElement::IShape* thisShape, BLOCKS::Geometry* shapeBlock);
		FCM::AutoPtr<DOM::Service::Shape::IShapeService>  pIShapeService;
		
	public:
		
		std::string current_scene_name;
		int grafik_cnt;
		int mat_cnt;
		int geom_cnt;
		int text_fomat_cnt;
		int text_field_cnt;
		int shape_instance_cnt;
// FlashToAWDEncoder.cpp:

		std::vector<DOM::FrameElement::IShape> all_shapes;

        FlashToAWDEncoder(FCM::PIFCMCallback pCallback, AWDProject* awd_project);
        ~FlashToAWDEncoder();		 

				
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
		
        FCM::Result ExportFilledShape(std::string& this_lookup_str, DOM::FrameElement::PIShape pIShape, TYPES::filled_region_type from_stroke);
        FCM::Result ExportStroke(std::string& this_lookup_str, DOM::FrameElement::PIShape pIShape);
		
        FCM::Result ExportPath_font(DOM::Service::Shape::PIPath pPath);
        FCM::Result ExportPath(std::string& this_lookup_str, DOM::Service::Shape::PIPath pPath);
        FCM::Result SetSegment(const DOM::Utils::SEGMENT& segment);
        FCM::Result ExportStrokeForFont(DOM::FrameElement::PIShape pIShape, AWD::FONT::FontShape*);

		
		
// FlashToAWDEncoder_fills.cpp:	
		FCM::Result ExportFillStyle(std::string& this_lookup_str, FCM::PIFCMUnknown pFillStyle);

        FCM::Result ExportSolidFillStyle(std::string& this_lookup_str, DOM::FillStyle::ISolidFillStyle* pSolidFillStyle);

        FCM::Result ExportRadialGradientFillStyle(std::string& this_lookup_str, DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportLinearGradientFillStyle(std::string& this_lookup_str, DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportBitmapFillStyle(std::string& this_lookup_str, DOM::FillStyle::IBitmapFillStyle* pBitmapFillStyle);
		
		
// FlashToAWDEncoder_fonts.cpp:	
		
		AWD::result ExportText(DOM::FrameElement::IClassicText*, AWDBlock** awd_block, const std::string);

		AWD::BASE::AWDBlock* ExportFont(DOM::LibraryItem::IFontItem* font);

		AWD::result FinalizeFonts(DOM::PIFLADocument pFlaDocument);
		
// FlashToAWDEncoder_mediaItems.cpp:	
		
		FCM::Result ExportBitmap(DOM::LibraryItem::IMediaItem* , BASE::AWDBlock** output_block);
		AWD::BASE::AWDBlock* ExportSound(DOM::LibraryItem::IMediaItem* , BASE::AWDBlock** output_block, const std::string res_id);
};

#endif