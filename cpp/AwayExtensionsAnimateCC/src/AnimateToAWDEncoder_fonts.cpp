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


 // converts a entire flash font-item found in the library into awd-font (genertes shapes for font)
 
AWD::BASE::AWDBlock* AnimateToAWDEncoder::ExportFont(DOM::LibraryItem::IFontItem* font, std::string name)
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
	//if(fontName)AwayJS::Utils::Trace(this->m_pCallback, "fontName = %s \n",AwayJS::Utils::ToString(fontName, this->m_pCallback).c_str());
	//if(name)AwayJS::Utils::Trace(this->m_pCallback, "name = %s \n",name.c_str());
	FCM::StringRep8 fontStyle;
	font->GetFontStyle(&fontStyle);
	//if(fontStyle)Utils::Trace(this->m_pCallback, "fontStyle = %s \n",Utils::ToString(Utils::ToString16(fontStyle, this->m_pCallback), this->m_pCallback).c_str());
			
	
	//not really used right now:
	FCM::StringRep16 embbedChars;
	font->GetEmbeddedCharacters(&embbedChars);
	//if(embbedChars)AwayJS::Utils::Trace(this->m_pCallback, "embbedChars size = %s \n",AwayJS::Utils::ToString(embbedChars, this->m_pCallback).c_str());				
	//if(embbedChars)
	//	this->pCalloc->Free((FCM::PVoid)embbedChars);

	FCM::StringRep16 embbedRanges;
	font->GetEmbeddedRanges(&embbedRanges);
	//if(embbedRanges)AwayJS::Utils::Trace(this->m_pCallback, "embbedRanges size = %s \n",AwayJS::Utils::ToString(embbedRanges, this->m_pCallback).c_str());
	//if(embbedRanges)
	//		this->pCalloc->Free((FCM::PVoid)embbedRanges);
		
				
	res=fontTableGenerator->CreateFontTableForFontItem(font, thisFontTable.m_Ptr);
	if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success create font table\n");}

	FCM::U_Int16 fontsize;
	res = thisFontTable->GetEMSquare(fontsize);
	if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success getting  fontsize\n");}	
	FCM::Double ascent;
	res = thisFontTable->GetAscent(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, ascent);
	if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success getting  ascent\n");}	
	FCM::Double descent;
	res = thisFontTable->GetDescent(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, descent);
	if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success getting  descent\n");}	
				
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
		//AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-1:%d\n", kerningPair.first);
		//AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-2:%d\n", kerningPair.second);
		//AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-Amount:%f\n", kerningPair.kernAmount);
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
	BLOCKS::Font* font_block=reinterpret_cast<BLOCKS::Font*>(this->awd_project->get_block_by_name_and_type(name, BLOCK::block_type::FONT, true));
	font_block->set_name(fontName2);
	font_block->add_scene_name(this->current_scene_name);
	//AwayJS::Utils::Trace(this->m_pCallback, "Created font block");
	if(font_block==NULL)
		return NULL;	

	FONT::FontStyle* awdFontStyle=font_block->get_font_style(fontStyle);
	awdFontStyle->set_style_size(fontsize);	
	awdFontStyle->set_whitespace_size(fontsize);	
	awdFontStyle->set_ascent(ascent);	
	awdFontStyle->set_descent(descent);	
    //AwayJS::Utils::Trace(this->m_pCallback, "font:%s\n", font_block->get_name().c_str());
    //AwayJS::Utils::Trace(this->m_pCallback, "fontsize:%d ascent:%f descent:%f\n", fontsize, ascent, descent);
	
	//Utils::Trace(this->m_pCallback, "glyph-count:%d\n", glyphCount);
	for (FCM::U_Int32 g = 0; g < glyphCount; g++)
	{
		//Utils::Trace(this->m_pCallback, "Read glyph:\n");					
        FCM::AutoPtr<DOM::Service::FontTable::IGlyph> thisGlyph = pGlyphs[g];
		DOM::FrameElement::PIShape glyphShape;
		thisGlyph->GetOutline(glyphShape);
		FCM::U_Int16 charCode;
		thisGlyph->GetCharCode(charCode);
		FCM::Double advance;
		thisGlyph->GetAdvance(advance);
		//AwayJS::Utils::Trace(this->m_pCallback, "Created advanced %f \n", advanced);
		//thisGlyph->GetAdvance
		if(charCode==32){
			awdFontStyle->set_whitespace_size(advance);
		}
		else{
			FONT::FontShape*  thisFontShape= awdFontStyle->get_fontShape(charCode);
			if(thisFontShape!=NULL){
				//Utils::Trace(this->m_pCallback, "Export shape for glyph with charCode = %d \n", charCode);
				if(glyphShape){
					if (glyphShape)
					{
						thisFontShape->advanceValue=advance;
						this->ExportStrokeForFont(glyphShape, thisFontShape);
					}
				}
			}
		}
	}
	return font_block;	
}	
 
 // finalize a exisiting awd-font-block. checks if any chars have been added, without generating shapes for them (when converting text-fields, we add chars without shapes).
 // if we include library-fonts in export, all fonts should already have been finalized.

AWD::result AnimateToAWDEncoder::FinalizeFonts(DOM::PIFLADocument pFlaDocument)
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
    
    //  at this point, we might have multiple fonts with the same font name.
    //  we use a map to group them by name.
    //  after this step, we have a list of unique named fonts.
    //  each font inthis list must contain all chars that have been assigned to any of the fonts sharing the same name
    
    std::vector<BLOCKS::Font*> export_fonts;
    std::map<std::string, BLOCKS::Font*> fontmap;
    for(AWDBlock* one_block : all_fonts){
        awd_font = reinterpret_cast<BLOCKS::Font *>(one_block);
        if(awd_font == NULL){
            AwayJS::Utils::Trace(this->m_pCallback, "error exporting font %s\n", awd_font->get_name().c_str());
            return result::AWD_ERROR;
        }
        if(fontmap.find(awd_font->get_name())==fontmap.end()){
            fontmap[awd_font->get_name()]=awd_font;
            export_fonts.push_back(awd_font);
        }
        else{
            // todo: collect all chars from this awd_font into the already existing one
            awd_font->delegate_to_font=fontmap[awd_font->get_name()];
			fontmap[awd_font->get_name()]->addFontCharsFromFont(awd_font);
        }
    }
	// loop over all awd Font-blocks, and check for each font-style if all chars have been generated already.
	for(BLOCKS::Font* one_font : export_fonts){

        std::vector<U_Int16> ungenerated_charcode;
        std::vector<FONT::FontStyle*> thisFontSTyles = one_font->get_font_styles();
        //Utils::Trace(this->m_pCallback, "Font = %s \n", font_shapes->get_name().c_str());
        //Utils::Trace(this->m_pCallback, "FontStyles count: = %d \n", thisShapes.size());
        for(FONT::FontStyle* thisfontStyle:thisFontSTyles){
            //Utils::Trace(this->m_pCallback, "Found a style = %s \n", thisfontStyle->get_style_name().c_str());
            std::vector<FONT::FontShape*> ungenerated=thisfontStyle->get_ungenerated_chars();
            ungenerated_charcode.clear();
            // ungenerated_chars contains all ungenerated chars. convert this chars to subgeometries:
            //if(ungenerated_charcode.size()>0){
                FCM::Result res;
				// create a fonttable for the chars A and space, so we always get the correct ascent, descent, em_size, space_width
            ungenerated_charcode.push_back(U_Int16(32));
            ungenerated_charcode.push_back(U_Int16(40));
				
                res=fontTableGenerator->CreateFontTable(pFlaDocument,  ungenerated_charcode.data(), AwayJS::Utils::ToString16(one_font->get_name(), this->m_pCallback),  thisfontStyle->get_style_name().c_str(), thisFontTable.m_Ptr);
                if(res != FCM_SUCCESS){
                    AwayJS::Utils::Trace(this->m_pCallback, "no success create font table\n");
                }
				
                FCM::U_Int16 fontsize;
                res = thisFontTable->GetEMSquare(fontsize);
                if(res != FCM_SUCCESS){
                    AwayJS::Utils::Trace(this->m_pCallback, "no success getting fontsize\n");
                }
				FCM::Double ascent;
				res = thisFontTable->GetAscent(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, ascent);
				if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success getting  ascent\n");}
				FCM::Double descent;
				res = thisFontTable->GetDescent(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, descent);
				if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success getting  descent\n");}
                thisfontStyle->set_style_size(fontsize);
				thisfontStyle->set_ascent(ascent);
				thisfontStyle->set_descent(descent);
                thisfontStyle->set_whitespace_size(fontsize);

            ungenerated_charcode.clear();
            for(FONT::FontShape* fontShape:ungenerated){
				//AwayJS::Utils::Trace(this->m_pCallback, "charCode:%d\n", 	U_Int16(fontShape->get_charCode()));
				ungenerated_charcode.push_back(U_Int16(fontShape->get_charCode()));
            }
                res=fontTableGenerator->CreateFontTable(pFlaDocument,  ungenerated_charcode.data(), AwayJS::Utils::ToString16(one_font->get_name(), this->m_pCallback),  thisfontStyle->get_style_name().c_str(), thisFontTable.m_Ptr);
                if(res != FCM_SUCCESS){
                    AwayJS::Utils::Trace(this->m_pCallback, "no success create font table\n");
                }
				
                /*
                FCM::U_Int16 fontsize;
                res = thisFontTable->GetEMSquare(fontsize);
                if(res != FCM_SUCCESS){
                    AwayJS::Utils::Trace(this->m_pCallback, "no success getting fontsize\n");
                }
				FCM::Double ascent;
				res = thisFontTable->GetAscent(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, ascent);
				if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success getting  ascent\n");}
				FCM::Double descent;
				res = thisFontTable->GetDescent(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, descent);
				if(res != FCM_SUCCESS){AwayJS::Utils::Trace(this->m_pCallback, "no success getting  descent\n");}
                thisfontStyle->set_style_size(fontsize);
				thisfontStyle->set_ascent(ascent);
				thisfontStyle->set_descent(descent);
                thisfontStyle->set_whitespace_size(fontsize);
                */
                FCM::FCMListPtr kerningPairs;
                thisFontTable->GetKerningPairs(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL,kerningPairs.m_Ptr);
                //AwayJS::Utils::Trace(this->m_pCallback, "font:%s\n", one_font->get_name().c_str());
                //AwayJS::Utils::Trace(this->m_pCallback, "fontsize:%d ascent:%f descent:%f\n", fontsize, ascent, descent);
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
                   // AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-1:%d\n", 	kerningPair.first);
                   // AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-2:%d\n", 	kerningPair.second);
                   // AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-A:%f\n", 	kerningPair.kernAmount);
                }

                FCM::FCMListPtr pGlyphs;
                res=thisFontTable->GetGlyphs(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, pGlyphs.m_Ptr);
                if(res != FCM_SUCCESS){
                    AwayJS::Utils::Trace(this->m_pCallback, "no success getting  glyphs\n");
                }
                FCM::U_Int32 glyphCount;
                res = pGlyphs->Count(glyphCount);
                if(res != FCM_SUCCESS){
                    AwayJS::Utils::Trace(this->m_pCallback, "no success getting  glyphs count\n");
                }		
                //Utils::Trace(this->m_pCallback, "glyph-count:%d\n", glyphCount);
                for (FCM::U_Int32 g = 0; g < glyphCount; g++)
                {
                
                    FCM::AutoPtr<DOM::Service::FontTable::IGlyph> thisGlyph = pGlyphs[g];
                    FCM::Double advance_value;
                    thisGlyph->GetAdvance(advance_value);
                    DOM::FrameElement::PIShape glyphShape;
                    thisGlyph->GetOutline(glyphShape);
                    FCM::U_Int16 charCode;
                    thisGlyph->GetCharCode(charCode);
                
                    if(charCode==32){
                        thisfontStyle->set_whitespace_size(advance_value);
                        thisfontStyle->delete_fontShape(32);
                    }
                    else{
                        FONT::FontShape*  thisFontShape= thisfontStyle->get_fontShape(charCode);
                        if(thisFontShape!=NULL){
                            if(glyphShape){
								thisFontShape->advanceValue=advance_value;
                                this->ExportStrokeForFont(glyphShape, thisFontShape);
                            }
                        }
                    }
                }
                // we check again if there are any ungenerated chars in the font-style.
                // if this happens, we delete the invalid chars and output a message.
                std::vector<FONT::FontShape*> ungenerated_end=thisfontStyle->get_ungenerated_chars();
                std::string thisString_end="";
                for(FONT::FontShape* fontShape:ungenerated_end){
                    U_Int16 curCode=fontShape->get_charCode();
                    thisfontStyle->delete_fontShape(curCode);
                    thisString_end+=curCode;
                }			
                if(thisString_end.size()>0){
                    AwayJS::Utils::Trace(this->m_pCallback, "ERROR - Found ungenerated chars after creating font: '%s' \n", thisString_end.c_str());
                }
        }
    }
    
    // the textformats are already connected to font-blocks, but after merging the fonts by name,
    // they might point to the wrong blocks
    // we update this here:
    
    // get list of collected text-formats:
	std::vector<AWDBlock*> all_formats;
	res_awd = this->awd_project->get_blocks_by_type(all_formats, BLOCK::block_type::TEXT_FORMAT);
	if(res_awd!=result::AWD_SUCCESS)
		return res_awd;
    
    // for each text-format make sure that it points to the correct font
	BLOCKS::Font * font_block;
	BLOCKS::TextFormat * awd_textformat;
	for(AWDBlock* one_block : all_formats){
		awd_textformat = reinterpret_cast<BLOCKS::TextFormat *>(one_block);
		if(awd_textformat == NULL)
			continue;
		font_block = reinterpret_cast<BLOCKS::Font *>(awd_textformat->get_font());
		if(font_block == NULL)
			continue;
		if(font_block->delegate_to_font!=NULL)
			awd_textformat->set_font(font_block->delegate_to_font);
	}
	return res_awd;
}
