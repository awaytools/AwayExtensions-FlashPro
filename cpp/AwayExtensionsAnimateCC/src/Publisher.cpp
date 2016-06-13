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

#include <iostream>
#include <fstream>
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
		
		this->fla_document = pFlaDocument;

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

        const std::string	generator_name		= "Adobe Animate CC";
		FCM::U_Int32 app_version=0;
		AwayJS::Utils::GetAppVersion(GetCallback(), app_version);
		int majorversion = ((app_version >> 24) & 0xff);
		int minorversion = ((app_version >> 16) & 0xff);
		int patchversion = ((app_version >> 8) & 0xff);
		int buildversion = ((app_version) & 0xff);
		const std::string	generator_version = FILES::int_to_string(majorversion)+"."+FILES::int_to_string(minorversion)+"."+FILES::int_to_string(patchversion)+"."+FILES::int_to_string(buildversion);

// create a AWDProject

		this->awd_project = new AWDProject(TYPES::project_type::SINGLE_FILE_EXPORT, outFile, generator_name, generator_version);
		// validate the state of the Project.
		if(this->awd_project->get_state()==TYPES::state::INVALID){
			std::vector<std::string> errors;
			this->awd_project->get_errors(errors, "");
			for(std::string one_error : errors)
				Utils::Trace(GetCallback(), "AWDProject Error: : %s\n", one_error.c_str());
			return FCM_EXPORT_FAILED;
		}
		
// create a AnimateToAWDEncoder

		this->animate_to_awd_encode = new AnimateToAWDEncoder(GetCallback(), this->awd_project);
		

// Fill AWDProject Settings:

		SETTINGS::Settings* awd_settings = this->awd_project->get_settings();
		
		// general settings
		bool openPreview = GetSettingsBool(pDictPublishSettings, "PublishSettings.OpenPreview", true);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::OpenPreview, openPreview);
		bool copyRuntime = GetSettingsBool(pDictPublishSettings, "PublishSettings.CopyRuntime", false);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::CopyRuntime, copyRuntime);
		bool print_export_log = GetSettingsBool(pDictPublishSettings, "PublishSettings.PrintExportLog", false);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::PrintExportLog, print_export_log);
		bool print_export_log_timelines = GetSettingsBool(pDictPublishSettings, "PublishSettings.PrintExportLogTimelines", false);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::PrintExportLogTimelines, print_export_log_timelines);
		
		
		// timelines settings
		bool exportTimelines=GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportTimelines", true);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportTimelines, exportTimelines);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::IncludeInvisibleTimelineLayer, GetSettingsBool(pDictPublishSettings, "PublishSettings.IncludeInvisibleLayer", true));
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportFrameScript, GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportFrameScript", true));
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExternalScripts, GetSettingsBool(pDictPublishSettings, "PublishSettings.ExternalScripts", true));
		
		
		// geometries settings
		bool exportGeometries=GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportGeometries", true);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportGeometries, exportGeometries);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportShapesInDebugMode, GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportShapesInDebugMode", true));
		
		// font settings
		bool exportFonts=GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportFonts", true);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportFonts, exportFonts);
		bool export_lib_fonts = GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportLibFonts", true);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportLibFonts, export_lib_fonts);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::EmbbedAllChars, GetSettingsBool(pDictPublishSettings, "PublishSettings.EmbbedAllChars", true));
		
		// bitmap settings
		bool exportBitmaps=GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportBitmaps", true);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportBitmaps, exportBitmaps);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ForceTextureOverwrite, GetSettingsBool(pDictPublishSettings, "PublishSettings.ForceTextureOverwrite", true));
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::EmbbedTextures, GetSettingsBool(pDictPublishSettings, "PublishSettings.EmbbedTextures", true));
		bool export_lib_bitmaps = GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportLibBitmaps", true);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportLibBitmaps, export_lib_bitmaps);

		// sound settings
		bool exportSounds=GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportSounds", true);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportSounds, exportSounds);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ForceSoundOverwrite, GetSettingsBool(pDictPublishSettings, "PublishSettings.ForceSoundOverwrite", true));
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::EmbbedSounds, GetSettingsBool(pDictPublishSettings, "PublishSettings.EmbbedSounds", true));
		bool export_lib_sounds=GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportLibSounds", true);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportLibSounds, export_lib_sounds);
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportTimelineSounds, GetSettingsBool(pDictPublishSettings, "PublishSettings.ExportTimelineSounds", true));
		awd_settings->set_bool(AWD::SETTINGS::bool_settings::CreateAudioMap, GetSettingsBool(pDictPublishSettings, "PublishSettings.CreateAudioMap", false));
		
		std::string sound_file_type_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.SaveSoundsAs", sound_file_type_str);
		if(sound_file_type_str=="0"){
			sound_file_type_str="keep";}
		else if(sound_file_type_str=="1"){
			sound_file_type_str="wav";}
		else if(sound_file_type_str=="2"){
			sound_file_type_str="mp3";}
		awd_settings->set_sound_file_extension(sound_file_type_str);
		
		
		


		std::string preview_source_path;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.PreviewPathSource", preview_source_path);
		std::string preview_output_path;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.PreviewPath", preview_output_path);
		bool append_file_name = GetSettingsBool(pDictPublishSettings, "PublishSettings.AppendFilename", false);
					

		// some settings that are no longer available in the UI, but need setting anyway until they are refactored out for good:
		awd_settings->set_curve_threshold(0.05);
		awd_settings->set_max_iterations(50);
		awd_settings->set_exterior_threshold(0.0, TYPES::filled_region_type::STANDART_FILL);
		awd_settings->set_distinglish_interior_exterior_triangles_2d(false);
		awd_settings->set_resolve_intersections(true);
		awd_settings->set_keep_verticles_in_path_order(true);
		awd_settings->set_flipXaxis(false);
		awd_settings->set_flipYaxis(false);
		awd_settings->set_export_curves(true);

		
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
		awd_settings->set_fps(fps);
        
		// All Root-timelines (Scenes) of document
		FCM::FCMListPtr pTimelineList;
		res = pFlaDocument->GetTimelines(pTimelineList.m_Ptr);
		ASSERT(FCM_SUCCESS_CODE(res));

		// Number of Root-timelines (Scenes) of document
		FCM::U_Int32 timelineCount;
		res = pTimelineList->Count(timelineCount);
		ASSERT(FCM_SUCCESS_CODE(res));			
        
		
         
	// Check if only active timeline should be exported. if yes, we set timelinecount to 1

		if(pTimeline!=NULL){
			if(print_export_log)
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
		
		if(print_export_log)
			Utils::Trace(GetCallback(), "\nReading Settings for export took '%d' ms\n", this->awd_project->get_time_since_last_call());
		
		if(print_export_log)
			Utils::Trace(GetCallback(), "\nStart encoding Scenes:");
		
		// if we have a input-timeline (pTimeline), we only export this (timelineCount is already 1 in this case)
		int cntAllressources=0;
		for (i = 0; i < timelineCount; i++)
		{

			AutoPtr<DOM::ITimeline> timeline = pTimelineList[i];
			if(pTimeline!=NULL)
				timeline = pTimeline;
			
			this->ExportTimeline(pDictPublishSettings, timeline, i);		
		}
		if(print_export_log)
			Utils::Trace(GetCallback(), "\nEncoding  Scenes took '%d' ms\n", this->awd_project->get_time_since_last_call());
		
		
		this->awd_project->get_blocks_by_type(all_timelines, BLOCK::block_type::MOVIECLIP);
		if(all_timelines.size()>0){
            if(print_export_log){
                Utils::Trace(GetCallback(), "\nStart finalize %d Timelines", all_timelines.size());}
			this->awd_project->finalize_timelines();
			if(print_export_log)
				Utils::Trace(GetCallback(), "\nFinalizing Timelines took '%d' ms\n", this->awd_project->get_time_since_last_call());
		}
		
		if(print_export_log)
			Utils::Trace(GetCallback(), "\nStart encoding library (audio / bitmaps / movieclips with script linkage");
		FCM::FCMListPtr pLibraryItemList;
		res = pFlaDocument->GetLibraryItems(pLibraryItemList.m_Ptr);
		if (FCM_FAILURE_CODE(res)){
			Utils::Trace(GetCallback(), "\n		FAILED TO GET LIBRARY ITEMS FROM FLASH ");
			return FCM_EXPORT_FAILED;
		}
		this->animate_to_awd_encode->current_scene_name="Library";

		if(this->ExportLibraryItems(pLibraryItemList, export_lib_bitmaps, export_lib_sounds, export_lib_fonts, true)!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "\n		FAILED TO COLLECT LIBRARY ITEMS \n");
			return FCM_EXPORT_FAILED;
		}	
		if(print_export_log)
			Utils::Trace(GetCallback(), "\nEncoding Library took '%d' ms\n", this->awd_project->get_time_since_last_call());
		
		this->awd_project->get_blocks_by_type(all_timelines, BLOCK::block_type::MOVIECLIP);
		if(all_timelines.size()>0){
            if(print_export_log){
                Utils::Trace(GetCallback(), "\nStart finalize %d Timelines", all_timelines.size());}
			this->awd_project->finalize_timelines();
			if(print_export_log)
				Utils::Trace(GetCallback(), "\nFinalizing Timelines took '%d' ms\n", this->awd_project->get_time_since_last_call());
		}
		
		/*
		
		Utils::Trace(GetCallback(), "\nAll enountered object:\n");
			
		Utils::Trace(GetCallback(), "->		MovieClips:		%d\n", (all_timelines.size()-this->animate_to_awd_encode->grafik_cnt));
		Utils::Trace(GetCallback(), "->		Graphics:		%d\n", this->animate_to_awd_encode->grafik_cnt);
		Utils::Trace(GetCallback(), "->		Bitmaps:		%d\n", all_bitmaps.size());
		Utils::Trace(GetCallback(), "->		BillBoards:		%d\n", all_billboards.size());
		Utils::Trace(GetCallback(), "->		Sounds:			%d\n", all_audios.size());
		Utils::Trace(GetCallback(), "->		Fonts:			%d\n", all_fonts.size());
		Utils::Trace(GetCallback(), "->		Text-fields:	%d\n", all_tf.size());
		Utils::Trace(GetCallback(), "->		Encountered Shape-instances:			%d\n", this->animate_to_awd_encode->shape_instance_cnt);
		Utils::Trace(GetCallback(), "->		Cached Shapes:			%d\n", all_shapes.size());
		Utils::Trace(GetCallback(), "->		Meshes:			%d\n", all_meshes.size());
		Utils::Trace(GetCallback(), "->		Materials:		%d\n", all_materials.size());

		Utils::Trace(GetCallback(), "\nAll Data Collected\n");
		*/
		
		// process fonts
        
        AWD::result awd_res;
        
		this->awd_project->get_blocks_by_type(all_fonts, BLOCK::block_type::FONT);
		if(all_fonts.size()>0){
			this->awd_project->get_time_since_last_call();
            if(print_export_log){
                Utils::Trace(GetCallback(), "\nStart finalize %d Fonts", all_fonts.size());}
			this->animate_to_awd_encode->FinalizeFonts(this->fla_document);
			if(print_export_log)
				Utils::Trace(GetCallback(), "\nFinalizing Fonts took '%d' ms\n", this->awd_project->get_time_since_last_call());
		}
        
		
		// process geometries
		
		this->awd_project->get_time_since_last_call();
		this->awd_project->get_blocks_by_type(all_shapes, BLOCK::block_type::TRI_GEOM);
        
        
		if(all_shapes.size()>0){
            if(print_export_log){
                Utils::Trace(GetCallback(), "\nStart processing Geometry-Data");}
			this->awd_project->get_time_since_last_call();
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
			if(print_export_log){
				Utils::Trace(GetCallback(), "\n		Succesfully converted %d Shapes into AWD-Geometry", all_shapes.size());
				Utils::Trace(GetCallback(), "\nConverting Geometry took '%d' ms\n", this->awd_project->get_time_since_last_call());
			}
		}
		
		
		// create texture-atlas, create uvs-for shapes, merge and apply materials 
		if((all_shapes.size()>0)||(all_fonts.size())){
            if(print_export_log){
                Utils::Trace(GetCallback(), "\nStart creating Textureatlas");}
			std::string filepath_no_extension2;
			if(AWD::FILES::extract_path_without_file_extension(outFile, filepath_no_extension2)!=result::AWD_SUCCESS){
				Utils::Trace(GetCallback(), "\nFAILED TO GET FILENAME FROM PATH");
				return FCM_EXPORT_FAILED;
			}
			std::string fileName2;
			if(AWD::FILES::extract_name_from_path(filepath_no_extension2, fileName2)!=result::AWD_SUCCESS){
				Utils::Trace(GetCallback(), "\nFAILED TO GET FILENAME FROM PATH");
				return FCM_EXPORT_FAILED;
			}		
			this->awd_project->get_blocks_by_type(all_materials, BLOCK::block_type::SIMPLE_MATERIAL);
			AWD::create_TextureAtlasfor_timelines_refactor(this->awd_project, fileName2);
			
			cnt_before=all_materials.size();
			this->awd_project->get_blocks_by_type(all_materials, BLOCK::block_type::SIMPLE_MATERIAL);
			int matcnt = 0;
			for(cnt_blocks=0; cnt_blocks<all_materials.size(); cnt_blocks++){
				if(all_materials[cnt_blocks]->get_state()==TYPES::state::VALID)
					matcnt++;
			}
			if(matcnt!=cnt_before){
				if(print_export_log)
					Utils::Trace(GetCallback(), "\n		Merged %d materials into %d materials", cnt_before, matcnt);
			}
            if(print_export_log){
                Utils::Trace(GetCallback(), "\nCreating Textureatlas took '%d' ms\n", this->awd_project->get_time_since_last_call());}

		}
		
		
		// export the awd-file.
		//if(print_export_log)
        Utils::Trace(GetCallback(), "\nStart writing the file to disc");
        this->awd_project->create_merged_streams();
		awd_res = this->awd_project->export_file();
		if(awd_res!=result::AWD_SUCCESS){			
			Utils::Trace(GetCallback(), "FAILED TO EXPORT %d\n", awd_res);
			return FCM_EXPORT_FAILED;
		}		
         
        if(print_export_log){
            Utils::Trace(GetCallback(), "\nWriting the file to disc took '%d' ms\n", this->awd_project->get_time_since_last_call());}
		
		
		Utils::Trace(GetCallback(), "\nThe AWD-file: '%s' should have been created.\n", outFile.c_str());
		
			
		
		if((print_export_log_timelines)&&(all_timelines.size()>0)){
			Utils::Trace(GetCallback(), "\nTimelines exported: %d\n", all_timelines.size());

			for(AWDBlock* awd_block : all_timelines){
				BLOCKS::MovieClip* timeline = reinterpret_cast<BLOCKS::MovieClip*>(awd_block);
				if(timeline==NULL){
					Utils::Trace(GetCallback(), "FAILED TO GET TIMELINE FROM BASEBLOCK\n");
					return FCM_EXPORT_FAILED;
				}               
				if(timeline->is_grafic_instance)
					Utils::Trace(GetCallback(), "Graphic clip is not exported as sepperate timeline, but merged into parent timeline\n");
				else{
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
		
		/*

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
				*/
				/*
				if(awd_block->get_name().size()==0){
					cnt_tf++;
					awd_block->set_name("tf"+std::to_string(cnt_tf));
				}
				*/
				/*
			}
		}
		*/
		
		if(print_export_log){
			// return statistics of the awd-project.
			std::vector<std::string> statistic_str;
			this->awd_project->get_statistics(statistic_str);
			for(std::string message : statistic_str)
				Utils::Trace(GetCallback(), "%s\n", message.c_str());
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
                std::string findThis="plugin/lib/mac/AwayExtensionsAnimate.fcm.plugin/Contents";
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
			preview_values.push_back(FILES::int_to_string(fps));

			preview_ids.push_back("AWD_STAGEWIDTH_AWD");
			preview_values.push_back(FILES::int_to_string(stageWidth));

			preview_ids.push_back("AWD_STAGEHEIGHT_AWD");
			preview_values.push_back(FILES::int_to_string(stageHeight));
            
            Utils::Trace(GetCallback(), "TRY OPENING PREVIEW: PATH:%s \n", preview_output_path.c_str());
			if(this->awd_project->open_preview(preview_file, preview_ids, preview_values, preview_output_path, append_file_name)!=result::AWD_SUCCESS){
				Utils::Trace(GetCallback(), "FAILED TO OPEN PREVIEW FILE \n");
				return FCM_EXPORT_FAILED;
			}

		}
		
		Utils::Trace(GetCallback(), "\nAwayJS-Document-Export Complete. ");
		delete this->awd_project;
		delete this->animate_to_awd_encode;
					
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
         *              --------------------------------
         *             | Application.Component |  ----- | -----------------------------
         *              --------------------------------                               |
         *                                                                             |
         *  Level 1:                                   <-------------------------------
         *              ------------------------------  
         *             | CLSID_Publisher_GUID | ----- | -------------------------------
         *              ------------------------------                                 |
         *                                                                             |
         *  Level 2:                                      <---------------------------- 
         *              ---------------------------------------------------
         *             | Application.Component.Category.Publisher |  ----- |-----------
         *              ---------------------------------------------------            |
         *                                                                             |
         *  Level 3:                                                           <-------
         *              -------------------------------------------------------------------------
         *             | Application.Component.Category.Name           | PUBLISHER_NAME          |
         *              -------------------------------------------------------------------------|
         *             | Application.Component.Category.UniversalName  | PUBLISHER_UNIVERSAL_NAME|
         *              -------------------------------------------------------------------------|
         *             | Application.Component.Publisher.UI            | PUBLISH_SETTINGS_UI_ID  |
         *              -------------------------------------------------------------------------|
         *             | Application.Component.Publisher.TargetDocs    |              -----------|--
         *              -------------------------------------------------------------------------| |
         *                                                                                         |
         *  Level 4:                                                    <--------------------------
         *              -----------------------------------------------
         *             | CLSID_DocType   |  Empty String               |
         *              -----------------------------------------------
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
                    (const FCM::StringRep8)kApplicationCategoryKey_Publisher, 
                    pCategory.m_Ptr);

                {
                    // Level 3 Dictionary
                    std::string str_name = PUBLISHER_NAME;
                    res = pCategory->Add(
                        (const FCM::StringRep8)kApplicationCategoryKey_Name,
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)str_name.c_str(),
                        (FCM::U_Int32)str_name.length() + 1);

                    std::string str_uniname = PUBLISHER_UNIVERSAL_NAME;
                    res = pCategory->Add(
                        (const FCM::StringRep8)kApplicationCategoryKey_UniversalName,
                        kFCMDictType_StringRep8,
                        (FCM::PVoid)str_uniname.c_str(),
                        (FCM::U_Int32)str_uniname.length() + 1);

                    std::string str_ui = PUBLISH_SETTINGS_UI_ID;
                    res = pCategory->Add(
                        (const FCM::StringRep8)kApplicationPublisherKey_UI, 
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)str_ui.c_str(),
                        (FCM::U_Int32)str_ui.length() + 1);
					
                    AutoPtr<IFCMDictionary> pDocs;
                    res = pCategory->AddLevel((const FCM::StringRep8)kApplicationPublisherKey_TargetDocs, pDocs.m_Ptr);

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
