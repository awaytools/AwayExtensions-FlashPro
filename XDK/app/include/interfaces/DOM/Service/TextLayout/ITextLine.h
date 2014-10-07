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
 * @file  ITextLine.h
 *
 * @brief This file contains the interface for ITextLine. ITextLine interface 
 *        represents a line within the text element. 
 */

#ifndef ITEXT_LINE_H_
#define ITEXT_LINE_H_

#include "FCMPreConfig.h"
#include "FCMPluginInterface.h"
#include "IFCMUnknown.h"
#include "Utils/DOMTypes.h"


/* -------------------------------------------------- Forward Decl */


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros / Constants */

namespace DOM 
{
    namespace Service
    {
        namespace TextLayout
        {        
            /**
             * @brief Defines the Interface ID (which is universally unique) for 
             *        ITextLine
             *
             * @note  Textual Representation: {72330363-CBBB-4635-BDCC-04BAC5837871}
             */
            FCM::ConstFCMIID IID_ITEXT_LINE =
                {0x72330363, 0xcbbb, 0x4635, {0xbd, 0xcc, 0x4, 0xba, 0xc5, 0x83, 0x78, 0x71}};
        }
    }
}


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

namespace DOM 
{
    namespace Service
    {
        namespace TextLayout
        {        
            /**
             * @class ITextLine
             *
             * @brief Defines an interface that represents a line within the text element.
             */    
            BEGIN_DECLARE_INTERFACE(ITextLine, IID_ITEXT_LINE)                
                
                /**
                 * @brief  This function returns index of character within Text element where this line starts.
                 *
                 * @param  startIndex (OUT)
                 *         The index of character within Text element where this line starts.
                 *
                 * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned. 
                 */
                virtual FCM::Result _FCMCALL GetStartIndex(FCM::U_Int32& startIndex) = 0;
                

                /**
                 * @brief  This function returns length of the 
                 *         text(number of charachters in the text).
                 *
                 * @param  length (OUT)
                 *         Length of the text line.
                 *
                 * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned. 
                 */
                virtual FCM::Result _FCMCALL GetLength(FCM::U_Int32& length) = 0;    
                

                /**
                 * @brief This function gets the bounds if the charachter.
                 *
                 * @param  pCharBoundList (OUT)
                 *         Bounds of the charachters.
                 *         
                 * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned. 
                 */
                virtual FCM::Result _FCMCALL GetCharBounds(PIFCMList& pCharBoundList) = 0;
                

                /**
                 * @brief  This function is used to get the bounds of the line.
                 *
                 * @param  lineBound (OUT)
                 *         Bounds of the line of text.
                 *
                 * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned. 
                 */
                virtual FCM::Result _FCMCALL GetLineBound(Utils::RECT& lineBound) = 0;


                /**
                 * @brief  This function is used to get the start position of the text. 
                 *
                 * @param  startPos (OUT)
                 *         Start position of the text line.
                 *
                 * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned. 
                 */
                virtual FCM::Result _FCMCALL GetStartPos(Utils::POINT2D& startPos) = 0;
            
            END_DECLARE_INTERFACE 
        }
    }
}


/* -------------------------------------------------- Inline / Functions */


#include "FCMPostConfig.h"

#endif // ITEXT_LINE_H_

