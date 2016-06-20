#ifdef _WINDOWS
#include <Windows.h>
#include "ShellApi.h"
#endif
#include <vector>
#include <fstream>
#include "Utils.h"
#include <sstream>
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

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG


// Export a fillstyle as a Material. Called during shape-converting.

FCM::Result AnimateToAWDEncoder::ExportFillStyle(FCM::PIFCMUnknown pFillStyle)
{
    FCM::Result res = FCM_SUCCESS;
    FCM::AutoPtr<DOM::FillStyle::ISolidFillStyle> pSolidFillStyle;
    FCM::AutoPtr<DOM::FillStyle::IGradientFillStyle> pGradientFillStyle;
	FCM::AutoPtr<DOM::FillStyle::IBitmapFillStyle> pBitmapFillStyle;

    // Check for solid fill color
    pSolidFillStyle = pFillStyle;
    if (pSolidFillStyle)
    {
        res = ExportSolidFillStyle(pSolidFillStyle);
        ASSERT(FCM_SUCCESS_CODE(res));
		return res;
    }
	else{
		// Check for Gradient Fill
		pGradientFillStyle = pFillStyle;
		FCM::AutoPtr<FCM::IFCMUnknown> pGrad;

		if (pGradientFillStyle)
		{
			res = pGradientFillStyle->GetColorGradient(pGrad.m_Ptr);
			ASSERT(FCM_SUCCESS_CODE(res));

			if (FCM::AutoPtr<DOM::Utils::IRadialColorGradient>(pGrad))
			{
				res = ExportRadialGradientFillStyle(pGradientFillStyle);
				ASSERT(FCM_SUCCESS_CODE(res));
				return res;
			}
			else if (FCM::AutoPtr<DOM::Utils::ILinearColorGradient>(pGrad))
			{
				res = ExportLinearGradientFillStyle(pGradientFillStyle);
				ASSERT(FCM_SUCCESS_CODE(res));
				return res;
			}
		}
		else{
			pBitmapFillStyle = pFillStyle;
			if (pBitmapFillStyle)
			{
				res = ExportBitmapFillStyle(pBitmapFillStyle);
				ASSERT(FCM_SUCCESS_CODE(res));
				return res;
			}
		}
	}
    return res;
}
	
FCM::Result AnimateToAWDEncoder::ExportSolidFillStyle(DOM::FillStyle::ISolidFillStyle* pSolidFillStyle)
{
    FCM::Result res;
    DOM::Utils::COLOR color;
	
    FCM::AutoPtr<DOM::FillStyle::ISolidFillStyle> solidFill = pSolidFillStyle;
    ASSERT(solidFill);

    res = solidFill->GetColor(color);
    ASSERT(FCM_SUCCESS_CODE(res));
	
	// convert the color-components to a uint32 color.
	TYPES::COLOR awdColor = TYPES::create_color_from_ints( color.alpha, color.red, color.green, color.blue );

	// get the material-block for this color (create if does not exist)
	BLOCKS::Material* new_fill_material=this->awd_project->get_default_material_by_color(awdColor, true, MATERIAL::type::SOLID_COLOR_MATERIAL);
	new_fill_material->set_alpha(double(color.alpha)/double(255));
	new_fill_material->add_scene_name(this->current_scene_name);
	// set name for fill (incl color as hex-string)
	std::stringstream sstream_color;
	sstream_color << std::hex << awdColor;
	new_fill_material->set_name("SolidFill_#"+sstream_color.str());
	// set color-components for color.
	new_fill_material->set_color_components(color.red, color.green, color.blue, color.alpha);
	if(color.alpha!=255){
		new_fill_material->needsAlphaTex=true;
	}

	// assign the material to the currently active Path-Shape
	this->current_filled_region->set_material(new_fill_material);

	//AwayJS::Utils::Trace(this->m_pCallback, "The color mat: '%06X' should have been created.\n", awdColor);
	
    return res;
}

FCM::Result AnimateToAWDEncoder::ExportRadialGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle)
{

	DOM::FillStyle::GradientSpread spread;	
    FCM::AutoPtr<FCM::IFCMUnknown> pGrad;
	FCM::AutoPtr<DOM::FillStyle::IGradientFillStyle> gradientFill = pGradientFillStyle;
    FCM::Result res = gradientFill->GetSpread(spread);
    ASSERT(FCM_SUCCESS_CODE(res));

    res = gradientFill->GetColorGradient(pGrad.m_Ptr);
    ASSERT(FCM_SUCCESS_CODE(res));

    FCM::AutoPtr<DOM::Utils::IRadialColorGradient> radialColorGradient = pGrad;
    ASSERT(radialColorGradient);

    DOM::Utils::MATRIX2D matrix;
    res = gradientFill->GetMatrix(matrix);
    ASSERT(FCM_SUCCESS_CODE(res));
	this->current_filled_region->uv_transform->set(this->convert_matrix2x3(matrix));

    FCM::S_Int32 focalPoint = 0;
    res = radialColorGradient->GetFocalPoint(focalPoint);
	//AwayJS::Utils::Trace(this->m_pCallback, "focalPoint %d \n", focalPoint);
    ASSERT(FCM_SUCCESS_CODE(res));
	
  //  res = m_pOutputWriter->StartDefineRadialGradientFillStyle(spread, matrix, focalPoint);
  //  ASSERT(FCM_SUCCESS_CODE(res));
	std::string fill_identifier = "radial_";//+std::to_string(this->grafik_cnt++);
    FCM::U_Int8 nColors;
    res = radialColorGradient->GetKeyColorCount(nColors);
    ASSERT(FCM_SUCCESS_CODE(res));
	FCM::U_Int8 i = 0;
	std::vector<TYPES::UINT32> gradient_pos;
	std::vector<GEOM::VECTOR4D> gradient_color;
	bool needsAlpha=false;
    for ( i = 0; i < nColors; i++)
    {
        DOM::Utils::GRADIENT_COLOR_POINT point;

        res = radialColorGradient->GetKeyColorAtIndex(i, point);
        ASSERT(FCM_SUCCESS_CODE(res));
		fill_identifier+=AwayJS::Utils::ToString(point.color)+"#"+AwayJS::Utils::ToString(point.pos);
		gradient_pos.push_back(point.pos);
		GEOM::VECTOR4D newColor = GEOM::VECTOR4D();
		newColor.x = point.color.red;
		newColor.y = point.color.green;
		newColor.z = point.color.blue;
		newColor.w = point.color.alpha;
		if(point.color.alpha!=255){
			needsAlpha=true;
		}
		gradient_color.push_back(newColor);
    }
	
	if(gradient_pos.front()!=0){
		gradient_pos.insert(gradient_pos.begin(), 0);
		gradient_color.insert(gradient_color.begin(), gradient_color.front());
	}
	if(gradient_pos.back()!=255){
		gradient_pos.push_back(255);
		gradient_color.push_back( gradient_color.back());
	}
	// get the material-block for this color (create if does not exist)
	BLOCKS::Material* new_fill_material=reinterpret_cast<BLOCKS::Material*>(this->awd_project->get_block_by_external_id_and_type(fill_identifier,  BLOCK::SIMPLE_MATERIAL, true));
	new_fill_material->set_name("Radial Gradient");
	new_fill_material->set_external_id(fill_identifier);
	new_fill_material->needsAlphaTex=needsAlpha;
	new_fill_material->add_scene_name(this->current_scene_name);
	new_fill_material->set_material_type(MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL);
	new_fill_material->set_uv_transform_mtx(this->convert_matrix2x3(matrix));
	new_fill_material->gradient_colors = gradient_color;
	new_fill_material->gradient_positions = gradient_pos;
	new_fill_material->focalPoint=focalPoint;
	// assign the material to the currently active Path-Shape
	this->current_filled_region->set_material(new_fill_material);
	
	return FCM_SUCCESS;
}

FCM::Result AnimateToAWDEncoder::ExportLinearGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle)
{    
	DOM::FillStyle::GradientSpread spread;	
    FCM::AutoPtr<FCM::IFCMUnknown> pGrad;
    FCM::AutoPtr<DOM::FillStyle::IGradientFillStyle> gradientFill = pGradientFillStyle;
    FCM::Result res = gradientFill->GetSpread(spread);
	//AwayJS::Utils::Trace(this->m_pCallback, "spread %d \n", spread);
    ASSERT(FCM_SUCCESS_CODE(res));

	bool needsAlpha=false;
    res = gradientFill->GetColorGradient(pGrad.m_Ptr);
    ASSERT(FCM_SUCCESS_CODE(res));

    FCM::AutoPtr<DOM::Utils::ILinearColorGradient> linearColorGradient = pGrad;
    ASSERT(linearColorGradient);

    DOM::Utils::MATRIX2D matrix;
    res = gradientFill->GetMatrix(matrix);
    ASSERT(FCM_SUCCESS_CODE(res));
	this->current_filled_region->uv_transform->set(this->convert_matrix2x3(matrix));

    FCM::U_Int8 nColors;
    res = linearColorGradient->GetKeyColorCount(nColors);
    ASSERT(FCM_SUCCESS_CODE(res));
	
	std::string fill_identifier = "linear_";//+std::to_string(this->grafik_cnt++);
	std::vector<TYPES::UINT32> gradient_pos;
	std::vector<GEOM::VECTOR4D> gradient_color;
    for (FCM::U_Int8 i = 0; i < nColors; i++)
    {
        DOM::Utils::GRADIENT_COLOR_POINT point;

        res = linearColorGradient->GetKeyColorAtIndex(i, point);
        ASSERT(FCM_SUCCESS_CODE(res));
		fill_identifier+=AwayJS::Utils::ToString(point.color) + "#" + AwayJS::Utils::ToString(point.pos);
		gradient_pos.push_back(point.pos);
		GEOM::VECTOR4D newColor = GEOM::VECTOR4D();
		newColor.x = point.color.red;
		newColor.y = point.color.green;
		newColor.z = point.color.blue;
		newColor.w = point.color.alpha;
		//AwayJS::Utils::Trace(this->m_pCallback, "alpha %d \n", AwayJS::Utils::ToString(point.color.alpha));
		if(point.color.alpha!=255){
			needsAlpha=true;
		}
		gradient_color.push_back(newColor);

    }
	if(gradient_pos.front()!=0){
		gradient_pos.insert(gradient_pos.begin(), 0);
		gradient_color.insert(gradient_color.begin(), gradient_color.front());
	}
	if(gradient_pos.back()!=255){
		gradient_pos.push_back(255);
		gradient_color.push_back( gradient_color.back());
	}
	// get the material-block for this color (create if does not exist)
	BLOCKS::Material* new_fill_material=reinterpret_cast<BLOCKS::Material*>(this->awd_project->get_block_by_external_id_and_type(fill_identifier,  BLOCK::SIMPLE_MATERIAL, true));
	new_fill_material->set_name("Linear Gradient");
	new_fill_material->needsAlphaTex=needsAlpha;
	new_fill_material->gradient_spread=MATERIAL::GradientSpread(spread);
	new_fill_material->add_scene_name(this->current_scene_name);
	new_fill_material->set_material_type(MATERIAL::type::LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL);
	new_fill_material->set_uv_transform_mtx(this->convert_matrix2x3(matrix));
	new_fill_material->gradient_colors = gradient_color;
	new_fill_material->gradient_positions = gradient_pos;

	// assign the material to the currently active Path-Shape
	this->current_filled_region->set_material(new_fill_material);

	return FCM_SUCCESS;
}

FCM::Result AnimateToAWDEncoder::ExportBitmapFillStyle(DOM::FillStyle::IBitmapFillStyle* pBitmapFillStyle)
{

    DOM::AutoPtr<DOM::LibraryItem::IMediaItem> pMediaItem;
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
	this->current_filled_region->uv_transform->set(this->convert_matrix2x3(matrix));

    // Get name
    res = pBitmapFillStyle->GetBitmap(pMediaItem.m_Ptr);
    ASSERT(FCM_SUCCESS_CODE(res));

	AWDBlock* new_texture = NULL;
	this->ExportBitmap(pMediaItem, &new_texture);
    if(new_texture==NULL)
		return FCM_SUCCESS;
	BLOCKS::BitmapTexture* new_bitmaptexture = reinterpret_cast<BLOCKS::BitmapTexture*>(new_texture);
    if(new_bitmaptexture==NULL)
		return FCM_SUCCESS;
	new_bitmaptexture->add_scene_name(this->current_scene_name);
	
    
	std::string matname = "matshape_"+new_bitmaptexture->get_name();//+std::to_string(this->grafik_cnt++);
	// get the material-block for this color (create if does not exist)
	BLOCKS::Material* new_fill_material=reinterpret_cast<BLOCKS::Material*>(this->awd_project->get_block_by_name_and_type(matname,  BLOCK::SIMPLE_MATERIAL, true));
	new_fill_material->set_name(matname);
	new_fill_material->needsAlphaTex=true;
	new_fill_material->add_scene_name(this->current_scene_name);
	new_fill_material->set_material_type(MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL);
	//new_fill_material->set_uv_transform_mtx(this->convert_matrix2x3(matrix));
	new_fill_material->set_texture(reinterpret_cast<BLOCKS::BitmapTexture*>(new_texture));

	// assign the material to the currently active Path-Shape
	this->current_filled_region->set_material(new_fill_material);
	return FCM_SUCCESS;
}