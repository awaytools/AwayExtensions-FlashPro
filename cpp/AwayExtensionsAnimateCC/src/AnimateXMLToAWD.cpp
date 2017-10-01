#ifdef _WINDOWS
#include <Windows.h>
#include "ShellApi.h"
#endif
#include <vector>
#include <fstream>
#include "Utils.h"

#include "ApplicationFCMPublicIDs.h"
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

#include "AnimateXMLToAWD.h"
#include "FCMPluginInterface.h"

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>
#include "xmlParser.h"

void myfree(char *t); // {free(t);}
ToXMLStringTool tx,tx2;
#include <locale>
#include <codecvt>
#include <string>

#ifdef _DEBUG
	#include <stdlib.h>
	#include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG


AnimateXMLToAWD::AnimateXMLToAWD(FCM::PIFCMCallback pCallback, AWDProject* awd_project, DOM::PIFLADocument pFlaDocument)
{
	
	this->m_pCallback=pCallback;
    FCM::Result res;
	FCM::StringRep16 path;
	res = pFlaDocument->GetPath(&path);
	std::string path_str=AwayJS::Utils::ToString(path, this->m_pCallback);
	ASSERT(FCM_SUCCESS_CODE(res));
	std::string root_directory;
	result res2 = FILES::extract_directory_from_path(path_str, root_directory);
	
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(root_directory.c_str())+L"DOMDocument.xml";
	XMLNode xMainNode=XMLNode::openFileHelper(wide.c_str(),L"DOMDocument");
	XMLNode xNode=xMainNode.getChildNode(L"symbols");
	int n=xNode.nChildNode(L"Include");
	FCM::U_Int32 i=0;
	int myIterator=0;
	for (i=0; i<n; i++){
		std::wstring wide3=xNode.getChildNode(L"Include", &myIterator).getAttribute(L"href");
		std::string narrow = converter.to_bytes(wide3);
		std::wstring symbolName = converter.from_bytes(root_directory.c_str())+L"LIBRARY\\"+wide3;
		preparseSymbol(symbolName);
	}
}

AnimateXMLToAWD::~AnimateXMLToAWD()
{

}
 

AWD::result AnimateXMLToAWD::preparseSymbol(std::wstring symbolName)
{
	XMLNode symbolNode=XMLNode::openFileHelper(symbolName.c_str(),L"DOMSymbolItem");
	AwayJS::Utils::Trace(this->m_pCallback, "    href %d\n", symbolNode.nChildNode(L"timeline"));
	return result::AWD_SUCCESS;
}