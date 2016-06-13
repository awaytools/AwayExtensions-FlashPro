#ifndef ANIMATEXMLTOAWD_H_
#define ANIMATEXMLTOAWD_H_

#include <vector>
#include <string>

#include "Utils.h"
#include "Utils/DOMTypes.h"
#include "FlashFCMPublicIDs.h"

#include "FrameElement/IClassicText.h"
#include "FrameElement/IShape.h"
#include "FrameElement/IBitmapInstance.h"
#include "FrameElement/ISound.h"

#include "MediaInfo/ISoundInfo.h"
#include "MediaInfo/IBitmapInfo.h"

#include "LibraryItem/IMediaItem.h"
#include "LibraryItem/IFolderItem.h"
#include "LibraryItem/IFontItem.h"
#include "LibraryItem/ISymbolItem.h"
#include "ILibraryItem.h"


#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"

#include "Service/Shape/IFilledRegion.h"
#include "Service/Shape/IStrokeGroup.h"
#include "Service/Shape/IPath.h"
#include "Service/Shape/IEdge.h"
#include "Service/Shape/IRegionGeneratorService.h"
#include "Service/Shape/IShapeService.h"
#include "IFLADocument.h"
#include "awd_project.h"

using namespace AWD;

/**
This class provides all the functions needed to encode flash-objects into AWD-Data. 
The only Flash-Object that has its dedicaded class is the Timeline. It needed its own Clas
*/

class AnimateXMLToAWD
{

    private:
		
		// pointer to awd-project needed to add blocks to awd-project
		AWDProject* awd_project;
		
		// needed to communicate with flash
		FCM::PIFCMCallback m_pCallback;
		
		AWD::result preparseSymbol(std::wstring smybolName);

		
		
	public:

        AnimateXMLToAWD(FCM::PIFCMCallback pCallback, AWDProject* awd_project, DOM::PIFLADocument);
        ~AnimateXMLToAWD();		 

};

#endif