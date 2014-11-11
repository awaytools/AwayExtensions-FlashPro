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
#include "shape2Dtimeline.h"
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
    static const char* htmlOutputJS = "<!DOCTYPE html> \r\n \
<html> \r\n \
<head> \r\n \
    <title></title> \r\n \
	<label id= \"awdPath\">%s</label> \r\n \
	<label id= \"awdWidth\">%d</label> \r\n \
	<label id= \"awdHeight\">%d</label> \r\n \
    <script type=\"text/javascript\" language=\"javascript\" src=\"js/AWD3Viewer.js\"></script> \r\n \
</head> \r\n \
<body> \r\n \
</body> \r\n \
</html>";
    static const char* htmlOutput = "<!DOCTYPE html> \r\n \
<html lang=\"en\"> \r\n \
<head> \r\n \
	<meta charset=\"utf-8\"/> \r\n \
	<title>awd_render</title>\r\n \
	<meta name=\"description\" content=\"\" />\r\n \
	<script src=\"js/swfobject.js\"></script> \r\n \
	<script> \r\n \
        var flashvars = { \r\n \
			awd: \"%s.awd\", \r\n \
			bgcolor: \"#%06X\", \r\n \
			fps: \"%d\", \r\n \
			width: \"%d\", \r\n \
			height: \"%d\" \r\n \
        }; \r\n \
		var params = { \r\n \
			menu: \"false\", \r\n \
			scale: \"noScale\", \r\n \
			allowFullscreen: \"true\", \r\n \
			allowScriptAccess: \"always\", \r\n \
			bgcolor: \"\", \r\n \
			wmode: \"direct\"  \r\n \
		};\r\n \
		var attributes = { \r\n \
			id:\"awdrender\" \r\n \
		};\r\n \
		swfobject.embedSWF(\r\n \
			\"awdrender.swf\", \r\n \
			\"altContent\", \"100%s\", \"100%s\", \"15.0.0.152\", \r\n \
			\"expressInstall.swf\", \r\n \
			flashvars, params, attributes);\r\n \
	</script>\r\n \
	<style>\r\n \
		html, body { height:100%s; overflow:hidden; }\r\n \
		body { margin:0; }\r\n \
	</style>\r\n \
</head>\r\n \
<body>\r\n \
	<div id=\"altContent\">\r\n \
		<h1>The AWD for your AwayJS-project can not be displayed.</h1>\r\n \
		<h4>The Demo needs FlashPlayer 15. </h4>\r\n \
		<h4>Also the runtime (\"awd_renderer.swf\" / \"js/swfobject.js\" should be located in same folder as this html-file.</h4>\r\n \
		<p><a href=\"http://www.adobe.com/go/getflashplayer\">Get Adobe Flash player</a></p>\r\n \
	</div>\r\n \
</body>\r\n \
</html>";


    /* -------------------------------------------------- AWDOutputWriter */

    FCM::Result AWDOutputWriter::StartOutput(std::string& outputFileName)
    {
        std::string parent;
        std::string awdName;
        std::string outputjsFolder;
		
        Utils::GetParent(outputFileName, parent);
        Utils::GetFileNameWithoutExtension(outputFileName, awdName);
        m_outputImageFolder = parent + IMAGE_FOLDER;
        m_outputSoundFolder = parent + SOUND_FOLDER;
        outputjsFolder = parent + "js";
        m_outputHTMLFile = parent + awdName + ".html";
		m_target_asViewer_path = parent + AS_VIEWER_NAME;
		m_target_swfObject_path = outputjsFolder + "/swfobject.js";
		m_target_JSViewer_path = outputjsFolder + "/AWD3Viewer.js";
		// create the 
		//mkdir(m_outputImageFolder.c_str(), 0);//TODO
		
#ifdef _WINDOWS
		mkdir(m_outputImageFolder.c_str());
        mkdir(m_outputSoundFolder.c_str());
        mkdir(outputjsFolder.c_str());
#else
        mkdir(m_outputImageFolder.c_str(), 0777);
        mkdir(m_outputSoundFolder.c_str(), 0777);
        mkdir(outputjsFolder.c_str(), 0777);
#endif
        return FCM_SUCCESS;
    }
	AWD *
	AWDOutputWriter::get_awd()
    {
        return this->awd;
    }
	void
	AWDOutputWriter::set_awd(AWD * awd)
    {
        this->awd=awd;
		this->outline_threshold=0.2;
		this->save_interstect=true;
		this->double_subdivide=true;
    }
	
double
AWDOutputWriter::get_outline_threshold(){
	return this->outline_threshold;
}
void
AWDOutputWriter::set_outline_threshold(double outline_threshold){
	this->outline_threshold=outline_threshold;
}
bool
AWDOutputWriter::get_save_interstect(){
	return this->save_interstect;
}
void
AWDOutputWriter::set_save_interstect(bool save_interstect){
	this->save_interstect=save_interstect;
}
bool
AWDOutputWriter::get_double_subdivide(){
	return this->double_subdivide;
}
void
AWDOutputWriter::set_double_subdivide(bool double_subdivide){
	this->double_subdivide=double_subdivide;
}


    FCM::Result AWDOutputWriter::EndOutput()
    {

        return FCM_SUCCESS;
    }
	
    FCM::Result AWDOutputWriter::PreParePreview(
        const DOM::Utils::COLOR& background,
        FCM::U_Int32 stageHeight, 
        FCM::U_Int32 stageWidth,
        FCM::U_Int32 fps,
		bool doJS)
    {
		
        FCM::U_Int32 backColor;
				
        std::string awdName;
        Utils::GetFileNameWithoutExtension(m_outputHTMLFile, awdName);
		if(doJS){
			m_HTMLOutputJS = new char[strlen(htmlOutputJS) + 50];
			if (m_HTMLOutputJS == NULL)
			{
				return FCM_MEM_NOT_AVAILABLE;
			}
			awdName+=".awd";
			sprintf(m_HTMLOutputJS, htmlOutputJS, awdName.c_str(), stageWidth, stageHeight);

			std::string js_viewer_path;
			Utils::GetModuleFilePath(js_viewer_path, m_pCallback);
        
			std::string findThisjs="AwayExtensionsFlashPro.fcm.plugin/Contents/MacOS/";
			int foundIndexjs=js_viewer_path.find(findThisjs);
			if(foundIndexjs>0){
				js_viewer_path = js_viewer_path.substr(0, foundIndexjs);
			}
			js_viewer_path += JS_VIEWER_NAME;

        
			AwayJS::Utils::CopyOneFile(js_viewer_path, m_target_JSViewer_path, m_pCallback);

			std::ofstream file;
			remove(m_outputHTMLFile.c_str());

			file.open(m_outputHTMLFile.c_str());
			file << m_HTMLOutputJS;
			file.close();
			//Utils::Trace(m_pCallback, "\nShould have Created HTML: %s !\n\n",m_outputHTMLFile.c_str());

			delete [] m_HTMLOutputJS;
		}
		else{
			m_HTMLOutput = new char[strlen(htmlOutput) + 50];
			if (m_HTMLOutput == NULL)
			{
				return FCM_MEM_NOT_AVAILABLE;
			}
		
			//awd_color awdColor=awdutil_int_color( background.alpha, background.red, background.green, background.blue );
			backColor = (background.red << 16) | (background.green << 8) | (background.blue);
			sprintf(m_HTMLOutput, htmlOutput, awdName.c_str(), backColor, fps, stageWidth, stageHeight, "%", "%", "%");
			std::string as_viewer_path;
			std::string swfObjectSourcePath;
			Utils::GetModuleFilePath(as_viewer_path, m_pCallback);
        
			std::string findThis="AwayExtensionsFlashPro.fcm.plugin/Contents/MacOS/";
			int foundIndex=as_viewer_path.find(findThis);
			if(foundIndex>0){
				as_viewer_path = as_viewer_path.substr(0, foundIndex);
			}
			swfObjectSourcePath = as_viewer_path + SWF_OBJECT_PATH;
			as_viewer_path += AS_VIEWER_NAME;

        
			AwayJS::Utils::CopyOneFile(as_viewer_path, m_target_asViewer_path, m_pCallback);
			AwayJS::Utils::CopyOneFile(swfObjectSourcePath, m_target_swfObject_path, m_pCallback);

			std::ofstream file;
			remove(m_outputHTMLFile.c_str());

			file.open(m_outputHTMLFile.c_str());
			file << m_HTMLOutput;
			file.close();
			//Utils::Trace(m_pCallback, "\nShould have Created HTML: %s !\n\n",m_outputHTMLFile.c_str());

			delete [] m_HTMLOutput;
		}
        return FCM_SUCCESS;
    }

    FCM::Result AWDOutputWriter::StartDocument(
        const DOM::Utils::COLOR& background,
        FCM::U_Int32 stageHeight, 
        FCM::U_Int32 stageWidth,
        FCM::U_Int32 fps)
    {
		
        return FCM_SUCCESS;
    }


    FCM::Result AWDOutputWriter::EndDocument()
    {
		
  	
        return FCM_SUCCESS;
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
        AWDTimelineWriter* pWriter = static_cast<AWDTimelineWriter*> (pTimelineWriter);
		if(pWriter!=NULL){
			pWriter->Finish(resId, pName);
		}
		
        return FCM_SUCCESS;
    }


    FCM::Result AWDOutputWriter::StartDefineShape()
    {
		shape_encoder = new ShapeEncoder(&m_pCallback, this->awd);
        std::string namestr="shapeName";
		AWDShape2D* newShape=new AWDShape2D(namestr);
		awd->add_shape2Dblock(newShape);
		shape_encoder->reset(newShape);
        return FCM_SUCCESS;
    }


    // Marks the end of a shape
    FCM::Result AWDOutputWriter::EndDefineShape(FCM::U_Int32 resId)
    {
        string objID_string=AwayJS::Utils::ToString(resId);
		AWDShape2D* shape=shape_encoder->get_shape();
		if(shape!=NULL){
			shape->set_objectID( objID_string);
			this->awd->add_shape2Dblock(shape);
		}	
		//delete shape_encoder;
        return FCM_SUCCESS;
    }
    // Marks the end of a shape
    FCM::Result AWDOutputWriter::EndDefineShapeLetter(AWDFontShape* fontShape)
    {
        /*
		
        string objID_string=AwayJS::Utils::ToString(100);
		AWDShape2D* shape=shape_encoder->get_shape();
		if(shape!=NULL){
			shape->set_objectID( objID_string);
			this->awd->add_shape2Dblock(shape);
		}	
		delete shape_encoder;
		*/
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
		//AwayJS::Utils::Trace(m_pCallback, "Alpha = %d\n",color.alpha);
		awd_color awdColor=awdutil_int_color( color.alpha, color.red, color.green, color.blue );
		AWDShape2DFill* newFill=awd->get_solid_fill(awdColor);
		newFill->set_color_components(color.red, color.green, color.blue, color.alpha);
		shape_encoder->get_shape()->set_fill(newFill);
		shape_encoder->set_color(color.red, color.green, color.blue, color.alpha);
        return FCM_SUCCESS;
    }

		
    // Bitmap fill style definition
    FCM::Result AWDOutputWriter::DefineText(
            FCM::U_Int32 resId, 
            DOM::FrameElement::PIClassicText pTextItem2)
    {
		
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
        
        return FCM_SUCCESS;
    }


    // Start Linear Gradient fill style definition
    FCM::Result AWDOutputWriter::StartDefineLinearGradientFillStyle(
        DOM::FillStyle::GradientSpread spread,
        const DOM::Utils::MATRIX2D& matrix)
    {

		shape_encoder->get_shape()->set_fill(awd->get_linear_gradient_fill());
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
		shape_encoder->get_shape()->set_fill(awd->get_radial_gradient_fill());
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
    FCM::Result AWDOutputWriter::SetAWDSegment(AWDPathSegment* segment)
    {
        /*
		if(segment->get_this_hole_idx()>0){
		}
		int hole_idx=shape_encoder->get_hole_idx();
		segment->set_hole_idx(hole_idx);
		shape_encoder->add_path_segment(segment);	
         */
        return FCM_SUCCESS;

	}
    
	// Sets a segment of a path (Used for boundary, holes)
    FCM::Result AWDOutputWriter::SetSegment(const DOM::Utils::SEGMENT& segment)
    {
	
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
		shape_encoder->add_hole();
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
        // No need to do anything
        return FCM_SUCCESS;
    }


    // Start of stroke 
    FCM::Result AWDOutputWriter::StartDefineStroke()
    {
		/*
        m_pathElem = new JSONNode(JSON_NODE);
        ASSERT(m_pathElem);

        m_pathCmdStr.clear();
        StartDefinePath();
		*/
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
        // No need to do anything
        return FCM_SUCCESS;
    }


    // End of fill style definition	
	// End of one AWDSubShape2D
    FCM::Result AWDOutputWriter::EndDefineFill()
    {
		// this calculates a acctual subgeometry
		//AwayJS::Utils::Trace(m_pCallback, "End define Shape\n");			
		shape_encoder->encode_subShape(false);
		//AwayJS::Utils::Trace(m_pCallback, "End define Shape2\n");
        return FCM_SUCCESS;
    }
	FCM::Result AWDOutputWriter::EndDefineFontStroke(AWDFontShape* fontShape)
    {
		// this calculates a acctual subgeometry
		//AwayJS::Utils::Trace(m_pCallback, "End define Shape3\n");
		shape_encoder->encode_subShape(false);
		
		AWDShape2D* shape=shape_encoder->get_shape();
		shape->set_delete_subs(false);
		if((shape->get_mergerSubShapes().size()>1)||(shape->get_mergerSubShapes().size()==0)){
			AwayJS::Utils::Trace(m_pCallback, "ERROR CONVERTING FONT!!\n");
			return FCM_SUCCESS;
		}
		shape->merge_subs();
		fontShape->set_subShape(shape->get_first_sub());
		delete shape;
		delete shape_encoder;

		
		//AwayJS::Utils::Trace(m_pCallback, "End define Shape4\n");
        return FCM_SUCCESS;
    }

	
    FCM::Result AWDOutputWriter::DefineSound(
            FCM::U_Int32 resId, 
			const std::string& name, 
            DOM::LibraryItem::PIMediaItem pMediaItem)
    {
		
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

        return FCM_SUCCESS;
    }


    AWDOutputWriter::AWDOutputWriter(FCM::PIFCMCallback pCallback)
        : m_pCallback(pCallback),
          m_firstSegment(false),
          m_HTMLOutput(NULL)
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
        m_firstSegment = true;

        return FCM_SUCCESS;
    }

    FCM::Result AWDOutputWriter::EndDefinePath()
    {
        // No need to do anything
        return FCM_SUCCESS;
    }
   /* -------------------------------------------------- AWDTimelineWriter */
	
	
	AWDShape2DTimeline* AWDTimelineWriter::get_timeline(){
		return this->thisTimeLine;
	}
	awd_float64 *
		AWDTimelineWriter::get_color_mtx_4x5(const DOM::Utils::COLOR_MATRIX* mtx)
	{	
		awd_float64* newMtx = (awd_float64*)malloc(20 * sizeof(awd_float64));
		if(mtx!=NULL){
			for(int i=0; i<10; i++){
				newMtx[i]  = mtx->colorArray[i];
			}
		}
		else{
			newMtx[0]  = 1.0; 
			newMtx[1]  = 0.0; 
			newMtx[2]  = 0.0;
			newMtx[3]  = 1.0;
			newMtx[4]  = 0.0; 
			newMtx[5]  = 0.0; 
			newMtx[6]  = 0.0; 
			newMtx[7]  = 0.0; 
			newMtx[8]  = 0.0; 
			newMtx[9]  = 0.0; 
			newMtx[10]  = 0.0; 
			newMtx[11]  = 0.0; 
			newMtx[12]  = 0.0; 
			newMtx[13]  = 0.0; 
			newMtx[14]  = 0.0; 
			newMtx[15]  = 0.0; 
			newMtx[16]  = 0.0; 
			newMtx[17]  = 0.0; 
			newMtx[18]  = 0.0; 
			newMtx[19]  = 0.0; 
		}
		return newMtx;
	} 
	awd_float64 *
	AWDTimelineWriter::get_mtx_2x3(const DOM::Utils::MATRIX2D* mtx)
	{	
		awd_float64* newMtx = (awd_float64*)malloc(6 * sizeof(awd_float64));
		if(mtx!=NULL){
			newMtx[0]  = mtx->a; 
			newMtx[1]  = mtx->b; 
			newMtx[2]  = mtx->c;
			newMtx[3]  = mtx->d;
			newMtx[4]  = mtx->tx*-1;
			newMtx[5]  = mtx->ty*-1;
		}
		else{
			newMtx[0]  = 1.0; 
			newMtx[1]  = 0.0; 
			newMtx[2]  = 0.0;
			newMtx[3]  = 1.0;
			newMtx[4]  = 0.0; 
			newMtx[5]  = 0.0; 
		}
		return newMtx;
	} 
    FCM::Result AWDTimelineWriter::PlaceObject(
        FCM::U_Int32 resId,
        FCM::U_Int32 objectId,
        FCM::U_Int32 placeAfterObjectId,
        const DOM::Utils::MATRIX2D* pMatrix,
        FCM::PIFCMUnknown pUnknown /* = NULL*/)
    {
		
		AWDFrameCommandDisplayObject* frameCommand=(AWDFrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		frameCommand->set_command_type(AWD_FRAME_COMMAND_UPDATE_OBJECT);
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
			frameCommand->set_display_matrix(get_mtx_2x3(pMatrix));
        }
		

        return FCM_SUCCESS;
    }
	
    FCM::Result AWDTimelineWriter::PlaceObject(
        FCM::U_Int32 resId,
        FCM::U_Int32 objectId,
        FCM::PIFCMUnknown pUnknown /* = NULL*/)
    {
        FCM::Result res=FCM_SUCCESS;
		
		AWDFrameCommandDisplayObject* frameCommand=(AWDFrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		frameCommand->set_command_type(AWD_FRAME_COMMAND_SOUND);
        string resID=AwayJS::Utils::ToString(resId);
		frameCommand->set_resID(resID);
		//Utils::Trace(m_pCallback, "		-> Placing a SoundObject into the timeline.\n");
		
		/*
        JSONNode commandElement(JSON_NODE);
        FCM::AutoPtr<DOM::FrameElement::ISound> pSound;

        commandElement.push_back(JSONNode("cmdType", "Place"));
        commandElement.push_back(JSONNode("charid", CreateJS::Utils::ToString(resId)));
        commandElement.push_back(JSONNode("objectId", CreateJS::Utils::ToString(objectId)));

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

            commandElement.push_back(JSONNode("loopMode", 
                CreateJS::Utils::ToString(lMode.loopMode)));
            commandElement.push_back(JSONNode("repeatCount", 
                CreateJS::Utils::ToString(lMode.repeatCount)));

            res = pSound->GetSyncMode(syncMode);
            ASSERT(FCM_SUCCESS_CODE(res));

            commandElement.push_back(JSONNode("syncMode", 
                CreateJS::Utils::ToString(syncMode)));

            // We should not get SOUND_SYNC_STOP as for stop, "RemoveObject" command will
            // be generated by Exporter Service.
            ASSERT(syncMode != DOM::FrameElement::SOUND_SYNC_STOP); 

            res = pSound->GetSoundLimit(soundLimit);
            ASSERT(FCM_SUCCESS_CODE(res));

            commandElement.push_back(JSONNode("LimitInPos44", 
                CreateJS::Utils::ToString(soundLimit.inPos44)));
            commandElement.push_back(JSONNode("LimitOutPos44", 
                CreateJS::Utils::ToString(soundLimit.outPos44)));

            // TODO: Dump sound effect
        }

        m_pCommandArray->push_back(commandElement);*/

        return res;
    }
    FCM::Result AWDTimelineWriter::RemoveObject(
        FCM::U_Int32 objectId)
    {
		
		AWDFrameCommandDisplayObject* frameCommand=(AWDFrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		frameCommand->set_command_type(AWD_FRAME_COMMAND_REMOVE_OBJECT);
		
        return FCM_SUCCESS;
    
	}
	


    FCM::Result AWDTimelineWriter::UpdateZOrder(
        FCM::U_Int32 objectId,
        FCM::U_Int32 placeAfterObjectId)
    {
		
		AWDFrameCommandDisplayObject* frameCommand=(AWDFrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		frameCommand->set_depth(placeAfterObjectId);
		
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


    FCM::Result AWDTimelineWriter::UpdateBlendMode(
        FCM::U_Int32 objectId,
        DOM::FrameElement::BlendMode blendMode)
    {
        // Not tested yet
		
		AWDFrameCommandDisplayObject* frameCommand=(AWDFrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		//TODO: convert blendmode into int before passing to awdlib
		//frameCommand->set_blendMode(blendMode);
		
        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::UpdateVisibility(
        FCM::U_Int32 objectId,
        FCM::Boolean visible)
    {
		
		AWDFrameCommandDisplayObject* frameCommand=(AWDFrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		frameCommand->set_visible(visible);
        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::AddGraphicFilter(
        FCM::U_Int32 objectId,
        FCM::PIFCMUnknown pFilter)
    {
		
		AWDFrameCommandDisplayObject* frameCommand=(AWDFrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		
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
			/*
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
			*/

        }
        if(pBlurFilter)
        {
            FCM::Boolean enabled;
            FCM::Double  blurX;
            FCM::Double  blurY;
            DOM::Utils::FilterQualityType qualityType;

			/*
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
				*/
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
			/*
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
			*/
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
			/*
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
				*/

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
			/*
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
			*/
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

			/*
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
			*/
        }

        if(pAdjustColorFilter)
        {
            FCM::Double brightness;
            FCM::Double contrast;
            FCM::Double saturation;
            FCM::Double hue;
            FCM::Boolean enabled;
			/*
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
			*/
        }

        //m_pCommandArray->push_back(commandElement);

        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::UpdateDisplayTransform(
        FCM::U_Int32 objectId,
        const DOM::Utils::MATRIX2D& matrix)
    {
		AWDFrameCommandDisplayObject* frameCommand=(AWDFrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		frameCommand->set_display_matrix(get_mtx_2x3(&matrix));

        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::UpdateColorTransform(
        FCM::U_Int32 objectId,
        const DOM::Utils::COLOR_MATRIX& colorMatrix)
    {
		AWDFrameCommandDisplayObject* frameCommand=(AWDFrameCommandDisplayObject*)thisTimeLine->get_frame()->get_command(objectId);
		frameCommand->set_color_matrix(get_color_mtx_4x5(&colorMatrix));
        // Not tested yet
        return FCM_SUCCESS;
    }


    FCM::Result AWDTimelineWriter::ShowFrame(FCM::U_Int32 frameNum)
    {
		AWDTimeLineFrame* newFrame=new AWDTimeLineFrame();
		newFrame->set_frame_duration(1000/awd->getExporterSettings()->get_fps());
		thisTimeLine->add_frame(newFrame, false);

        return FCM_SUCCESS;
    }

	
    FCM::Result AWDTimelineWriter::AddFrameScript(FCM::CStringRep16 pScript, FCM::U_Int32 layerNum)
    {
        std::string script = Utils::ToString(pScript, m_pCallback);

        std::string scriptWithLayerNumber = "script Layer" +  Utils::ToString(layerNum);

        std::string find = "\n";
        std::string replace = "\\n";
        std::string::size_type i =0;
        //JSONNode textElem(JSON_NODE);

        while (true) {
            /* Locate the substring to replace. */
            i = script.find(find, i);
           
            if (i == std::string::npos) break;
            /* Make the replacement. */
            script.replace(i, find.length(), replace);

            /* Advance index forward so the next iteration doesn't pick it up as well. */
            i += replace.length();
        }

        
        //Utils::Trace(m_pCallback, "[AddFrameScript] (Layer: %d): %s\n", layerNum, script.c_str());
		
		thisTimeLine->get_frame()->set_frame_code(script);

		//m_pFrameElement->push_back(JSONNode(scriptWithLayerNumber,script));

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
				thisTimeLine->get_frame()->add_label(AWD_FRAME_LABEL_NAME, label);
			else if(labelType == 2)//COMMENTS DONT SEAM TO GET EXPORTED BY IEXPORTERSERVICE
				thisTimeLine->get_frame()->add_label(AWD_FRAME_LABEL_COMMENT, label);
			else if(labelType == 3)
				thisTimeLine->get_frame()->add_label(AWD_FRAME_LABEL_ANCOR, label);
		}
        return FCM_SUCCESS;
    }

    AWDTimelineWriter::AWDTimelineWriter(AWD * awd, FCM::PIFCMCallback pcallback)
    {
	
		this->awd=awd;
		string name("");
		thisTimeLine=new AWDShape2DTimeline(name);		
		m_pCallback = pcallback;
		AWDTimeLineFrame* newFrame=new AWDTimeLineFrame();
		thisTimeLine->add_frame(newFrame, false);
    }


    AWDTimelineWriter::~AWDTimelineWriter()
    {
		/*
        delete m_pCommandArray;

        delete m_pFrameArray;

        delete m_pTimelineElement;
		*/
    }


	AWDShape2DTimeline* AWDTimelineWriter::GetRoot()
    {
        return thisTimeLine;
    }


    void AWDTimelineWriter::Finish(FCM::U_Int32 resId, FCM::StringRep16 pName)
    {
		string timeLineName("Scene");
        if (resId != 0)
        {
            string objID_string=AwayJS::Utils::ToString(resId);
			thisTimeLine->set_objectID(objID_string);
			thisTimeLine->set_scene_id(resId);
		}

		else{
			thisTimeLine->set_name(timeLineName);
			thisTimeLine->set_scene_id(resId);
			awd->add_root_scene(thisTimeLine);
			//Utils::Trace(m_pCallback, "ADDED THE ROOT SCENE TO AWD!\n");
		}
		if(pName!=NULL){
			
            timeLineName=AwayJS::Utils::ToString(pName, m_pCallback);
			thisTimeLine->set_name(timeLineName);
			//Utils::Trace(m_pCallback, "ADDED A CHILD TIMELINE NAME = %s\n", timeLineName.c_str());
		}
		
		awd->add_timelineBlock(thisTimeLine);
    }

};
