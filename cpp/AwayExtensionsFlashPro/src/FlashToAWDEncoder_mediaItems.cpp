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

#include "MediaInfo/IBitmapInfo.h"
#include "FrameElement/IBitmapInstance.h"
#include "FrameElement/ISound.h"
#include "MediaInfo/ISoundInfo.h"
#include "LibraryItem/IMediaItem.h"
#include "LibraryItem/IFolderItem.h"
#include "LibraryItem/IFontItem.h"
#include "LibraryItem/ISymbolItem.h"
#include "ILibraryItem.h"

#include "Service/Image/IBitmapExportService.h"
#include "Service/Sound/ISoundExportService.h"

#include "FlashToAWDEncoder.h"
#include "FCMPluginInterface.h"

#include "utils/util.h"
#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG


FCM::Result 
FlashToAWDEncoder::ExportBitmap(DOM::LibraryItem::IMediaItem* media_item,  BASE::AWDBlock** output_block, const std::string res_id)
{
	FCM::Result fcm_result;
	
    DOM::AutoPtr<DOM::ILibraryItem> pLibItem = media_item;

	FCM::StringRep16 libName;
	fcm_result=pLibItem->GetName(&libName);
    ASSERT(FCM_SUCCESS_CODE(fcm_result));
	
	FCM::PIFCMUnknown unknownMediaInfo;
	media_item->GetMediaInfo(unknownMediaInfo); 

	FCM::AutoPtr<DOM::MediaInfo::IBitmapInfo> bitmapInfo = unknownMediaInfo;

	if(bitmapInfo){

		FCM::S_Int32 bmWidth;
		bitmapInfo->GetWidth(bmWidth);
		FCM::S_Int32 bmheight;
		bitmapInfo->GetHeight(bmheight);

		//AwayJS::Utils::Trace(this->m_pCallback, "					BITMAP %d , %d\n", bmWidth, bmheight);	
		//AwayJS::Utils::Trace(this->m_pCallback, "					BITMAP %s , %d\n", AwayJS::Utils::ToString(libName, m_pCallback).c_str());		
		std::string bitmapPath;
		// Form the image path
		std::string bitmaptmpName = AwayJS::Utils::ToString(libName, m_pCallback);
		std::string bitmapName;
		FILES::extract_name_from_path(bitmaptmpName, bitmapName);
		std::size_t pos=1;
		std::vector<const std::string> forbitten_chars;
		forbitten_chars.push_back(">");
		forbitten_chars.push_back("<");
		forbitten_chars.push_back("!");
		forbitten_chars.push_back("#");
		forbitten_chars.push_back("+");
		for (int i=0;i<bitmapName.length();i++){
			for(const std::string forbitten_char:forbitten_chars){
				if(bitmapName[i]==forbitten_char[0]){
					bitmapName.replace(i, i+1, "x");
					break;
				}
			}
		}

		// check if this points to a file.
		pos = bitmapName.rfind(".");

		
		// if it doesnt point to a file, make sure that png extension is added.
		if (pos == std::string::npos)
			bitmapName = bitmapName + ".png";
		else{            
			// todo: we might want to look at the transparentzy of the file, to decide if it is transparent or not. 
			// in a perfect world, this would be the job of the libawd.

			if ((bitmapName.substr(pos + 1) != "jpg") || (bitmapName.substr(pos + 1) != "png"))	{
				std::string bitmapNameWithoutExt;
				bitmapNameWithoutExt = bitmapName.substr(0, pos);
				bitmapName = bitmapNameWithoutExt + ".png";
			}
		}
		FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
		std::string bitmapRelPath;
		std::string bitmapExportPath = this->awd_project->get_settings()->get_texture_directory();
		BLOCKS::BitmapTexture* bitmap_tex =  reinterpret_cast<BLOCKS::BitmapTexture*>(this->awd_project->get_block_by_name_and_type(bitmapName, BLOCK::block_type::BITMAP_TEXTURE, false));
		if(bitmap_tex!=NULL){
			bitmap_tex->set_external_id(res_id);
			return FCM_SUCCESS;
		}

		// add the validated filename to the export-path.
		bitmapExportPath += bitmapName;       

		// also create the relative-path that should be saved into AWD (in case it is set to external-data)
		bitmapRelPath = "./" + this->awd_project->get_settings()->get_texture_directory_name() + bitmapName;

		// export the image using the IBitmapExportService

		FCM::Result res = m_pCallback->GetService(DOM::FLA_BITMAP_SERVICE, pUnk.m_Ptr);
		ASSERT(FCM_SUCCESS_CODE(res));

		FCM::AutoPtr<DOM::Service::Image::IBitmapExportService> bitmapExportService = pUnk;
		if (bitmapExportService)
		{
			res = bitmapExportService->ExportToFile(media_item, AwayJS::Utils::ToString16(bitmapExportPath, m_pCallback), 100);
			ASSERT(FCM_SUCCESS_CODE(res));
		}
		bitmap_tex =  reinterpret_cast<BLOCKS::BitmapTexture*>(this->awd_project->get_block_by_name_and_type(bitmapName, BLOCK::block_type::BITMAP_TEXTURE, true));
		if(bitmap_tex==NULL)
			return FCM_GENERAL_ERROR_BASE;
		bitmap_tex->set_external_id(res_id);
		bitmap_tex->set_width(bmWidth);				
		bitmap_tex->set_height(bmheight);		
		bitmap_tex->set_url(bitmapRelPath);
		bitmap_tex->set_input_url(bitmapExportPath);
		if(output_block)
			*output_block = bitmap_tex;
		
	}
	
	return FCM_SUCCESS;
}

FCM::Result 
FlashToAWDEncoder::ExportSound(DOM::LibraryItem::IMediaItem* media_item,  BASE::AWDBlock** output_block, const std::string res_id)
{
	FCM::Result fcm_result;	
    DOM::AutoPtr<DOM::ILibraryItem> pLibItem = media_item;

	FCM::StringRep16 libName;
	fcm_result=pLibItem->GetName(&libName);
    ASSERT(FCM_SUCCESS_CODE(fcm_result));
	
	FCM::PIFCMUnknown unknownMediaInfo;
	media_item->GetMediaInfo(unknownMediaInfo); 

	FCM::AutoPtr<DOM::MediaInfo::ISoundInfo> pSoundInfo = unknownMediaInfo;
	ASSERT(pSoundInfo);      

    FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
    std::string soundpath=AwayJS::Utils::ToString(libName,m_pCallback);	
    std::string soundName;
	FILES::extract_name_from_path(soundpath, soundName);
	std::size_t pos=1;
	std::vector<const std::string> forbitten_chars;
	forbitten_chars.push_back(">");
	forbitten_chars.push_back("<");
	forbitten_chars.push_back("!");
	forbitten_chars.push_back("#");
	forbitten_chars.push_back("+");
	for (int i=0;i<soundName.length();i++){
		for(const std::string forbitten_char:forbitten_chars){
			if(soundName[i]==forbitten_char[0]){
				soundName.replace(i, 0, "a");
			}
		}
	}
    // DOM APIs do not provide a way to get the compression of the sound.
    // For time being, we will use the extension of the library item name.
    pos = soundName.rfind(".");
    if (pos != std::string::npos)
    {
        if ((soundName.substr(pos + 1) != "WAV")&&(soundName.substr(pos + 1) != "wav")&&(soundName.substr(pos + 1) != "mp3")&&(soundName.substr(pos + 1) != "MP3"))
        {
			soundName = soundName.substr(0, pos) + ".wav";
        }
    }
        
	BLOCKS::Audio* audio_block = reinterpret_cast<BLOCKS::Audio*>(this->awd_project->get_block_by_name_and_type(soundName, BLOCK::block_type::SOUND_SOURCE, false));
	if(audio_block!=NULL){
		audio_block->set_external_id(res_id);
	}
	std::string soundExportPath = this->awd_project->get_settings()->get_audio_directory() + soundName;
	std::string soundRelPath = "./" + this->awd_project->get_settings()->get_audio_directory_name() + soundName;

    fcm_result = m_pCallback->GetService(DOM::FLA_SOUND_SERVICE, pUnk.m_Ptr);
    ASSERT(FCM_SUCCESS_CODE(fcm_result));
    FCM::AutoPtr<DOM::Service::Sound::ISoundExportService> soundExportService = pUnk;
    if (soundExportService)
    {
        FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;
        FCM::StringRep16 pFilePath = AwayJS::Utils::ToString16(soundExportPath, m_pCallback);
        fcm_result = soundExportService->ExportToFile(media_item, pFilePath);
        ASSERT(FCM_SUCCESS_CODE(fcm_result));
		pCalloc = AwayJS::Utils::GetCallocService(m_pCallback);
        ASSERT(pCalloc.m_Ptr != NULL);
        pCalloc->Free(pFilePath);
		//Utils::Trace(m_pCallback, "The Sound-file: '%s' should have been created in this folder.\n", soundExportPath.c_str());
		audio_block = reinterpret_cast<BLOCKS::Audio*>(this->awd_project->get_block_by_name_and_type(soundRelPath, BLOCK::block_type::SOUND_SOURCE, true));
		if(audio_block==NULL)
			return FCM_GENERAL_ERROR_BASE;	
		audio_block->set_external_id(res_id);
		audio_block->set_url(soundRelPath);
		audio_block->set_input_url(soundExportPath);
		if(output_block)
			*output_block = audio_block;
    }

        
	/*
    // Free the name
	FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
    fcm_result = m_pCallback->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
    FCM::AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;
    callocService->Free((FCM::PVoid)pName);
	
	*/
	
	return FCM_SUCCESS;
}
