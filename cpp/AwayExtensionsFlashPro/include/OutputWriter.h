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

/**
 * @file  OutputWriter.h
 *
 * @brief This file contains declarations for a output writer.
 */

#ifndef OUTPUT_WRITER_H_
#define OUTPUT_WRITER_H_

#include "FCMPluginInterface.h"
#include "awd.h"
#include "IOutputWriter.h"
#include <string>
#include "shape2D.h"
#include "shape2Dtimeline.h"
#include "FCMTypes.h"
#include "FCMPluginInterface.h"
#include "Exporter/Service/IResourcePalette.h"
#include "Exporter/Service/ITimelineBuilder.h"
#include "Exporter/Service/ITimelineBuilderFactory.h"
#include "Publisher/IPublisher.h"
#include "FillStyle/ISolidFillStyle.h"
#include "FillStyle/IGradientFillStyle.h"
#include "FillStyle/IBitmapFillStyle.h"
#include "Exporter/Service/IFrameCommandGenerator.h"

#include "ShapeEncoder.h"

using namespace std;
using namespace DOM;

/* -------------------------------------------------- Forward Decl */


namespace AwayJS
{
    class ITimelineWriter;
}

/* -------------------------------------------------- Enums */

namespace AwayJS
{
    enum StrokeStyleType
    {
        INVALID_STROKE_STYLE_TYPE,
        SOLID_STROKE_STYLE_TYPE
    };
}


/* -------------------------------------------------- Macros / Constants */

#define IMAGE_FOLDER "textures"
#define SOUND_FOLDER "sounds"
#define SWF_OBJECT_PATH "js/swfobject.js"
#define AS_VIEWER_NAME "awdrender.swf"
#define JS_VIEWER_NAME "js/AWD3Viewer.txt"

#define JSON_OUTPUT_FILE_NAME "sample.json"


/* -------------------------------------------------- Structs / Unions */

namespace AwayJS
{
    struct SOLID_STROKE_STYLE
    {
        FCM::Double thickness;
        DOM::StrokeStyle::JOIN_STYLE joinStyle;
        DOM::StrokeStyle::CAP_STYLE capStyle;
        DOM::Utils::ScaleType scaleType;
        FCM::Boolean strokeHinting;
    };

    struct STROKE_STYLE
    {
        StrokeStyleType type;

        union
        {
            SOLID_STROKE_STYLE solidStrokeStyle;
        };
    };
}


/* -------------------------------------------------- Class Decl */

namespace AwayJS
{
    class AWDOutputWriter : public IOutputWriter
    {
    public:
        // Marks the begining of the output
        virtual FCM::Result StartOutput(std::string& outputFileName);

        // Marks the end of the output
        virtual FCM::Result EndOutput();
		
		virtual FCM::Result PreParePreview(
			const DOM::Utils::COLOR& background,
			FCM::U_Int32 stageHeight, 
			FCM::U_Int32 stageWidth,
			FCM::U_Int32 fps,
			bool doJS);

        // Marks the begining of the Document
        virtual FCM::Result StartDocument(
            const DOM::Utils::COLOR& background,
            FCM::U_Int32 stageHeight, 
            FCM::U_Int32 stageWidth,
            FCM::U_Int32 fps);

        // Marks the end of the Document
        virtual FCM::Result EndDocument();

        // Marks the start of a timeline
        virtual FCM::Result StartDefineTimeline();

        // Marks the end of a timeline
        virtual FCM::Result EndDefineTimeline(
            FCM::U_Int32 resId, 
            FCM::StringRep16 pName, 
            ITimelineWriter* pTimelineWriter);

        // Marks the start of a shape
        virtual FCM::Result StartDefineShape();

        // Start of fill region definition
        virtual FCM::Result StartDefineFill();

        // Solid fill style definition
        virtual FCM::Result DefineSolidFillStyle(const DOM::Utils::COLOR& color);

        // Bitmap fill style definition
        virtual FCM::Result DefineBitmapFillStyle(
            FCM::Boolean clipped,
            const DOM::Utils::MATRIX2D& matrix,
            FCM::S_Int32 height, 
            FCM::S_Int32 width,
            FCM::StringRep16 pName,
            DOM::LibraryItem::PIMediaItem pMediaItem);

        // Start Linear Gradient fill style definition
        virtual FCM::Result StartDefineLinearGradientFillStyle(
            DOM::FillStyle::GradientSpread spread,
            const DOM::Utils::MATRIX2D& matrix);

        // Sets a specific key point in a color ramp (for both radial and linear gradient)
        virtual FCM::Result SetKeyColorPoint(
            const DOM::Utils::GRADIENT_COLOR_POINT& colorPoint);

        // End Linear Gradient fill style definition
        virtual FCM::Result EndDefineLinearGradientFillStyle();

        // Start Radial Gradient fill style definition
        virtual FCM::Result StartDefineRadialGradientFillStyle(
            DOM::FillStyle::GradientSpread spread,
            const DOM::Utils::MATRIX2D& matrix,
            FCM::S_Int32 focalPoint);

        // End Radial Gradient fill style definition
        virtual FCM::Result EndDefineRadialGradientFillStyle();

        // Start of fill region boundary
        virtual FCM::Result StartDefineBoundary();

        // Sets a segment of a path (Used for boundary, holes)
        virtual FCM::Result SetSegment(const DOM::Utils::SEGMENT& segment);
        virtual FCM::Result SetAWDSegment(AWDPathSegment* segment);
        // End of fill region boundary
        virtual FCM::Result EndDefineBoundary();

        // Start of fill region hole
        virtual FCM::Result StartDefineHole();

        // End of fill region hole
        virtual FCM::Result EndDefineHole();

        // Start of stroke group
        virtual FCM::Result StartDefineStrokeGroup();

        // Start solid stroke style definition
        virtual FCM::Result StartDefineSolidStrokeStyle(
            FCM::Double thickness,
            const DOM::StrokeStyle::JOIN_STYLE& joinStyle,
            const DOM::StrokeStyle::CAP_STYLE& capStyle,
            DOM::Utils::ScaleType scaleType,
            FCM::Boolean strokeHinting);

        // End of solid stroke style
        virtual FCM::Result EndDefineSolidStrokeStyle();

        // Start of stroke 
        virtual FCM::Result StartDefineStroke();

        // End of a stroke 
        virtual FCM::Result EndDefineStroke();

        // End of stroke group
        virtual FCM::Result EndDefineStrokeGroup();

        // End of fill style definition
        virtual FCM::Result EndDefineFill();
        // End of fill style definition
        virtual FCM::Result EndDefineFontStroke(AWDFontShape*);
		
        // Marks the end of a shape
        virtual FCM::Result EndDefineShape(FCM::U_Int32 resId);
 
        // Define a bitmap
        virtual FCM::Result DefineBitmap(
            FCM::U_Int32 resId,
            FCM::S_Int32 height, 
            FCM::S_Int32 width,
            FCM::StringRep16 pName,
            DOM::LibraryItem::PIMediaItem pMediaItem);

		// Define text
		virtual FCM::Result DefineText(
			FCM::U_Int32 resId, 
			DOM::FrameElement::PIClassicText pTextItem);

        virtual FCM::Result DefineSound(
            FCM::U_Int32 resId, 
			const std::string& name, 
            DOM::LibraryItem::PIMediaItem pMediaItem);
        AWDOutputWriter(FCM::PIFCMCallback pCallback);
        virtual ~AWDOutputWriter();

        // Start of a path
        virtual FCM::Result StartDefinePath();

        // End of a path 
        virtual FCM::Result EndDefinePath();
		
        virtual bool get_save_interstect();
        virtual void set_save_interstect(bool);
		virtual bool get_double_subdivide();
		virtual void set_double_subdivide(bool);
		virtual double get_outline_threshold();
		virtual void set_outline_threshold(double);

		virtual  FCM::Result EndDefineShapeLetter(AWDFontShape*);
		virtual  AWD * get_awd();
		
		virtual void set_awd(AWD*);

		virtual FCM::PIFCMCallback* get_mCallback();
        std::string m_outputImageFolder;
		
    private:
		
		bool save_interstect;
		bool double_subdivide;
		double outline_threshold;
		// the main AWD lib object 
        AWD * awd;
		
		PIFLADocument doc;

		ShapeEncoder* shape_encoder;

        std::string m_pathCmdStr;

        bool        m_firstSegment;

        STROKE_STYLE m_strokeStyle;
		
		
		
        std::string m_outputHTMLFile;
        std::string m_outputJSONFile;


        std::string m_outputSoundFolder;
        std::string m_target_asViewer_path;
        std::string m_target_swfObject_path;
        std::string m_target_JSViewer_path;

        std::string m_source_JSViewer_path;
		
        std::string m_source_asViewer_path;
        std::string m_source_swfObject_path;

        char* m_HTMLOutput;
        char* m_HTMLOutputJS;

        FCM::PIFCMCallback m_pCallback;
    };


	class AWDTimelineWriter : public ITimelineWriter
    {
    public:
		
		AWDShape2DTimeline* get_timeline();
        virtual FCM::Result PlaceObject(
            FCM::U_Int32 resId,
            FCM::U_Int32 objectId,
            FCM::U_Int32 placeAfterObjectId,
            const DOM::Utils::MATRIX2D* pMatrix,
            FCM::PIFCMUnknown pUnknown = NULL);
        virtual FCM::Result PlaceObject(
            FCM::U_Int32 resId,
            FCM::U_Int32 objectId,
            FCM::PIFCMUnknown pUnknown = NULL);

        virtual FCM::Result RemoveObject(
            FCM::U_Int32 objectId);

        virtual FCM::Result UpdateZOrder(
            FCM::U_Int32 objectId,
            FCM::U_Int32 placeAfterObjectId);

        virtual FCM::Result UpdateBlendMode(
            FCM::U_Int32 objectId,
            DOM::FrameElement::BlendMode blendMode);

        virtual FCM::Result UpdateVisibility(
            FCM::U_Int32 objectId,
            FCM::Boolean visible);

        virtual FCM::Result AddGraphicFilter(
            FCM::U_Int32 objectId,
            FCM::PIFCMUnknown pFilter);

        virtual FCM::Result UpdateDisplayTransform(
            FCM::U_Int32 objectId,
            const DOM::Utils::MATRIX2D& matrix);

        virtual FCM::Result UpdateColorTransform(
            FCM::U_Int32 objectId,
            const DOM::Utils::COLOR_MATRIX& colorMatrix);

        virtual FCM::Result ShowFrame(FCM::U_Int32 frameNum);
		
        virtual FCM::Result AddFrameScript(FCM::CStringRep16 pScript, FCM::U_Int32 layerNum);

        virtual FCM::Result RemoveFrameScript(FCM::U_Int32 layerNum);

        virtual FCM::Result SetFrameLabel(FCM::StringRep16 pLabel, DOM::KeyFrameLabelType labelType);

        AWDTimelineWriter(AWD*, FCM::PIFCMCallback);

        virtual ~AWDTimelineWriter();

		AWDShape2DTimeline* GetRoot();

        void Finish(FCM::U_Int32 resId, FCM::StringRep16 pName);

    private:
		AWD* awd;
		AWDShape2DTimeline* thisTimeLine;
		awd_float64 *get_mtx_2x3(const DOM::Utils::MATRIX2D* mtx);
		awd_float64 *get_color_mtx_4x5(const DOM::Utils::COLOR_MATRIX* mtx);
		vector<AWDFrameCommandDisplayObject*> frameCommands;

		
		vector<AWDTimeLineFrame*> timeLineFrames;


		AWDShape2DTimeline* awdTimeLine;
    
		
		FCM::PIFCMCallback m_pCallback;

    };
};

#endif // OUTPUT_WRITER_H_

