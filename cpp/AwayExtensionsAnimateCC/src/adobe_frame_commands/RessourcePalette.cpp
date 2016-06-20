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


    void ResourcePalette::Init(AnimateToAWDEncoder* awd_converter)
    {
		this->test_shape=NULL;
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
		
		if(pName!=NULL){
			std::string symbol_name = AwayJS::Utils::ToString(pName, this->GetCallback());
			BLOCKS::MovieClip* thisTimeline = this->awd_converter->get_project()->get_timeline_by_symbol_name(symbol_name);
			if(thisTimeline!=NULL){
				//Utils::Trace(GetCallback(), "Allready-exists: %s\n", symbol_name.c_str());
				thisTimeline->add_res_id(FILES::int_to_string(resourceId));
				thisTimeline->add_scene_name(this->awd_converter->current_scene_name);
				return FCM_SUCCESS;
			}
			//Utils::Trace(GetCallback(), "Start exporting name %s\n", symbol_name.c_str());

		}
		//else{
			TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);
			ITimelineWriter* pTimelineWriter;
			return pTimeline->Build(resourceId, pName, &pTimelineWriter);
		//}
    }
	
    FCM::Result ResourcePalette::AddShape(
        FCM::U_Int32 resourceId, 
        DOM::FrameElement::PIShape pShape)
    {
		//Utils::Trace(GetCallback(), "Start adding shape\n");
        m_resourceList.push_back(resourceId);
		
		//	this will create a new geom for this shape, add it to the AWDProject, and fill it with the path-data.
		//	the geometries will be processed later.
		//	we do not check for errors here. the created geometry_blocks can be queried for warning / error messages.
		std::string geom_res_id = FILES::int_to_string(resourceId);
		BLOCKS::Geometry* this_geom=this->awd_converter->get_geom_for_shape(reinterpret_cast< DOM::FrameElement::IShape*>(pShape), geom_res_id, true);
		this_geom->get_mesh_instance()->add_scene_name(this->awd_converter->current_scene_name);
		this_geom->add_scene_name(this->awd_converter->current_scene_name);
		
        return FCM_SUCCESS;
    }


    FCM::Result ResourcePalette::AddSound(FCM::U_Int32 resourceId, DOM::LibraryItem::PIMediaItem pMediaItem)
    {
        m_resourceList.push_back(resourceId);
		this->awd_converter->ExportSound(pMediaItem, NULL, FILES::int_to_string(resourceId));
        return FCM_SUCCESS;
    }
	
    FCM::Result ResourcePalette::AddBitmap(FCM::U_Int32 resourceId, DOM::LibraryItem::PIMediaItem pMediaItem)
    {		
        m_resourceList.push_back(resourceId);
		
		AWDBlock* new_texture = NULL;
		this->awd_converter->ExportBitmap(pMediaItem, &new_texture);
		if(new_texture==NULL)
			return FCM_SUCCESS;
		BLOCKS::BitmapTexture* new_bitmaptexture = reinterpret_cast<BLOCKS::BitmapTexture*>(new_texture);
		if(new_bitmaptexture==NULL)
			return FCM_SUCCESS;  
		new_bitmaptexture->add_scene_name(this->awd_converter->current_scene_name);
		// get the material-block for this bitmap (create if does not exist)
		std::string matname = "mat_"+new_bitmaptexture->get_name();
		BLOCKS::Material* new_fill_material=reinterpret_cast<BLOCKS::Material*>(this->awd_converter->get_project()->get_block_by_name_and_type(matname,  BLOCK::SIMPLE_MATERIAL, true));
		new_fill_material->add_res_id(FILES::int_to_string(resourceId));
		new_fill_material->add_scene_name(this->awd_converter->current_scene_name);
		new_fill_material->set_name(matname);
		new_fill_material->needsAlphaTex=true;
		new_fill_material->set_material_type(MATERIAL::type::TEXTURE_MATERIAL);
	//	new_fill_material->set_uv_transform_mtx(this->convert_matrix2x3(matrix));
		new_fill_material->set_texture(reinterpret_cast<BLOCKS::BitmapTexture*>(new_texture));
		
		return FCM_SUCCESS;
	}

	FCM::Result ResourcePalette::AddClassicText(FCM::U_Int32 resourceId, DOM::FrameElement::PIClassicText pClassicText)
    {
        m_resourceList.push_back(resourceId);
		
		AWDBlock* new_text = NULL;
		this->awd_converter->ExportText(pClassicText, &new_text, FILES::int_to_string(resourceId));
		new_text->add_scene_name(this->awd_converter->current_scene_name);
        
		
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
		




    FCM::Result ResourcePalette::ExportStrokeStyle(FCM::PIFCMUnknown pStrokeStyle)
    {
        return FCM_SUCCESS;// strokes are exported as fills
    }	
    FCM::Result ResourcePalette::ExportSolidStrokeStyle(DOM::StrokeStyle::ISolidStrokeStyle* pSolidStrokeStyle)
    {
        return FCM_SUCCESS;// strokes are exported as fills
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
       return FCM_SUCCESS;
    }




};
