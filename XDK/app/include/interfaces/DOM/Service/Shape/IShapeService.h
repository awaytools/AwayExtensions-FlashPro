/******************************************************************************
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
******************************************************************************/

/**
 * @file  IShapeService.h
 *
 * @brief This file contains interface for IShapeService. IShapeService is a 
 *        service that contains utility methods related to operations on shape 
 *        objects.
 */

#ifndef ISHAPE_SERVICE_H_
#define ISHAPE_SERVICE_H_

#include "FCMPreConfig.h"
#include "FCMPluginInterface.h"
#include "Utils/DOMTypes.h"


/* -------------------------------------------------- Forward Decl */

namespace FCM
{
    FORWARD_DECLARE_INTERFACE(IFCMList);
}


namespace DOM
{
    namespace FrameElement
    {
        FORWARD_DECLARE_INTERFACE(IShape);
    }
}



/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros / Constants */

namespace DOM
{
    namespace Service
    {
        namespace Shape
        {
            /**
             * @brief Defines the Interface ID (which is universally unique) for 
             *        IShapeService
             *
             * @note  Textual Representation:  {8365E61D-558D-4540-A7A4-F4F98E6AF09B}
             */
            FCM::ConstFCMIID IID_ISHAPE_SERVICE =
                {0x8365e61d, 0x558d, 0x4540, {0xa7, 0xa4, 0xf4, 0xf9, 0x8e, 0x6a, 0xf0, 0x9b}};
        }
    }
}


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

namespace DOM
{
    namespace Service
    {
        namespace Shape
        {
            /**
             * @class IShapeService
             *
             * @brief Defines a service that provides shape related utility methods.
             */
            BEGIN_DECLARE_INTERFACE(IShapeService, IID_ISHAPE_SERVICE)

                /**
                 * @brief  This function takes a shape "pSrcShape" as input and converts all the 
                 *         strokes in the shape to fills and returns a new temporary shape 
                 *         object "pDstShape". The fills in the original shape are not part of 
                 *         the destination shape. Only the strokes are copied and converted to fills. 
                 *         The source shape object is not modified.
                 *
                 * @param  pSrcShape (IN)
                 *         Shape whose strokes need to be converted
                 *
                 * @param  pDstShape (OUT)
                 *         A new shape with only the strokes of pSrcShape converted to fills. If 
                 *         "pShape" contains "fills" along with "strokes", only the strokes 
                 *         will be converted and placed in the resultant shape "pConvertedShape".
                 *
                 * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
                 */
                virtual FCM::Result _FCMCALL ConvertStrokeToFill(
                    DOM::FrameElement::PIShape pSrcShape,  
                    DOM::FrameElement::PIShape& pDstShape) = 0;

                
                /**
                 * @brief  This function takes two shapes as input and compares different parameter
                 *         to check if they are similar.
                 *
                 * @param  pShapeA (IN)
                 *         First shape to be compared.
                 *        
                 * @param  pShapeB (IN)
                 *         Second shape to be compared with first shapeA.
                 *
                 * @param similar (OUT)
                 *        Boolean variable set to true when shapes are similar.
                 *
                 * @param mapAtoB (OUT)
                 *        Transformation matrix that when applied on shape A will result in shape B.
                 *
                 * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
                 */
                virtual FCM::Result _FCMCALL TestShapeSimilarity(
                    DOM::FrameElement::PIShape pShapeA, 
                    DOM::FrameElement::PIShape pShapeB, 
                    FCM::Boolean& similar, 
                    DOM::Utils::MATRIX2D& mapAtoB) = 0;

            END_DECLARE_INTERFACE
        }
    }
}


/* -------------------------------------------------- Inline / Functions */


#include "FCMPostConfig.h"

#endif // ISHAPE_SERVICE_H_

