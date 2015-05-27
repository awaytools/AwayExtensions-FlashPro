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

#include "Publisher.h"
#include "Utils.h"
#include "FlashFCMPublicIDs.h"
#include "awd_project.h"
#include "Utils/DOMTypes.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

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

#include "Utils/ILinearColorGradient.h"
#include "Utils/IRadialColorGradient.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder2.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

#include "AWDTimelineWriter.h"
#include "RessourcePalette.h"
#include "TimelineBuilder.h"
#include "blocks/mesh_library.h"
#include "blocks/billboard.h"

namespace AwayJS
{
	using namespace AWD;
    /* ----------------------------------------------------- CPublisher */
	
    CPublisher::CPublisher()
    {
    }

    CPublisher::~CPublisher()
    {

    }
	

    FCM::Result CPublisher::Publish(
        DOM::PIFLADocument pFlaDocument, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
        return Export(pFlaDocument, NULL, NULL, pDictPublishSettings, pDictConfig);
    }

    // This function will be currently called in "Test-Scene" workflow. 
    // In future, it might be called in other workflows as well. 
    FCM::Result CPublisher::Publish(
        DOM::PIFLADocument pFlaDocument, 
        DOM::PITimeline pTimeline, 
        const Exporter::Service::RANGE &frameRange, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
        return Export(pFlaDocument, pTimeline, &frameRange, pDictPublishSettings, pDictConfig);
    }
	

    FCM::Result CPublisher::Export(
        DOM::PIFLADocument pFlaDocument, 
        DOM::PITimeline pTimeline, 
        const Exporter::Service::RANGE* pFrameRange, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
		_pDict=pDictPublishSettings;
		
        std::string outFile;
        FCM::Result res;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk2;
        FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;


        pCalloc = AwayJS::Utils::GetCallocService(GetCallback());
        ASSERT(pCalloc.m_Ptr != NULL);

        Utils::Trace(GetCallback(), "Publishing AwayJS-document (AWD export)\n");

        res = GetOutputFileName(pFlaDocument, pTimeline, pDictPublishSettings, outFile);
        if (FCM_FAILURE_CODE(res))
        {
            // FLA is untitled. Ideally, we should use a temporary location for output generation.
            // However, for now, we report an error.
            Utils::Trace(GetCallback(), "\nFailed to publish. Either save the AwayJS-Document or provide a output path in publish settings.\n");
            return res;
        }

// create minimum information needed for AWDProject (output-path and generator-name and version-nr)

        const std::string	generator_name		= "Adobe Flash Professional";
		FCM::U_Int32 app_version=0;
		AwayJS::Utils::GetAppVersion(GetCallback(), app_version );
		int majorversion = ((app_version >> 24) & 0xff);
		int minorversion = ((app_version >> 16) & 0xff);
		int patchversion = ((app_version >> 8) & 0xff);
		int buildversion = ((app_version) & 0xff);

		const std::string	generator_version = std::to_string(majorversion)+"."+std::to_string(minorversion)+"."+std::to_string(patchversion)+"."+std::to_string(buildversion);
		
	// create a AWDProject

		this->awd_project = new AWDProject(TYPES::project_type::SINGLE_FILE_EXPORT, outFile, generator_name, generator_version);	
		this->flash_to_awd_encoder = new FlashToAWDEncoder(GetCallback(), this->awd_project);
		this->fla_document = pFlaDocument;

		// validate the state of the Project.
		if(this->awd_project->get_state()==TYPES::state::INVALID){
			std::vector<std::string> errors;
			this->awd_project->get_errors(errors, "");
			for(std::string one_error : errors)
				Utils::Trace(GetCallback(), "AWDProject Error: : %s\n", one_error.c_str());
			return FCM_EXPORT_FAILED;
		}

	// convert the settings from string into data types.
		     
		// defines if the preview should be opened (as3) 
		bool openPreview=false;	
		std::string openPreview_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.OpenPreview", openPreview_str);
		if(openPreview_str=="true")
			openPreview=true;

		
		// defines if the preview should be opened (js) 
		bool export_shapes_in_debug_mode=false;
		std::string export_shapes_in_debug_mode_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.ExportShapesInDebugMode", export_shapes_in_debug_mode_str);
		if(export_shapes_in_debug_mode_str=="true")
			export_shapes_in_debug_mode=true;
		
		// defines if the adobe-frame-generator should be used (true) or not (false) 
		bool useADobeFramegenerator=true;
		std::string useADobeFramegenerator_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.UseAdobeFrameCommands", useADobeFramegenerator_str);
		if(useADobeFramegenerator_str=="false")
			useADobeFramegenerator=false;
			

		// defines if the adobe-frame-generator should be used (true) or not (false) 
		bool distinglishexteriorandinterior_tris=false;
		/*std::string distinglishexteriorandinterior_tris_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.DistinglishExteriorAndinterior", distinglishexteriorandinterior_tris_str);
		if(distinglishexteriorandinterior_tris_str=="true")
			distinglishexteriorandinterior_tris=true;*/

		// defines if curve-intersections should be resolved when generating the geometry-data
		bool resolveIntersects=true;
		/*std::string resolveIntersects_s;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.ResolvCurveIntersections", resolveIntersects_s);
		if(resolveIntersects_s=="true")
			resolveIntersects=true;*/

		std::string preview_source_path;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.PreviewPathSource", preview_source_path);

		std::string preview_output_path;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.PreviewPath", preview_output_path);
		
		// defines if the preview should be opened (js) 
		bool append_file_name=false;
		std::string append_file_name_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.AppendFilename", append_file_name_str);
		if(append_file_name_str=="true")
			append_file_name=true;

			
		// get the settings for resolve_intersections, or set them to default, if resolve_intersections is false
		double curveThreshold=0.1;
		int maxIterations=4;
		/*if(resolveIntersects){
			// defines the max threshold of "curviness" before subdividing of curves should be stopped
			std::string curveThreshold_s;
			ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.CurveTreshold", curveThreshold_s);
			if(!curveThreshold_s.empty())
				curveThreshold=std::stod(curveThreshold_s);
		
			// defines the max number of iterations when subdividng curves to resolve intersections
			std::string max_itereations_s;
			ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.MaximalIterations", max_itereations_s);
			if(!max_itereations_s.empty())
				maxIterations=int(std::stod(max_itereations_s));
		}*/
		
		bool print_export_log=true;
		std::string print_export_log_s;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.PrintExportLog", print_export_log_s);
		if(print_export_log_s=="false")
			print_export_log=false;

		bool export_framescripts=true;
		std::string export_framescripts_s;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.ExportFrameScript", export_framescripts_s);
		if(export_framescripts_s=="false")
			export_framescripts=false;

		// defines the max threshold of "curviness" before subdividing of curves should be stopped
		double exterior_threshold=0.0;
		std::string exterior_threshold_s;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.MinimizeExteriorTreshold", exterior_threshold_s);
		if(!exterior_threshold_s.empty())
			exterior_threshold=std::stod(exterior_threshold_s);

		bool export_invisible_timeline_layer=false;
		std::string export_invisible_timeline_layer_s;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.IncludeInvisibleLayer", export_invisible_timeline_layer_s);
		if(export_invisible_timeline_layer_s=="true")
			export_invisible_timeline_layer=true;

		bool exterior_threshold_font=false;
		std::string exterior_threshold_font_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.MinimizeExteriorTresholdFonts", exterior_threshold_font_str);
		if(exterior_threshold_font_str=="true")
			exterior_threshold_font=true;

		bool exterior_threshold_strokes=false;
		std::string exterior_threshold_strokes_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.MinimizeExteriorTresholdStrokes", exterior_threshold_strokes_str);
		if(exterior_threshold_strokes_str=="true")
			exterior_threshold_strokes=true;

		bool keep_vert_in_path_order=true;
		/*std::string keep_vert_in_path_order_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.KeepPathOrder", keep_vert_in_path_order_str);
		if(keep_vert_in_path_order_str=="true")
			keep_vert_in_path_order=true;
		*/
		
		
		bool embbed_textures=true;
		std::string embbed_textures_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.EmbbedTextures", embbed_textures_str);
		if(embbed_textures_str=="false")
			embbed_textures=false;

		bool embbed_sounds=true;
		std::string embbed_sounds_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.EmbbedSounds", embbed_sounds_str);
		if(embbed_sounds_str=="false")
			embbed_sounds=false;

		
		bool export_lib_bitmaps=false;
		std::string export_lib_bitmaps_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.ExportLibBitmaps", export_lib_bitmaps_str);
		if(export_lib_bitmaps_str=="true")
			export_lib_bitmaps=true;
		
		bool export_lib_sounds=false;
		std::string export_lib_sounds_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.ExportLibSounds", export_lib_sounds_str);
		if(export_lib_sounds_str=="true")
			export_lib_sounds=true;
        
		bool export_lib_fonts=false;
		std::string export_lib_fonts_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.ExportLibFonts", export_lib_fonts_str);
		if(export_lib_fonts_str=="true")
			export_lib_fonts=true;

		std::string sound_file_type_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.SaveSoundsAs", sound_file_type_str);
		if(sound_file_type_str=="0"){
			sound_file_type_str="keep";}
		else if(sound_file_type_str=="1"){
			sound_file_type_str="wav";}
		else if(sound_file_type_str=="2"){
			sound_file_type_str="mp3";}
		
		std::string geom_data_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.GeometryData", geom_data_str);
		if(geom_data_str=="0"){
			geom_data_str="7f";}
		else if(geom_data_str=="1"){
			geom_data_str="pos";}
		else if(geom_data_str=="2"){
			geom_data_str="posuv";}
		
		std::string font_data_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.FontData", font_data_str);
		if(font_data_str=="0"){
			font_data_str="7f";}
		else if(font_data_str=="1"){
			font_data_str="pos";}
		
		std::string timeline_depth_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.TimelineDepth", timeline_depth_str);
		if(timeline_depth_str=="0"){
			timeline_depth_str="AS2";}
		else if(timeline_depth_str=="1"){
			timeline_depth_str="AS3";}

		std::string radial_gradient_size_str;
		TYPES::UINT32 radial_gradient_size=512;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.RadialGradientSize", radial_gradient_size_str);
		if(radial_gradient_size_str=="0"){
			radial_gradient_size=128;}
		else if(radial_gradient_size_str=="1"){
			radial_gradient_size=256;}
		else if(radial_gradient_size_str=="2"){
			radial_gradient_size=512;}
		else if(radial_gradient_size_str=="3"){
			radial_gradient_size=1024;}
		else if(radial_gradient_size_str=="4"){
			radial_gradient_size=2048;}

		bool flipXaxis=false;
		/*std::string flipXaxis_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.FlipXaxis", flipXaxis_str);
		if(flipXaxis_str=="true")
			flipXaxis=true;*/
		bool flipYaxis=false;
		/*std::string flipYaxis_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.FlipYaxis", flipYaxis_str);
		if(flipYaxis_str=="true")
			flipYaxis=true;*/
		
	// get some properties from the AwayJS Document (color, framerate etc)		
		
		// background color of document
        DOM::Utils::COLOR color;
		res = pFlaDocument->GetBackgroundColor(color);
		ASSERT(FCM_SUCCESS_CODE(res));
		
		// height of document
        FCM::U_Int32 stageHeight;
		res = pFlaDocument->GetStageHeight(stageHeight);
		ASSERT(FCM_SUCCESS_CODE(res));
		
		// width of document
        FCM::U_Int32 stageWidth;
		res = pFlaDocument->GetStageWidth(stageWidth);
		ASSERT(FCM_SUCCESS_CODE(res));
		
		// fps of document
        FCM::Double fps;
		res = pFlaDocument->GetFrameRate(fps);
		ASSERT(FCM_SUCCESS_CODE(res));		
        
		// All Root-timelines (Scenes) of document
		FCM::FCMListPtr pTimelineList;
		res = pFlaDocument->GetTimelines(pTimelineList.m_Ptr);
		ASSERT(FCM_SUCCESS_CODE(res));

		// Number of Root-timelines (Scenes) of document
		FCM::U_Int32 timelineCount;
		res = pTimelineList->Count(timelineCount);
		ASSERT(FCM_SUCCESS_CODE(res));			
        
	// Fill AWDProject Settings:

		SETTINGS::Settings* awd_settings = this->awd_project->get_settings();
		
		awd_settings->set_export_framescripts(export_framescripts);
		awd_settings->set_radial_gradient_size(radial_gradient_size);
		awd_settings->set_fps(fps);
		awd_settings->set_sound_file_extension(sound_file_type_str);
		awd_settings->set_keep_verticles_in_path_order(keep_vert_in_path_order);
		awd_settings->set_max_iterations(maxIterations);
		awd_settings->set_curve_threshold(curveThreshold);
		awd_settings->set_resolve_intersections(resolveIntersects);
		awd_settings->set_flipXaxis(flipXaxis);
		awd_settings->set_flipYaxis(flipYaxis);
		awd_settings->set_export_invisible_timeline_layer(export_invisible_timeline_layer);
		awd_settings->set_export_shapes_in_debug_mode(export_shapes_in_debug_mode);
		awd_settings->set_export_curves(true);
		awd_settings->set_distinglish_interior_exterior_triangles_2d(distinglishexteriorandinterior_tris);
		awd_settings->set_exterior_threshold(exterior_threshold, TYPES::filled_region_type::STANDART_FILL);

		if(exterior_threshold_strokes)
			awd_settings->set_exterior_threshold(exterior_threshold, TYPES::filled_region_type::FILL_CONVERTED_FROM_STROKE);
		if(exterior_threshold_font)
			awd_settings->set_exterior_threshold(exterior_threshold, TYPES::filled_region_type::GENERATED_FONT_OUTLINES);
		
		awd_settings->set_embbed_audio(embbed_sounds);
		awd_settings->set_embbed_textures(embbed_textures);
         
	// Check if only active timeline should be exported. if yes, we set timelinecount to 1

		if(pTimeline!=NULL){
			Utils::Trace(GetCallback(), "\nExport only the current active TimeLine !\n");
			timelineCount=1;
		}
		
		
	// fill the awd-project with data:

		FCM::U_Int32 i=0;
		
		std::vector<BASE::AWDBlock*> all_timelines;	
		std::vector<BASE::AWDBlock*> all_bitmaps;	
		std::vector<BASE::AWDBlock*> all_audios;
		std::vector<BASE::AWDBlock*> all_fonts;	
		std::vector<BASE::AWDBlock*> all_tf;	
		std::vector<BASE::AWDBlock*> all_textformats;	
		std::vector<BASE::AWDBlock*> all_shapes;
		std::vector<BASE::AWDBlock*> all_billboards;
		std::vector<BASE::AWDBlock*> all_materials;
		std::vector<BASE::AWDBlock*> all_meshes;
		
		int cnt_before=0;
		int grafics_cnt_start=0;
		int cnt_blocks=0;
		
		Utils::Trace(GetCallback(), "\nStart encoding Scenes:\n");
		Utils::Trace(GetCallback(), "Encoding Using Adobe-Frame-Generator.\n");
			
		// Here the code splits into using Adobe-Frame-Generator or own implementation:
		
		AutoPtr<IFrameCommandGenerator> m_frameCmdGeneratorService;
		AutoPtr<IResourcePalette> m_pResourcePalette;		
		AwayJS::ResourcePalette* pResPalette;
		Utils::Trace(GetCallback(), "Encoding data....\n");
		res = GetCallback()->GetService(Exporter::Service::EXPORTER_FRAME_CMD_GENERATOR_SERVICE, pUnk.m_Ptr);
		ASSERT(FCM_SUCCESS_CODE(res));
		m_frameCmdGeneratorService = pUnk;
					
		res = GetCallback()->CreateInstance(NULL, CLSID_ResourcePalette, IID_IResourcePalette, (void**)&m_pResourcePalette);
		ASSERT(FCM_SUCCESS_CODE(res));
			
		pResPalette = static_cast<ResourcePalette*>(m_pResourcePalette.m_Ptr);
		pResPalette->Init(this->flash_to_awd_encoder);	
		AutoPtr<ITimelineBuilderFactory> pTimelineBuilderFactory;
		res = GetCallback()->CreateInstance(NULL, CLSID_TimelineBuilderFactory, IID_ITimelineBuilderFactory, (void**)&pTimelineBuilderFactory);
		ASSERT(FCM_SUCCESS_CODE(res));
		(static_cast<TimelineBuilderFactory*>(pTimelineBuilderFactory.m_Ptr))->Init(this->flash_to_awd_encoder);
			
		// if we have a input-timeline (pTimeline), we only export this (timelineCount is already 1 in this case)
		int cntAllressources=0;
		for (i = 0; i < timelineCount; i++)
		{			
			AutoPtr<DOM::ITimeline> timeline = pTimelineList[i];
			if(pTimeline!=NULL)
				timeline = pTimeline;

			AutoPtr<ITimelineBuilder> pTimelineBuilder;
			ITimelineWriter* pTimelineWriter;
			Exporter::Service::RANGE range;		
			range.min = 0;
			res = timeline->GetMaxFrameCount(range.max);
			range.max--;
				

			FCM::StringRep16 scene_name_str;
			timeline->GetName(&scene_name_str);
			std::string scene_name=AwayJS::Utils::ToString(scene_name_str, GetCallback());
			this->flash_to_awd_encoder->current_scene_name=scene_name;
			// Generate frame commands		
			res = m_frameCmdGeneratorService->GenerateFrameCommands(timeline, range, pDictPublishSettings,	m_pResourcePalette, pTimelineBuilderFactory, pTimelineBuilder.m_Ptr);
			ASSERT(FCM_SUCCESS_CODE(res));

				
			((TimelineBuilder*)pTimelineBuilder.m_Ptr)->Build(0, scene_name_str, &pTimelineWriter);
				
			BLOCKS::Timeline* scene_timeline_block = reinterpret_cast<BLOCKS::Timeline* >(this->awd_project->get_block_by_external_id_and_type_shared(std::to_string(0), BLOCK::block_type::TIMELINE, false));
			if(scene_timeline_block!=NULL){
				scene_timeline_block->set_scene_id(i+1);
			}
			if(this->awd_project->get_blocks_for_external_ids()!=result::AWD_SUCCESS)
				Utils::Trace(GetCallback(), "PROBLEM IN CONVERTING RESSOURCE_ID TO AWDBLOCKS FOR FRAMECOMMANDS!!!\nEXPORT STILL CONTINUES !!!\n");

			pResPalette->Clear();

					
			// print out results for this scene:
			Utils::Trace(GetCallback(), "->	Encoded '%s'\n", scene_name.c_str());

			cnt_before=all_timelines.size();
			this->awd_project->get_blocks_by_type(all_timelines, BLOCK::block_type::TIMELINE);
				
			if(((all_timelines.size()-cnt_before)-(this->flash_to_awd_encoder->grafik_cnt-grafics_cnt_start))>0)
				Utils::Trace(GetCallback(), "->		MovieClips:		%d\n", ((all_timelines.size()-cnt_before)-(this->flash_to_awd_encoder->grafik_cnt-grafics_cnt_start)));
						
			if((this->flash_to_awd_encoder->grafik_cnt-grafics_cnt_start)>0)
				Utils::Trace(GetCallback(), "->		Graphics:		%d\n", (this->flash_to_awd_encoder->grafik_cnt-grafics_cnt_start));
					
			grafics_cnt_start = this->flash_to_awd_encoder->grafik_cnt;	

			cnt_before=all_bitmaps.size();
			this->awd_project->get_blocks_by_type(all_bitmaps, BLOCK::block_type::BITMAP_TEXTURE);
			if((all_bitmaps.size()-cnt_before)>0)
				Utils::Trace(GetCallback(), "->		Bitmaps:		%d\n", (all_bitmaps.size()-cnt_before));				
				
			cnt_before=all_billboards.size();
			this->awd_project->get_blocks_by_type(all_billboards, BLOCK::block_type::BILLBOARD);
			if((all_billboards.size()-cnt_before)>0)
				Utils::Trace(GetCallback(), "->		Billboards:		%d\n", (all_billboards.size()-cnt_before));
				
			cnt_before=all_audios.size();
			this->awd_project->get_blocks_by_type(all_audios, BLOCK::block_type::SOUND_SOURCE);
			if((all_audios.size()-cnt_before)>0)
				Utils::Trace(GetCallback(), "->		Sounds:			%d\n", (all_audios.size()-cnt_before));				
	
			cnt_before=all_fonts.size();
			this->awd_project->get_blocks_by_type(all_fonts, BLOCK::block_type::FONT);
			if((all_fonts.size()-cnt_before)>0)
				Utils::Trace(GetCallback(), "->		Fonts:			%d\n", (all_fonts.size()-cnt_before));				
			
			cnt_before=all_textformats.size();
			this->awd_project->get_blocks_by_type(all_textformats, BLOCK::block_type::FORMAT);
			if((all_textformats.size()-cnt_before)>0)
				Utils::Trace(GetCallback(), "->		Text-formats:	%d\n", (all_textformats.size()-cnt_before));
				

			cnt_before=all_tf.size();
			this->awd_project->get_blocks_by_type(all_tf, BLOCK::block_type::TEXT_ELEMENT);
			if((all_tf.size()-cnt_before)>0)
				Utils::Trace(GetCallback(), "->		Text-fields:	%d\n", (all_tf.size()-cnt_before));
				
					
			cnt_before=all_shapes.size();
			this->awd_project->get_blocks_by_type(all_shapes, BLOCK::block_type::TRI_GEOM);
			if((all_shapes.size()-cnt_before)>0)
				Utils::Trace(GetCallback(), "->		Shapes/Meshes:	%d\n", (all_shapes.size()-cnt_before));
				
			cnt_before=all_meshes.size();
			this->awd_project->get_blocks_by_type(all_meshes, BLOCK::block_type::MESH_INSTANCE_2);

			cnt_before=all_materials.size();
			this->awd_project->get_blocks_by_type(all_materials, BLOCK::block_type::SIMPLE_MATERIAL);
			if((all_materials.size()-cnt_before)>0)
				Utils::Trace(GetCallback(), "->		Materials:		%d\n", (all_materials.size()-cnt_before));
		}			

		// now all timelines should have been filled with data (Timelineframes)
		// for all encountered frame-objects (display-objects / sounds) a AWDBlock should have been created.

		// we might want to include assets that are not part of any timeline, and hence not added to the awd_project yet.

		Utils::Trace(GetCallback(), "\nScenes encoded\n");

		Utils::Trace(GetCallback(), "\nStart encoding library\n");
		
		FCM::FCMListPtr pLibraryItemList;
		res = pFlaDocument->GetLibraryItems(pLibraryItemList.m_Ptr);
		if (FCM_FAILURE_CODE(res)){
			Utils::Trace(GetCallback(), "FAILED TO GET LIBRARY ITEMS FROM FLASH \n");
			return FCM_EXPORT_FAILED;
		}
		this->flash_to_awd_encoder->current_scene_name="Library";
		if(this->ExportLibraryItems(pLibraryItemList, export_lib_bitmaps, export_lib_sounds, export_lib_fonts, useADobeFramegenerator)!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "FAILED TO COLLECT LIBRARY ITEMS \n");
			return FCM_EXPORT_FAILED;
		}		
		Utils::Trace(GetCallback(), "Library encoded.\n");
			
		cnt_before=all_timelines.size();
		this->awd_project->get_blocks_by_type(all_timelines, BLOCK::block_type::TIMELINE);
		for(cnt_blocks=cnt_before; cnt_blocks<all_timelines.size(); cnt_blocks++){
			all_timelines[cnt_blocks]->set_encountered_at("Library");
		}
		if(((all_timelines.size()-cnt_before)-(this->flash_to_awd_encoder->grafik_cnt-grafics_cnt_start))>0){
			Utils::Trace(GetCallback(), "->		MovieClips:		%d\n", ((all_timelines.size()-cnt_before)-(this->flash_to_awd_encoder->grafik_cnt-grafics_cnt_start)));
		}
		
		if((this->flash_to_awd_encoder->grafik_cnt-grafics_cnt_start)>0){
			Utils::Trace(GetCallback(), "->		Graphics:		%d\n", (this->flash_to_awd_encoder->grafik_cnt-grafics_cnt_start));
		}
		
		cnt_before=all_bitmaps.size();
		this->awd_project->get_blocks_by_type(all_bitmaps, BLOCK::block_type::BITMAP_TEXTURE);
		if((all_bitmaps.size()-cnt_before)>0)
			Utils::Trace(GetCallback(), "->		Bitmaps:		%d\n", (all_bitmaps.size()-cnt_before));
		
		cnt_before=all_billboards.size();
		this->awd_project->get_blocks_by_type(all_billboards, BLOCK::block_type::BILLBOARD);
		if((all_billboards.size()-cnt_before)>0)
			Utils::Trace(GetCallback(), "->		Billboards:			%d\n", (all_billboards.size()-cnt_before));
		
		for(AWDBlock* one_block : all_billboards){
			BLOCKS::Billboard* this_lib_mesh = reinterpret_cast<BLOCKS::Billboard*>(one_block);
			for(std::string one_name:this_lib_mesh->get_material()->scene_names){
				one_block->add_scene_name(one_name); 
			}
		}
		cnt_before=all_audios.size();
		this->awd_project->get_blocks_by_type(all_audios, BLOCK::block_type::SOUND_SOURCE);
		if((all_audios.size()-cnt_before)>0)
			Utils::Trace(GetCallback(), "->		Sounds:			%d\n", (all_audios.size()-cnt_before));
	
		cnt_before=all_fonts.size();
		this->awd_project->get_blocks_by_type(all_fonts, BLOCK::block_type::FONT);
		if((all_fonts.size()-cnt_before)>0)
			Utils::Trace(GetCallback(), "->		Fonts:			%d\n", (all_fonts.size()-cnt_before));
			
		cnt_before=all_textformats.size();
		this->awd_project->get_blocks_by_type(all_textformats, BLOCK::block_type::FORMAT);
		if((all_textformats.size()-cnt_before)>0)
			Utils::Trace(GetCallback(), "->		Text-formats:	%d\n", (all_textformats.size()-cnt_before));

		cnt_before=all_tf.size();
		this->awd_project->get_blocks_by_type(all_tf, BLOCK::block_type::TEXT_ELEMENT);
		if((all_tf.size()-cnt_before)>0)
			Utils::Trace(GetCallback(), "->		Text-fields:	%d\n", (all_tf.size()-cnt_before));
					
		cnt_before=all_shapes.size();
		this->awd_project->get_blocks_by_type(all_shapes, BLOCK::block_type::TRI_GEOM);
		if((all_shapes.size()-cnt_before)>0)
			Utils::Trace(GetCallback(), "->		Shapes/Meshes:	%d\n", (all_shapes.size()-cnt_before));

		cnt_before=all_meshes.size();
		this->awd_project->get_blocks_by_type(all_meshes, BLOCK::block_type::MESH_INSTANCE_2);
		if((all_meshes.size()-cnt_before)>0){
			for(cnt_blocks=cnt_before; cnt_blocks<all_meshes.size(); cnt_blocks++){
				all_meshes[cnt_blocks]->set_encountered_at("Library");
			}
		}
		for(AWDBlock* one_block : all_meshes){
			BLOCKS::MeshLibrary* this_lib_mesh = reinterpret_cast<BLOCKS::MeshLibrary*>(one_block);
			for(std::string one_name:this_lib_mesh->get_geom()->scene_names){
				one_block->add_scene_name(one_name); 
			}
		}
		
		cnt_before=all_materials.size();
		this->awd_project->get_blocks_by_type(all_materials, BLOCK::block_type::SIMPLE_MATERIAL);
		if((all_materials.size()-cnt_before)>0)
			Utils::Trace(GetCallback(), "->		Materials:	%d\n", (all_materials.size()-cnt_before));
		


		Utils::Trace(GetCallback(), "\nAll enountered object:\n");
			
		Utils::Trace(GetCallback(), "->		MovieClips:		%d\n", (all_timelines.size()-this->flash_to_awd_encoder->grafik_cnt));
		Utils::Trace(GetCallback(), "->		Graphics:		%d\n", this->flash_to_awd_encoder->grafik_cnt);
		Utils::Trace(GetCallback(), "->		Bitmaps:		%d\n", all_bitmaps.size());
		Utils::Trace(GetCallback(), "->		BillBoards:		%d\n", all_billboards.size());
		Utils::Trace(GetCallback(), "->		Sounds:			%d\n", all_audios.size());
		Utils::Trace(GetCallback(), "->		Fonts:			%d\n", all_fonts.size());
		Utils::Trace(GetCallback(), "->		Text-fields:	%d\n", all_tf.size());
		Utils::Trace(GetCallback(), "->		Encountered Shape-instances:			%d\n", this->flash_to_awd_encoder->shape_instance_cnt);
		Utils::Trace(GetCallback(), "->		Cached Shapes:			%d\n", all_shapes.size());
		Utils::Trace(GetCallback(), "->		Meshes:			%d\n", all_meshes.size());
		Utils::Trace(GetCallback(), "->		Materials:		%d\n", all_materials.size());

		
		Utils::Trace(GetCallback(), "\nAll Data Collected\n");
		
		// process fonts
		if(all_fonts.size()>0){
			Utils::Trace(GetCallback(), "\nStart finalize Fonts\n");
			this->flash_to_awd_encoder->FinalizeFonts(this->fla_document);
			Utils::Trace(GetCallback(), "All Fonts processed succesfully.\n");
		}
		
		// process geometries
		AWD::result awd_res;		
		if(all_shapes.size()>0){
			Utils::Trace(GetCallback(), "\nStart processing Geometry-Data\n");
			for(AWDBlock* awd_block : all_shapes){
				BLOCKS::Geometry* geom = reinterpret_cast<BLOCKS::Geometry*>(awd_block);
				if(geom==NULL){
					Utils::Trace(GetCallback(), "FAILED TO GET GEOMETRY FROM BASEBLOCK\n");
					return FCM_EXPORT_FAILED;
				}
                
				DOM::FrameElement::IShape* testshape = reinterpret_cast<DOM::FrameElement::IShape*>(geom->get_external_object());
				if(testshape!=NULL){
					testshape->Release();
				}
                // create stream settings for normal shapes
				awd_res = GEOM::ProcessShapeGeometry(geom, awd_project, this->awd_project->get_settings());
				if(awd_res!=result::AWD_SUCCESS){
					Utils::Trace(GetCallback(), "ERROR WHILE PROCESSING GEOMETRY name %s\n", awd_block->get_name().c_str());
					return FCM_EXPORT_FAILED;
				}
                /*
				std::vector<std::string> messages;
				geom->get_messages(messages, " -> ");
				Utils::Trace(GetCallback(), "Geometry process report: name %s\n", geom->get_name().c_str());
				for(std::string one_message : messages){
					if(one_message.size()<1024)
						Utils::Trace(GetCallback(), "%s\n", one_message.c_str());
				}
				*/
			}
			Utils::Trace(GetCallback(), "Succesfully converted %d Shapes into AWD-Geometry\n", all_shapes.size());
		}
		
		// create textuzre-atlas, create uvs-for shapes, merge and apply materials 
		if((all_shapes.size()>0)||(all_fonts.size())){
			Utils::Trace(GetCallback(), "\nStart creating Textureatlas\n");
			std::string filepath_no_extension2;
			if(AWD::FILES::extract_path_without_file_extension(outFile, filepath_no_extension2)!=result::AWD_SUCCESS){
				Utils::Trace(GetCallback(), "FAILED TO GET FILENAME FROM PATH \n");
				return FCM_EXPORT_FAILED;
			}
			std::string fileName2;
			if(AWD::FILES::extract_name_from_path(filepath_no_extension2, fileName2)!=result::AWD_SUCCESS){
				Utils::Trace(GetCallback(), "FAILED TO GET FILENAME FROM PATH \n");
				return FCM_EXPORT_FAILED;
			}		
			AWD::create_TextureAtlasfor_timelines(this->awd_project, fileName2);
			
			cnt_before=all_materials.size();
			this->awd_project->get_blocks_by_type(all_materials, BLOCK::block_type::SIMPLE_MATERIAL);
			int matcnt = 0;
			for(cnt_blocks=0; cnt_blocks<all_materials.size(); cnt_blocks++){
				if(all_materials[cnt_blocks]->get_state()==TYPES::state::VALID)
					matcnt++;
			}
			if(matcnt!=cnt_before){
				Utils::Trace(GetCallback(), "	Merged %d materials into %d materials\n", cnt_before, matcnt);
			}
			Utils::Trace(GetCallback(), "Succesfully created Textureatlas\n", all_shapes.size());

		}
		
		// export the awd-file.
		Utils::Trace(GetCallback(), "\nStart exporting into awd-file\n");
		awd_res = this->awd_project->export_file();
		if(awd_res!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "FAILED TO EXPORT %d\n", awd_res);
			return FCM_EXPORT_FAILED;
		}		
		Utils::Trace(GetCallback(), "The AWD-file: '%s' should have been created.\n", outFile.c_str());

			
		if((print_export_log)&&(all_timelines.size()>0)){
			Utils::Trace(GetCallback(), "\nTimelines exported: %d\n", all_timelines.size());

			for(AWDBlock* awd_block : all_timelines){
				BLOCKS::Timeline* timeline = reinterpret_cast<BLOCKS::Timeline*>(awd_block);
				if(timeline==NULL){
					Utils::Trace(GetCallback(), "FAILED TO GET TIMELINE FROM BASEBLOCK\n");
					return FCM_EXPORT_FAILED;
				}               
				if(false)//timeline->is_grafic_instance)
					Utils::Trace(GetCallback(), "Timelines for grapic instances no longer exported\n");
				else{
					//startTime=(double)AWD::GetTimeMs64();
					Utils::Trace(GetCallback(), "	Timeline: '%s' framecount: %d\n", timeline->get_name().c_str(), timeline->get_frames().size());
					std::vector<std::string> messages;
					timeline->get_frames_info(messages);
					for(std::string one_message : messages){
						if(one_message.size()<1024)
							Utils::Trace(GetCallback(), "		%s\n", one_message.c_str());
					}
				}
			}
		}
		

		// make sure all textfields have a name. if they do not have a name already, give them a incremental ("tf_"+cnt)
		if((print_export_log)&&(all_tf.size()>0)){
			int cnt_tf=0;
			for(AWDBlock* awd_block: all_tf){
				BLOCKS::TextElement* this_text = reinterpret_cast<BLOCKS::TextElement*>(awd_block);
				if(this_text->get_tf_type()!=FONT::Textfield_type::STATIC){
					std::string tf_type="";
					if(this_text->get_tf_type()==FONT::Textfield_type::DYNAMIC){tf_type="DYNAMIC";}
					else if(this_text->get_tf_type()==FONT::Textfield_type::INPUT){tf_type="INPUT";}
					else if(this_text->get_tf_type()==FONT::Textfield_type::INPUT_PSW){tf_type="INPUT_PSW";}
					Utils::Trace(GetCallback(), "Textfield - name: '%s' type: '%s' text: '%s'\n", this_text->get_name().c_str(), tf_type.c_str(), this_text->get_text().c_str());
				}
				/*
				if(awd_block->get_name().size()==0){
					cnt_tf++;
					awd_block->set_name("tf"+std::to_string(cnt_tf));
				}
				*/
			}
		}
		if(print_export_log){
			// return statistics of the awd-project.
			std::vector<std::string> statistic_str;
			this->awd_project->get_statistics(statistic_str);
			for(std::string message : statistic_str)
				Utils::Trace(GetCallback(), "%s\n", message.c_str());
		


			int all_bytes=0;
			int this_bytes=0;
			for(AWDBlock* awd_block : all_timelines){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "Timelines:	%d kb\n", (this_bytes/1024));
			}

			this_bytes=0;
			for(AWDBlock* awd_block : all_shapes){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "Shapes:	%d kb\n", (this_bytes/1024));
			}
			this_bytes=0;
			for(AWDBlock* awd_block : all_meshes){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "Meshes:	%d kb\n", (this_bytes/1024));
			}

			this_bytes=0;
			for(AWDBlock* awd_block : all_audios){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "Sounds:	%d kb\n", (this_bytes/1024));
			}
			this_bytes=0;
			for(AWDBlock* awd_block : all_materials){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "Materials:	%d kb\n", (this_bytes/1024));
			}
			this_bytes=0;
			for(AWDBlock* awd_block : all_bitmaps){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "Bitmaps:	%d kb\n", (this_bytes/1024));
			}
			this_bytes=0;
			for(AWDBlock* awd_block : all_billboards){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "Billboards:	%d kb\n", (this_bytes/1024));
			}
			this_bytes=0;
			for(AWDBlock* awd_block : all_fonts){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "Fonts:	%d kb\n", (this_bytes/1024));
			}
			this_bytes=0;
			for(AWDBlock* awd_block : all_textformats){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "TextFormats:	%d kb\n", (this_bytes/1024));
			}
			this_bytes=0;
			for(AWDBlock* awd_block : all_tf){
				this_bytes += awd_block->byte_cnt;
				all_bytes += awd_block->byte_cnt;
			}
			if(this_bytes>0){
				Utils::Trace(GetCallback(), "TextFields:	%d kb\n", (this_bytes/1024));
			}
			if(all_bytes>0){
				Utils::Trace(GetCallback(), "\nAll:	%d kb\n", (all_bytes/1024));
			}
		}





		// copy the Preview files to output directory
		 
		if(openPreview){
			
			std::string preview_file;
			if(preview_source_path.empty()){
				std::string plugin_directory;
				Utils::GetModuleFilePath(plugin_directory, GetCallback());
        
#ifdef _WINDOWS
                std::string immParent;
                Utils::GetParent(plugin_directory, immParent);
                Utils::GetParent(immParent, plugin_directory);
                Utils::GetParent(plugin_directory, immParent);
                Utils::GetParent(immParent, plugin_directory);
                Utils::GetParent(plugin_directory, immParent);
                Utils::GetParent(immParent, plugin_directory);
                preview_file = plugin_directory+"ressources/preview_awayjs/index.html";
#endif
                
#ifdef __APPLE__
                std::string findThis="plugin/lib/mac/AwayExtensionsFlashPro.fcm.plugin/Contents";
                int foundIndex=plugin_directory.find(findThis);
                if(foundIndex>0){
                    plugin_directory = plugin_directory.substr(0, foundIndex);
                }
                preview_file = plugin_directory+"ressources/preview_awayjs/index.html";
#endif
			}
			else{
				preview_file=preview_source_path;
			}
			 
			std::string fileName;
			if(AWD::FILES::extract_name_from_path(outFile, fileName)!=result::AWD_SUCCESS){
				Utils::Trace(GetCallback(), "FAILED TO GET FILENAME FROM PATH \n");
				return FCM_EXPORT_FAILED;
			}
			// lists that contain the id and the values thta should be used to replace options in preview-file
			std::vector<std::string> preview_ids;
			std::vector<std::string> preview_values;
		
			preview_ids.push_back("AWD_FILENAME_AWD");
			preview_values.push_back(fileName);

			preview_ids.push_back("AWD_BGCOLOR_AWD");
			preview_values.push_back(AwayJS::Utils::ToString(color));

			preview_ids.push_back("AWD_FPS_AWD");
			preview_values.push_back(std::to_string(fps));

			preview_ids.push_back("AWD_STAGEWIDTH_AWD");
			preview_values.push_back(std::to_string(stageWidth));

			preview_ids.push_back("AWD_STAGEHEIGHT_AWD");
			preview_values.push_back(std::to_string(stageHeight));
		

			if(this->awd_project->open_preview(preview_file, preview_ids, preview_values, preview_output_path, append_file_name)!=result::AWD_SUCCESS){
				Utils::Trace(GetCallback(), "FAILED TO OPEN PREVIEW FILE \n");
				return FCM_EXPORT_FAILED;
			}

		}
		Utils::Trace(GetCallback(), "AwayJS-Document-Export Complete.");
		
		delete this->awd_project;
		delete this->flash_to_awd_encoder;
					
		//_CrtDumpMemoryLeaks();
        return FCM_SUCCESS;
    }

    FCM::Result RegisterPublisher(PIFCMDictionary pPlugins, FCM::FCMCLSID docId)
    {
        FCM::Result res;
        /*
         * Dictionary structure for a Publisher plugin is as follows:
         *
         *  Level 0 :    
         *              -----------------------
         *             | Flash.Plugin |  ----- | --------------------------------
         *              -----------------------                                  |
         *                                                                       |
         *  Level 1:                                   <-------------------------                          
         *              ------------------------------  
         *             | CLSID_Publisher_GUID | ----- | -------------------------
         *              ------------------------------                           |
         *                                                                       |
         *  Level 2:                                      <---------------------- 
         *              -----------------------------------
         *             | Flash.Category.Publisher |  ----- |---------------------
         *              -----------------------------------                      |
         *                                                                       |
         *  Level 3:                                                           <-
         *              ---------------------------------------------------------
         *             | Flash.Plugin.Name                  | "SamplePlugin"     |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.UI                    | "com.example..."   |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.TargetFormatName      | "xyz"              |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.TargetFormatExtension | "abc"              |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.CanExportImage        | "true"             |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.TargetDocs            |           ---------|-
         *              ---------------------------------------------------------  |
         *                                                                         |
         *  Level 4:                                                    <----------
         *              -----------------------------------------------
         *             | CLSID_DocType   |  Empty String               |
         *              -----------------------------------------------
         *
         *  Note that before calling this function the level 0 dictionary has already
         *  been added. Here, the 1st, 2nd and 3rd level dictionaries are being added.
         */ 

        {
            // Level 1 Dictionary
            AutoPtr<IFCMDictionary> pPlugin;
            res = pPlugins->AddLevel(
                (const FCM::StringRep8)Utils::ToString(CLSID_Publisher).c_str(), 
                pPlugin.m_Ptr);

            {
                // Level 2 Dictionary
                AutoPtr<IFCMDictionary> pCategory;
                res = pPlugin->AddLevel(
                    (const FCM::StringRep8)kFlashCategoryKey_Publisher, 
                    pCategory.m_Ptr);

                {
                    // Level 3 Dictionary
                    std::string str_name = "AwayJS";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_Name,
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)str_name.c_str(),
                        (FCM::U_Int32)str_name.length() + 1);

                    std::string str_uniname = "AwayJS";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_UniversalName,
                        kFCMDictType_StringRep8,
                        (FCM::PVoid)str_uniname.c_str(),
                        (FCM::U_Int32)str_uniname.length() + 1);

                    std::string str_ui = "AwayExtensionsFlashPro.PublishSettings";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_UI, 
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)str_ui.c_str(),
                        (FCM::U_Int32)str_ui.length() + 1);
					
                    AutoPtr<IFCMDictionary> pDocs;
                    res = pCategory->AddLevel((const FCM::StringRep8)kFlashPublisherKey_TargetDocs, pDocs.m_Ptr);

                    {
                        // Level 4 Dictionary
                        std::string empytString = "";   
                        res = pDocs->Add(
                            (const FCM::StringRep8)Utils::ToString(docId).c_str(), 
                            kFCMDictType_StringRep8, 
                            (FCM::PVoid)empytString.c_str(),
                            (FCM::U_Int32)empytString.length() + 1);
                    }
                }
            }
        }

        return res;
    }

};
