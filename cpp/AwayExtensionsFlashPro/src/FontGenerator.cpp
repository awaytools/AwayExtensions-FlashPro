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

	
	
    void CPublisher::GenerateFonts(DOM::PIFLADocument pFlaDocument, ResourcePalette* pResPalette)
	{
		/*
		FONTS:

		// use this website, to check wich charCode corresponds to which char 
		http://unicode-table.com/en/

		Whenever all font that we created while encoding the text-fields, we convert chars to shapes.
		If we 

		- step 1: collect all fonts into one list. make it optional if we want to include only fonts from librar

		*/
		
		// translate previous collected text-elements into shapes
		
        FCM::Result res;
		FCM::AutoPtr<FCM::IFCMUnknown> pUnk_fontTable;
		res = GetCallback()->GetService(DOM::FLA_FONTTABLE_GENERATOR_SERVICE, pUnk_fontTable.m_Ptr);
		ASSERT(FCM_SUCCESS_CODE(res));        
		FCM::AutoPtr<DOM::Service::FontTable::IFontTableGeneratorService> fontTableGenerator = pUnk_fontTable;	
		if(fontTableGenerator){
			Utils::Trace(GetCallback(), "IFontTablegenerator created\n");
		}
		//Read all embed fonts
		FCM::FCMListPtr pLibraryItems;
		res = pFlaDocument->GetLibraryItems(pLibraryItems.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));
        FCM::U_Int32 libCnt;
        res = pLibraryItems->Count(libCnt);
        for (FCM::U_Int32 l = 0; l < libCnt; l++)
        
		{
			DOM::AutoPtr<DOM::ILibraryItem> pFontItem = pLibraryItems[l];
			DOM::AutoPtr<DOM::LibraryItem::IFontItem> pFontItem2 = pFontItem;
			if(pFontItem2){

				//Utils::Trace(GetCallback(), "Read a font\n");
				//fontTableGenerator->CreateFontTable(
				FCM::StringRep16 fontName;
				pFontItem2->GetFontName(&fontName);
				if(fontName)
					Utils::Trace(GetCallback(), "fontName = %s \n",Utils::ToString(fontName, GetCallback()).c_str());
				FCM::StringRep8 fontStyle;
				pFontItem2->GetFontStyle(&fontStyle);
				if(fontStyle)
					Utils::Trace(GetCallback(), "fontStyle = %s \n",Utils::ToString(Utils::ToString16(fontStyle, GetCallback()), GetCallback()).c_str());
				
				FCM::StringRep16 embbedChars;
				pFontItem2->GetEmbeddedCharacters(&embbedChars);
				if(fontName)
					Utils::Trace(GetCallback(), "embbedChars size = %s \n",Utils::ToString(embbedChars, GetCallback()).c_str());
				
				FCM::StringRep16 embbedRanges;
				pFontItem2->GetEmbeddedRanges(&embbedRanges);
				if(fontName)
					Utils::Trace(GetCallback(), "embbedRanges size = %s \n",Utils::ToString(embbedRanges, GetCallback()).c_str());
				
				DOM::AutoPtr<DOM::Service::FontTable::IFontTable> thisFontTable;
				std::string thisString ("test");
				std::string str = thisString;
				for(char& c : str) {
					//Utils::Trace(GetCallback(), "get charCode = %d \n", c);
				}
				//res=fontTableGenerator->CreateFontTable(pFlaDocument, Utils::ToString16(thisString, GetCallback()), fontName, fontStyle, thisFontTable.m_Ptr);

				res=fontTableGenerator->CreateFontTableForFontItem(pFontItem2, thisFontTable.m_Ptr);
				if(res != FCM_SUCCESS){
					Utils::Trace(GetCallback(), "no success create font table\n");
				}

				FCM::U_Int16 fontsize;
				res = thisFontTable->GetEMSquare(fontsize);
				if(res != FCM_SUCCESS){
					Utils::Trace(GetCallback(), "no success getting  fontsize\n");
				}	
				//Utils::Trace(GetCallback(), "fontsize:%d\n", fontsize);	

				FCM::FCMListPtr kerningPairs;
				thisFontTable->GetKerningPairs(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL,kerningPairs.m_Ptr);
				FCM::U_Int32 kerningPairsCount;
				res = kerningPairs->Count(kerningPairsCount);
				if(res != FCM_SUCCESS){
					Utils::Trace(GetCallback(), "no success getting  kerningPairsCount\n");
				}	
				//Utils::Trace(GetCallback(), "kerningPairs-count:%d\n", kerningPairsCount);	

				FCM::FCMListPtr pGlyphs;
				res=thisFontTable->GetGlyphs(DOM::FrameElement::ORIENTATION_MODE_HORIZONTAL, pGlyphs.m_Ptr);
				if(res != FCM_SUCCESS){
					Utils::Trace(GetCallback(), "no success getting  glyths\n");
				}
				FCM::U_Int32 glyphCount;
				res = pGlyphs->Count(glyphCount);
				if(res != FCM_SUCCESS){
					Utils::Trace(GetCallback(), "no success getting  glyths count\n");
				}				
				//Utils::Trace(GetCallback(), "glyph-count:%d\n", glyphCount);
				for (FCM::U_Int32 g = 0; g < glyphCount; g++)
				{
					//Utils::Trace(GetCallback(), "Read glyph:\n");
					
					AutoPtr<DOM::Service::FontTable::IGlyph> thisGlyph = pGlyphs[g];
					DOM::FrameElement::PIShape glyphShape;
					thisGlyph->GetOutline(glyphShape);
					FCM::U_Int16 charCode;
					thisGlyph->GetCharCode(charCode);
					//Utils::Trace(GetCallback(), "Export shape for glyph with charCode = %d \n", charCode);
					if(glyphShape){
						if (glyphShape)
						{
							pResPalette->ExportStrokeForFont(glyphShape);
						}
					}					
				}
			}
		}	        
    }
};
