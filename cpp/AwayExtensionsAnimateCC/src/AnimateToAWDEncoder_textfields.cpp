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

#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"

#include "Exporter/Service/ISWFExportService.h"
#include "Application/Service/IOutputConsoleService.h"

#include "AnimateToAWDEncoder.h"
#include "FCMPluginInterface.h"

#include "Publisher.h"
#include "Utils/DOMTypes.h"
#include <codecvt>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "utf8.h"
#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG
#include <climits>


AWD::result 
AnimateToAWDEncoder::ExportText(DOM::FrameElement::IClassicText* classic_text, AWDBlock** awd_block, const std::string ressource_id)
{
	FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
	FCM::Result res = this->m_pCallback->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
    FCM::AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

	FCM::FCMListPtr pParagraphs;
	classic_text->GetParagraphs(pParagraphs.m_Ptr);
	FCM::U_Int32 pParagraphsCount;
    pParagraphs->Count(pParagraphsCount);
    
	DOM::FrameElement::AA_MODE_PROP aamode;
	classic_text->GetAntiAliasModeProp(aamode);
    // possible AA : ANTI_ALIAS_MODE_DEVICE, ANTI_ALIAS_MODE_BITMAP, ANTI_ALIAS_MODE_STANDARD, ANTI_ALIAS_MODE_ADVANCED,  ANTI_ALIAS_MODE_CUSTOM

	FCM::AutoPtr<FCM::IFCMCalloc> pCalloc = AwayJS::Utils::GetCallocService(this->m_pCallback);
    ASSERT(pCalloc.m_Ptr != NULL);

	FCM::StringRep16 text;
	classic_text->GetText(&text);
    

    std::string text_field_text = AwayJS::Utils::ToString(text, this->m_pCallback);
    //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    
    //std::wstring text_field_text2 = (wchar_t*)text;//converter.from_bytes(text_field_text.c_str());
#ifdef _WINDOWS
    std::wstring text_field_text2 = (wchar_t*)text;//converter.from_bytes(text_field_text.c_str());
	//if(text)
	//	pCalloc->Free((FCM::PVoid)text);
#endif

    BLOCKS::TextElement* thistext_block = reinterpret_cast<BLOCKS::TextElement*>(this->awd_project->get_block_by_external_id_and_type(ressource_id, BLOCK::block_type::TEXT_ELEMENT, true));
    thistext_block->set_text(text_field_text);
	thistext_block->add_res_id(ressource_id);
	DOM::FrameElement::PITextBehaviour textBehaviour;
	classic_text->GetTextBehaviour(textBehaviour);

	FCM::Boolean is_selectable;
    textBehaviour->IsSelectable(is_selectable);
	thistext_block->set_is_selectable(is_selectable);

	FCM::AutoPtr<DOM::FrameElement::IStaticTextBehaviour> staticTextBehaviour = textBehaviour;
    if(staticTextBehaviour){
		DOM::FrameElement::TextFlow this_textflow;
		staticTextBehaviour->GetFlow(this_textflow);
		thistext_block->set_textflow((FONT::TextFlow)this_textflow);
		
		DOM::FrameElement::OrientationMode this_orient_mode;
		staticTextBehaviour->GetOrientationMode(this_orient_mode);
		thistext_block->set_orientationMode((FONT::OrientationMode)this_orient_mode);
    }
    else{
        FCM::AutoPtr<DOM::FrameElement::IModifiableTextBehaviour> modifiyableTextBehaviour = textBehaviour;
        if(modifiyableTextBehaviour){
			
			thistext_block->set_tf_type(FONT::Textfield_type::DYNAMIC);

			FCM::StringRep16 textfield_name;
			modifiyableTextBehaviour->GetInstanceName(&textfield_name);
			std::string textfield_name_str=AwayJS::Utils::ToString(textfield_name, this->m_pCallback);
			if(textfield_name)
				pCalloc->Free((FCM::PVoid)textfield_name);
			thistext_block->set_name(textfield_name_str);			

			DOM::FrameElement::LineMode this_lineMode;
			modifiyableTextBehaviour->GetLineMode(this_lineMode);
			thistext_block->set_line_mode((FONT::LineMode)this_lineMode);

			FCM::Boolean render_as_html;
            modifiyableTextBehaviour->GetRenderAsHtml(render_as_html);
			thistext_block->set_render_as_html(render_as_html);

			FCM::Boolean is_border_drawn;
            modifiyableTextBehaviour->IsBorderDrawn(is_border_drawn);
			thistext_block->set_is_border_drawn(is_border_drawn);

			FCM::Boolean is_scrollable;
            modifiyableTextBehaviour->IsScrollable(is_scrollable);
			thistext_block->set_is_scrollable(is_scrollable);


            FCM::AutoPtr<DOM::FrameElement::IInputTextBehaviour> inputTextBehaviour = modifiyableTextBehaviour;
            if(inputTextBehaviour){
				thistext_block->set_tf_type(FONT::Textfield_type::INPUT);
				FCM::Boolean is_password;
				inputTextBehaviour->IsLineModePassword(is_password);
				if(is_password)
					thistext_block->set_tf_type(FONT::Textfield_type::INPUT_PSW);
            }
        }
        else{
            // this is a error (textbehaviour should be either static or modifiyable)
        }
        
    }

    for(int i = 0; i<pParagraphsCount; i++)
    {
        FONT::Paragraph* this_awd_paragraph = new FONT::Paragraph();
        FCM::AutoPtr<DOM::FrameElement::IParagraph> paragraph = pParagraphs[i];
        FCM::FCMListPtr pTextruns;
        paragraph->GetTextRuns(pTextruns.m_Ptr);

		DOM::FrameElement::PARAGRAPH_STYLE paragraphstyle;
		paragraphstyle.structSize=sizeof(DOM::FrameElement::PARAGRAPH_STYLE);
		paragraph->GetParagraphStyle(paragraphstyle);
		std::string paragraph_props="";
		if(paragraphstyle.alignment==DOM::FrameElement::AlignMode::ALIGN_MODE_CENTER)
			paragraph_props+="center";
		if(paragraphstyle.alignment==DOM::FrameElement::AlignMode::ALIGN_MODE_JUSTIFY)
			paragraph_props+="justify";
		if(paragraphstyle.alignment==DOM::FrameElement::AlignMode::ALIGN_MODE_LEFT)
			paragraph_props+="left";
		if(paragraphstyle.alignment==DOM::FrameElement::AlignMode::ALIGN_MODE_RIGHT)
			paragraph_props+="right";
		
                
        /** Paragraph indentation. Legal value is between -720 to 720. */
		paragraph_props += "/i:"+std::to_string(paragraphstyle.indent);

        /** Line spacing of paragraph (also known as leading). Legal value is between -360 to 720. */
		paragraph_props += "/ls:"+std::to_string(paragraphstyle.lineSpacing);
                
        /** Paragraph's left margin. Legal value is between 0 to 720. */
		paragraph_props += "/lm:"+std::to_string(paragraphstyle.leftMargin);
		
        /** Paragraph's right margin. Legal value is between 0 to 720. */
		paragraph_props += "/rm:"+std::to_string(paragraphstyle.rightMargin);
                
        FCM::U_Int32 textrunCount;
        pTextruns->Count(textrunCount);
        for(int t = 0; t<textrunCount; t++)
        {
            FONT::TextRun* awd_textrun = new FONT::TextRun();
            FCM::AutoPtr<DOM::FrameElement::ITextRun> textrun = pTextruns[t];

			// get textrun properties.
            FCM::U_Int32 startIdx;
            textrun->GetStartIndex(startIdx);
            FCM::U_Int32 txt_length;
            textrun->GetLength(txt_length);

			// get text for this textrun
            std::string text_run_text = text_field_text.substr (startIdx, txt_length);
            //std::wstring text_run_text2 = text_field_text2.substr (startIdx, txt_length);

#ifdef _WINDOWS
			std::wstring text_run_text2 = text_field_text2.substr (startIdx, txt_length);
#endif
				

            // get text-style for this text-run and convert to TextFormat-Block
            FCM::AutoPtr<DOM::FrameElement::ITextStyle> text_style;
            textrun->GetTextStyle(text_style.m_Ptr);
                
            FCM::StringRep16 font_name;
            text_style->GetFontName(&font_name);
            std::string font_name_str = AwayJS::Utils::ToString(font_name, this->m_pCallback); 
            std::string str2="*";
            std::size_t found = font_name_str.find(str2);
            if (found!=std::string::npos)
                font_name_str.replace(font_name_str.find(str2),str2.length(),"");
         
            FCM::StringRep8 font_style;
            text_style->GetFontStyle(&font_style);
            // string build from all properties is used as identifier for all text-format (cache)
            std::string text_format_string = paragraph_props + "#" + font_name_str+"#"+font_style;
                
            FCM::U_Int16 font_size;
            text_style->GetFontSize(font_size);
            text_format_string +="#"+AwayJS::Utils::ToString(font_size);

            DOM::Utils::COLOR font_color;
            text_style->GetFontColor(font_color);
            text_format_string +="#"+AwayJS::Utils::ToString(font_color);

            FCM::S_Int16 letter_spaceing;
            text_style->GetLetterSpacing(letter_spaceing);
            text_format_string +="#"+AwayJS::Utils::ToString(letter_spaceing);

            FCM::Boolean is_rotated;
            text_style->IsRotated(is_rotated);
            text_format_string +="#"+AwayJS::Utils::ToString(is_rotated);

            FCM::Boolean auto_kernable;
            text_style->IsAutoKernEnabled(auto_kernable);
            text_format_string +="#"+AwayJS::Utils::ToString(auto_kernable);
                
            DOM::FrameElement::BaseLineShiftStyle base_line_shift_style;
            text_style->GetBaseLineShiftStyle(base_line_shift_style);
            text_format_string +="#"+AwayJS::Utils::ToString(base_line_shift_style);
                
            // this is not used yet: if a link is set, the textrun should be handled as a url link - pointing to the given link
            // the link target is _blank / _parent / _self / _top
            // i think this might be more of a text-run property than a text-format.    

            FCM::StringRep16 this_link;
            text_style->GetLink(&this_link);
            FCM::StringRep16 this_link_target;
            text_style->GetLinkTarget(&this_link_target);
			
            //AwayJS::Utils::Trace(this->m_pCallback, "text_format_string = %s\n", text_format_string.c_str());
            BLOCKS::TextFormat* this_text_format = reinterpret_cast<BLOCKS::TextFormat*>(this->awd_project->get_block_by_external_id_and_type(text_format_string, BLOCK::block_type::TEXT_FORMAT, true));
            this_text_format->set_name("text_format_"+std::to_string(this->text_fomat_cnt));
            this_text_format->add_scene_name(this->current_scene_name);
            this->text_fomat_cnt++;
            // convert the color-components to a uint32 color.
            TYPES::COLOR awdColor = TYPES::create_color_from_ints( font_color.alpha, font_color.red, font_color.green, font_color.blue );

            // get the material-block for this color (create if does not exist)
            BLOCKS::Material* new_fill_material=this->awd_project->get_default_material_by_color(awdColor, true, MATERIAL::type::SOLID_COLOR_MATERIAL);
            new_fill_material->set_alpha(double(font_color.alpha)/double(255));
        
            // set color-components for color.
            new_fill_material->set_color_components(font_color.red, font_color.green, font_color.blue, font_color.alpha);

            this_text_format->set_fontStyle(font_style);
            this_text_format->set_fontSize(font_size);
            this_text_format->set_fill_material(new_fill_material);
            this_text_format->set_letterSpacing(letter_spaceing);
            this_text_format->set_is_rotated(is_rotated);
            this_text_format->set_autokerning(auto_kernable);
            this_text_format->set_baseLineShift(FONT::baselineshift_type(base_line_shift_style));
                    
            this_text_format->set_alignment(FONT::AlignMode(paragraphstyle.alignment));
            this_text_format->set_indent(paragraphstyle.indent);
            this_text_format->set_linespacing(paragraphstyle.lineSpacing);
            this_text_format->set_leftmargin(paragraphstyle.leftMargin);
            this_text_format->set_rightmargin(paragraphstyle.rightMargin);


            awd_textrun->set_format(this_text_format);
                
            BLOCKS::Font* this_font = reinterpret_cast<BLOCKS::Font*>(this->awd_project->get_block_by_name_and_type(font_name_str, BLOCK::block_type::FONT, true));
            this_font->set_name(font_name_str);
            this_font->add_scene_name(this->current_scene_name);
            FONT::FontStyle* this_font_style = this_font->get_font_style(font_style);
				
			if(text_run_text.size()>0){
				
#ifdef _WINDOWS
                
				std::wstring wstr2=L"\r";
				std::size_t found_linebreak = text_run_text2.find(wstr2);
				while (found_linebreak!=std::string::npos){
					text_run_text2.replace(text_run_text2.find(wstr2),wstr2.length(),L"\\n");
					found_linebreak = text_run_text2.find(wstr2);
				}
                
				string utf8line; 
				utf8::utf16to8(text_run_text2.begin(), text_run_text2.end(), back_inserter(utf8line));
				string::iterator end_it = utf8::find_invalid(utf8line.begin(), utf8line.end());
				if (end_it != utf8line.end()) {
					AwayJS::Utils::Trace(m_pCallback, "Invalid UTF-8 encoding detected at lines\n");
				}
			
				
				if(this->awd_project->get_settings()->get_bool(SETTINGS::bool_settings::EmbbedAllChars)){
					for(U_Int16 c : text_run_text2) {
                        this_font_style->get_fontShape(c);
					}
				}
				awd_textrun->set_text(utf8line);
#endif
                
		
#ifdef __APPLE__
                
                // stupid way to collect the correct portion of the utf16 string into a U_int16 vector
                // chars with code 13 (\r) will be replaced with a escaped linebreak (\\n)
				std::vector<U_Int16> chars_as_uint16;
				if(this->awd_project->get_settings()->get_bool(SETTINGS::bool_settings::EmbbedAllChars)){
					int t2=0;
					for(t2=startIdx; t2<(startIdx+txt_length); t2++){
                        if(text[t2]==13){
                            chars_as_uint16.push_back(92);
                            chars_as_uint16.push_back(110);
                        }
                        else{
                            chars_as_uint16.push_back(text[t2]);
                            this_font_style->get_fontShape(text[t2]);
                        }
					}
				}
                // convert the utf16 string to utf8
                FCM::AutoPtr<FCM::IFCMStringUtils> pIFCMStringUtils = AwayJS::Utils::GetStringUtilsService(m_pCallback);
                FCM::StringRep8 outstr;
                pIFCMStringUtils->ConvertStringRep16to8(chars_as_uint16.data(), outstr);
                awd_textrun->set_text(outstr);
#endif
			}
                
            this_text_format->set_font(this_font);
            this_awd_paragraph->add_textrun(awd_textrun);
            if(font_name)
                pCalloc->Free((FCM::PVoid)font_name);
            if(font_style)
                pCalloc->Free((FCM::PVoid)font_style);
            if(this_link)
                pCalloc->Free((FCM::PVoid)this_link);
            if(this_link_target)
				pCalloc->Free((FCM::PVoid)this_link_target);
            
        }
        thistext_block->add_paragraph(this_awd_paragraph);
    }
    *awd_block=thistext_block;
	return result::AWD_SUCCESS;
}
