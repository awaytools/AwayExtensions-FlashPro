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
#include "OutputWriter.h"
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

#include "OutputWriter.h"

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

    static const FCM::Float GRADIENT_VECTOR_CONSTANT = 16384.0;
    
    /* -------------------------------------------------- AWDOutputWriter */



	FlashToAWDEncoder *
	AWDOutputWriter::get_awd()
    {
        return this->awd;
    }

	void
	AWDOutputWriter::set_awd(FlashToAWDEncoder * awd)
    {
        this->awd=awd;
    }
	

    FCM::Result AWDOutputWriter::StartDefineTimeline()
    {
        return FCM_SUCCESS;
    }


    FCM::Result AWDOutputWriter::EndDefineTimeline(
        FCM::U_Int32 resId, 
        FCM::StringRep16 pName,
        ITimelineWriter* pTimelineWriter)
    {
        return FCM_SUCCESS;
    }


    FCM::Result AWDOutputWriter::StartDefineShape()
    {
        return FCM_SUCCESS;
    }


    // Marks the end of a shape
    FCM::Result AWDOutputWriter::EndDefineShape(FCM::U_Int32 resId)
    {
        return FCM_SUCCESS;
    }


    // Start of fill region definition
	// Start of one AWDSubShape2D
    FCM::Result AWDOutputWriter::StartDefineFill()
    {

		/*
        m_pathElem = new JSONNode(JSON_NODE);
        ASSERT(m_pathElem);

        m_pathCmdStr.clear();
		*/
        return FCM_SUCCESS;
    }


    // Solid fill style definition
    FCM::Result AWDOutputWriter::DefineSolidFillStyle(const DOM::Utils::COLOR& color)
    {
		/*
		//AwayJS::Utils::Trace(m_pCallback, "Alpha = %d\n",color.alpha);
		awd_color awdColor=awdutil_int_color( color.alpha, color.red, color.green, color.blue );
		AWDShape2DFill* newFill=awd->get_solid_fill(awdColor);
		newFill->set_color_components(color.red, color.green, color.blue, color.alpha);
		shape_encoder->get_shape()->set_fill(newFill);
		shape_encoder->set_color(color.red, color.green, color.blue, color.alpha);
		*/
        return FCM_SUCCESS;
    }

		
    // Bitmap fill style definition
    FCM::Result AWDOutputWriter::DefineText(
            FCM::U_Int32 resId, 
            DOM::FrameElement::PIClassicText pTextItem2)
    {
		/*
		
        DOM::AutoPtr<DOM::FrameElement::IClassicText> pTextItem;
        FCMListPtr pParagraphsList;
        FCM::StringRep16 textDisplay;
        FCM::StringRep16 txt_instance_name16;
        FCM::U_Int32 count = 0;
        FCM::U_Int16 fontSize;
        std::string fName; 
        std::string displayText2; 
        DOM::Utils::COLOR fontColor;
        FCM::Result res;     
		
        std::string text_instance_name=Utils::ToString(resId);

        pTextItem = pTextItem2;
        AutoPtr<DOM::FrameElement::ITextBehaviour> textBehaviour;
        pTextItem->GetTextBehaviour(textBehaviour.m_Ptr);
        AutoPtr<DOM::FrameElement::IDynamicTextBehaviour> dynamicTextBehaviour = textBehaviour.m_Ptr;
        AutoPtr<DOM::FrameElement::IModifiableTextBehaviour> modifyableTextBehaviour = textBehaviour.m_Ptr;
		 
		bool merge_Paragraphs=false;		
		bool export_localized=false;	
		bool use_localized_str=false;
		bool export_this_localized=false;
		string localized_str="_AWDlocal";
        if(modifyableTextBehaviour)
        {
			modifyableTextBehaviour->GetInstanceName(&txt_instance_name16);
			text_instance_name = Utils::ToString(txt_instance_name16, m_pCallback);
			if((export_localized)&&(!use_localized_str)){
				export_this_localized=true;
			}
			if((export_localized)&&(use_localized_str)){
				int find_localized=text_instance_name.find(use_localized_str);
				if(find_localized>0){
					export_this_localized=true;
				}
			}
		}
		
		AWDTextElement * textElement;
		textElement=awd->get_text(text_instance_name);
		string objID_string=AwayJS::Utils::ToString(resId);
		textElement->set_objectID(objID_string);

		if(export_this_localized){
			textElement->set_isLocalized(export_this_localized);
		}
		else{
			pTextItem->GetParagraphs(pParagraphsList.m_Ptr);
			res = pParagraphsList->Count(count);
			ASSERT(FCM_SUCCESS_CODE(res));
			res = pTextItem->GetText(&textDisplay);
			ASSERT(FCM_SUCCESS_CODE(res));
			displayText2 = AwayJS::Utils::ToString(textDisplay, m_pCallback);
				
			//Utils::Trace(m_pCallback, "The text-element encountered: '%s'\n", displayText2.c_str());
		
			AWDFont * fontShapes;
			AWDFontStyle * fontStyle;
			for (FCM::U_Int32 pIndex = 0; pIndex < count; pIndex++)
			{
				AutoPtr<DOM::FrameElement::IParagraph> pParagraph = pParagraphsList[pIndex];
			
				AWDParagraph * awdParagraph=NULL;
				if (pParagraph)
				{
					if((!merge_Paragraphs)||(awdParagraph==NULL)){
						awdParagraph = new AWDParagraph();
						textElement->add_paragraph(awdParagraph);
					}
					FCMListPtr pTextRunList;
					pParagraph->GetTextRuns(pTextRunList.m_Ptr);
					FCM::U_Int32 trCount;
					pTextRunList->Count(trCount);

					for (FCM::U_Int32 trIndex = 0; trIndex < trCount; trIndex++)
					{
						AWDTextRun* awdTextRun = new AWDTextRun();
						AutoPtr<DOM::FrameElement::ITextRun> pTextRun = pTextRunList[trIndex];
						AutoPtr<DOM::FrameElement::ITextStyle> trStyle;

						pTextRun->GetTextStyle(trStyle.m_Ptr);  

						string formatString="";
						
						FCM::StringRep16 pFontName;
						res = trStyle->GetFontName(&pFontName);
						ASSERT(FCM_SUCCESS_CODE(res));
						formatString+=AwayJS::Utils::ToString(pFontName, m_pCallback);
						
						FCM::StringRep8 pFontStyle;
						res = trStyle->GetFontStyle(&pFontStyle);
						ASSERT(FCM_SUCCESS_CODE(res));
						formatString+=pFontStyle;

						res = trStyle->GetFontSize(fontSize);
						ASSERT(FCM_SUCCESS_CODE(res));
						formatString+=AwayJS::Utils::ToString(fontSize);

						res = trStyle->GetFontColor(fontColor);
						ASSERT(FCM_SUCCESS_CODE(res));
						awd_color awdColor=awdutil_int_color( fontColor.alpha, fontColor.red, fontColor.green, fontColor.blue );
						formatString+=AwayJS::Utils::ToString(awdColor);

						FCM::S_Int16 letterSpacing;
						trStyle->GetLetterSpacing(letterSpacing);
						formatString+=AwayJS::Utils::ToString(letterSpacing);
												                 
						DOM::FrameElement::BaseLineShiftStyle baselineshiftStyle;
						trStyle->GetBaseLineShiftStyle(baselineshiftStyle);
						AWD_baselineshift_type baselineshift=BASELINE_NORMAL;
						if(baselineshiftStyle==DOM::FrameElement::BASE_LINE_SHIFT_STYLE_SUBSCRIPT){
							baselineshift=BASELINE_DOWN;
						}
						else if(baselineshiftStyle==DOM::FrameElement::BASE_LINE_SHIFT_STYLE_SUPERSCRIPT){
							baselineshift=BASELINE_UP;
						}
						formatString+=AwayJS::Utils::ToString(baselineshift);
						
						FCM::Boolean autoKerning;
						res = trStyle->IsAutoKernEnabled(autoKerning);
						ASSERT(FCM_SUCCESS_CODE(res));
						formatString+=AwayJS::Utils::ToString(autoKerning);

						FCM::Boolean isRotated;
						res = trStyle->IsAutoKernEnabled(isRotated);
						ASSERT(FCM_SUCCESS_CODE(res));
						formatString+=AwayJS::Utils::ToString(isRotated);

						AWDTextFormat* thisTextFormat=awd->get_text_format(formatString);						
						string fontstyle="";
						fontstyle+=pFontStyle;
						thisTextFormat->set_fontStyle(fontstyle);
                        std::string tfstring=AwayJS::Utils::ToString(pFontName, m_pCallback);
						thisTextFormat->set_fontName(tfstring);
						thisTextFormat->set_fontSize(fontSize);
						thisTextFormat->set_fill(awd->get_solid_fill(awdColor));
						thisTextFormat->set_letterSpacing(letterSpacing);
						thisTextFormat->set_baseLineShift(baselineshift);
						// set autoKerning
						// set IsRotated

						awdTextRun->set_format(thisTextFormat);

						FCM::U_Int32 startIndex;
						FCM::U_Int32 length;
						pTextRun->GetStartIndex(startIndex);
						pTextRun->GetLength(length);
						std::string str = displayText2.substr (startIndex,length);
						//Utils::Trace(m_pCallback, "	The text-run content: '%s'\n", str.c_str());
						awdTextRun->set_text(str);
						std::string styleStr;					
						
						styleStr = pFontStyle;
                        std::string fontNamestr=Utils::ToString(pFontName, m_pCallback);
						fontShapes=awd->get_font_shapes(fontNamestr);
						fontStyle=fontShapes->get_font_style(styleStr);
						int strcnt=0;
						for(char& c : str) {
							if(int(c)<0){
								Utils::Trace(m_pCallback, "Found Unsupported Font-char  = %d \n", c);
							}
							else{
								fontStyle->get_fontShape(c);
								strcnt++;
							}
						}
						awdParagraph->add_textrun(awdTextRun);
					}
				}
			} 
		}
		*/
        return FCM_SUCCESS;
	}

    // Bitmap fill style definition
    FCM::Result AWDOutputWriter::DefineBitmapFillStyle(
        FCM::Boolean clipped,
        const DOM::Utils::MATRIX2D& matrix,
        FCM::S_Int32 height, 
        FCM::S_Int32 width,
        FCM::StringRep16 pName,
        DOM::LibraryItem::PIMediaItem pMediaItem)
    {
		/*
        std::string bitmapPath;
        std::string bitmapName;
		std::string bitmapRelPath;
        std::string bitmapExportPath = m_outputImageFolder + "/";
		FCM::Result res;
        // Form the image path
        bitmapName = Utils::ToString(pName, m_pCallback);
        std::size_t pos = bitmapName.find(".");

        if (pos != std::string::npos)
        {
            FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
            
            if ((bitmapName.substr(pos + 1) != "jpg") ||
                (bitmapName.substr(pos + 1) != "png"))
            {
                std::string bitmapNameWithoutExt;

                bitmapNameWithoutExt = bitmapName.substr(0, pos);
                bitmapName = bitmapNameWithoutExt + ".png";
            }
            
            bitmapRelPath = "./";
            bitmapRelPath += IMAGE_FOLDER;
            bitmapRelPath += "/" + bitmapName;
			
            res = m_pCallback->GetService(DOM::FLA_BITMAP_SERVICE, pUnk.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            FCM::AutoPtr<DOM::Service::Image::IBitmapExportService> bitmapExportService = pUnk;
            if (bitmapExportService)
            {
                res = bitmapExportService->ExportToFile(pMediaItem, Utils::ToString16(bitmapExportPath, m_pCallback), 100);
                ASSERT(FCM_SUCCESS_CODE(res));
            }
		}
		else{
			ASSERT(0);
		}
		shape_encoder->get_shape()->set_fill(awd->get_bitmap_fill(bitmapRelPath));

        DOM::Utils::MATRIX2D matrix1 = matrix;
        matrix1.a /= 20.0;
        matrix1.b /= 20.0;
        matrix1.c /= 20.0;
        matrix1.d /= 20.0;
		// TO DO: USE UV-MATRIX ON AWD-SHAPE
        */
        return FCM_SUCCESS;
    }


    // Start Linear Gradient fill style definition
    FCM::Result AWDOutputWriter::StartDefineLinearGradientFillStyle(
        DOM::FillStyle::GradientSpread spread,
        const DOM::Utils::MATRIX2D& matrix)
    {

		//shape_encoder->get_shape()->set_fill(awd->get_linear_gradient_fill());
		/*
        DOM::Utils::POINT2D point;
        m_gradientColor = new JSONNode(JSON_NODE);
        ASSERT(m_gradientColor);
        m_gradientColor->set_name("linearGradient");

        point.x = -GRADIENT_VECTOR_CONSTANT / 20;
        point.y = 0;
        Utils::TransformPoint(matrix, point, point);

        m_gradientColor->push_back(JSONNode("x1", Utils::ToString((double) (point.x))));
        m_gradientColor->push_back(JSONNode("y1", Utils::ToString((double) (point.y))));

        point.x = GRADIENT_VECTOR_CONSTANT / 20;
        point.y = 0;
        Utils::TransformPoint(matrix, point, point);

        m_gradientColor->push_back(JSONNode("x2", Utils::ToString((double) (point.x))));
        m_gradientColor->push_back(JSONNode("y2", Utils::ToString((double) (point.y))));

        m_gradientColor->push_back(JSONNode("spreadMethod", Utils::ToString(spread)));

        m_stopPointArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_stopPointArray);
        m_stopPointArray->set_name("stop");
		*/
        return FCM_SUCCESS;
    }


    // Sets a specific key point in a color ramp (for both radial and linear gradient)
    FCM::Result AWDOutputWriter::SetKeyColorPoint(
        const DOM::Utils::GRADIENT_COLOR_POINT& colorPoint)
    {

		/*
        JSONNode stopEntry(JSON_NODE);
        FCM::Float offset;
        
        offset = (float)((colorPoint.pos * 100) / 255.0);

        stopEntry.push_back(JSONNode("offset", Utils::ToString((double) offset)));
        stopEntry.push_back(JSONNode("stopColor", Utils::ToString(colorPoint.color)));
        stopEntry.push_back(JSONNode("stopOpacity", Utils::ToString((double)(colorPoint.color.alpha / 255.0))));

        m_stopPointArray->push_back(stopEntry);
		*/
        return FCM_SUCCESS;
    }


    // End Linear Gradient fill style definition
    FCM::Result AWDOutputWriter::EndDefineLinearGradientFillStyle()
    {
		/*
        m_gradientColor->push_back(*m_stopPointArray);
        m_pathElem->push_back(*m_gradientColor);

        delete m_stopPointArray;
        delete m_gradientColor;
		*/
        return FCM_SUCCESS;
    }


    // Start Radial Gradient fill style definition
    FCM::Result AWDOutputWriter::StartDefineRadialGradientFillStyle(
        DOM::FillStyle::GradientSpread spread,
        const DOM::Utils::MATRIX2D& matrix,
        FCM::S_Int32 focalPoint)
    {
		//shape_encoder->get_shape()->set_fill(awd->get_radial_gradient_fill());
		/*
        DOM::Utils::POINT2D point;
        DOM::Utils::POINT2D point1;
        DOM::Utils::POINT2D point2;

        m_gradientColor = new JSONNode(JSON_NODE);
        ASSERT(m_gradientColor);
        m_gradientColor->set_name("radialGradient");

        point.x = 0;
        point.y = 0;
        Utils::TransformPoint(matrix, point, point1);

        point.x = GRADIENT_VECTOR_CONSTANT / 20;
        point.y = 0;
        Utils::TransformPoint(matrix, point, point2);

        FCM::Float xd = point1.x - point2.x;
        FCM::Float yd = point1.y - point2.y;
        FCM::Float r = sqrt(xd * xd + yd * yd);

        FCM::Float angle = atan2(yd, xd);
        double focusPointRatio = focalPoint / 255.0;
        double fx = -r * focusPointRatio * cos(angle);
        double fy = -r * focusPointRatio * sin(angle);

        m_gradientColor->push_back(JSONNode("cx", "0"));
        m_gradientColor->push_back(JSONNode("cy", "0"));
        m_gradientColor->push_back(JSONNode("r", Utils::ToString((double) r)));
        m_gradientColor->push_back(JSONNode("fx", Utils::ToString((double) fx)));
        m_gradientColor->push_back(JSONNode("fy", Utils::ToString((double) fy)));

        FCM::Float scaleFactor = (GRADIENT_VECTOR_CONSTANT / 20) / r;
        DOM::Utils::MATRIX2D matrix1 = {};
        matrix1.a = matrix.a * scaleFactor;
        matrix1.b = matrix.b * scaleFactor;
        matrix1.c = matrix.c * scaleFactor;
        matrix1.d = matrix.d * scaleFactor;
        matrix1.tx = matrix.tx;
        matrix1.ty = matrix.ty;

        m_gradientColor->push_back(JSONNode("gradientTransform", Utils::ToString(matrix1)));
        m_gradientColor->push_back(JSONNode("spreadMethod", Utils::ToString(spread)));

        m_stopPointArray = new JSONNode(JSON_ARRAY);
        ASSERT(m_stopPointArray);
        m_stopPointArray->set_name("stop");
		*/
        return FCM_SUCCESS;
    }


    // End Radial Gradient fill style definition
    FCM::Result AWDOutputWriter::EndDefineRadialGradientFillStyle()
    {
		/*
        m_gradientColor->push_back(*m_stopPointArray);
        m_pathElem->push_back(*m_gradientColor);

        delete m_stopPointArray;
        delete m_gradientColor;
		*/

        return FCM_SUCCESS;
    }


    // Start of fill region boundary
    FCM::Result AWDOutputWriter::StartDefineBoundary()
    {
        return StartDefinePath();
    }
	
    
	// Sets a segment of a path (Used for boundary, holes)
    FCM::Result AWDOutputWriter::SetSegment(const DOM::Utils::SEGMENT& segment)
    {
		/*
	
		ShapePoint* startPoint= new ShapePoint();
		ShapePoint* endPoint= new ShapePoint();
		AWDPathSegment* awdPathSeg = new AWDPathSegment();
        if (segment.segmentType == DOM::Utils::LINE_SEGMENT)
        {			
			//AwayJS::Utils::Trace(m_pCallback, "Line\n");
			startPoint->x=segment.line.endPoint1.x;
			startPoint->y=segment.line.endPoint1.y;
			endPoint->x=segment.line.endPoint2.x;
			endPoint->y=segment.line.endPoint2.y;
		}
        else{
			//AwayJS::Utils::Trace(m_pCallback, "Curve\n");
			startPoint->x=segment.quadBezierCurve.anchor1.x;
			startPoint->y=segment.quadBezierCurve.anchor1.y;
			endPoint->x=segment.quadBezierCurve.anchor2.x;
			endPoint->y=segment.quadBezierCurve.anchor2.y;
			ShapePoint* controlPoint= new ShapePoint();
			controlPoint->x=segment.quadBezierCurve.control.x;
			controlPoint->y=segment.quadBezierCurve.control.y;
			awdPathSeg->set_controlPoint(controlPoint);
			awdPathSeg->set_edgeType(CURVED_EDGE);
		}	
		awdPathSeg->set_startPoint(startPoint);
		awdPathSeg->set_endPoint(endPoint);
		int hole_idx=shape_encoder->get_hole_idx();
		awdPathSeg->set_hole_idx(hole_idx);
		shape_encoder->add_path_segment(awdPathSeg);
		
		m_firstSegment = false;
		*/
        return FCM_SUCCESS;
    }


    // End of fill region boundary
    FCM::Result AWDOutputWriter::EndDefineBoundary()
    {
        return EndDefinePath();
    }


    // Start of fill region hole
    FCM::Result AWDOutputWriter::StartDefineHole()
    {
		//shape_encoder->add_hole();
        return StartDefinePath();
    }


    // End of fill region hole
    FCM::Result AWDOutputWriter::EndDefineHole()
    {
        return EndDefinePath();
    }


    // Start of stroke group
    FCM::Result AWDOutputWriter::StartDefineStrokeGroup()
    {
        // No need to do anything
        return FCM_SUCCESS;
    }


    // Start solid stroke style definition
    FCM::Result AWDOutputWriter::StartDefineSolidStrokeStyle(
        FCM::Double thickness,
        const DOM::StrokeStyle::JOIN_STYLE& joinStyle,
        const DOM::StrokeStyle::CAP_STYLE& capStyle,
        DOM::Utils::ScaleType scaleType,
        FCM::Boolean strokeHinting)
    {
		/*
        m_strokeStyle.type = SOLID_STROKE_STYLE_TYPE;
        m_strokeStyle.solidStrokeStyle.capStyle = capStyle;
        m_strokeStyle.solidStrokeStyle.joinStyle = joinStyle;
        m_strokeStyle.solidStrokeStyle.thickness = thickness;
        m_strokeStyle.solidStrokeStyle.scaleType = scaleType;
        m_strokeStyle.solidStrokeStyle.strokeHinting = strokeHinting;
		*/
        return FCM_SUCCESS;
    }


    // End of solid stroke style
    FCM::Result AWDOutputWriter::EndDefineSolidStrokeStyle()
    {
        return FCM_SUCCESS;
    }


    // Start of stroke 
    FCM::Result AWDOutputWriter::StartDefineStroke()
    {
        return FCM_SUCCESS;
    }


    // End of a stroke 
    FCM::Result AWDOutputWriter::EndDefineStroke()
    {
		// Not used anymore...
        return FCM_SUCCESS;
    }


    // End of stroke group
    FCM::Result AWDOutputWriter::EndDefineStrokeGroup()
    {
        return FCM_SUCCESS;
    }

    FCM::Result AWDOutputWriter::EndDefineFill()
    {
        return FCM_SUCCESS;
    }
		
    FCM::Result AWDOutputWriter::DefineSound(
            FCM::U_Int32 resId, 
			const std::string& name, 
            DOM::LibraryItem::PIMediaItem pMediaItem)
    {
		/*
		
        FCM::Result res;
        std::string soundPath;
        std::string objectID = AwayJS::Utils::ToString(resId);
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
        std::string soundRelPath;
        std::string soundName=name;
        std::string soundExportPath = m_outputSoundFolder + "\\";
        soundRelPath = ".\\";
        soundRelPath += SOUND_FOLDER;
        soundRelPath += "\\";
        FCM::AutoPtr<DOM::ILibraryItem> libItem = pMediaItem;
		FCM::StringRep16 sName;
		libItem->GetName(&sName);
		if(libItem){
			//Utils::Trace(m_pCallback, "		Sound name = '%s'\n",Utils::ToString(sName,m_pCallback).c_str());  
			soundName=Utils::ToString(sName,m_pCallback);   
            FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;
			pCalloc = AwayJS::Utils::GetCallocService(m_pCallback);
			ASSERT(pCalloc.m_Ptr != NULL);
			pCalloc->Free(sName);  
		}
        soundRelPath += soundName;
        soundExportPath += soundName;
        res = m_pCallback->GetService(DOM::FLA_SOUND_SERVICE, pUnk.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));
        FCM::AutoPtr<DOM::Service::Sound::ISoundExportService> soundExportService = pUnk;
        if (soundExportService)
        {
            FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;
            FCM::StringRep16 pFilePath = Utils::ToString16(soundExportPath, m_pCallback);
            res = soundExportService->ExportToFile(pMediaItem, pFilePath);
            ASSERT(FCM_SUCCESS_CODE(res));
			pCalloc = AwayJS::Utils::GetCallocService(m_pCallback);
            ASSERT(pCalloc.m_Ptr != NULL);
            pCalloc->Free(pFilePath);
			//Utils::Trace(m_pCallback, "The Sound-file: '%s' should have been created in this folder.\n", soundExportPath.c_str());

        }
		AWDAudio* newAudio=awd->get_audio(soundName);		
		newAudio->set_url(soundExportPath);
		newAudio->set_objectID(objectID);
		Utils::Trace(m_pCallback, "The Sound-file: '%s' should have been exported.\n", soundExportPath.c_str());
		newAudio->set_embed_data();
		newAudio->set_saving_type(EMBEDDED_AUDIO);
		//double length=newAudio->get_embbed_length()/1024;
		//Utils::Trace(m_pCallback, "The Sound-file embbed length: '%d'.\n", sizeof(data));
		//Utils::Trace(m_pCallback, "The Sound-file embbed length: '%f'.\n", length);
				*/
        return FCM_SUCCESS;
	}
    
    // Define a bitmap
    FCM::Result AWDOutputWriter::DefineBitmap(
        FCM::U_Int32 resId,
        FCM::S_Int32 height, 
        FCM::S_Int32 width,
        FCM::StringRep16 pName,
        DOM::LibraryItem::PIMediaItem pMediaItem)
    {
		/*
        FCM::Result res;	
		

        std::string bitmapPath;
        std::string bitmapName;
        // Form the image path
        bitmapName = Utils::ToString(pName, m_pCallback);
        std::size_t pos = bitmapName.find(".");

        if (pos != std::string::npos)
        {
            FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
            std::string bitmapRelPath;
            std::string bitmapExportPath = m_outputImageFolder + "/";
            
            if ((bitmapName.substr(pos + 1) != "jpg") ||
                (bitmapName.substr(pos + 1) != "png"))
            {
                std::string bitmapNameWithoutExt;

                bitmapNameWithoutExt = bitmapName.substr(0, pos);
                bitmapName = bitmapNameWithoutExt + ".png";
            }

            bitmapExportPath += bitmapName;
            
            bitmapRelPath = "./";
            bitmapRelPath += IMAGE_FOLDER;
            bitmapRelPath += "/" + bitmapName;

            res = m_pCallback->GetService(DOM::FLA_BITMAP_SERVICE, pUnk.m_Ptr);
            ASSERT(FCM_SUCCESS_CODE(res));

            FCM::AutoPtr<DOM::Service::Image::IBitmapExportService> bitmapExportService = pUnk;
            if (bitmapExportService)
            {
                res = bitmapExportService->ExportToFile(pMediaItem, Utils::ToString16(bitmapExportPath, m_pCallback), 100);
                ASSERT(FCM_SUCCESS_CODE(res));
            }
			std::string objectID=Utils::ToString(resId);
			AWDBitmapTexture* newTexture = awd->get_texture(bitmapRelPath);
			newTexture->set_objectID(objectID);
        }
        else
        {
            // Should not reach here
            ASSERT(0);
        }
		*/
        return FCM_SUCCESS;
    }


    AWDOutputWriter::AWDOutputWriter(FCM::PIFCMCallback pCallback)
        : m_pCallback(pCallback),
          m_firstSegment(false)
    {
    }


    AWDOutputWriter::~AWDOutputWriter()
    {
    }	
	
    FCM::PIFCMCallback* AWDOutputWriter::get_mCallback()
    {
		return &m_pCallback;
	}
	
    FCM::Result AWDOutputWriter::StartDefinePath()
    {
        return FCM_SUCCESS;
    }

    FCM::Result AWDOutputWriter::EndDefinePath()
    {
        return FCM_SUCCESS;
    }

};
