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

#include "AWDTimelineWriter.h"
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
	FCM::Result CPublisher::ClearCache() {
        return FCM_SUCCESS;
    }
	
	AWD::result CPublisher::ExportTimeline(const PIFCMDictionary pDictPublishSettings, DOM::ITimeline* pTimeline, int scene_id)
	{
			FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
			AutoPtr<IFrameCommandGenerator> m_frameCmdGeneratorService;
			AutoPtr<IResourcePalette> m_pResourcePalette;
			AwayJS::ResourcePalette* pResPalette;
			AutoPtr<ITimelineBuilder> pTimelineBuilder;
			FCM::Result res = GetCallback()->GetService(Exporter::Service::EXPORTER_FRAME_CMD_GENERATOR_SERVICE, pUnk.m_Ptr);
			ASSERT(FCM_SUCCESS_CODE(res));
			m_frameCmdGeneratorService = pUnk;
					
			res = GetCallback()->CreateInstance(NULL, CLSID_ResourcePalette, IID_IResourcePalette, (void**)&m_pResourcePalette);
			ASSERT(FCM_SUCCESS_CODE(res));
			
			pResPalette = static_cast<ResourcePalette*>(m_pResourcePalette.m_Ptr);
			pResPalette->Init(this->animate_to_awd_encode);

			// Create a Timeline Builder Factory 
			AutoPtr<ITimelineBuilderFactory> pTimelineBuilderFactory;
			res = GetCallback()->CreateInstance(NULL, CLSID_TimelineBuilderFactory, IID_ITimelineBuilderFactory, (void**)&pTimelineBuilderFactory);
			ASSERT(FCM_SUCCESS_CODE(res));
			(static_cast<TimelineBuilderFactory*>(pTimelineBuilderFactory.m_Ptr))->Init(this->animate_to_awd_encode);
				

			ITimelineWriter* pTimelineWriter;
			Exporter::Service::RANGE range;
			range.min = 0;
			res = pTimeline->GetMaxFrameCount(range.max);
			range.max--;

			FCM::StringRep16 scene_name_str;
			pTimeline->GetName(&scene_name_str);
			std::string scene_name=AwayJS::Utils::ToString(scene_name_str, GetCallback());
		
			Utils::Trace(GetCallback(), "\n		Start exporting Timeline name: '%s'", scene_name.c_str());

			this->animate_to_awd_encode->current_scene_name=scene_name;
			// Generate frame commands
			res = m_frameCmdGeneratorService->GenerateFrameCommands(pTimeline, range, pDictPublishSettings,	m_pResourcePalette, pTimelineBuilderFactory, pTimelineBuilder.m_Ptr);
			ASSERT(FCM_SUCCESS_CODE(res));
				
			((TimelineBuilder*)pTimelineBuilder.m_Ptr)->Build(0, scene_name_str, &pTimelineWriter);
				
			BLOCKS::MovieClip* scene_timeline_block = reinterpret_cast<BLOCKS::MovieClip* >(this->awd_project->get_block_by_external_id_and_type_shared(AWD::FILES::int_to_string(0), BLOCK::block_type::MOVIECLIP, false));
			if(scene_id>=0){
				scene_timeline_block->set_scene_id(scene_id+1);
			}
			// clear the external ids from all awd-blocks
			if(this->awd_project->clear_external_ids()!=result::AWD_SUCCESS)
				Utils::Trace(GetCallback(), "PROBLEM IN CONVERTING RESSOURCE_ID TO AWDBLOCKS FOR FRAMECOMMANDS!!!\nEXPORT STILL CONTINUES !!!\n");
			
			if(scene_name_str){
				FCM::AutoPtr<FCM::IFCMCalloc> pCalloc = AwayJS::Utils::GetCallocService(GetCallback());
				pCalloc->Free((FCM::PVoid)scene_name_str);
			}
			pResPalette->Clear();
			return AWD::result::AWD_SUCCESS;
		}
		
	bool CPublisher::GetSettingsBool(const PIFCMDictionary pDictPublishSettings, std::string settingsname, bool default_value){
			std::string value_str;
			ReadString(pDictPublishSettings, (FCM::StringRep8)settingsname.c_str(), value_str);
			if(value_str=="false")
				return false;
			if(value_str=="true")
				return true;
			return default_value;
	}

	bool CPublisher::GetSettingsFloat(const PIFCMDictionary pDictPublishSettings, std::string settingsname, double default_value){
			std::string value_str;
			ReadString(pDictPublishSettings, (FCM::StringRep8)settingsname.c_str(), value_str);
			//if(!value_str.empty())
			//	return std::stod(value_str);
			return default_value;
	}

	bool CPublisher::GetSettingsInt(const PIFCMDictionary pDictPublishSettings, std::string settingsname, int default_value){
			std::string value_str;
			ReadString(pDictPublishSettings, (FCM::StringRep8)settingsname.c_str(), value_str);
			//if(!value_str.empty())
			//	return std::stoi(value_str);
			return default_value;
	}

	FCM::Result CPublisher::GetOutputFileName(DOM::PIFLADocument pFlaDocument, DOM::PITimeline pTimeline, const PIFCMDictionary pDictPublishSettings, std::string& outFile)
		{
			FCM::Result res = FCM_SUCCESS;
			FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
			FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;

			pCalloc = AwayJS::Utils::GetCallocService(GetCallback());
			ASSERT(pCalloc.m_Ptr != NULL);

			// Read the output file name from the publish settings
			ReadString(pDictPublishSettings, (FCM::StringRep8)"out_file", outFile);
			if (outFile.empty() || outFile=="")
			{
				FCM::StringRep16 path;

				res = pFlaDocument->GetPath(&path);
				ASSERT(FCM_SUCCESS_CODE(res));

				if (path)
				{
					std::string parent;
					std::string ext;
					std::string filePath = Utils::ToString(path, GetCallback());

					Utils::GetFileNameWithoutExtension(filePath, outFile);

					if (pTimeline)
					{
						FCM::StringRep16 pSceneName;
						std::string sceneName;

						res = pTimeline->GetName(&pSceneName);
						ASSERT(FCM_SUCCESS_CODE(res));

						sceneName = Utils::ToString(pSceneName, GetCallback());

						outFile += "_";
						outFile += sceneName;
						pCalloc->Free(pSceneName);
					}

					outFile += ".";
					outFile += OUTPUT_FILE_EXTENSION;

					Utils::GetFileExtension(filePath, ext);
                
					// TODO: Convert "ext" to lower case and then compare
					if (ext.compare("xfl") == 0)
					{
						std::string immParent;
						Utils::GetParent(filePath, immParent);
						Utils::GetParent(immParent, parent);
					}
					else
					{
						// FLA
						Utils::GetParent(filePath, parent);
					}

					// Extract the extension and append output file extension.
					outFile = parent + outFile;

					pCalloc->Free(path);

					res = FCM_SUCCESS;
				}
				else
				{
					res = FCM_INVALID_PARAM;
				}
			}

			return res;
		}
  
	AWD::result CPublisher::CreateAWDProject()
	{
		 AWD::result awd_result =  AWD::result::AWD_SUCCESS;

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
			return AWD::result::AWD_ERROR;
		}
		
		// create a AnimateToAWDEncoder

		this->animate_to_awd_encode = new AnimateToAWDEncoder(GetCallback(), this->awd_project);
		return awd_result;
	}

	AWD::result CPublisher::CreateAWDSettings()
	{
		 AWD::result awd_result =  AWD::result::AWD_SUCCESS;
	
		// Fill AWDProject Settings:

		this->awd_settings = this->awd_project->get_settings();
		
		// general settings
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::OpenPreview, GetSettingsBool(this->pDict, "PublishSettings.OpenPreview", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::CopyRuntime, GetSettingsBool(this->pDict, "PublishSettings.CopyRuntime", false));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::PrintExportLog, GetSettingsBool(this->pDict, "PublishSettings.PrintExportLog", false));
		//this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::PrintExportLogTimelines, GetSettingsBool(this->pDict, "PublishSettings.PrintExportLogTimelines", false));
		
		
		// timelines settings
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportTimelines, GetSettingsBool(this->pDict, "PublishSettings.ExportTimelines", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::IncludeInvisibleTimelineLayer, GetSettingsBool(this->pDict, "PublishSettings.IncludeInvisibleLayer", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportFrameScript, GetSettingsBool(this->pDict, "PublishSettings.ExportFrameScript", true));
		//this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExternalScripts, GetSettingsBool(this->pDict, "PublishSettings.ExternalScripts", true));
		
		
		// geometries settings
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportGeometries, GetSettingsBool(this->pDict, "PublishSettings.ExportGeometries", true));

		// font settings
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportFonts, GetSettingsBool(this->pDict, "PublishSettings.ExportFonts", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportLibFonts, GetSettingsBool(this->pDict, "PublishSettings.ExportLibFonts", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::EmbbedAllChars, GetSettingsBool(this->pDict, "PublishSettings.EmbbedAllChars", true));
		
		// bitmap settings
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportBitmaps, GetSettingsBool(this->pDict, "PublishSettings.ExportBitmaps", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ForceTextureOverwrite, GetSettingsBool(this->pDict, "PublishSettings.ForceTextureOverwrite", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::EmbbedTextures, GetSettingsBool(this->pDict, "PublishSettings.EmbbedTextures", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportLibBitmaps, GetSettingsBool(this->pDict, "PublishSettings.ExportLibBitmaps", true));

		// sound settings
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportSounds, GetSettingsBool(this->pDict, "PublishSettings.ExportSounds", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ForceSoundOverwrite, GetSettingsBool(this->pDict, "PublishSettings.ForceSoundOverwrite", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::EmbbedSounds, GetSettingsBool(this->pDict, "PublishSettings.EmbbedSounds", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportLibSounds, GetSettingsBool(this->pDict, "PublishSettings.ExportLibSounds", true));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportTimelineSounds, GetSettingsBool(this->pDict, "PublishSettings.ExportTimelineSounds", true));
		//this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::CreateAudioMap, GetSettingsBool(this->pDict, "PublishSettings.CreateAudioMap", false));
		this->awd_settings->set_bool(AWD::SETTINGS::bool_settings::ExportEmptyFontsForFNT, GetSettingsBool(this->pDict, "PublishSettings.ExportEmptyFontsForFNT", true));
		
		std::string sound_file_type_str;
		ReadString(this->pDict, (FCM::StringRep8)"PublishSettings.SaveSoundsAs", sound_file_type_str);
		if(sound_file_type_str=="0"){
			sound_file_type_str="keep";}
		else if(sound_file_type_str=="1"){
			sound_file_type_str="wav";}
		else if(sound_file_type_str=="2"){
			sound_file_type_str="mp3";}
		this->awd_settings->set_sound_file_extension(sound_file_type_str);
				
		/*
		std::string preview_source_path;
		ReadString(this->pDict, (FCM::StringRep8)"PublishSettings.PreviewPathSource", preview_source_path);
		std::string preview_output_path;
		ReadString(this->pDict, (FCM::StringRep8)"PublishSettings.PreviewPath", preview_output_path);
		bool append_file_name = GetSettingsBool(this->pDict, "PublishSettings.AppendFilename", false);
		*/
					

		// some settings that are no longer available in the UI, but need setting anyway until they are refactored out for good:
		this->awd_settings->set_curve_threshold(0.02); // 0.08
		this->awd_settings->set_max_iterations(50);
		this->awd_settings->set_exterior_threshold(0.0, TYPES::filled_region_type::STANDART_FILL);
		//this->awd_settings->set_distinglish_interior_exterior_triangles_2d(false);
		this->awd_settings->set_resolve_intersections(true);
		//this->awd_settings->set_keep_verticles_in_path_order(true);
		this->awd_settings->set_flipXaxis(false);
		this->awd_settings->set_flipYaxis(false);

		
		// get some properties from the AwayJS Document (color, framerate etc)		
		
		res = this->fla_document->GetBackgroundColor(this->color);
		ASSERT(FCM_SUCCESS_CODE(res));
		res = this->fla_document->GetStageHeight(this->stageHeight);
		ASSERT(FCM_SUCCESS_CODE(res));
		res = this->fla_document->GetStageWidth(this->stageWidth);
		ASSERT(FCM_SUCCESS_CODE(res));
		res = this->fla_document->GetFrameRate(this->fps);
		ASSERT(FCM_SUCCESS_CODE(res));
		awd_settings->set_fps(fps);
        
		return awd_result;
	}

	AWD::result CPublisher::CollectDocumentData()
	{	
		 AWD::result awd_result =  AWD::result::AWD_SUCCESS;
		// All Root-timelines (Scenes) of document
		FCM::FCMListPtr pTimelineList;
		res = this->fla_document->GetTimelines(pTimelineList.m_Ptr);
		ASSERT(FCM_SUCCESS_CODE(res));

		// Number of Root-timelines (Scenes) of document
		FCM::U_Int32 timelineCount;
		res = pTimelineList->Count(timelineCount);
		ASSERT(FCM_SUCCESS_CODE(res));
		
		// Check if only active timeline should be exported. if yes, we set timelinecount to 1

		if(this->timeline!=NULL){
			if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
				Utils::Trace(GetCallback(), "\nExport only the current active TimeLine !\n");
			timelineCount=1;
		}
		
		// fill the awd-project with data:

		FCM::U_Int32 i=0;
		
		int cnt_before=0;
		int grafics_cnt_start=0;
		int cnt_blocks=0;
	
		
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nStart encoding Scenes:");
		
		// if we have a input-timeline (pTimeline), we only export this (timelineCount is already 1 in this case)
		int cntAllressources=0;
		for (i = 0; i < timelineCount; i++)
		{
			AutoPtr<DOM::ITimeline> oneTimeline = pTimelineList[i];
			if(this->timeline!=NULL)
				oneTimeline = this->timeline;			
			this->ExportTimeline(this->pDict, oneTimeline, i);
		}
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nEncoding  Scenes took '%d' ms\n", this->awd_project->get_time_since_last_call());
		
		
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nStart finalizing MovieClips");
		this->awd_project->finalize_timelines();
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nFinalizing MovieClips took '%d' ms\n", this->awd_project->get_time_since_last_call());

		
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nStart exporting library (export libary items that have a script linkage set)");
		FCM::FCMListPtr pLibraryItemList;
		res = this->fla_document->GetLibraryItems(pLibraryItemList.m_Ptr);
		if (FCM_FAILURE_CODE(res)){
			Utils::Trace(GetCallback(), "\n		FAILED TO GET LIBRARY ITEMS FROM FLASH ");
			return awd_result;
		}
		this->animate_to_awd_encode->current_scene_name="Library";

		if(this->ExportLibraryItems(pLibraryItemList)!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "\n		FAILED TO COLLECT LIBRARY ITEMS \n");
			return awd_result;
		}	
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nExporting Library took '%d' ms\n", this->awd_project->get_time_since_last_call());
		
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nStart finalizing MovieClips");
		this->awd_project->finalize_timelines();
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nFinalizing MovieClips took '%d' ms\n", this->awd_project->get_time_since_last_call());

		return awd_result;
	}

	AWD::result CPublisher::ProcessData(){	
		 AWD::result awd_result =  AWD::result::AWD_SUCCESS;
        
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
	
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nStart finalizing Fonts");
		this->animate_to_awd_encode->FinalizeFonts(this->fla_document);
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nFinalizing Fonts took '%d' ms\n", this->awd_project->get_time_since_last_call());

		this->awd_project->get_blocks_by_type(all_shapes, BLOCK::block_type::TRI_GEOM);
		if(all_shapes.size()>0){
			if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
				Utils::Trace(GetCallback(), "\nStart finalizing Graphics");
			for(AWDBlock* awd_block : all_shapes){
				BLOCKS::Geometry* geom = reinterpret_cast<BLOCKS::Geometry*>(awd_block);
				if(geom==NULL){
					Utils::Trace(GetCallback(), "FAILED TO GET GEOMETRY FROM BASEBLOCK\n");
					return awd_result;
				}
				DOM::FrameElement::IShape* testshape = reinterpret_cast<DOM::FrameElement::IShape*>(geom->get_external_object());
				if(testshape!=NULL)	testshape->Release();
				// create stream settings for normal shapes
				awd_result = GEOM::ProcessShapeGeometry(geom, awd_project, this->awd_project->get_settings());
				if(awd_result!=result::AWD_SUCCESS){
					Utils::Trace(GetCallback(), "ERROR WHILE PROCESSING GEOMETRY name %s\n", awd_block->get_name().c_str());
					return awd_result;
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
			if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog)){
				Utils::Trace(GetCallback(), "\n		Succesfully converted %d Shapes into AWD-Graphics", all_shapes.size());
				Utils::Trace(GetCallback(), "\nConverting Graphics took '%d' ms\n", this->awd_project->get_time_since_last_call());
			}
		}
		
		
		// create texture-atlas, create uvs-for shapes, merge and apply materials 
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nStart creating Textureatlas");
		std::string filepath_no_extension2;
		if(AWD::FILES::extract_path_without_file_extension(outFile, filepath_no_extension2)!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "\nFAILED TO GET FILENAME FROM PATH");
			return awd_result;
		}
		std::string fileName2;
		if(AWD::FILES::extract_name_from_path(filepath_no_extension2, fileName2)!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "\nFAILED TO GET FILENAME FROM PATH");
			return awd_result;
		}		
		AWD::create_TextureAtlasfor_timelines_refactor(this->awd_project, fileName2);
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nCreating Textureatlas took '%d' ms\n", this->awd_project->get_time_since_last_call());
		
	
		
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nStart mergin vertex-streams");
		this->awd_project->create_merged_streams();
		if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::PrintExportLog))
			Utils::Trace(GetCallback(), "\nMerging vertex-streams took '%d' ms\n", this->awd_project->get_time_since_last_call());
			

		return awd_result;
	}

	AWD::result CPublisher::ExportLibraryItems(FCM::FCMListPtr pLibraryItemList) {
		FCM::U_Int32 count = 0;
		FCM::Result res;

		ASSERT(pLibraryItemList);
		res = pLibraryItemList->Count(count);
		ASSERT(FCM_SUCCESS_CODE(res));
		
		Utils::Trace(GetCallback(), "\n		Encountered %d library items", count);
		FCM::AutoPtr<FCM::IFCMCalloc> pCalloc = AwayJS::Utils::GetCallocService(GetCallback());
		ASSERT(pCalloc.m_Ptr != NULL);

		AWD::result awd_res=result::AWD_SUCCESS;
		
		for (FCM::U_Int32 index = 0; index < count ; index++)
		{
			AutoPtr<DOM::ILibraryItem> plibrary_item = pLibraryItemList[index];
		
			DOM::AutoPtr<DOM::LibraryItem::IFolderItem> pFolder_item = plibrary_item;
			if(pFolder_item){
				//Utils::Trace(GetCallback(), "\n	->	Found Folder");
				// export a folder item from library. just call this function for all its children.			
				FCM::FCMListPtr pLibraryItems;
				res = pFolder_item->GetChildren(pLibraryItems.m_Ptr);
				ASSERT(FCM_SUCCESS_CODE(res));
				awd_res=ExportLibraryItems(pLibraryItems);
				if(awd_res!=result::AWD_SUCCESS)
					return awd_res;
				continue;
			}
			
			AutoPtr<IFCMDictionary> pDict;
			FCM::FCMDictRecTypeID link_type;
			FCM::U_Int32 valLen;
			std::string script_name="";

			res = plibrary_item->GetProperties(pDict.m_Ptr);
			ASSERT(FCM_SUCCESS_CODE(res));
			res = pDict->GetInfo(kLibProp_LinkageClass_DictKey, link_type, valLen);
			if (FCM_SUCCESS_CODE(res))
			{
				FCM::StringRep8 script_name_8 = new char[valLen];
				res = pDict->Get(kLibProp_LinkageClass_DictKey, link_type, script_name_8, valLen);
				ASSERT(FCM_SUCCESS_CODE(res));
				script_name+=script_name_8;
				//delete []script_name_8;
			}
			if(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::ExportLibFonts)){
				DOM::AutoPtr<DOM::LibraryItem::IFontItem> pFontItem = plibrary_item;
				if(pFontItem){
					FCM::StringRep16 symbol_name=NULL;
					plibrary_item->GetName(&symbol_name);
					std::string symbol_name_str=AwayJS::Utils::ToString(symbol_name, GetCallback());
					//Utils::Trace(GetCallback(), "\n	->	Found Font");
					// export a font item from library
					AWDBlock* new_font = this->animate_to_awd_encode->ExportFont(pFontItem, symbol_name_str);
					if(new_font==NULL)
                		return awd_res;
					if(script_name.size()>0){
						new_font->set_name(script_name);
						new_font->add_scene_name("script-linkage");
					}
					continue;
				}
			}
			DOM::AutoPtr<DOM::LibraryItem::ISymbolItem> pSymbol_item = plibrary_item;
			if(pSymbol_item){	
				if(script_name.size()>0){
					FCM::StringRep16 symbol_name=NULL;
					plibrary_item->GetName(&symbol_name);
					std::string symbol_name_str=AwayJS::Utils::ToString(symbol_name, GetCallback());
					// export a symbol (timeline). If the timeline has already been encoded, nothing will happen. 
					//Utils::Trace(GetCallback(), "\n	->	Found Symbol %s | %s", symbol_name_str.c_str(), timeline_name_str.c_str());
					BLOCKS::MovieClip* this_timeline = this->awd_project->get_timeline_by_symbol_name(symbol_name_str);
					if(this_timeline!=NULL){
						this_timeline->add_scene_name("script-linkage");
						this_timeline->set_name(script_name);
						if(symbol_name)
							pCalloc->Free((FCM::PVoid)symbol_name);
						continue;
					}
					AutoPtr<DOM::ITimeline> timeline;
					res = pSymbol_item->GetTimeLine(timeline.m_Ptr);
					this->ExportTimeline(this->pDict, timeline, -1);
					this_timeline = this->awd_project->get_timeline_by_symbol_name(symbol_name_str);
					this_timeline->set_name(script_name);
					
					if(symbol_name)
						pCalloc->Free((FCM::PVoid)symbol_name);
				}
				continue;
				//}	
			}
			if((this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::ExportLibSounds))||(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::ExportLibBitmaps))){
				DOM::AutoPtr<DOM::LibraryItem::IMediaItem> pMedia_item = plibrary_item;
				if(pMedia_item){	
					//Utils::Trace(GetCallback(), "\n	->	Found Media");
					// export a media item. 
					FCM::PIFCMUnknown unknownMediaInfo;
					res = pMedia_item->GetMediaInfo(unknownMediaInfo); 
					ASSERT(FCM_SUCCESS_CODE(res));
					FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmapInfo = unknownMediaInfo;
					FCM::AutoPtr<DOM::MediaInfo::ISoundInfo> SoundInfo = unknownMediaInfo;
					if((bitmapInfo)&&(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::ExportLibBitmaps))){
						//AwayJS::Utils::Trace(GetCallback(), "	ENCOUNTERED LIB BITMAP 1!!");
						if(script_name.size()>0){
							AWDBlock* new_bitmap=NULL; 
							animate_to_awd_encode->ExportBitmap(pMedia_item, &new_bitmap);
							if(new_bitmap==NULL)
								continue;
							//AwayJS::Utils::Trace(GetCallback(), "	ENCOUNTERED LIB BITMAP 2!!");
							new_bitmap->set_name(script_name);
							new_bitmap->add_scene_name("script-linkage");
						}
					}
					else if((SoundInfo)&&(this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::ExportLibSounds))){
						if(script_name.size()>0){
							AWDBlock* new_sound = animate_to_awd_encode->ExportSound(pMedia_item, NULL, "");
							if(new_sound==NULL)
								continue;
							new_sound->set_name(script_name);
							new_sound->add_scene_name("script-linkage");
						}
					}
					continue;
				}
			}
		}
		return awd_res;
	}

	AWD::result CPublisher::OpenPreview(){
	
		if(!this->awd_settings->get_bool(AWD::SETTINGS::bool_settings::OpenPreview))
			return AWD::result::AWD_SUCCESS;
		
		AWD::result awd_result =  AWD::result::AWD_SUCCESS;
		//if(preview_source_path.empty()){
	
		std::string preview_file_url;
		std::string plugin_directory;
		Utils::GetModuleFilePath(plugin_directory, this->GetCallback());
        
	#ifdef _WINDOWS
		std::string immParent;
		Utils::GetParent(plugin_directory, immParent);
		Utils::GetParent(immParent, plugin_directory);
		Utils::GetParent(plugin_directory, immParent);
		Utils::GetParent(immParent, plugin_directory);
		Utils::GetParent(plugin_directory, immParent);
		Utils::GetParent(immParent, plugin_directory);
		preview_file_url = plugin_directory+"ressources/preview_awayjs/index.html";
	#endif

	#ifdef __APPLE__
		std::string findThis="plugin/lib/mac/AwayExtensionsAnimate.fcm.plugin/Contents";
		int foundIndex=plugin_directory.find(findThis);
		if(foundIndex>0){
			plugin_directory = plugin_directory.substr(0, foundIndex);
		}
		preview_file_url = plugin_directory+"ressources/preview_awayjs/index.html";
	#endif

		//else{
		//	preview_file_url=preview_source_path;
		//}
			 
		std::string fileName;
		awd_result = AWD::FILES::extract_name_from_path(this->outFile, fileName);
		if(awd_result!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "FAILED TO GET FILENAME FROM PATH \n");
			return awd_result;
		}
		// lists that contain the id and the values thta should be used to replace options in preview-file
		std::vector<std::string> preview_ids;
		std::vector<std::string> preview_values;
		
		preview_ids.push_back("AWD_FILENAME_AWD");
		preview_values.push_back(fileName);

		preview_ids.push_back("AWD_BGCOLOR_AWD");
		preview_values.push_back(AwayJS::Utils::ToString(this->color));

		preview_ids.push_back("AWD_FPS_AWD");
		preview_values.push_back(FILES::int_to_string(this->fps));

		preview_ids.push_back("AWD_STAGEWIDTH_AWD");
		preview_values.push_back(FILES::int_to_string(this->stageWidth));

		preview_ids.push_back("AWD_STAGEHEIGHT_AWD");
		preview_values.push_back(FILES::int_to_string(this->stageHeight));
        
        std::string preview_path = "";
		awd_result = this->awd_project->open_preview(preview_file_url, preview_ids, preview_values, preview_path, false);
		if(awd_result!=result::AWD_SUCCESS){
			Utils::Trace(GetCallback(), "FAILED TO OPEN PREVIEW FILE \n");
			return awd_result;
		}
		return awd_result;
	}

	AWD::result CPublisher::PrintStatistics(){
		 AWD::result awd_result =  AWD::result::AWD_SUCCESS;
			
		
		//if((print_export_log_timelines)&&(all_timelines.size()>0)){
		/*
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
		*/
		//}
		
		/*

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
		
		//if(print_export_log){
		// return statistics of the awd-project.
		/*
		std::vector<std::string> statistic_str;
		this->awd_project->get_statistics(statistic_str);
		for(std::string message : statistic_str)
			Utils::Trace(GetCallback(), "%s\n", message.c_str());
			*/
		//}
		return awd_result;
	}

	bool CPublisher::ReadString(const FCM::PIFCMDictionary pDict, FCM::StringRep8 key, std::string &retString)
		{
			FCM::U_Int32 valueLen;
			FCM::FCMDictRecTypeID type;

			FCM::Result res = pDict->GetInfo(key, type, valueLen);
			if (FCM_FAILURE_CODE(res))
			{
				return false;
			}

			FCM::StringRep8 strValue = new char[valueLen];
			res = pDict->Get(key, type, (FCM::PVoid)strValue, valueLen);
			if (FCM_FAILURE_CODE(res))
			{
				delete [] strValue;
				return false;
			}

			retString = strValue;

			delete [] strValue;
			return true;
		}

	FCM::Boolean CPublisher::IsPreviewNeeded(const PIFCMDictionary pDictConfig) {
			FCM::Boolean found;
			std::string previewNeeded;

			found = ReadString(pDictConfig, (FCM::StringRep8)kPublishSettingsKey_PreviewNeeded, previewNeeded);
			ASSERT(found);

			if (previewNeeded == "true")
			{
				return true;
			}
			return false;
		}

};
