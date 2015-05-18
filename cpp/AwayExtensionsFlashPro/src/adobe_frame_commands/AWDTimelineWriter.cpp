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

#include "FrameElement/IClassicText.h"
#include "FrameElement/IParagraph.h"
#include "FrameElement/ITextRun.h"
#include "FrameElement/ITextStyle.h"
#include "FrameElement/IShape.h"
#include "FrameElement/ISound.h"
#include "AWDTimelineWriter.h"
#include <cstring>
#include <fstream>
#include "FlashFCMPublicIDs.h"
#include "FCMPluginInterface.h"
#include "Utils.h"
#include "Service/Image/IBitmapExportService.h"
#include "Service/TextLayout/ITextLinesGeneratorService.h"
#include "Service/TextLayout/ITextLine.h"
#include "Service/Sound/ISoundExportService.h"
#include <fstream>
#include "Publisher.h"
#include "Utils.h"
#include "FlashFCMPublicIDs.h"
#include "GraphicFilter/IDropShadowFilter.h"
#include "GraphicFilter/IAdjustColorFilter.h"
#include "GraphicFilter/IBevelFilter.h"
#include "GraphicFilter/IBlurFilter.h"
#include "GraphicFilter/IGlowFilter.h"
#include "GraphicFilter/IGradientBevelFilter.h"
#include "GraphicFilter/IGradientGlowFilter.h"

#include "FrameElement/IClassicText.h"
#include "FrameElement/IParagraph.h"
#include "FrameElement/ITextRun.h"
#include "FrameElement/ITextStyle.h"
#include "FrameElement/IShape.h"
#include "FrameElement/ISound.h"
#include "FrameElement/IGraphic.h"
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
#include "LibraryItem/IMediaItem.h"
#include "LibraryItem/IFontItem.h"
#include "ILibraryItem.h"

#include "Service/Shape/IShapeService.h"
#include "Service/Shape/IRegionGeneratorService.h"
#include "Service/Shape/IFilledRegion.h"
#include "Service/Shape/IStrokeGroup.h"
#include "Service/Shape/IPath.h"
#include "Service/Shape/IEdge.h"
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
#include "Application/Service/IFlashApplicationService.h"
#include <stdio.h>
#include <iostream>
#ifdef _WINDOWS
#include "direct.h"
#else
#include <sys/stat.h>
#endif
namespace AwayJS
{
	
   /* -------------------------------------------------- AWDTimelineWriter */
		
    FCM::Result AWDTimelineWriter::PlaceObject(
        FCM::U_Int32 resId,
        FCM::U_Int32 objectId,
        FCM::U_Int32 placeAfterObjectId,
        const DOM::Utils::MATRIX2D* pMatrix,
        FCM::PIFCMUnknown pUnknown /* = NULL*/)
    {
		
		
		AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD);
		string resID=AwayJS::Utils::ToString(resId);

		frameCommand->set_resID(resID);
		frameCommand->set_depth(placeAfterObjectId);
		string newString("");
		FCM::AutoPtr<DOM::FrameElement::IMovieClip>pMovieClip = pUnknown;
        if (pMovieClip)
        {
			// if this is a movieclip, we check instance name
			FCM::StringRep16 newName;
			pMovieClip->GetName(&newName);
			//pMovieClip->GetBlendMode();
			//pMovieClip->IsVisible();
			newString=AwayJS::Utils::ToString(newName, m_pCallback);
			frameCommand->set_instanceName(newString);

            FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;
			pCalloc = AwayJS::Utils::GetCallocService(m_pCallback);
			ASSERT(pCalloc.m_Ptr != NULL);
			pCalloc->Free(newName);
			//Utils::Trace(m_pCallback, "		-> Placing a MovieClip into the timeline. resID = %d, objID = %d, instanceName = %s\n", resId, objectId, newString.c_str());
		}
		else{
			frameCommand->set_instanceName(newString);
			//Utils::Trace(m_pCallback, "		-> Placing a Shape, Image or text-field into the timeline.  resID = %d, objID = %d\n", resId, objectId);
		}

        if (pMatrix)
        {
            // only apply export matrix if it is not the identity matrix
			//if((pMatrix->a!=1.0)||(pMatrix->b!=0.0)||(pMatrix->c!=0.0)||(pMatrix->d!=1.0)||(pMatrix->tx!=0.0)||(pMatrix->ty!=0.0))
			frameCommand->set_display_matrix(awd->convert_matrix2x3(*pMatrix));
        }
        return FCM_SUCCESS;
    }

    FCM::Result AWDTimelineWriter::PlaceText(
        FCM::U_Int32 resId,
        FCM::U_Int32 objectId,
        FCM::U_Int32 placeAfterObjectId,
        const DOM::Utils::MATRIX2D* pMatrix,
        DOM::Utils::RECT rect /* = NULL*/)
    {
		AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD);
		string resID=AwayJS::Utils::ToString(resId);

		frameCommand->set_resID(resID);
		frameCommand->set_depth(placeAfterObjectId);
		
		BLOCKS::TextElement* thistext_block = reinterpret_cast<BLOCKS::TextElement*>(this->awd->get_project()->get_block_by_external_id_and_type(resID, BLOCK::block_type::TEXT_ELEMENT, true));
		thistext_block->add_res_id(resID);		
		//thistext_block->set_bounds(GEOM::BOUNDS2D(rect.topLeft.x, rect.bottomRight.x, rect.topLeft.y,rect.bottomRight.y));
        thistext_block->text_width=(rect.topLeft.x - rect.bottomRight.x);
        thistext_block->text_height=(rect.topLeft.y - rect.bottomRight.y);
        if (pMatrix)
        {
			TYPES::F64* new_mesh_mtx=(TYPES::F64*)malloc(6*8);
			new_mesh_mtx[0]= pMatrix->a;
			new_mesh_mtx[1]= pMatrix->b;
			new_mesh_mtx[2]= pMatrix->c;
			new_mesh_mtx[3]= pMatrix->d;
			new_mesh_mtx[4]= (rect.topLeft.x * pMatrix->a + rect.topLeft.y * pMatrix->c + pMatrix->tx);
			new_mesh_mtx[5]= (rect.topLeft.x * pMatrix->b + rect.topLeft.y * pMatrix->d + pMatrix->ty);
			double test2x = (rect.bottomRight.x * pMatrix->a + rect.bottomRight.y * pMatrix->c + pMatrix->tx);
			double test2y= (rect.bottomRight.x * pMatrix->b + rect.bottomRight.y * pMatrix->d + pMatrix->ty);
			double testx = new_mesh_mtx[4];
			double testy = new_mesh_mtx[5];
			frameCommand->set_display_matrix(new_mesh_mtx);
        }
        return FCM_SUCCESS;
    }
	
    FCM::Result AWDTimelineWriter::PlaceObject(
        FCM::U_Int32 resId,
        FCM::U_Int32 objectId,
        FCM::PIFCMUnknown pUnknown /* = NULL*/)
    {
        FCM::Result res=FCM_SUCCESS;
		/*
		AWD::ANIM::FrameCommandSoundObject* frameCommand=(AWD::ANIM::FrameCommandSoundObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::AWD_FRAME_COMMAND_SOUND);
        string resID=AwayJS::Utils::ToString(resId);
		frameCommand->set_resID(resID);
		//Utils::Trace(m_pCallback, "		-> Placing a SoundObject into the timeline.\n");
		
        FCM::AutoPtr<DOM::FrameElement::ISound> pSound;


        pSound = pUnknown;
        if (pSound)
        {
            DOM::FrameElement::SOUND_LOOP_MODE lMode;
            DOM::FrameElement::SOUND_LIMIT soundLimit;
            DOM::FrameElement::SoundSyncMode syncMode;

            soundLimit.structSize = sizeof(DOM::FrameElement::SOUND_LIMIT);
            lMode.structSize = sizeof(DOM::FrameElement::SOUND_LOOP_MODE);

            res = pSound->GetLoopMode(lMode);
            ASSERT(FCM_SUCCESS_CODE(res));
            
            // set the loop mode for the sound.
            // can either be infitiv for endless palying
            // or a finitve with a number of repeats
            // default for awd is infitive.
            if (lMode.loopMode != DOM::FrameElement::SoundRepeatMode::SOUND_REPEAT_INFINITE){
                frameCommand->set_loop_mode(TYPES::UINT32(lMode.repeatCount));
            }
            
            res = pSound->GetSyncMode(syncMode);
            ASSERT(FCM_SUCCESS_CODE(res));
            
            // We should not get SOUND_SYNC_STOP as for stop, "RemoveObject" command will
            // be generated by Exporter Service.
            ASSERT(syncMode != DOM::FrameElement::SOUND_SYNC_STOP);
            if (syncMode == DOM::FrameElement::SOUND_SYNC_EVENT){
                // do nothing but create a start sound command
            }
            else if (syncMode == DOM::FrameElement::SOUND_SYNC_START){
                // add another remove command in front of this command, so that sound is stopped if it already plays (the exporterservice does this already ?)
            }
            else if (syncMode == DOM::FrameElement::SOUND_SYNC_STREAM){
                // do nothing
            }
            res = pSound->GetSoundLimit(soundLimit);
            ASSERT(FCM_SUCCESS_CODE(res));
            frameCommand->set_sound_limit(TYPES::UINT32 (soundLimit.inPos44), TYPES::UINT32(soundLimit.outPos44));

        }
		*/
        return res;
    }
    FCM::Result AWDTimelineWriter::RemoveObject(
        FCM::U_Int32 objectId)
    {	
		
		AWD::ANIM::FrameCommandRemoveObject* frameCommand=(AWD::ANIM::FrameCommandRemoveObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::FRAME_COMMAND_REMOVE);
		frameCommand->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_REMOVE);
        return FCM_SUCCESS;    
	}	


    FCM::Result AWDTimelineWriter::UpdateZOrder(
        FCM::U_Int32 objectId,
        FCM::U_Int32 placeAfterObjectId)
    {
		// we dont need the updateZorder commands for our depth managment. (this is to validate)
		AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::FRAME_COMMAND_UPDATE);
		frameCommand->set_depth(placeAfterObjectId);
		frameCommand->adobe_depth_change=true;
		//TODO: test if the updateZOrder works...
        // Goutam: Commenting out the code for demo as fix is necessary in the Exporter service for it work properly.
#if 0
        commandElement.push_back(JSONNode("cmdType", "UpdateZOrder"));
        commandElement.push_back(JSONNode("objectId", std::to_string((FCM::U_Int64)(objectId)).c_str()));
        commandElement.push_back(JSONNode("placeAfter", std::to_string((FCM::U_Int64)(placeAfterObjectId)).c_str()));

        m_pCommandArray->push_back(commandElement);
#endif
		

        return FCM_SUCCESS;
    }

	
    FCM::Result AWDTimelineWriter::UpdateMask(
        FCM::U_Int32 objectId,
        FCM::U_Int32 maskTillObjectId)
    {
		AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::FRAME_COMMAND_UPDATE);
		frameCommand->set_clipDepth(maskTillObjectId);
        
        return FCM_SUCCESS;
    }

    FCM::Result AWDTimelineWriter::UpdateBlendMode(
        FCM::U_Int32 objectId,
        DOM::FrameElement::BlendMode blendMode)
    {
	
		AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::FRAME_COMMAND_UPDATE);
		
		// convert blendmode to away3d blenmode
        if(blendMode == 0)  //Normal
			frameCommand->set_blendmode(0);
        else if(blendMode == 1) //Layer
			frameCommand->set_blendmode(8);
        else if(blendMode == 2) //Darken
			frameCommand->set_blendmode(3);
        else if(blendMode == 3)//Multiply
			frameCommand->set_blendmode(10);
        else if(blendMode == 4)//Lighten
			frameCommand->set_blendmode(9);
        else if(blendMode == 5)//Screen
			frameCommand->set_blendmode(13);
        else if(blendMode == 6)//Overlay
			frameCommand->set_blendmode(12);
        else if(blendMode == 7)//Hardlight
			frameCommand->set_blendmode(6);
        else if(blendMode == 8)//Add
			frameCommand->set_blendmode(1);
        else if(blendMode == 9)//Substract //todo: do we have subtract in away3d (?)
			frameCommand->set_blendmode(0);
            //commandElement.push_back(JSONNode("blendMode","Substract"));
        else if(blendMode == 10)//Difference
			frameCommand->set_blendmode(4);
        else if(blendMode == 11)//Invert
			frameCommand->set_blendmode(7);
        else if(blendMode == 12)//Alpha
			frameCommand->set_blendmode(2);
        else if(blendMode == 13)//Erase
			frameCommand->set_blendmode(5);
		
        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::UpdateVisibility(
        FCM::U_Int32 objectId,
        FCM::Boolean visible)
    {
		
		AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::FRAME_COMMAND_UPDATE);
		frameCommand->set_visible(visible);
        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::AddGraphicFilter(
        FCM::U_Int32 objectId,
        FCM::PIFCMUnknown pFilter)
    {
		/*
		AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		
        FCM::Result res;
        //JSONNode commandElement(JSON_NODE);
        //commandElement.push_back(JSONNode("cmdType", "UpdateFilter"));
        //commandElement.push_back(JSONNode("objectId", CreateJS::Utils::ToString(objectId)));
        FCM::AutoPtr<DOM::GraphicFilter::IDropShadowFilter> pDropShadowFilter = pFilter;
        FCM::AutoPtr<DOM::GraphicFilter::IBlurFilter> pBlurFilter = pFilter;
        FCM::AutoPtr<DOM::GraphicFilter::IGlowFilter> pGlowFilter = pFilter;
        FCM::AutoPtr<DOM::GraphicFilter::IBevelFilter> pBevelFilter = pFilter;
        FCM::AutoPtr<DOM::GraphicFilter::IGradientGlowFilter> pGradientGlowFilter = pFilter;
        FCM::AutoPtr<DOM::GraphicFilter::IGradientBevelFilter> pGradientBevelFilter = pFilter;
        FCM::AutoPtr<DOM::GraphicFilter::IAdjustColorFilter> pAdjustColorFilter = pFilter;

        if (pDropShadowFilter)
        {
			
            FCM::Boolean enabled;
            FCM::Double  angle;
            FCM::Double  blurX;
            FCM::Double  blurY;
            FCM::Double  distance;
            FCM::Boolean hideObject;
            FCM::Boolean innerShadow;
            FCM::Boolean knockOut;
            DOM::Utils::FilterQualityType qualityType;
            DOM::Utils::COLOR color;
            FCM::S_Int32 strength;
            std::string colorStr;

            //commandElement.push_back(JSONNode("filterType", "DropShadowFilter"));
			
            pDropShadowFilter->IsEnabled(enabled);
            if(enabled)
            {
                commandElement.push_back(JSONNode("enabled", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("enabled", "false"));
            }

            res = pDropShadowFilter->GetAngle(angle);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("angle", CreateJS::Utils::ToString((double)angle)));

            res = pDropShadowFilter->GetBlurX(blurX);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurX", CreateJS::Utils::ToString((double)blurX)));

            res = pDropShadowFilter->GetBlurY(blurY);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurY", CreateJS::Utils::ToString((double)blurY)));

            res = pDropShadowFilter->GetDistance(distance);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("distance", CreateJS::Utils::ToString((double)distance)));

            res = pDropShadowFilter->GetHideObject(hideObject);
            ASSERT(FCM_SUCCESS_CODE(res));
            if(hideObject)
            {
                commandElement.push_back(JSONNode("hideObject", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("hideObject", "false"));
            }

            res = pDropShadowFilter->GetInnerShadow(innerShadow);
            ASSERT(FCM_SUCCESS_CODE(res));
            if(innerShadow)
            {
                commandElement.push_back(JSONNode("innerShadow", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("innerShadow", "false"));
            }

            res = pDropShadowFilter->GetKnockout(knockOut);
            ASSERT(FCM_SUCCESS_CODE(res));
            if(knockOut)
            {
                commandElement.push_back(JSONNode("knockOut", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("knockOut", "false"));
            }

            res = pDropShadowFilter->GetQuality(qualityType);
            ASSERT(FCM_SUCCESS_CODE(res));
            if (qualityType == 0)
                commandElement.push_back(JSONNode("qualityType", "low"));
            else if (qualityType == 1)
                commandElement.push_back(JSONNode("qualityType", "medium"));
            else if (qualityType == 2)
                commandElement.push_back(JSONNode("qualityType", "high"));

            res = pDropShadowFilter->GetStrength(strength);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("strength", CreateJS::Utils::ToString(strength)));

            res = pDropShadowFilter->GetShadowColor(color);
            ASSERT(FCM_SUCCESS_CODE(res));
            colorStr = Utils::ToString(color);
            commandElement.push_back(JSONNode("shadowColor", colorStr.c_str()));

        }
        if(pBlurFilter)
        {
            FCM::Boolean enabled;
            FCM::Double  blurX;
            FCM::Double  blurY;
            DOM::Utils::FilterQualityType qualityType;

            commandElement.push_back(JSONNode("filterType", "BlurFilter"));

            res = pBlurFilter->IsEnabled(enabled);
            if(enabled)
            {
                commandElement.push_back(JSONNode("enabled", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("enabled", "false"));
            }

            res = pBlurFilter->GetBlurX(blurX);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurX", CreateJS::Utils::ToString((double)blurX)));

            res = pBlurFilter->GetBlurY(blurY);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurY", CreateJS::Utils::ToString((double)blurY)));

            res = pBlurFilter->GetQuality(qualityType);
            ASSERT(FCM_SUCCESS_CODE(res));
            if (qualityType == 0)
                commandElement.push_back(JSONNode("qualityType", "low"));
            else if (qualityType == 1)
                commandElement.push_back(JSONNode("qualityType", "medium"));
            else if (qualityType == 2)
                commandElement.push_back(JSONNode("qualityType", "high"));
        }

        if(pGlowFilter)
        {
			
            FCM::Boolean enabled;
            FCM::Double  blurX;
            FCM::Double  blurY;
            FCM::Boolean innerShadow;
            FCM::Boolean knockOut;
            DOM::Utils::FilterQualityType qualityType;
            DOM::Utils::COLOR color;
            FCM::S_Int32 strength;
            std::string colorStr;
			
            commandElement.push_back(JSONNode("filterType", "GlowFilter"));

            res = pGlowFilter->IsEnabled(enabled);
            if(enabled)
            {
                commandElement.push_back(JSONNode("enabled", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("enabled", "false"));
            }

            res = pGlowFilter->GetBlurX(blurX);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurX", CreateJS::Utils::ToString((double)blurX)));

            res = pGlowFilter->GetBlurY(blurY);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurY", CreateJS::Utils::ToString((double)blurY)));

            res = pGlowFilter->GetInnerShadow(innerShadow);
            ASSERT(FCM_SUCCESS_CODE(res));
            if(innerShadow)
            {
                commandElement.push_back(JSONNode("innerShadow", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("innerShadow", "false"));
            }

            res = pGlowFilter->GetKnockout(knockOut);
            ASSERT(FCM_SUCCESS_CODE(res));
            if(knockOut)
            {
                commandElement.push_back(JSONNode("knockOut", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("knockOut", "false"));
            }

            res = pGlowFilter->GetQuality(qualityType);
            ASSERT(FCM_SUCCESS_CODE(res));
            if (qualityType == 0)
                commandElement.push_back(JSONNode("qualityType", "low"));
            else if (qualityType == 1)
                commandElement.push_back(JSONNode("qualityType", "medium"));
            else if (qualityType == 2)
                commandElement.push_back(JSONNode("qualityType", "high"));

            res = pGlowFilter->GetStrength(strength);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("strength", CreateJS::Utils::ToString(strength)));

            res = pGlowFilter->GetShadowColor(color);
            ASSERT(FCM_SUCCESS_CODE(res));
            colorStr = Utils::ToString(color);
            commandElement.push_back(JSONNode("shadowColor", colorStr.c_str()));
        }

        if(pBevelFilter)
        {
            FCM::Boolean enabled;
            FCM::Double  angle;
            FCM::Double  blurX;
            FCM::Double  blurY;
            FCM::Double  distance;
            DOM::Utils::COLOR highlightColor;
            FCM::Boolean knockOut;
            DOM::Utils::FilterQualityType qualityType;
            DOM::Utils::COLOR color;
            FCM::S_Int32 strength;
            DOM::Utils::FilterType filterType;
            std::string colorStr;
            std::string colorString;
            commandElement.push_back(JSONNode("filterType", "BevelFilter"));

            res = pBevelFilter->IsEnabled(enabled);
            if(enabled)
            {
                commandElement.push_back(JSONNode("enabled", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("enabled", "false"));
            }

            res = pBevelFilter->GetAngle(angle);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("angle", CreateJS::Utils::ToString((double)angle)));

            res = pBevelFilter->GetBlurX(blurX);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurX", CreateJS::Utils::ToString((double)blurX)));

            res = pBevelFilter->GetBlurY(blurY);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurY", CreateJS::Utils::ToString((double)blurY)));

            res = pBevelFilter->GetDistance(distance);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("distance", CreateJS::Utils::ToString((double)distance)));

            res = pBevelFilter->GetHighlightColor(highlightColor);
            ASSERT(FCM_SUCCESS_CODE(res));
            colorString = Utils::ToString(highlightColor);
            commandElement.push_back(JSONNode("highlightColor",colorString.c_str()));

            res = pBevelFilter->GetKnockout(knockOut);
            ASSERT(FCM_SUCCESS_CODE(res));
            if(knockOut)
            {
                commandElement.push_back(JSONNode("knockOut", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("knockOut", "false"));
            }

            res = pBevelFilter->GetQuality(qualityType);
            ASSERT(FCM_SUCCESS_CODE(res));
            if (qualityType == 0)
                commandElement.push_back(JSONNode("qualityType", "low"));
            else if (qualityType == 1)
                commandElement.push_back(JSONNode("qualityType", "medium"));
            else if (qualityType == 2)
                commandElement.push_back(JSONNode("qualityType", "high"));

            res = pBevelFilter->GetStrength(strength);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("strength", CreateJS::Utils::ToString(strength)));

            res = pBevelFilter->GetShadowColor(color);
            ASSERT(FCM_SUCCESS_CODE(res));
            colorStr = Utils::ToString(color);
            commandElement.push_back(JSONNode("shadowColor", colorStr.c_str()));

            res = pBevelFilter->GetFilterType(filterType);
            ASSERT(FCM_SUCCESS_CODE(res));
            if (filterType == 0)
                commandElement.push_back(JSONNode("filterType", "inner"));
            else if (filterType == 1)
                commandElement.push_back(JSONNode("filterType", "outer"));
            else if (filterType == 2)
                commandElement.push_back(JSONNode("filterType", "full"));

        }

        if(pGradientGlowFilter)
        {
            FCM::Boolean enabled;
            FCM::Double  angle;
            FCM::Double  blurX;
            FCM::Double  blurY;
            FCM::Double  distance;
            FCM::Boolean knockOut;
            DOM::Utils::FilterQualityType qualityType;
            FCM::S_Int32 strength;
            DOM::Utils::FilterType filterType;
            commandElement.push_back(JSONNode("filterType", "GradientGlowFilter"));

            pGradientGlowFilter->IsEnabled(enabled);
            if(enabled)
            {
                commandElement.push_back(JSONNode("enabled", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("enabled", "false"));
            }

            res = pGradientGlowFilter->GetAngle(angle);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("angle", CreateJS::Utils::ToString((double)angle)));

            res = pGradientGlowFilter->GetBlurX(blurX);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurX", CreateJS::Utils::ToString((double)blurX)));

            res = pGradientGlowFilter->GetBlurY(blurY);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurY", CreateJS::Utils::ToString((double)blurY)));

            res = pGradientGlowFilter->GetDistance(distance);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("distance", CreateJS::Utils::ToString((double)distance)));

            res = pGradientGlowFilter->GetKnockout(knockOut);
            ASSERT(FCM_SUCCESS_CODE(res));
            if(knockOut)
            {
                commandElement.push_back(JSONNode("knockOut", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("knockOut", "false"));
            }

            res = pGradientGlowFilter->GetQuality(qualityType);
            ASSERT(FCM_SUCCESS_CODE(res));
            if (qualityType == 0)
                commandElement.push_back(JSONNode("qualityType", "low"));
            else if (qualityType == 1)
                commandElement.push_back(JSONNode("qualityType", "medium"));
            else if (qualityType == 2)
                commandElement.push_back(JSONNode("qualityType", "high"));

            res = pGradientGlowFilter->GetStrength(strength);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("strength", CreateJS::Utils::ToString(strength)));

            res = pGradientGlowFilter->GetFilterType(filterType);
            ASSERT(FCM_SUCCESS_CODE(res));
            if (filterType == 0)
                commandElement.push_back(JSONNode("filterType", "inner"));
            else if (filterType == 1)
                commandElement.push_back(JSONNode("filterType", "outer"));
            else if (filterType == 2)
                commandElement.push_back(JSONNode("filterType", "full"));

            FCM::AutoPtr<FCM::IFCMUnknown> pColorGradient;
            res = pGradientGlowFilter->GetGradient(pColorGradient.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            FCM::AutoPtr<DOM::Utils::ILinearColorGradient> pLinearGradient = pColorGradient;
            if (pLinearGradient)
            {

                FCM::U_Int8 colorCount;
                //DOM::Utils::GRADIENT_COLOR_POINT colorPoint;

                res = pLinearGradient->GetKeyColorCount(colorCount);
                ASSERT(FCM_SUCCESS_CODE(res));

                std::string colorArray ;
                std::string posArray ;
                JSONNode*   stopPointArray = new JSONNode(JSON_ARRAY);

                for (FCM::U_Int32 l = 0; l < colorCount; l++)
                {
                    DOM::Utils::GRADIENT_COLOR_POINT colorPoint;
                    pLinearGradient->GetKeyColorAtIndex(l, colorPoint);
                    JSONNode stopEntry(JSON_NODE);
                    FCM::Float offset;

                    offset = (float)((colorPoint.pos * 100) / 255.0);

                    stopEntry.push_back(JSONNode("offset", Utils::ToString((double) offset)));
                    stopEntry.push_back(JSONNode("stopColor", Utils::ToString(colorPoint.color)));
                    stopEntry.push_back(JSONNode("stopOpacity", Utils::ToString((double)(colorPoint.color.alpha / 255.0))));
                    stopPointArray->set_name("GradientStops");
                    stopPointArray->push_back(stopEntry);
                }

                commandElement.push_back(*stopPointArray);

            }//lineargradient
        }

        if(pGradientBevelFilter)
        {
            FCM::Boolean enabled;
            FCM::Double  angle;
            FCM::Double  blurX;
            FCM::Double  blurY;
            FCM::Double  distance;
            FCM::Boolean knockOut;
            DOM::Utils::FilterQualityType qualityType;
            FCM::S_Int32 strength;
            DOM::Utils::FilterType filterType;

            commandElement.push_back(JSONNode("filterType", "GradientBevelFilter"));

            pGradientBevelFilter->IsEnabled(enabled);
            if(enabled)
            {
                commandElement.push_back(JSONNode("enabled", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("enabled", "false"));
            }

            res = pGradientBevelFilter->GetAngle(angle);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("angle", CreateJS::Utils::ToString((double)angle)));

            res = pGradientBevelFilter->GetBlurX(blurX);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurX", CreateJS::Utils::ToString((double)blurX)));

            res = pGradientBevelFilter->GetBlurY(blurY);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("blurY", CreateJS::Utils::ToString((double)blurY)));

            res = pGradientBevelFilter->GetDistance(distance);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("distance", CreateJS::Utils::ToString((double)distance)));

            res = pGradientBevelFilter->GetKnockout(knockOut);
            ASSERT(FCM_SUCCESS_CODE(res));
            if(knockOut)
            {
                commandElement.push_back(JSONNode("knockOut", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("knockOut", "false"));
            }

            res = pGradientBevelFilter->GetQuality(qualityType);
            ASSERT(FCM_SUCCESS_CODE(res));
            if (qualityType == 0)
                commandElement.push_back(JSONNode("qualityType", "low"));
            else if (qualityType == 1)
                commandElement.push_back(JSONNode("qualityType", "medium"));
            else if (qualityType == 2)
                commandElement.push_back(JSONNode("qualityType", "high"));

            res = pGradientBevelFilter->GetStrength(strength);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("strength", CreateJS::Utils::ToString(strength)));

            res = pGradientBevelFilter->GetFilterType(filterType);
            ASSERT(FCM_SUCCESS_CODE(res));
            if (filterType == 0)
                commandElement.push_back(JSONNode("filterType", "inner"));
            else if (filterType == 1)
                commandElement.push_back(JSONNode("filterType", "outer"));
            else if (filterType == 2)
                commandElement.push_back(JSONNode("filterType", "full"));

            FCM::AutoPtr<FCM::IFCMUnknown> pColorGradient;
            res = pGradientBevelFilter->GetGradient(pColorGradient.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            FCM::AutoPtr<DOM::Utils::ILinearColorGradient> pLinearGradient = pColorGradient;
            if (pLinearGradient)
            {

                FCM::U_Int8 colorCount;
                //DOM::Utils::GRADIENT_COLOR_POINT colorPoint;

                res = pLinearGradient->GetKeyColorCount(colorCount);
                ASSERT(FCM_SUCCESS_CODE(res));

                std::string colorArray ;
                std::string posArray ;
                JSONNode*   stopPointsArray = new JSONNode(JSON_ARRAY);

                for (FCM::U_Int32 l = 0; l < colorCount; l++)
                {
                    DOM::Utils::GRADIENT_COLOR_POINT colorPoint;
                    pLinearGradient->GetKeyColorAtIndex(l, colorPoint);
                    JSONNode stopEntry(JSON_NODE);
                    FCM::Float offset;

                    offset = (float)((colorPoint.pos * 100) / 255.0);

                    stopEntry.push_back(JSONNode("offset", Utils::ToString((double) offset)));
                    stopEntry.push_back(JSONNode("stopColor", Utils::ToString(colorPoint.color)));
                    stopEntry.push_back(JSONNode("stopOpacity", Utils::ToString((double)(colorPoint.color.alpha / 255.0))));
                    stopPointsArray->set_name("GradientStops");
                    stopPointsArray->push_back(stopEntry);
                }

                commandElement.push_back(*stopPointsArray);

            }//lineargradient
        }

        if(pAdjustColorFilter)
        {
            FCM::Double brightness;
            FCM::Double contrast;
            FCM::Double saturation;
            FCM::Double hue;
            FCM::Boolean enabled;
			
            commandElement.push_back(JSONNode("filterType", "AdjustColorFilter"));

            pAdjustColorFilter->IsEnabled(enabled);
            if(enabled)
            {
                commandElement.push_back(JSONNode("enabled", "true"));
            }
            else
            {
                commandElement.push_back(JSONNode("enabled", "false"));
            }

            res = pAdjustColorFilter->GetBrightness(brightness);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("brightness", CreateJS::Utils::ToString((double)brightness)));

            res = pAdjustColorFilter->GetContrast(contrast);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("contrast", CreateJS::Utils::ToString((double)contrast)));

            res = pAdjustColorFilter->GetSaturation(saturation);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("saturation", CreateJS::Utils::ToString((double)saturation)));

            res = pAdjustColorFilter->GetHue(hue);
            ASSERT(FCM_SUCCESS_CODE(res));
            commandElement.push_back(JSONNode("hue", CreateJS::Utils::ToString((double)hue)));
			
        }

		*/
        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::UpdateDisplayTransform(
        FCM::U_Int32 objectId,
        const DOM::Utils::MATRIX2D& matrix)
    {
		
		AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::FRAME_COMMAND_UPDATE);
		frameCommand->set_display_matrix(awd->convert_matrix2x3(matrix));
        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::UpdateColorTransform(
        FCM::U_Int32 objectId,
        const DOM::Utils::COLOR_MATRIX& colorMatrix)
    {
		AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId, ANIM::frame_command_type::FRAME_COMMAND_UPDATE);
		frameCommand->set_color_matrix(awd->convert_matrix4x5_to_colortransform(colorMatrix));
        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::ShowFrame(FCM::U_Int32 frameNum)
    {
		ANIM::TimelineFrame* newFrame=new ANIM::TimelineFrame();
		newFrame->set_frame_duration(1);//TYPES::UINT32(1000/awd->get_project()->get_settings()->get_fps()));
		thisTimeLine->add_frame(newFrame);	
        return FCM_SUCCESS;
    }

	
    FCM::Result AWDTimelineWriter::AddFrameScript(FCM::CStringRep16 pScript, FCM::U_Int32 layerNum)
    {
        std::string script = Utils::ToString(pScript, m_pCallback);

        std::string scriptWithLayerNumber = "script Layer" +  Utils::ToString(layerNum);

		/*
        std::string find = "\n";
        std::string replace = "";
        std::string::size_type i =0;

        while (true) {
            i = script.find(find, i);
           
            if (i == std::string::npos) break;
            script.replace(i, find.length(), replace);

            i += replace.length();
        }
        */
        //Utils::Trace(m_pCallback, "[AddFrameScript] (Layer: %d): %s\n", layerNum, script.c_str());

		thisTimeLine->get_frame()->set_frame_code(script);

        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::RemoveFrameScript(FCM::U_Int32 layerNum)
    {
        //Utils::Trace(m_pCallback, "[RemoveFrameScript] (Layer: %d)\n", layerNum);

        return FCM_SUCCESS;
    }

    FCM::Result AWDTimelineWriter::SetFrameLabel(FCM::StringRep16 pLabel, DOM::KeyFrameLabelType labelType)
    {
		
        std::string label = Utils::ToString(pLabel, m_pCallback);
        //Utils::Trace(m_pCallback, "[SetFrameLabel] (Type: %d): %s\n", labelType, label.c_str());
		if(label.empty()){
			return FCM_SUCCESS;
		}
		else{
			if(labelType == 1)
				thisTimeLine->get_frame()->add_label(ANIM::frame_label_type::AWD_FRAME_LABEL_NAME, label);
			else if(labelType == 2)//COMMENTS DONT SEAM TO GET EXPORTED BY IEXPORTERSERVICE
				thisTimeLine->get_frame()->add_label(ANIM::frame_label_type::AWD_FRAME_LABEL_COMMENT, label);
			else if(labelType == 3)
				thisTimeLine->get_frame()->add_label(ANIM::frame_label_type::AWD_FRAME_LABEL_ANCOR, label);
		}
		
        return FCM_SUCCESS;
    }

	AWDTimelineWriter::AWDTimelineWriter(FlashToAWDEncoder * awd, FCM::PIFCMCallback pcallback):
        m_pCallback(pcallback)
    {
	
		this->awd=awd;
		this->thisTimeLine=new BLOCKS::Timeline();
		m_pCallback = pcallback;
		ANIM::TimelineFrame* newFrame=new ANIM::TimelineFrame();
		newFrame->set_frame_duration(1);
		thisTimeLine->add_frame(newFrame);
    }


    AWDTimelineWriter::~AWDTimelineWriter()
    {
		/*
        delete m_pCommandArray;

        delete m_pFrameArray;

        delete m_pTimelineElement;
		*/
    }

	
    void AWDTimelineWriter::Finish(FCM::U_Int32 resId, FCM::StringRep16 pName)
    {
		
        string resID_string=AwayJS::Utils::ToString(resId);
		thisTimeLine->add_res_id(resID_string);
		if(pName!=NULL){
			thisTimeLine->is_grafic_instance=false;
			thisTimeLine->set_symbol_name(AwayJS::Utils::ToString(pName, m_pCallback));
			// set the name as fallback, if we cannot connect this instance to any library-symbol
			thisTimeLine->set_name(AwayJS::Utils::ToString(pName, m_pCallback));
			//Utils::Trace(m_pCallback, "ADDED A CHILD TIMELINE NAME = %s\n", timelineName.c_str());
		}
		thisTimeLine->get_frames().pop_back();		
		// we cache timelines by function awd_project->get_timeline_by_symbol_name
		awd->get_project()->add_block(thisTimeLine);
		
    }

};
