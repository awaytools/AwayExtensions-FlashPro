#ifndef TIMELINEENCODER_H_
#define TIMELINEENCODER_H_
/*
#include "Utils.h"

#ifdef _WINDOWS
#include <Windows.h>
#include "ShellApi.h"
#endif
#include <vector>
#include <fstream>
#include "Utils.h"
#include "Publisher.h"

#include "FlashFCMPublicIDs.h"
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

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"
#include "FCMPluginInterface.h"
*/

#include "Utils.h"

#include <vector>
#include <fstream>

#include "FlashToAWDEncoder.h"
#include "awd_project.h"
#include "ILayer.h"
#include "FCMPluginInterface.h"
#include "Utils/DOMTypes.h"

using namespace AWD;
struct FrameCommandGroup
		{
			std::vector<ANIM::FrameCommandDisplayObject*> pre_frame;
			std::vector<ANIM::FrameCommandDisplayObject*> cur_frame;

			FrameCommandGroup()
			{
			}
		};
struct LayerData
		{
			std::vector<ANIM::FrameCommandDisplayObject*> display_commands;
			std::vector<ANIM::FrameCommandSoundObject*> audio_commands;
			std::string frame_script;
			std::string frame_label;
			bool isDirty;

			LayerData()
			{
				isDirty=false;
			}
		};
class TimelineEncoder 
{
    private:
		BLOCKS::Timeline* awd_timeline;
		DOM::ITimeline* timeline;
		AWDProject* awd;
		int curDepth;
		int maskDepth;
		int clipMask;
		int mask_group;
		int layerIdx;
		ANIM::TimelineFrame* curFrame;
		FlashToAWDEncoder* flash_to_awd;
		std::vector<LayerData*> cur_layer_data;
		std::vector<int> layer_mask_skeleton;
		std::vector<int> layer_object_ids;
		std::vector<std::vector<LayerData*>> all_timeline_data;
		std::vector<int> all_durations;
		bool is_dirty;



		FCM::PIFCMCallback m_pCallback;

    public:
        TimelineEncoder(FCM::PIFCMCallback pCallback,  AWDProject* awd,FlashToAWDEncoder* flash_to_awd);
        ~TimelineEncoder();
		
		BLOCKS::Timeline* get_awd_timeLine();
		AWD::result encode(DOM::ITimeline* timeline, BLOCKS::Timeline*);
		int layer_obj_id_cnt;
		void get_layer_skeleton(DOM::ILayer* iLayer, int mask_layer_id);
		AWD::result collectFrameCommands(int frameIdx, DOM::ILayer* iLayer);
		AWD::result collectFrameDisplayElement(DOM::FrameElement::IFrameDisplayElement* frameDisplayElement, DOM::Utils::MATRIX2D groupMatrix, bool group_member, int);

};

#endif