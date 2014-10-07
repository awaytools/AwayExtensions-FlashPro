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

    /* ----------------------------------------------------- CPublisher */
	
    CPublisher::CPublisher()
    {
    }

    CPublisher::~CPublisher()
    {

    }
	

    FCM::Result CPublisher::Publish(
        DOM::PIFLADocument pFlaDocument, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
        return Export(pFlaDocument, NULL, NULL, pDictPublishSettings, pDictConfig);
    }

    // This function will be currently called in "Test-Scene" workflow. 
    // In future, it might be called in other workflows as well. 
    FCM::Result CPublisher::Publish(
        DOM::PIFLADocument pFlaDocument, 
        DOM::PITimeline pTimeline, 
        const Exporter::Service::RANGE &frameRange, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {
        return Export(pFlaDocument, pTimeline, &frameRange, pDictPublishSettings, pDictConfig);
    }
	

    FCM::Result CPublisher::Export(
        DOM::PIFLADocument pFlaDocument, 
        DOM::PITimeline pTimeline, 
        const Exporter::Service::RANGE* pFrameRange, 
        const PIFCMDictionary pDictPublishSettings, 
        const PIFCMDictionary pDictConfig)
    {

		

        std::string outFile;
        FCM::Result res;
        FCM::AutoPtr<FCM::IFCMUnknown> pUnk;
        FCM::AutoPtr<FCM::IFCMCalloc> pCalloc;

        Init();
		
        pCalloc = AwayJS::Utils::GetCallocService(GetCallback());
        ASSERT(pCalloc.m_Ptr != NULL);
		
        Utils::Trace(GetCallback(), "Publishing begins for AwayJS-document (AWD export)\n");

			
        res = GetOutputFileName(pFlaDocument, pTimeline, pDictPublishSettings, outFile);
        if (FCM_FAILURE_CODE(res))
        {
            // FLA is untitled. Ideally, we should use a temporary location for output generation.
            // However, for now, we report an error.
            Utils::Trace(GetCallback(), "Failed to publish. Either save the FLA or provide output path in publish settings.\n");
            return res;
        }

        Utils::Trace(GetCallback(), "Creating output file : %s\n", outFile.c_str());
        Utils::Trace(GetCallback(), "Encoding data....\n");

		// INIT THE AWD LIB:

		// Create blockSettings to pass to AWD.
		BlockSettings * blocksettings=new BlockSettings(false,false,false,false,1.0);
		bool externalTex=false;	
		int flags = 0;
		//bit1 = streaming
		if (blocksettings->get_wide_matrix())//bit2 = storagePrecision Matrix
			flags |= 0x02;
		if (blocksettings->get_wide_geom())//bit3 = storagePrecision Geo
			flags |= 0x04;
		if (blocksettings->get_wide_props())//bit4 = storagePrecision Props
			flags |= 0x08;
		if (blocksettings->get_wide_attributes())//bit5 = storagePrecision attributes
			flags |= 0x10;
		//if (opts->ExportNormals())//bit6 = storeNormals
		//	flags |= 0x20;
		//bit7 = storeTangents //flags |= 0x40;
		if (externalTex)//bit8 = embbedTetures
			flags |= 0x80;			


		// create awd-object
		AWD * awd = new AWD(blocksettings, outFile);	
		
		
		//generate and add the AWDMetaDataBlock to the awd-object
        AWDMetaData *newMetaData=new AWDMetaData();
		std::string thisName ("Adobe Flash Professional CC Prerelease");
		std::string thisVersion = std::to_string(2015);
		newMetaData->set_generator_metadata(thisName, thisVersion);        
        awd->set_metadata(newMetaData);



        DOM::Utils::COLOR color;
        FCM::U_Int32 stageWidth;
        FCM::U_Int32 stageHeight;
        FCM::Double fps;
        FCM::U_Int32 framesPerSec;
        AutoPtr<ITimelineBuilderFactory> pTimelineBuilderFactory;

        // Create a output writer
		std::auto_ptr<IOutputWriter> pOutputWriter(new AWDOutputWriter(GetCallback()));
        if (pOutputWriter.get() == NULL)
        {
            return FCM_MEM_NOT_AVAILABLE;
        }
		//set the awd object as property of outputWriter, so we can access it everywhere outputwriter is available.
		pOutputWriter->set_awd(awd);
		//pOutputWriter->set_doc(&pFlaDocument);       
		
		std::string openPreview_str;
		bool openPreview=false;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.OpenPreview", openPreview_str);
		if(openPreview_str=="true"){
			openPreview=true;
			//Utils::Trace(GetCallback(), "Creating openPreview_str : %s\n", openPreview_str.c_str());
		}
		//double outline_threshold=0.0;
		// set the shape-encoder properties...
        pOutputWriter->StartOutput(outFile);
		std::string outline_threshold_s;
        ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.OutlineThreshold", outline_threshold_s);
		double outline_threshold=0.0;
		if(!outline_threshold_s.empty())
			outline_threshold=std::stod(outline_threshold_s);

		std::string intersection_precision_s;
        bool double_subdivide=false;
        bool save_interstect=true;
        //Utils::Trace(GetCallback(), "Creating output file : %s\n", outline_threshold_s.c_str());
		ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.IntersectionsPrecision", intersection_precision_s);
		if(intersection_precision_s=="SaveSingle"){
			//Utils::Trace(GetCallback(), "\nIntersectiontestMode = SaveSingle !\n");
		}
		else if(intersection_precision_s=="SaveDouble"){
			//Utils::Trace(GetCallback(), "\nIntersectiontestMode = SaveDouble !\n");
			double_subdivide=true;
		}		
		else if(intersection_precision_s=="Fast"){
			//Utils::Trace(GetCallback(), "\nIntersectiontestMode = Fast !\n");
			save_interstect=false;
			double_subdivide=true;
		}
		pOutputWriter->set_save_interstect(save_interstect);
		pOutputWriter->set_double_subdivide(double_subdivide);
		pOutputWriter->set_outline_threshold(outline_threshold);

        // Create a Timeline Builder Factory for the root timeline of the document
        res = GetCallback()->CreateInstance(
            NULL, 
            CLSID_TimelineBuilderFactory, 
            IID_ITimelineBuilderFactory, 
            (void**)&pTimelineBuilderFactory);
        if (FCM_FAILURE_CODE(res))
        {
            return res;
        }

        (static_cast<TimelineBuilderFactory*>(pTimelineBuilderFactory.m_Ptr))->Init(
            pOutputWriter.get());

        ResourcePalette* pResPalette = static_cast<ResourcePalette*>(m_pResourcePalette.m_Ptr);
        pResPalette->Clear();
        pResPalette->Init(pOutputWriter.get());
						
        // Get all the timelines for the document
        FCM::FCMListPtr pTimelineList;
        res = pFlaDocument->GetTimelines(pTimelineList.m_Ptr);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pFlaDocument->GetBackgroundColor(color);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pFlaDocument->GetStageHeight(stageHeight);
        ASSERT(FCM_SUCCESS_CODE(res));

        res = pFlaDocument->GetStageWidth(stageWidth);
        ASSERT(FCM_SUCCESS_CODE(res));
		
        res = pFlaDocument->GetFrameRate(fps);
        ASSERT(FCM_SUCCESS_CODE(res));
		
        framesPerSec = (FCM::U_Int32)fps;

        res = pOutputWriter->StartDocument(color, stageHeight, stageWidth, framesPerSec);
        ASSERT(FCM_SUCCESS_CODE(res));
		


        FCM::U_Int32 timelineCount;
        res = pTimelineList->Count(timelineCount);
        if (FCM_FAILURE_CODE(res))
        {
            return res;
        }
		

        // Generate frame commands for each timeline
        for (FCM::U_Int32 i = 0; i < timelineCount; i++)
        {
			
            Exporter::Service::RANGE range;
            AutoPtr<ITimelineBuilder> pTimelineBuilder;
            ITimelineWriter* pTimelineWriter;

            AutoPtr<DOM::ITimeline> timeline = pTimelineList[i];
			if(pTimeline!=NULL){
				timeline = pTimeline;
				Utils::Trace(GetCallback(), "\nExport only the current active TimeLine !\n\n");
			}
            range.min = 0;
            res = timeline->GetMaxFrameCount(range.max);
            if (FCM_FAILURE_CODE(res))
            {
                return res;
            }

			FCM::FCMListPtr pLayerList;
			timeline->GetLayers(pLayerList.m_Ptr);
			
			FCM::U_Int32 layerCount;
			res = pLayerList->Count(layerCount);
			//Utils::Trace(GetCallback(), "Count of all layers: %d\n", layerCount);



			for (FCM::U_Int32 l = 0; l < timelineCount; l++)
			{
			}


			range.max--;
            // Generate frame commands
		
            res = m_frameCmdGeneratorService->GenerateFrameCommands(
                    timeline, 
                    range, 
                    pDictPublishSettings,
                    m_pResourcePalette, 
                    pTimelineBuilderFactory, 
                    pTimelineBuilder.m_Ptr);

            if (FCM_FAILURE_CODE(res))
            {
                return res;
            }

            ((TimelineBuilder*)pTimelineBuilder.m_Ptr)->Build(0, NULL, &pTimelineWriter);
			
			
        }		

        //GenerateFonts(pFlaDocument, pResPalette);
		if(openPreview){
			res = pOutputWriter->EndDocument();
			ASSERT(FCM_SUCCESS_CODE(res));
		}

        res = pOutputWriter->EndOutput();
        ASSERT(FCM_SUCCESS_CODE(res));
		
		AWDShape2D *block;
		AWDBlockIterator it( awd->get_shape2D_blocks());
		while ((block = (AWDShape2D*)it.next()) != NULL) {
			//Utils::Trace(GetCallback(), "Convert bshape %d.\n", block->get_mergerSubShapes().size());
			for(AWDMergedSubShape* mergSubShape : block->get_mergerSubShapes())
			{
				AWDShape2DFill* testFill=(AWDShape2DFill*)mergSubShape->get_fill();
				if(testFill!=NULL){
					mergSubShape->setColor(testFill->get_color_red(), testFill->get_color_green(), testFill->get_color_blue(), testFill->get_color_alpha());
				}
				else{
					mergSubShape->setColor(0.5, 0.5, 0.5, 1.0);
				}				
				//Utils::Trace(GetCallback(), "Convert bshape %d.\n", mergSubShape->getAllSubsTris().size());
				for(vector<int> faceList : mergSubShape->getAllSubsTris())
				{
					//Utils::Trace(GetCallback(), "Convert FaceList %d.\n", faceList.size());
				}
			}
			
			block->merge_subs();
		}
		awd->flush();
		Utils::Trace(GetCallback(), "The AWD-file: '%s' should have been created.\n", outFile.c_str());
		//std::string outputFile;
        //ReadString(pDictPublishSettings, (FCM::StringRep8)"PublishSettings.OpenURL", outputFile);
		
        std::string parent;
        std::string awdName;
        std::string openFile;
		if(openPreview){
			Utils::GetParent(outFile, parent);
			Utils::GetFileNameWithoutExtension(outFile, awdName);
			openFile = parent + awdName + ".html";
			Utils::Trace(GetCallback(), "The AWD-file: '%s' should have been created.\n", openFile.c_str());
			LaunchBrowser(openFile);
		}
		
        FCM::AutoPtr<FCM::IFCMUnknown> pUnkCalloc;
        res = GetCallback()->GetService(SRVCID_Core_Memory, pUnkCalloc.m_Ptr);
        AutoPtr<FCM::IFCMCalloc> callocService  = pUnkCalloc;

		/*
		AWDShape2DFill * block2;
		AWDBlockIterator it3(awd->get_shapeFill_blocks());		
		while ((block2 = (AWDShape2DFill*)it3.next()) != NULL) {

			callocService->Free((FCM::PVoid)block2);
		}
		AWDShape2D * block3;
		AWDBlockIterator it4(awd->get_shape2D_blocks());		
		while ((block3 = (AWDShape2D*)it4.next()) != NULL) {
			
				AWDSubShape2D *sub;
				// Write all sub-meshes
				sub = block3->get_first_sub();
				while (sub) {
					for(int strCnt=0; strCnt<sub->get_num_streams(); strCnt++){
						AWDDataStream* thisStream=sub->get_stream_at(strCnt);
						
						callocService->Free((FCM::PVoid)thisStream->data.v);
					}
					//sub->write_sub(fileWriter, curBlockSettings, curBlockSettings->get_scale());
					sub = sub->next;
				}

		}
		*/
		//delete awd;
        //callocService->Free((FCM::PVoid)awd);
        return FCM_SUCCESS;
    }

    FCM::Result RegisterPublisher(PIFCMDictionary pPlugins, FCM::FCMCLSID docId)
    {
        FCM::Result res;
        /*
         * Dictionary structure for a Publisher plugin is as follows:
         *
         *  Level 0 :    
         *              -----------------------
         *             | Flash.Plugin |  ----- | --------------------------------
         *              -----------------------                                  |
         *                                                                       |
         *  Level 1:                                   <-------------------------                          
         *              ------------------------------  
         *             | CLSID_Publisher_GUID | ----- | -------------------------
         *              ------------------------------                           |
         *                                                                       |
         *  Level 2:                                      <---------------------- 
         *              -----------------------------------
         *             | Flash.Category.Publisher |  ----- |---------------------
         *              -----------------------------------                      |
         *                                                                       |
         *  Level 3:                                                           <-
         *              ---------------------------------------------------------
         *             | Flash.Plugin.Name                  | "SamplePlugin"     |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.UI                    | "com.example..."   |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.TargetFormatName      | "xyz"              |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.TargetFormatExtension | "abc"              |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.CanExportImage        | "true"             |
         *              ---------------------------------------------------------
         *             | Flash.Plugin.TargetDocs            |           ---------|-
         *              ---------------------------------------------------------  |
         *                                                                         |
         *  Level 4:                                                    <----------
         *              -----------------------------------------------
         *             | CLSID_DocType   |  Empty String               |
         *              -----------------------------------------------
         *
         *  Note that before calling this function the level 0 dictionary has already
         *  been added. Here, the 1st, 2nd and 3rd level dictionaries are being added.
         */ 

        {
            // Level 1 Dictionary
            AutoPtr<IFCMDictionary> pPlugin;
            res = pPlugins->AddLevel(
                (const FCM::StringRep8)Utils::ToString(CLSID_Publisher).c_str(), 
                pPlugin.m_Ptr);

            {
                // Level 2 Dictionary
                AutoPtr<IFCMDictionary> pCategory;
                res = pPlugin->AddLevel(
                    (const FCM::StringRep8)kFlashCategoryKey_Publisher, 
                    pCategory.m_Ptr);

                {
                    // Level 3 Dictionary
                    std::string str_name = "AwayJS";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_Name,
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)str_name.c_str(),
                        (FCM::U_Int32)str_name.length() + 1);

                    std::string str_uniname = "AwayJS";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashCategoryKey_UniversalName,
                        kFCMDictType_StringRep8,
                        (FCM::PVoid)str_uniname.c_str(),
                        (FCM::U_Int32)str_uniname.length() + 1);

                    std::string str_ui = "AwayExtensionsFlashPro.PublishSettings";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_UI, 
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)str_ui.c_str(),
                        (FCM::U_Int32)str_ui.length() + 1);
					/*
                    std::string format = "AWD";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_TargetFormatName, 
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)format.c_str(),
                        (FCM::U_Int32)format.length() + 1);

                    std::string exten = "awd";
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_TargetFormatExtn, 
                        kFCMDictType_StringRep8, 
                        (FCM::PVoid)exten.c_str(),
                        (FCM::U_Int32)exten.length() + 1);

                    FCM::Boolean canExport = true;
                    res = pCategory->Add(
                        (const FCM::StringRep8)kFlashPublisherKey_CanExportImage,
                        kFCMDictType_Bool, 
                        (FCM::PVoid)&canExport,
                        sizeof(FCM::Boolean));*/

                    AutoPtr<IFCMDictionary> pDocs;
                    res = pCategory->AddLevel((const FCM::StringRep8)kFlashPublisherKey_TargetDocs, pDocs.m_Ptr);

                    {
                        // Level 4 Dictionary
                        std::string empytString = "";   
                        res = pDocs->Add(
                            (const FCM::StringRep8)Utils::ToString(docId).c_str(), 
                            kFCMDictType_StringRep8, 
                            (FCM::PVoid)empytString.c_str(),
                            (FCM::U_Int32)empytString.length() + 1);
                    }
                }
            }
        }

        return res;
    }

};
