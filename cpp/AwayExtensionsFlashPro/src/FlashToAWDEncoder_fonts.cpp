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

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG

AWD::result 
FlashToAWDEncoder::ExportText(DOM::FrameElement::IClassicText* classic_text, AWDBlock** awd_block)
{
	
	FCM::FCMListPtr pParagraphs;
	classic_text->GetParagraphs(pParagraphs.m_Ptr);
	FCM::U_Int32 pParagraphsCount;
	pParagraphs->Count(pParagraphsCount);
	DOM::FrameElement::AA_MODE_PROP aamode;
	classic_text->GetAntiAliasModeProp(aamode);
	FCM::StringRep16 text;
	classic_text->GetText(&text);
	DOM::FrameElement::PITextBehaviour textBehaviour;
	classic_text->GetTextBehaviour(textBehaviour);
	std::string extern_id="kfdsm";
	*awd_block = this->awd_project->get_block_by_external_id_and_type(extern_id, BLOCK::block_type::TEXT_ELEMENT, true);

	return result::AWD_SUCCESS;
}
/*
AWD::result FlashToAWDEncoder::ExportFont(DOM::LibraryItem::IFontItem* font, AWD::AWDProject* awd)
{
	AWD::result res_awd = result::AWD_ERROR;
		
	DOM::AutoPtr<DOM::Service::FontTable::IFontTable> thisFontTable;
    FCM::Result res;
	FCM::AutoPtr<FCM::IFCMUnknown> pUnk_fontTable;
	res = this->m_pCallback->GetService(DOM::FLA_FONTTABLE_GENERATOR_SERVICE, pUnk_fontTable.m_Ptr);
	ASSERT(FCM_SUCCESS_CODE(res));        
	FCM::AutoPtr<DOM::Service::FontTable::IFontTableGeneratorService> fontTableGenerator = pUnk_fontTable;	
	if(!fontTableGenerator){
		AwayJS::Utils::Trace(this->m_pCallback, "Could not generate IFontTable");
		return result::AWD_ERROR;
	}

	FCM::StringRep16 fontName;
	font->GetFontName(&fontName);
	//if(fontName)Utils::Trace(this->m_pCallback, "fontName = %s \n",Utils::ToString(fontName, this->m_pCallback).c_str());
	FCM::StringRep8 fontStyle;
	font->GetFontStyle(&fontStyle);
	//if(fontStyle)Utils::Trace(this->m_pCallback, "fontStyle = %s \n",Utils::ToString(Utils::ToString16(fontStyle, this->m_pCallback), this->m_pCallback).c_str());
				
	FCM::StringRep16 embbedChars;
	font->GetEmbeddedCharacters(&embbedChars);
	//if(embbedChars)Utils::Trace(this->m_pCallback, "embbedChars size = %s \n",Utils::ToString(embbedChars, this->m_pCallback).c_str());
				
	FCM::StringRep16 embbedRanges;
	font->GetEmbeddedRanges(&embbedRanges);
	//if(embbedRanges)Utils::Trace(this->m_pCallback, "embbedRanges size = %s \n",Utils::ToString(embbedRanges, this->m_pCallback).c_str());
				
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
	//Utils::Trace(this->m_pCallback, "kerningPairs-count:%d\n", kerningPairsCount);					
	for (FCM::U_Int32 g = 0; g < kerningPairsCount; g++)
	{
		AutoPtr< DOM::Service::FontTable::IKerningPair> kerningpair_getter=kerningPairs[g];
		DOM::Service::FontTable::KERNING_PAIR kerningPair;
		kerningpair_getter->GetKerningPair(kerningPair);
		AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-1:%d\n", 	kerningPair.first);
		AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-2:%d\n", 	kerningPair.second);
		AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-A:%f\n", 	kerningPair.kernAmount);
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
    std::string fontStyle2=fontStyle;

	BLOCKS::Font* font_block=reinterpret_cast<BLOCKS::Font*>(awd->get_block_by_external_id_and_type(fontName2, BLOCK::block_type::FONT, true));
	if(font_block==NULL)
		return result::AWD_ERROR;

	FONT::FontStyle* awdFontStyle=font_block->get_font_style(fontStyle2);
	awdFontStyle->set_style_size(fontsize);	
	//Utils::Trace(this->m_pCallback, "glyph-count:%d\n", glyphCount);
	for (FCM::U_Int32 g = 0; g < glyphCount; g++)
	{
		//Utils::Trace(this->m_pCallback, "Read glyph:\n");					
		AutoPtr<DOM::Service::FontTable::IGlyph> thisGlyph = pGlyphs[g];
		DOM::FrameElement::PIShape glyphShape;
		thisGlyph->GetOutline(glyphShape);
		FCM::U_Int16 charCode;
		thisGlyph->GetCharCode(charCode);

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
	return result::AWD_SUCCESS;	
}	


AWD::result FlashToAWDEncoder::FinalizeFonts(DOM::PIFLADocument pFlaDocument ,AWD::AWDProject* awd)
{
	DOM::AutoPtr<DOM::Service::FontTable::IFontTable> thisFontTable;
    FCM::Result res;
	FCM::AutoPtr<FCM::IFCMUnknown> pUnk_fontTable;
	res = this->m_pCallback->GetService(DOM::FLA_FONTTABLE_GENERATOR_SERVICE, pUnk_fontTable.m_Ptr);
	ASSERT(FCM_SUCCESS_CODE(res));        
	FCM::AutoPtr<DOM::Service::FontTable::IFontTableGeneratorService> fontTableGenerator = pUnk_fontTable;	
	if(!fontTableGenerator){
		AwayJS::Utils::Trace(this->m_pCallback, "Could not generate IFontTable");
		return result::AWD_ERROR;
	}
	BLOCKS::Font * awd_font;
	std::vector<AWDBlock*> all_fonts;
	AWD::result res_awd = awd->get_blocks_by_type(all_fonts, BLOCK::block_type::FONT);
	if(res_awd!=result::AWD_SUCCESS)
		return res_awd;

	// loop over all awd Font-blocks, and check if they need any more Fonts to be converted.

	for(AWDBlock* one_block : all_fonts){
		awd_font = reinterpret_cast<BLOCKS::Font *>(one_block);
		if(awd_font == NULL)
			return result::AWD_ERROR;

		vector<FONT::FontStyle*> thisShapes = awd_font->get_font_styles();
		//Utils::Trace(this->m_pCallback, "Font = %s \n", font_shapes->get_name().c_str());
		//Utils::Trace(this->m_pCallback, "FontStyles count: = %d \n", thisShapes.size());
		for(FONT::FontStyle* thisfontStyle:thisShapes){
			//Utils::Trace(this->m_pCallback, "Found a style = %s \n", thisfontStyle->get_style_name().c_str());
			vector<FONT::FontShape*> ungenerated=thisfontStyle->get_ungenerated_chars();
			int number_ungen=0;
			string thisString="";
			for(FONT::FontShape* fontShape:ungenerated){
				int curCode=fontShape->get_charCode();
				//Utils::Trace(this->m_pCallback, "ungenerated chars with code = %d \n", curCode);
				wchar_t c = static_cast<wchar_t>(curCode);
				char thisStr;
				wcstombs(&thisStr, &c, 1);
				string thisString_test="";
				thisString_test+=thisStr;
				int testcurCode=curCode;
				for(char& c : thisString_test) {
					testcurCode=c;
				}
				if(testcurCode!=curCode){
					AwayJS::Utils::Trace(this->m_pCallback, "Unsupported unicode char '%d' - converting to string and back changed the code to  = '%d' \n", curCode ,testcurCode);
					thisfontStyle->delete_fontShape(curCode);
				}
				else{
					thisString+=thisString_test;
				}
			}
				
			if(thisString.size()>0){
				//Utils::Trace(this->m_pCallback, "ungenerated chars = %s \n", thisString.c_str());
				FCM::Result res;
				res=fontTableGenerator->CreateFontTable(pFlaDocument, AwayJS::Utils::ToString16(thisString, this->m_pCallback), AwayJS::Utils::ToString16(awd_font->get_name(), this->m_pCallback),  thisfontStyle->get_style_name().c_str(), thisFontTable.m_Ptr);
				if(res != FCM_SUCCESS){
					AwayJS::Utils::Trace(this->m_pCallback, "no success create font table\n");
				}

				FCM::U_Int16 fontsize;
				res = thisFontTable->GetEMSquare(fontsize);
				if(res != FCM_SUCCESS){
					AwayJS::Utils::Trace(this->m_pCallback, "no success getting  fontsize\n");
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
					AutoPtr< DOM::Service::FontTable::IKerningPair> kerningpair_getter=kerningPairs[g];
					DOM::Service::FontTable::KERNING_PAIR kerningPair;
					kerningpair_getter->GetKerningPair(kerningPair);
					AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-1:%d\n", 	kerningPair.first);
					AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-2:%d\n", 	kerningPair.second);
					AwayJS::Utils::Trace(this->m_pCallback, "kerningPairs-A:%f\n", 	kerningPair.kernAmount);
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
					
					AutoPtr<DOM::Service::FontTable::IGlyph> thisGlyph = pGlyphs[g];
					DOM::FrameElement::PIShape glyphShape;
					thisGlyph->GetOutline(glyphShape);
					FCM::U_Int16 charCode;
					thisGlyph->GetCharCode(charCode);

					FONT::FontShape*  thisFontShape= thisfontStyle->get_fontShape(charCode);
					if(thisFontShape!=NULL){
						if(glyphShape){
							if (glyphShape)
							{
								this->ExportStrokeForFont(glyphShape, thisFontShape);
							}
						}
					}
				}
				vector<FONT::FontShape*> ungenerated_end=thisfontStyle->get_ungenerated_chars();
				int number_ungen=0;
				string thisString_end="";
				for(FONT::FontShape* fontShape:ungenerated_end){
					int curCode=fontShape->get_charCode();
					wchar_t c = static_cast<wchar_t>(curCode);
					//Utils::Trace(this->m_pCallback, "Found ungenerated char with code = %d \n", sizeof(c));
					char thisStr;
					wcstombs(&thisStr, &c, 1);
					string thisString_test="";
					thisString_test+=thisStr;
					int testcurCode=curCode;
					for(char& c : thisString_test) {
						testcurCode=c;
					}
					if(testcurCode!=curCode){
						AwayJS::Utils::Trace(this->m_pCallback, "Unsupported unicode char '%d' - converting to string and back changed the code to  = '%d' \n", curCode ,testcurCode);
						thisfontStyle->delete_fontShape(curCode);
					}
					else{
						thisString_end+=thisString_test;
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
*/