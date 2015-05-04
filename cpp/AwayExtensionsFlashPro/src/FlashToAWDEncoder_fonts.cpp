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

#include "FlashToAWDEncoder.h"
#include "FCMPluginInterface.h"

#include "Publisher.h"
#include "Utils/DOMTypes.h"

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG

AWD::result 
FlashToAWDEncoder::ExportText(DOM::FrameElement::IClassicText* classic_text, AWDBlock** awd_block, const std::string ressource_id)
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
	if(text)
		pCalloc->Free((FCM::PVoid)text);

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
		paragraph_props += "/"+std::to_string(paragraphstyle.indent);

        /** Line spacing of paragraph (also known as leading). Legal value is between -360 to 720. */
		paragraph_props += "/"+std::to_string(paragraphstyle.lineSpacing);
                
        /** Paragraph's left margin. Legal value is between 0 to 720. */
		paragraph_props += "/"+std::to_string(paragraphstyle.leftMargin);
		
        /** Paragraph's right margin. Legal value is between 0 to 720. */
		paragraph_props += "/"+std::to_string(paragraphstyle.rightMargin);
                
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
			
			// get text-style for this text-run and convert to TextFormat-Block
            FCM::AutoPtr<DOM::FrameElement::ITextStyle> text_style;
            textrun->GetTextStyle(text_style.m_Ptr);
			
            FCM::StringRep16 font_name;
            text_style->GetFontName(&font_name);
            std::string font_name_str = AwayJS::Utils::ToString(font_name, this->m_pCallback);      

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
                      
                        
            // todo: finish text_format_string
            
            BLOCKS::TextFormat* this_text_format = reinterpret_cast<BLOCKS::TextFormat*>(this->awd_project->get_block_by_external_id_and_type(text_format_string, BLOCK::block_type::FORMAT, true));
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
			//AwayJS::Utils::Trace(m_pCallback, "text  = %s \n", text_run_text.c_str());
			for(char& c : text_run_text) {
				AwayJS::Utils::Trace(m_pCallback, "Converted char to int %d \n", c);
				if(int(c)<0)
					AwayJS::Utils::Trace(m_pCallback, "Found Unsupported Font-char  = %d \n", c);
				else
					this_font_style->get_fontShape(c);
            }
			awd_textrun->set_text(text_run_text);
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

 // converts a entire flash font-item found in the library into awd-font (genertes shapes for font)
 
AWD::BASE::AWDBlock* FlashToAWDEncoder::ExportFont(DOM::LibraryItem::IFontItem* font)
{
	
	FCM::AutoPtr<FCM::IFCMCalloc> pCalloc = AwayJS::Utils::GetCallocService(this->m_pCallback);
    ASSERT(pCalloc.m_Ptr != NULL);

	DOM::AutoPtr<DOM::Service::FontTable::IFontTable> thisFontTable;
    FCM::Result res;
	FCM::AutoPtr<FCM::IFCMUnknown> pUnk_fontTable;
	res = this->m_pCallback->GetService(DOM::FLA_FONTTABLE_GENERATOR_SERVICE, pUnk_fontTable.m_Ptr);
	ASSERT(FCM_SUCCESS_CODE(res));        
	FCM::AutoPtr<DOM::Service::FontTable::IFontTableGeneratorService> fontTableGenerator = pUnk_fontTable;	
	if(!fontTableGenerator){
		AwayJS::Utils::Trace(this->m_pCallback, "Could not generate IFontTable");
		return NULL;
	}

	FCM::StringRep16 fontName;
	font->GetFontName(&fontName);
	//if(fontName)Utils::Trace(this->m_pCallback, "fontName = %s \n",Utils::ToString(fontName, this->m_pCallback).c_str());
	FCM::StringRep8 fontStyle;
	font->GetFontStyle(&fontStyle);
	//if(fontStyle)Utils::Trace(this->m_pCallback, "fontStyle = %s \n",Utils::ToString(Utils::ToString16(fontStyle, this->m_pCallback), this->m_pCallback).c_str());
			
	/*
	not really used right now:
	FCM::StringRep16 embbedChars;
	font->GetEmbeddedCharacters(&embbedChars);
	//if(embbedChars)Utils::Trace(this->m_pCallback, "embbedChars size = %s \n",Utils::ToString(embbedChars, this->m_pCallback).c_str());				
	if(embbedChars)
		this->pCalloc->Free((FCM::PVoid)embbedChars);

	FCM::StringRep16 embbedRanges;
	font->GetEmbeddedRanges(&embbedRanges);
	//if(embbedRanges)Utils::Trace(this->m_pCallback, "embbedRanges size = %s \n",Utils::ToString(embbedRanges, this->m_pCallback).c_str());
	if(embbedRanges)
			this->pCalloc->Free((FCM::PVoid)embbedRanges);
		*/
				
	res=fontTableGenerator->CreateFontTableForFontItem(font, thisFontTable.m_Ptr);
	if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success create font table\n");}

	FCM::U_Int16 fontsize;
	res = thisFontTable->GetEMSquare(fontsize);
	if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success getting  fontsize\n");}	
				
	//Utils::Trace(this->m_pCallback, "fontsize:%d\n", fontsize);

	FCM::FCMListPtr kerningPairs;
	thisFontTable->GetKerningPairs(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL,kerningPairs.m_Ptr);
	FCM::U_Int32 kerningPairsCount;
	res = kerningPairs->Count(kerningPairsCount);
	if(res != FCM_SUCCESS){
		AwayJS::Utils::Trace(this->m_pCallback, "no success getting  kerningPairsCount\n");
	}	
	//AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-count:%d\n", kerningPairsCount);					
	for (FCM::U_Int32 g = 0; g < kerningPairsCount; g++)
	{
        FCM::AutoPtr< DOM::Service::FontTable::IKerningPair> kerningpair_getter=kerningPairs[g];
		DOM::Service::FontTable::KERNING_PAIR kerningPair;
		kerningpair_getter->GetKerningPair(kerningPair);
		/*AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-1:%d\n", kerningPair.first);
		AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-2:%d\n", kerningPair.second);
		AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-Amount:%f\n", kerningPair.kernAmount);*/
	}

	FCM::FCMListPtr pGlyphs;
	res=thisFontTable->GetGlyphs(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, pGlyphs.m_Ptr);
	if(res != FCM_SUCCESS){
		AwayJS::Utils::Trace(this->m_pCallback, "no success getting  glyths\n");
	}
	FCM::U_Int32 glyphCount;
	res = pGlyphs->Count(glyphCount);
	if(res != FCM_SUCCESS){
		AwayJS::Utils::Trace(this->m_pCallback, "no success getting  glyths count\n");
	}

    std::string fontName2=AwayJS::Utils::ToString(fontName, this->m_pCallback);
	if(fontName)
		pCalloc->Free((FCM::PVoid)fontName);
	BLOCKS::Font* font_block=reinterpret_cast<BLOCKS::Font*>(this->awd_project->get_block_by_name_and_type(fontName2, BLOCK::block_type::FONT, true));
	font_block->set_name(fontName2);
	font_block->add_scene_name(this->current_scene_name);
	//AwayJS::Utils::Trace(this->m_pCallback, "Created font block");
	if(font_block==NULL)
		return NULL;	

	FONT::FontStyle* awdFontStyle=font_block->get_font_style(fontStyle);
	awdFontStyle->set_style_size(fontsize);	
	//Utils::Trace(this->m_pCallback, "glyph-count:%d\n", glyphCount);
	for (FCM::U_Int32 g = 0; g < glyphCount; g++)
	{
		//Utils::Trace(this->m_pCallback, "Read glyph:\n");					
        FCM::AutoPtr<DOM::Service::FontTable::IGlyph> thisGlyph = pGlyphs[g];
		DOM::FrameElement::PIShape glyphShape;
		thisGlyph->GetOutline(glyphShape);
		FCM::U_Int16 charCode;
		thisGlyph->GetCharCode(charCode);
		//AwayJS::Utils::Trace(this->m_pCallback, "Created font %d \n", charCode);
		//thisGlyph->GetAdvance
		FONT::FontShape*  thisFontShape= awdFontStyle->get_fontShape(charCode);
		if(thisFontShape!=NULL){
			//Utils::Trace(this->m_pCallback, "Export shape for glyph with charCode = %d \n", charCode);
			if(glyphShape){
				if (glyphShape)
				{
					this->ExportStrokeForFont(glyphShape, thisFontShape);
				}
			}
		}
	}
	return font_block;	
}	
 
 // finalize a exisiting awd-font-block. checks if any chars have been added, without generating shapes for them (when converting text-fields, we add chars without shapes).
 // if we include library-fonts in export, all fonts should already have been finalized.


AWD::result FlashToAWDEncoder::FinalizeFonts(DOM::PIFLADocument pFlaDocument)
{
	
    FCM::Result res;

	// get a font-table generator:
	DOM::AutoPtr<DOM::Service::FontTable::IFontTable> thisFontTable;
	FCM::AutoPtr<FCM::IFCMUnknown> pUnk_fontTable;
	res = this->m_pCallback->GetService(DOM::FLA_FONTTABLE_GENERATOR_SERVICE, pUnk_fontTable.m_Ptr);
	ASSERT(FCM_SUCCESS_CODE(res));        
	FCM::AutoPtr<DOM::Service::FontTable::IFontTableGeneratorService> fontTableGenerator = pUnk_fontTable;	
	if(!fontTableGenerator){
		AwayJS::Utils::Trace(this->m_pCallback, "Could not generate IFontTable");
		return result::AWD_ERROR;
	}

	// get list of font-blocks from awd-project
	BLOCKS::Font * awd_font;
	std::vector<AWDBlock*> all_fonts;
	AWD::result res_awd = this->awd_project->get_blocks_by_type(all_fonts, BLOCK::block_type::FONT);
	if(res_awd!=result::AWD_SUCCESS)
		return res_awd;

	// loop over all awd Font-blocks, and check for each font-style if all chars have been generated already.
	for(AWDBlock* one_block : all_fonts){
		awd_font = reinterpret_cast<BLOCKS::Font *>(one_block);
		if(awd_font == NULL)
			return result::AWD_ERROR;

        std::vector<FONT::FontStyle*> thisShapes = awd_font->get_font_styles();
		//Utils::Trace(this->m_pCallback, "Font = %s \n", font_shapes->get_name().c_str());
		//Utils::Trace(this->m_pCallback, "FontStyles count: = %d \n", thisShapes.size());
		for(FONT::FontStyle* thisfontStyle:thisShapes){
			//Utils::Trace(this->m_pCallback, "Found a style = %s \n", thisfontStyle->get_style_name().c_str());
            std::vector<FONT::FontShape*> ungenerated=thisfontStyle->get_ungenerated_chars();
            std::string ungenerated_chars="";
			for(FONT::FontShape* fontShape:ungenerated){
				int curCode=fontShape->get_charCode();
                std::string ungenerated_chars_test="";
#ifdef _WINDOWS
				//Utils::Trace(this->m_pCallback, "ungenerated chars with code = %d \n", curCode);
				wchar_t c = static_cast<wchar_t>(curCode);
				char thisStr;
				wcstombs(&thisStr, &c, 1);
                ungenerated_chars_test+=thisStr;
                
#endif
                
#ifdef __APPLE__
                wchar_t c = static_cast<wchar_t>(curCode);
                ungenerated_chars_test+=c;
#endif
				int testcurCode=curCode;
				for(char& c : ungenerated_chars_test) {
					testcurCode=c;
				}
				if(testcurCode!=curCode){
					AwayJS::Utils::Trace(this->m_pCallback, "Unsupported unicode char '%d' - converting to string and back changed the code to  = '%d' \n", curCode ,testcurCode);
					thisfontStyle->delete_fontShape(curCode);
				}
				else{
					ungenerated_chars+=ungenerated_chars_test;
				}
			}
			// thisString contains all ungenerated chars. convert this chars to subgeometries:
			if(ungenerated_chars.size()>0){
				AwayJS::Utils::Trace(this->m_pCallback, "ungenerated chars = %s \n", ungenerated_chars.c_str());
				FCM::Result res;
				res=fontTableGenerator->CreateFontTable(pFlaDocument, AwayJS::Utils::ToString16(ungenerated_chars, this->m_pCallback), AwayJS::Utils::ToString16(awd_font->get_name(), this->m_pCallback),  thisfontStyle->get_style_name().c_str(), thisFontTable.m_Ptr);
				if(res != FCM_SUCCESS){
					AwayJS::Utils::Trace(this->m_pCallback, "no success create font table\n");
				}

				FCM::U_Int16 fontsize;
				res = thisFontTable->GetEMSquare(fontsize);
				if(res != FCM_SUCCESS){
					AwayJS::Utils::Trace(this->m_pCallback, "no success getting fontsize\n");
				}	
				//Utils::Trace(this->m_pCallback, "fontsize:%d\n", fontsize);	
				thisfontStyle->set_style_size(fontsize);
				FCM::FCMListPtr kerningPairs;
				thisFontTable->GetKerningPairs(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL,kerningPairs.m_Ptr);
				FCM::U_Int32 kerningPairsCount;
				res = kerningPairs->Count(kerningPairsCount);
				if(res != FCM_SUCCESS){
					AwayJS::Utils::Trace(this->m_pCallback, "no success getting  kerningPairsCount\n");
				}
				//Utils::Trace(this->m_pCallback, "kerningPairs-count:%d\n", kerningPairsCount);
				for (FCM::U_Int32 g = 0; g < kerningPairsCount; g++)
				{
                    FCM::AutoPtr< DOM::Service::FontTable::IKerningPair> kerningpair_getter=kerningPairs[g];
					DOM::Service::FontTable::KERNING_PAIR kerningPair;
					kerningpair_getter->GetKerningPair(kerningPair);
					/*AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-1:%d\n", 	kerningPair.first);
					AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-2:%d\n", 	kerningPair.second);
					AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-A:%f\n", 	kerningPair.kernAmount);*/
				}

				FCM::FCMListPtr pGlyphs;
				res=thisFontTable->GetGlyphs(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, pGlyphs.m_Ptr);
				if(res != FCM_SUCCESS){
					AwayJS::Utils::Trace(this->m_pCallback, "no success getting  glyths\n");
				}
				FCM::U_Int32 glyphCount;
				res = pGlyphs->Count(glyphCount);
				if(res != FCM_SUCCESS){
					AwayJS::Utils::Trace(this->m_pCallback, "no success getting  glyths count\n");
				}		
				//Utils::Trace(this->m_pCallback, "glyph-count:%d\n", glyphCount);
				for (FCM::U_Int32 g = 0; g < glyphCount; g++)
				{
					
                    FCM::AutoPtr<DOM::Service::FontTable::IGlyph> thisGlyph = pGlyphs[g];
					DOM::FrameElement::PIShape glyphShape;
					thisGlyph->GetOutline(glyphShape);
					FCM::U_Int16 charCode;
					thisGlyph->GetCharCode(charCode);

					FONT::FontShape*  thisFontShape= thisfontStyle->get_fontShape(charCode);
					if(thisFontShape!=NULL){
						if(glyphShape){
							this->ExportStrokeForFont(glyphShape, thisFontShape);
						}
					}
				}

				// we check again if there are any ungenerated chars in the font-style.
				// if this happens, we delete the invalid chars and output a message.
                std::vector<FONT::FontShape*> ungenerated_end=thisfontStyle->get_ungenerated_chars();
                std::string thisString_end="";
				for(FONT::FontShape* fontShape:ungenerated_end){
					int curCode=fontShape->get_charCode();
                    std::string ungenerated_chars_test="";
#ifdef _WINDOWS
                    wchar_t c = static_cast<wchar_t>(curCode);
                    char thisStr;
                    wcstombs(&thisStr, &c, 1);
                    ungenerated_chars_test+=thisStr;
                    
#endif
                    
#ifdef __APPLE__
                    wchar_t c = static_cast<wchar_t>(curCode);
                    ungenerated_chars_test+=c;
#endif
					int testcurCode=curCode;
					for(char& c : ungenerated_chars_test) {
						testcurCode=c;
					}
					if(testcurCode!=curCode){
						AwayJS::Utils::Trace(this->m_pCallback, "Unsupported unicode char '%d' - converting to string and back changed the code to  = '%d' \n", curCode ,testcurCode);
						thisfontStyle->delete_fontShape(curCode);
					}
					else{
						thisString_end+=ungenerated_chars_test;
					}
				}			
				if(thisString_end.size()>0){
					AwayJS::Utils::Trace(this->m_pCallback, "Still found ungenerated chars after conversation = %s \n", thisString_end.c_str());
				}
			}
		}
    }
	return res_awd;
}
