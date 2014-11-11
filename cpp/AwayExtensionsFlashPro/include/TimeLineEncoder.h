#ifndef TIMELINEENCODER_H_
#define TIMELINEENCODER_H_

#include "awd.h"
#include "Utils.h"

#ifdef _WINDOWS
#include <Windows.h>
#include "ShellApi.h"
#endif
#include <vector>
#include <fstream>
#include "Publisher.h"
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

#include "OutputWriter.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"
#include "ShapePool.h"

class TimeLineEncoder 
{
    private:
		AWDShape2DTimeline* awd_timeline;
		DOM::ITimeline* timeline;
		AWD* awd;
		int curDepth;
		int maskDepth;
		int clipMask;
		int layerIdx;
		AWDTimeLineFrame* curFrame;
		ShapePool* shapePool;
		std::vector<AutoPtr<DOM::FrameElement::IShape> > shapes;
		std::vector<TimeLineEncoder*> childTimeLines;
		std::vector<AWDBlock*> lastFrame;
		FCM::PIFCMCallback m_pCallback;

    public:
        TimeLineEncoder(FCM::PIFCMCallback pCallback, DOM::ITimeline* timeline, AWD* awd,ShapePool* shapePool, int sceneID);
        ~TimeLineEncoder();
		
		awd_float64 *get_mtx_2x3(const DOM::Utils::MATRIX2D* mtx);
		awd_float64 *get_color_mtx_4x5(const DOM::Utils::COLOR_MATRIX* mtx);
		AWDShape2DTimeline* get_awd_timeLine();
		void encode();
		void collectFrameCommands(int frameIdx, DOM::ILayer* iLayer, bool isMasked);
		void collectFrameScripts(int frameIdx, DOM::ILayer* iLayer);
		void collectFrameDisplayElement(DOM::FrameElement::IFrameDisplayElement* frameDisplayElement, DOM::Utils::MATRIX2D groupMatrix, int layerCnt, int isMask);

		
};
#endif