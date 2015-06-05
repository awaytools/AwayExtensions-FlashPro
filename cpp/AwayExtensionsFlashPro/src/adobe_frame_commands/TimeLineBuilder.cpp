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

#include "FrameElement/IButton.h"
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


#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

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
	

    /* ----------------------------------------------------- TimelineBuilder */

    FCM::Result TimelineBuilder::AddShape(FCM::U_Int32 objectId, SHAPE_INFO* pShapeInfo)
    {
        return m_pTimelineWriter->PlaceObject(pShapeInfo->resourceId, objectId, pShapeInfo->placeAfterObjectId, &pShapeInfo->matrix);
    }

    FCM::Result TimelineBuilder::AddClassicText(FCM::U_Int32 objectId, CLASSIC_TEXT_INFO* pClassicTextInfo)
    {	
        ASSERT(pClassicTextInfo);
        ASSERT(pClassicTextInfo->structSize >= sizeof(CLASSIC_TEXT_INFO));

        LOG(("[AddClassicText] ObjId: %d ResId: %d PlaceAfter: %d\n", 
            objectId, pClassicTextInfo->resourceId, pClassicTextInfo->placeAfterObjectId));
        
        //To get the bounding rect of the text
        DOM::Utils::RECT rect;
        if(pClassicTextInfo->structSize >= sizeof(DISPLAY_OBJECT_INFO_2))
        {
            DISPLAY_OBJECT_INFO_2 *ptr = static_cast<DISPLAY_OBJECT_INFO_2*>(pClassicTextInfo);
            if(ptr)
            {
                rect = ptr->bounds;
                // This rect object gives the bound of the text filed.
                // This will have to be transformed using the pClassicTextInfo->matrix
                // to map it to its parent's co-orinate space to render it.
            }
			// todo: set the bound for the textfield_block
        }
		return m_pTimelineWriter->PlaceText(pClassicTextInfo->resourceId, objectId, pClassicTextInfo->placeAfterObjectId, &pClassicTextInfo->matrix, rect);     
    }

    FCM::Result TimelineBuilder::AddBitmap(FCM::U_Int32 objectId, BITMAP_INFO* pBitmapInfo)
    {
        return m_pTimelineWriter->PlaceObject(pBitmapInfo->resourceId, objectId, pBitmapInfo->placeAfterObjectId, &pBitmapInfo->matrix);
    }

    FCM::Result TimelineBuilder::AddMovieClip(FCM::U_Int32 objectId, MOVIE_CLIP_INFO* pMovieClipInfo, DOM::FrameElement::PIMovieClip pMovieClip)
    {
        FCM::Result res;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnknown = pMovieClip;
		
        AutoPtr<DOM::FrameElement::IButton> pButton = pMovieClip;
        if(pButton.m_Ptr)
        {
            DOM::FrameElement::ButtonTrackMode trackMode;
            pButton->GetTrackingMode(trackMode);
            if(trackMode == DOM::FrameElement::TRACK_AS_BUTTON)
            {
                LOG(("[AddMovieClip] ObjId: %d, is a button with TrackingMode set to TRACK_AS_BUTTON\n", objectId));
				std::string res_id = std::to_string(pMovieClipInfo->resourceId);
				BLOCKS::Timeline* thisBlock = reinterpret_cast<BLOCKS::Timeline*>(this->m_pTimelineWriter->awd->get_project()->get_block_by_external_id_shared(res_id));
				if(thisBlock==NULL){
					return FCM_EXPORT_FAILED;
				}
				thisBlock->isButton=true;

            }else
            {
                LOG(("[AddMovieClip] ObjId: %d, is a button with TrackingMode set to TRACK_AS_MENU_ITEM\n", objectId));
            }
        }
        res = m_pTimelineWriter->PlaceObject(pMovieClipInfo->resourceId, objectId, pMovieClipInfo->placeAfterObjectId, &pMovieClipInfo->matrix, pUnknown);
        return res;
    }

    FCM::Result TimelineBuilder::AddGraphic(FCM::U_Int32 objectId, GRAPHIC_INFO* pGraphicInfo)
    {
        FCM::Result res;
        res = m_pTimelineWriter->PlaceObject(pGraphicInfo->resourceId, objectId, pGraphicInfo->placeAfterObjectId, &pGraphicInfo->matrix);
        return res;
    }

    FCM::Result TimelineBuilder::AddSound( FCM::U_Int32 objectId, SOUND_INFO* pSoundInfo, DOM::FrameElement::PISound pSound)
    {
        FCM::AutoPtr<FCM::IFCMUnknown> pUnknown = pSound;
        FCM::Result res;		
        res = m_pTimelineWriter->PlaceObject(pSoundInfo->resourceId, objectId, pUnknown);
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
	
     FCM::Result TimelineBuilder::UpdateMask(FCM::U_Int32 objectId, FCM::U_Int32 maskTillObjectId)
    {
        FCM::Result res = FCM_SUCCESS;

        LOG(("[UpdateMask] ObjId: %d MaskTill: %d\n", 
            objectId, maskTillObjectId));

        res = m_pTimelineWriter->UpdateMask(objectId, maskTillObjectId);

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
		if(this->m_pTimelineWriter!=NULL){
			this->m_pTimelineWriter->Finish(resourceId, pName);
		}
        *ppTimelineWriter = this->m_pTimelineWriter;
		return FCM_SUCCESS;
    }


    TimelineBuilder::TimelineBuilder() :
        m_frameIndex(0)
    {
    }

    TimelineBuilder::~TimelineBuilder()
    {
    }

	void TimelineBuilder::Init(FlashToAWDEncoder* awd)
    {
		m_pTimelineWriter = new AWDTimelineWriter(awd, GetCallback());
        //ASSERT(m_pTimelineWriter);
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
		FCM::Result res = GetCallback()->CreateInstance(NULL, CLSID_TimelineBuilder, IID_ITIMELINE_BUILDER_2, (void**)&pTimelineBuilder);

        TimelineBuilder* pTimeline = static_cast<TimelineBuilder*>(pTimelineBuilder);
        
        pTimeline->Init(m_flash_to_awd);

        return res;
    }

    void TimelineBuilderFactory::Init(FlashToAWDEncoder* p_flash_to_awd)
    {
        m_flash_to_awd = p_flash_to_awd;
    }


};
