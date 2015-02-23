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

#include "TimelineEncoder.h"

#include "AWDTimelineWriter.h"
#include "RessourcePalette.h"
#include "TimelineBuilder.h"

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

		
        std::string outFile;
        FCM::Result res;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk2;
        FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;

		
        pCalloc = AwayJS::Utils::GetCallocService(GetCallback());
        ASSERT(pCalloc.m_Ptr != NULL);
		
        Utils::Trace(GetCallback(), "Publishing begins for AwayJS-document (AWD export)\n");

			
        res = GetOutputFileName(pFlaDocument, pTimeline, pDictPublishSettings, outFile);
        if (FCM_FAILURE_CODE(res))
        {
            // FLA is untitled. Ideally, we should use a temporary location for output generation.
            // However, for now, we report an error.
            Utils::Trace(GetCallback(), "Failed to publish. Either save the AwayJS-Document or provide a output path in publish settings.\n");
            return res;
        }	
		
	// create minimum information needed for AWDProject (output-path and generator-name and version-nr)
		
        const std::string	generator_name		= "Adobe Flash Professional";
		FCM::U_Int32 app_version=0;
		AwayJS::Utils::GetAppVersion(GetCallback(), app_version );
		const std::string	generator_version = std::to_string(app_version);
		
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
		bool useADobeFramegenerator=false;
		std::string useADobeFramegenerator_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.UseAdobeFrameCommands", useADobeFramegenerator_str);
		if(useADobeFramegenerator_str=="true")
			useADobeFramegenerator=true;

		// defines if the adobe-frame-generator should be used (true) or not (false) 
		bool distinglishexteriorandinterior_tris=false;
		std::string distinglishexteriorandinterior_tris_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.DistinglishExteriorAndinterior", distinglishexteriorandinterior_tris_str);
		if(distinglishexteriorandinterior_tris_str=="true")
			distinglishexteriorandinterior_tris=true;

		// defines if curve-intersections should be resolved when generating the geometry-data
		bool resolveIntersects=false;
		std::string resolveIntersects_s;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.ResolvCurveIntersections", resolveIntersects_s);
		if(resolveIntersects_s=="true")
			resolveIntersects=true;

		std::string preview_source_path;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.PreviewPathSource", preview_source_path);

		std::string preview_output_path;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.PreviewPath", preview_output_path);
		if(preview_output_path.size()>0)
			preview_output_path+="/index.html";
		
		// defines if the preview should be opened (js) 
		bool append_file_name=false;
		std::string append_file_name_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.AppendFilename", append_file_name_str);
		if(append_file_name_str=="true")
			append_file_name=true;

			
		// get the settings for resolve_intersections, or set them to default, if resolve_intersections is false
		double curveThreshold=0.02;
		int maxIterations=0;
		if(resolveIntersects){
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
		}
		
		// defines the max threshold of "curviness" before subdividing of curves should be stopped
		double exterior_threshold=0.0;
		std::string exterior_threshold_s;
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.MinimizeExteriorTreshold", exterior_threshold_s);
		if(!exterior_threshold_s.empty())
			exterior_threshold=std::stod(exterior_threshold_s);

		
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

		bool keep_vert_in_path_order=false;
		std::string keep_vert_in_path_order_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.KeepPathOrder", keep_vert_in_path_order_str);
		if(keep_vert_in_path_order_str=="true")
			keep_vert_in_path_order=true;
		
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

		bool flipXaxis=false;
		std::string flipXaxis_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.FlipXaxis", flipXaxis_str);
		if(flipXaxis_str=="true")
			flipXaxis=true;
		bool flipYaxis=false;
		std::string flipYaxis_str;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.FlipYaxis", flipYaxis_str);
		if(flipYaxis_str=="true")
			flipYaxis=true;
		
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
		
		awd_settings->set_fps(fps);
		awd_settings->set_keep_verticles_in_path_order(keep_vert_in_path_order);
		awd_settings->set_max_iterations(maxIterations);
		awd_settings->set_curve_threshold(curveThreshold);
		awd_settings->set_resolve_intersections(resolveIntersects);
		awd_settings->set_flipXaxis(flipXaxis);
		awd_settings->set_flipYaxis(flipYaxis);
		
		awd_settings->set_export_shapes_in_debug_mode(export_shapes_in_debug_mode);
		awd_settings->set_export_curves(true);
		awd_settings->set_distinglish_interior_exterior_triangles_2d(distinglishexteriorandinterior_tris);
		awd_settings->set_exterior_threshold(exterior_threshold, TYPES::filled_region_type::STANDART_FILL);
		if(exterior_threshold_strokes)
			awd_settings->set_exterior_threshold(exterior_threshold, TYPES::filled_region_type::FILL_CONVERTED_FROM_STROKE);
		if(exterior_threshold_font)
			awd_settings->set_exterior_threshold(exterior_threshold, TYPES::filled_region_type::GENERATED_FONT_OUTLINES);
		
	// Check if only active timeline should be exported. if yes, we set timelinecount to 1

		if(pTimeline!=NULL){
			Utils::Trace(GetCallback(), "\nExport only the current active TimeLine !\n");
			timelineCount=1;
		}
		
		
	// fill the awd-project with data:

		// Here the code splits into using Adobe-Frame-Generator or own implementation:

        Utils::Trace(GetCallback(), "Encoding data....\n");
		
		double      thisTime=0;
		long        startTime=0;
    
        //startTime=(double)AWD::GetTimeMs64();

		if(useADobeFramegenerator){
			Utils::Trace(GetCallback(), "Encoding Using Adobe-Frame-Generator.\n");
			
			AutoPtr<IFrameCommandGenerator> m_frameCmdGeneratorService;
					
			res = GetCallback()->GetService(Exporter::Service::EXPORTER_FRAME_CMD_GENERATOR_SERVICE, pUnk.m_Ptr);
			ASSERT(FCM_SUCCESS_CODE(res));
			m_frameCmdGeneratorService = pUnk;
						
			AutoPtr<IResourcePalette> m_pResourcePalette;
			res = GetCallback()->CreateInstance(NULL, CLSID_ResourcePalette, IID_IResourcePalette, (void**)&m_pResourcePalette);
			ASSERT(FCM_SUCCESS_CODE(res));
			
			ResourcePalette* pResPalette = static_cast<ResourcePalette*>(m_pResourcePalette.m_Ptr);
			pResPalette->Clear();
			pResPalette->Init(this->flash_to_awd_encoder);	
			
			// Create a Timeline Builder Factory for the root timelines of the document
			AutoPtr<ITimelineBuilderFactory> pTimelineBuilderFactory;
			res = GetCallback()->CreateInstance(NULL, CLSID_TimelineBuilderFactory, IID_ITimelineBuilderFactory, (void**)&pTimelineBuilderFactory);
			ASSERT(FCM_SUCCESS_CODE(res));
			(static_cast<TimelineBuilderFactory*>(pTimelineBuilderFactory.m_Ptr))->Init(this->flash_to_awd_encoder);

			// encode the timelines
			// if we have a input-timeline (pTimeline), we only export this (timelineCount is already 1 in this case)
			for (FCM::U_Int32 i = 0; i < timelineCount; i++)
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

				// Generate frame commands		
				res = m_frameCmdGeneratorService->GenerateFrameCommands(timeline, range, pDictPublishSettings,	m_pResourcePalette, pTimelineBuilderFactory, pTimelineBuilder.m_Ptr);
				ASSERT(FCM_SUCCESS_CODE(res));
				((TimelineBuilder*)pTimelineBuilder.m_Ptr)->Build(0, NULL, &pTimelineWriter);

				BLOCKS::Timeline* scene_timeline_block = reinterpret_cast<BLOCKS::Timeline* >(this->awd_project->get_block_by_external_id_and_type(std::to_string(0), BLOCK::block_type::TIMELINE, false));
				if(scene_timeline_block!=NULL){
					scene_timeline_block->set_scene_id(i+1);
					scene_timeline_block->set_name("Scene " + std::to_string(i+1));
				}
				if(this->awd_project->get_blocks_for_external_ids()!=result::AWD_SUCCESS)
					Utils::Trace(GetCallback(), "PROBLEM IN CONVERTING RESSOURCE_ID TO AWDBLOCKS FOR FRAMECOMMANDS!!!\nEXPORT STILL CONTINUES !!!\n");

				pResPalette->Clear();
			}
			
			
		}
		else{
			Utils::Trace(GetCallback(), "Encoding Using AwayJS-Frame-Generator.\n");				
		
			// encode the timelines
			// if we have a input-timeline (pTimeline), we only export this (timelineCount is already 1 in this case)
			for (FCM::U_Int32 i = 0; i < timelineCount; i++){						
				AutoPtr<DOM::ITimeline> timeline = pTimelineList[i];
				if(pTimeline!=NULL)
					timeline = pTimeline;
				TimelineEncoder* newTimeLineEncoder = new TimelineEncoder(GetCallback(), timeline, this->awd_project, this->flash_to_awd_encoder, i);
				newTimeLineEncoder->encode();	
			}
			
		}
		
		// now all timelines should have been filled with data (Timelineframes)
		// for all encountered frame-objects (display-objects / sounds) a AWDBlock should have been created.

		// we might want to include assets that are not part of any timeline, and hence not added to the awd_project yet.
		/*
        thisTime=(double)AWD::GetTimeMs64();
		Utils::Trace(GetCallback(), "Creating Frame-Commands took %f ms\n", (thisTime-startTime));
		startTime=thisTime;
		*/
		Utils::Trace(GetCallback(), "All timeline Data Collected\n");
		
		if((export_lib_bitmaps)||(export_lib_sounds)){
			if(this->ExportLibraryItems(export_lib_bitmaps, export_lib_sounds)!=result::AWD_SUCCESS){
				Utils::Trace(GetCallback(), "FAILED TO COLLECT LIBRARY ITEMS \n");
				return FCM_EXPORT_FAILED;
			}
		}

		

		// now we tell the AWDProject, that it should process everything.
		// at this step, Geometrys are getting processed, and FrameCommands are getting finalized.
		//
		
		/*
		BLOCKS::ObjectContainer3D* new_container = new BLOCKS::ObjectContainer3D("test");
		BLOCKS::ObjectContainer3D* new_container3 = new BLOCKS::ObjectContainer3D("test3");
		BLOCKS::ObjectContainer3D* new_container5 = new BLOCKS::ObjectContainer3D("test5");
		new_container->add_child(new_container3);
		new_container->add_child(new_container5);

		if(this->awd_project->add_block(new_container)!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "Problem when adding Block to file : %s\n", new_container->get_name().c_str());
			return FCM_EXPORT_FAILED;
		}
		*/
			/*	
		std::vector<std::string> statistic_str2;
		this->awd_project->get_statistics(statistic_str2);
		for(std::string message : statistic_str2)
			Utils::Trace(GetCallback(), "%s\n", message.c_str());
		*/
		Utils::Trace(GetCallback(), "All Data Collected\n");

		
		AWD::result awd_res;
		
		std::vector<BASE::AWDBlock*> allgeos;		
		awd_res = this->awd_project->get_blocks_by_type(allgeos, BLOCK::block_type::TRI_GEOM);
		if(awd_res==result::NO_BLOCKS_FOUND)
			Utils::Trace(GetCallback(), "No Geometries to process (no valide shapes converted)\n");
		else{
			std::string geom_count = std::to_string(allgeos.size());
			Utils::Trace(GetCallback(), "GEOMETRIES TO PROCESS: %s\n", geom_count.c_str());

			for(AWDBlock* awd_block : allgeos){
				BLOCKS::Geometry* geom = reinterpret_cast<BLOCKS::Geometry*>(awd_block);
				if(geom==NULL){
					Utils::Trace(GetCallback(), "FAILED TO GET GEOMETRY FROM BASEBLOCK\n");
					return FCM_EXPORT_FAILED;
				}
                
                //startTime=(double)AWD::GetTimeMs64();
                
				awd_res = GEOM::ProcessShapeGeometry(geom, awd_project);
				if(awd_res!=result::AWD_SUCCESS){
					Utils::Trace(GetCallback(), "ERROR WHILE PROCESSING GEOMETRY name %s\n", awd_block->get_name().c_str());
					return FCM_EXPORT_FAILED;
				}
                /*
                thisTime=(double)AWD::GetTimeMs64();
                Utils::Trace(GetCallback(), "Processing Geometry '%s' took %f ms\n", (thisTime-startTime));
                startTime=thisTime;
                */
				std::vector<std::string> messages;
				geom->get_messages(messages, " -> ");
				Utils::Trace(GetCallback(), "Geometry process report: name %s\n", geom->get_name().c_str());
				for(std::string one_message : messages){
					if(one_message.size()<1024)
						Utils::Trace(GetCallback(), "%s\n", one_message.c_str());
				}
				/*
				std::string warnings;
				geom->get_warnings(warnings, "   ->   ");
				if(warnings.size()>0){
					Utils::Trace(GetCallback(), "Geometry warnings: %s = %s\n", awd_block->get_name().c_str(), warnings.c_str());
				}
				*/

			}
		}
		// export the awd-file.
		
		AWD::create_TextureAtlasfor_timelines(this->awd_project, "test");

		awd_res = this->awd_project->export_file();
		if(awd_res!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "FAILED TO EXPORT %d\n", awd_res);
			return FCM_EXPORT_FAILED;
		}
		
		Utils::Trace(GetCallback(), "The AWD-file: '%s' should have been created.\n", outFile.c_str());
			
		
		std::vector<BASE::AWDBlock*> all_timelines;		
		awd_res = this->awd_project->get_blocks_by_type(all_timelines, BLOCK::block_type::TIMELINE);
		if(awd_res==result::NO_BLOCKS_FOUND)
			Utils::Trace(GetCallback(), "No Timelines was exported\n");
		else{
			std::string tlcount = std::to_string(all_timelines.size());
			Utils::Trace(GetCallback(), "Timelines exported: %s\n", tlcount.c_str());

			for(AWDBlock* awd_block : all_timelines){
				BLOCKS::Timeline* timeline = reinterpret_cast<BLOCKS::Timeline*>(awd_block);
				if(timeline==NULL){
					Utils::Trace(GetCallback(), "FAILED TO GET TIMELINE FROM BASEBLOCK\n");
					return FCM_EXPORT_FAILED;
				}
                
                //startTime=(double)AWD::GetTimeMs64();
                std::vector<std::string> messages;
				messages.push_back("\ntimeline: '"+timeline->get_name()+"' framecount: "+std::to_string(timeline->get_frames().size()));
				timeline->get_frames_info(messages);
				for(std::string one_message : messages){
					Utils::Trace(GetCallback(), "	-> %s\n", one_message.c_str());
				}
				/*
				std::string warnings;
				geom->get_warnings(warnings, "   ->   ");
				if(warnings.size()>0){
					Utils::Trace(GetCallback(), "Geometry warnings: %s = %s\n", awd_block->get_name().c_str(), warnings.c_str());
				}
				*/

			}
		}


		// return statistics of the awd-project.
		std::vector<std::string> statistic_str;
		this->awd_project->get_statistics(statistic_str);
		for(std::string message : statistic_str)
			Utils::Trace(GetCallback(), "%s\n", message.c_str());
		
		// copy the Preview files to output directory
		 
		if(openPreview){
			
			std::string preview_file;
			if(preview_source_path.empty()){
				std::string plugin_directory;
				Utils::GetModuleFilePath(plugin_directory, GetCallback());
        
				std::string findThisjs="AwayExtensionsFlashPro.fcm.plugin/Contents/MacOS/";
				int foundIndexjs=plugin_directory.find(findThisjs);
				if(foundIndexjs>0)
					plugin_directory = plugin_directory.substr(0, foundIndexjs);
		

				// a file to modify, and copy to output-directory with file-name for preview

				preview_file = plugin_directory+"/../../../ressources/preview_awayjs/index.html";			
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
				Utils::Trace(GetCallback(), "FAILED TO OPEN REVIEW FILE \n");
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
