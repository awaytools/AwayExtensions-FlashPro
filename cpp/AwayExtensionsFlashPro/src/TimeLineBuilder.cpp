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

#include "OutputWriter.h"

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

namespace AwayJS
{
	

    /* ----------------------------------------------------- TimelineBuilder */

    FCM::Result TimelineBuilder::AddShape(FCM::U_Int32 objectId, SHAPE_INFO* pShapeInfo)
    {
        FCM::Result res;
        res = m_pTimelineWriter->PlaceObject(
            pShapeInfo->resourceId, 
            objectId, 
            pShapeInfo->placeAfterObjectId, 
            &pShapeInfo->matrix);

        return res;
    }

    FCM::Result TimelineBuilder::AddClassicText(FCM::U_Int32 objectId, CLASSIC_TEXT_INFO* pClassicTextInfo)
    {
        FCM::Result res;
		
		res = m_pTimelineWriter->PlaceObject(
			pClassicTextInfo->resourceId, 
			objectId, 
			pClassicTextInfo->placeAfterObjectId, 
			&pClassicTextInfo->matrix);
        return res;
    }

    FCM::Result TimelineBuilder::AddBitmap(FCM::U_Int32 objectId, BITMAP_INFO* pBitmapInfo)
    {
        FCM::Result res;		
        res = m_pTimelineWriter->PlaceObject(
            pBitmapInfo->resourceId, 
            objectId, 
            pBitmapInfo->placeAfterObjectId, 
            &pBitmapInfo->matrix);

        return res;
    }

    FCM::Result TimelineBuilder::AddMovieClip(FCM::U_Int32 objectId, MOVIE_CLIP_INFO* pMovieClipInfo, DOM::FrameElement::PIMovieClip pMovieClip)
    {
        FCM::Result res;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnknown = pMovieClip;
        res = m_pTimelineWriter->PlaceObject(
            pMovieClipInfo->resourceId, 
            objectId, 
            pMovieClipInfo->placeAfterObjectId, 
            &pMovieClipInfo->matrix,
            pUnknown);

        return res;
    }

    FCM::Result TimelineBuilder::AddGraphic(FCM::U_Int32 objectId, GRAPHIC_INFO* pGraphicInfo)
    {
        FCM::Result res;

        res = m_pTimelineWriter->PlaceObject(
            pGraphicInfo->resourceId, 
            objectId, 
            pGraphicInfo->placeAfterObjectId, 
            &pGraphicInfo->matrix);

        return res;
    }

    FCM::Result TimelineBuilder::AddSound( FCM::U_Int32 objectId,     SOUND_INFO* pSoundInfo, DOM::FrameElement::PISound pSound)
    {
        FCM::AutoPtr<FCM::IFCMUnknown> pUnknown = pSound;
        FCM::Result res;
		
        res = m_pTimelineWriter->PlaceObject(
            pSoundInfo->resourceId, 
            objectId, 
            pUnknown);

        return res;
    }

    FCM::Result TimelineBuilder::UpdateZOrder(FCM::U_Int32 objectId, FCM::U_Int32 placeAfterObjectId)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateZOrder(objectId, placeAfterObjectId);

        return res;
    }

    FCM::Result TimelineBuilder::Remove(FCM::U_Int32 objectId)
    {
        FCM::Result res;

        res = m_pTimelineWriter->RemoveObject(objectId);

        return res;
    }

    FCM::Result TimelineBuilder::UpdateBlendMode(FCM::U_Int32 objectId, DOM::FrameElement::BlendMode blendMode)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateBlendMode(objectId, blendMode);

        return res;
    }

    FCM::Result TimelineBuilder::UpdateVisibility(FCM::U_Int32 objectId, FCM::Boolean visible)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateVisibility(objectId, visible);

        return res;
    }

    FCM::Result TimelineBuilder::UpdateGraphicFilter(FCM::U_Int32 objectId, PIFCMList pFilterable)
    {
        FCM::U_Int32 count;
        FCM::Result res;

        res = pFilterable->Count(count);
        ASSERT(FCM_SUCCESS_CODE(res));
        
        for (FCM::U_Int32 i = 0; i < count; i++)
        {
        
            FCM::AutoPtr<FCM::IFCMUnknown> pUnknown = (*pFilterable)[i];
            res = m_pTimelineWriter->AddGraphicFilter(objectId, pUnknown.m_Ptr);

            if (FCM_FAILURE_CODE(res))
            {
                return res;
            }
		}
        return FCM_SUCCESS;
    
	}


    FCM::Result TimelineBuilder::UpdateDisplayTransform(FCM::U_Int32 objectId, const DOM::Utils::MATRIX2D& matrix)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateDisplayTransform(objectId, matrix);

        return res;
    }

    FCM::Result TimelineBuilder::UpdateColorTransform(FCM::U_Int32 objectId, const DOM::Utils::COLOR_MATRIX& colorMatrix)
    {
        FCM::Result res;

        res = m_pTimelineWriter->UpdateColorTransform(objectId, colorMatrix);

        return res;
    }

    FCM::Result TimelineBuilder::ShowFrame()        
    {
        FCM::Result res;

        res = m_pTimelineWriter->ShowFrame(m_frameIndex);

        m_frameIndex++;

        return res;
    }
	
    FCM::Result TimelineBuilder::AddFrameScript(FCM::CStringRep16 pScript, FCM::U_Int32 layerNum)
    {
        FCM::Result res = FCM_SUCCESS;

        LOG(("[AddFrameScript] LayerNum: %d\n", layerNum));

        if (pScript != NULL)
        {
            res = m_pTimelineWriter->AddFrameScript(pScript, layerNum);
        }

        return res;
    }

    FCM::Result TimelineBuilder::RemoveFrameScript(FCM::U_Int32 layerNum)
    {
        FCM::Result res = FCM_SUCCESS;

        LOG(("[RemoveFrameScript] LayerNum: %d\n", layerNum));

        res = m_pTimelineWriter->RemoveFrameScript(layerNum);

        return res;
    }

    FCM::Result TimelineBuilder::SetFrameLabel(FCM::StringRep16 pLabel, DOM::KeyFrameLabelType labelType)
    {
        FCM::Result res = FCM_SUCCESS;

        LOG(("[SetFrameLabel]\n"));

        if (pLabel != NULL)
        {
            res = m_pTimelineWriter->SetFrameLabel(pLabel, labelType);
        }

        return res;
    }
    FCM::Result TimelineBuilder::Build(
        FCM::U_Int32 resourceId, 
        FCM::StringRep16 pName,
        ITimelineWriter** ppTimelineWriter)
    {
        FCM::Result res;

        res = m_pOutputWriter->EndDefineTimeline(resourceId, pName, m_pTimelineWriter);

        *ppTimelineWriter = m_pTimelineWriter;

        return res;
    }


    TimelineBuilder::TimelineBuilder() :
        m_pOutputWriter(NULL),
        m_frameIndex(0)
    {
    }

    TimelineBuilder::~TimelineBuilder()
    {
    }

    void TimelineBuilder::Init(IOutputWriter* pOutputWriter, AWD* awd)
    {
        m_pOutputWriter = (AWDOutputWriter*)pOutputWriter;

        m_pOutputWriter->StartDefineTimeline();

		m_pTimelineWriter = new AWDTimelineWriter(awd, *m_pOutputWriter->get_mCallback());
        ASSERT(m_pTimelineWriter);
    }

    /* ----------------------------------------------------- TimelineBuilderFactory */

    TimelineBuilderFactory::TimelineBuilderFactory()
    {
    }

    TimelineBuilderFactory::~TimelineBuilderFactory()
    {
    }

    FCM::Result TimelineBuilderFactory::CreateTimelineBuilder(PITimelineBuilder& pTimelineBuilder)
    {
        //FCM::Result res = GetCallback()->CreateInstance(NULL, CLSID_TimelineBuilder, IID_ITWEEN_ATTACHER, (void**)&pTimelineBuilder);
		FCM::Result res = GetCallback()->CreateInstance(NULL, CLSID_TimelineBuilder, IID_ITimelineBuilder, (void**)&pTimelineBuilder);

        TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);
        
        pTimeline->Init(m_pOutputWriter, m_pOutputWriter->get_awd());

        return res;
    }

    void TimelineBuilderFactory::Init(IOutputWriter* pOutputWriter)
    {
        m_pOutputWriter = (AWDOutputWriter*)pOutputWriter;
    }


};
