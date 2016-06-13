#ifndef TIMELINEBUILDER_H_
#define TIMELINEBUILDER_H_
#include <vector>
#include "Version.h"
#include "FCMTypes.h"
#include "FCMPluginInterface.h"
#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder2.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"
#include "Publisher/IPublisher.h"
#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"
#include "FrameElement/IClassicText.h"
#include "FrameElement/ITextStyle.h"
#include "Exporter/Service/IFrameCommandGenerator.h"

#include "Exporter/Service/ITimelineBuilder2.h"
#include "AWDTimelineWriter.h"

using namespace FCM;
using namespace Publisher;
using namespace Exporter::Service;

namespace AwayJS
{
    class CPublisher;
    class ResourcePalette;
    class TimelineBuilder2;
    class TimelineBuilderFactory;
    class IOutputWriter;
    class ITimelineWriter;

	class TimelineBuilder : public ITimelineBuilder2, public FCMObjectBase
    {
    public:

		BEGIN_INTERFACE_MAP(TimelineBuilder, AWAYJS_PLUGIN_VERSION)    
            INTERFACE_ENTRY(ITimelineBuilder2)              
        END_INTERFACE_MAP    

        virtual FCM::Result _FCMCALL AddShape(
            FCM::U_Int32 objectId, 
            SHAPE_INFO* pShapeInfo);

        virtual FCM::Result _FCMCALL AddClassicText(
            FCM::U_Int32 objectId, 
            CLASSIC_TEXT_INFO* pClassicTextInfo);
            
        virtual FCM::Result _FCMCALL AddBitmap(
            FCM::U_Int32 objectId, 
            BITMAP_INFO* pBitmapInfo);
            
        virtual FCM::Result _FCMCALL AddMovieClip(
            FCM::U_Int32 objectId, 
            MOVIE_CLIP_INFO* pMovieClipInfo, 
            DOM::FrameElement::PIMovieClip pMovieClip);
            
        virtual FCM::Result _FCMCALL AddGraphic(
            FCM::U_Int32 objectId, 
            GRAPHIC_INFO* pGraphicInfo);

        virtual FCM::Result _FCMCALL AddSound(
            FCM::U_Int32 objectId, 
            SOUND_INFO* pSoundInfo, 
            DOM::FrameElement::PISound pSound);

        virtual FCM::Result _FCMCALL UpdateZOrder(
            FCM::U_Int32 objectId, 
            FCM::U_Int32 placeAfterObjectId);
		
        virtual FCM::Result UpdateMask(
            FCM::U_Int32 objectId,
            FCM::U_Int32 maskTillObjectId);

        virtual FCM::Result _FCMCALL Remove(FCM::U_Int32 objectId);
            
        virtual FCM::Result _FCMCALL UpdateBlendMode(
            FCM::U_Int32 objectId, 
            DOM::FrameElement::BlendMode blendMode);

        virtual FCM::Result _FCMCALL UpdateVisibility(
            FCM::U_Int32 objectId, 
            FCM::Boolean visible);

        virtual FCM::Result _FCMCALL UpdateGraphicFilter(
            FCM::U_Int32 objectId, 
            PIFCMList pFilterable);
            
        virtual FCM::Result _FCMCALL UpdateDisplayTransform(
            FCM::U_Int32 objectId, 
            const DOM::Utils::MATRIX2D& matrix);
            
        virtual FCM::Result _FCMCALL UpdateColorTransform(
            FCM::U_Int32 objectId, 
            const DOM::Utils::COLOR_MATRIX& colorMatrix);
		

        virtual FCM::Result _FCMCALL ShowFrame();        

        virtual FCM::Result _FCMCALL AddFrameScript(FCM::CStringRep16 pScript, FCM::U_Int32 layerNum);

        virtual FCM::Result _FCMCALL RemoveFrameScript(FCM::U_Int32 layerNum);

        virtual FCM::Result _FCMCALL SetFrameLabel(FCM::StringRep16 pLabel, DOM::KeyFrameLabelType labelType);

        TimelineBuilder();

        ~TimelineBuilder();

        virtual FCM::Result Build(
            FCM::U_Int32 resourceId, 
            FCM::StringRep16 pName,
            ITimelineWriter** ppTimelineWriter);

		void Init(AnimateToAWDEncoder *);

    private:		
        AWDTimelineWriter* m_pTimelineWriter;

        FCM::U_Int32 m_frameIndex;
    };


    class TimelineBuilderFactory : public ITimelineBuilderFactory, public FCMObjectBase
    {
    public:

        BEGIN_INTERFACE_MAP(TimelineBuilderFactory, AWAYJS_PLUGIN_VERSION)    
            INTERFACE_ENTRY(ITimelineBuilderFactory)            
        END_INTERFACE_MAP    

        virtual FCM::Result _FCMCALL CreateTimelineBuilder(PITimelineBuilder& pTimelineBuilder);

        TimelineBuilderFactory();

        ~TimelineBuilderFactory();

        void Init(AnimateToAWDEncoder* flash_to_awd);

    private:

        AnimateToAWDEncoder* m_flash_to_awd;
    };

}
#endif // PUBLISHER_H_