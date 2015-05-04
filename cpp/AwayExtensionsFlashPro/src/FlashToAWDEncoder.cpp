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


FlashToAWDEncoder::FlashToAWDEncoder(FCM::PIFCMCallback pCallback, AWDProject* awd_project)
{
	this->text_field_cnt=0;
	this->text_fomat_cnt=0;
	this->shape_instance_cnt=0;
	this->grafik_cnt=0;
	this->mat_cnt=0;
	this->geom_cnt=0;
    this->m_pCallback=pCallback;
	this->awd_project=awd_project;
	this->current_geom=NULL;
	FCM::AutoPtr<FCM::IFCMUnknown> pUnkShapeService;
	this->m_pCallback->GetService(DOM::FLA_SHAPE_SERVICE,pUnkShapeService.m_Ptr);
	pIShapeService=pUnkShapeService.m_Ptr;
	FCM::AutoPtr<FCM::IFCMUnknown> pUnkSRVReg;
	this->m_pCallback->GetService(DOM::FLA_REGION_GENERATOR_SERVICE,pUnkSRVReg.m_Ptr);
	pIRegionGeneratorService=pUnkSRVReg.m_Ptr;
	
}

FlashToAWDEncoder::~FlashToAWDEncoder()
{
}

AWDProject*
FlashToAWDEncoder::get_project(){
	return this->awd_project;
}

TYPES::F64* 
FlashToAWDEncoder::convert_matrix2x3(DOM::Utils::MATRIX2D matrix2x3)
{	
	int flipx=1;
	if(this->awd_project->get_settings()->get_flipXaxis())
		flipx=-1;
	int flipy=1;
	if(this->awd_project->get_settings()->get_flipYaxis())
		flipy=-1;
	TYPES::F64* output = (TYPES::F64* )malloc(6 * sizeof(TYPES::F64));
	output[0]=matrix2x3.a;
	output[1]=matrix2x3.b;
	output[2]=matrix2x3.c;
	output[3]=matrix2x3.d;
	output[4]=matrix2x3.tx*flipx;
	output[5]=matrix2x3.ty*flipy;
	return output;
}

TYPES::F64* 
FlashToAWDEncoder::convert_matrix4x5(DOM::Utils::COLOR_MATRIX color_mtx)
{
	TYPES::F64* output = (TYPES::F64* )malloc(8*sizeof(TYPES::F64));
	int i = 0;
	//for(i = 0; i < 20; i++)
	//	output[i]=(TYPES::F64)color_mtx.colorArray[i];
	output[0]=color_mtx.colorArray[0];// red multiply
	output[1]=color_mtx.colorArray[4];// red off
	output[2]=color_mtx.colorArray[6];// green multiply
	output[3]=color_mtx.colorArray[9];// green off
	output[4]=color_mtx.colorArray[12];// blue multiply
	output[5]=color_mtx.colorArray[14];// blue off
	output[6]=color_mtx.colorArray[18];// alpha multiply
	output[7]=color_mtx.colorArray[19];// alpha multiy
	return output;
}