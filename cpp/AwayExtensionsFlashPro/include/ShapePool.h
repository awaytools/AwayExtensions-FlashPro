#ifndef SHAPEPOOL_H_
#define SHAPEPOOL_H_

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

class ShapePool 
{
    private:
		std::vector<AutoPtr<DOM::FrameElement::IShape> > shapes_flash;
		std::vector<AWDShape2D*> shapes_awd;
		std::vector<int> last_frame_shapes;
		std::vector<int> this_frame_shapes;
		int shapeCnt;
		ShapeEncoder* shapeEncoder;
		AWD* awd;
		FCM::PIFCMCallback m_pCallback;
		AutoPtr<DOM::Service::Shape::IRegionGeneratorService> pIRegionGeneratorService;
		AutoPtr<DOM::Service::Shape::IShapeService >  pIShapeService;
    public:
        ShapePool(FCM::PIFCMCallback pCallback, AWD* awd);
        ~ShapePool();
		 
		AWDShape2D* getShape(DOM::FrameElement::IShape* thisShape);
		void encodeShape(DOM::FrameElement::IShape* thisShape, AWDShape2D* shapeBlock);
        FCM::Result SetSegment(const DOM::Utils::SEGMENT& segment);
        FCM::Result ExportFill(DOM::FrameElement::PIShape pIShape);
        FCM::Result ExportStroke(DOM::FrameElement::PIShape pIShape);
        FCM::Result ExportStrokeForFont(DOM::FrameElement::PIShape pIShape, AWDFontShape*);
		FCM::Result ExportFillStyle(FCM::PIFCMUnknown pFillStyle);
		
        void addFrame();
        void newTimeline();
        FCM::Result ExportPath(DOM::Service::Shape::PIPath pPath, bool is_hole);

        FCM::Result ExportSolidFillStyle(DOM::FillStyle::ISolidFillStyle* pSolidFillStyle);

        FCM::Result ExportRadialGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportLinearGradientFillStyle(DOM::FillStyle::IGradientFillStyle* pGradientFillStyle);

        FCM::Result ExportBitmapFillStyle(DOM::FillStyle::IBitmapFillStyle* pBitmapFillStyle);

};
#endif