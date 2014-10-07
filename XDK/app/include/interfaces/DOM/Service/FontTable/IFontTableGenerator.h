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
 * @file  IFontTableGenerator.h
 *
 * @brief This file contains interface for IFontTableGenerator. IFontTableGenerator 
 *        contains methods to generate font table for a set specified chars of a 
 *        specified font and also generates the font table given a font item.
 */

#ifndef IFONT_TABLE_GENERATOR_H_
#define IFONT_TABLE_GENERATOR_H_

#include "FCMPreConfig.h"
#include "FCMPluginInterface.h"
#include "IFCMUnknown.h"
#include "Utils/DOMTypes.h"


/* -------------------------------------------------- Forward Decl */

namespace DOM
{
    namespace Service
    {        
        namespace FontTable
        {
            FORWARD_DECLARE_INTERFACE(IFontTable);
        }
    }


    namespace LibraryItem
    {
        FORWARD_DECLARE_INTERFACE(IFontItem);
    }


    FORWARD_DECLARE_INTERFACE(IFLADocument);
}


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros / Constants */

namespace DOM 
{
    namespace Service
    {
        namespace FontTable
        {        
            /**
             * @brief Defines the Interface ID (which is universally unique) for 
             *        IFontTableGenerator
             *
             * @note  Textual Representation: {DFA12563-76B3-4F88-9E04-FD150CB63968}
             */
            FCM::ConstFCMIID IID_IFONT_TABLE_GENERATOR =
                {0xdfa12563, 0x76b3, 0x4f88, {0x9e, 0x4, 0xfd, 0x15, 0xc, 0xb6, 0x39, 0x68}};
        }
    }
}


/* -------------------------------------------------- Structs / Unions */

/* -------------------------------------------------- Class Decl */

namespace DOM 
{
    namespace Service
    {
        namespace FontTable
        {        
            /**
            * @class IFontTableGenerator
            *
            * @brief Defines the interface to generate font table for a set specified chars of 
            *        specified a font also generates the font table given a font item.
            */    
            BEGIN_DECLARE_INTERFACE(IFontTableGenerator, IID_IFONT_TABLE_GENERATOR)
                
                /**
                 * @brief  This function generates font table for a set of specified 
                 *         characters of a specified font.
                 *
                 * @param  pFlaDoc (IN)
                 *         Document for which font table needs to be generated.
                 *
                 * @param  pFontName (IN)
                 *         The name of the font for which font table needs to be generated.    
                 *          
                 * @param  pFontStyle (IN)
                 *         The style of the font in string.
                 *
                 * @param  pCharacterString (IN)
                 *         The set of characters.
                 *
                 * @param  pFontTable (OUT)
                 *         The font table for the specified chars in the font is returned.
                 *
                 * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned. 
                 *
                 * @see    IFontTable                 
                 */        
                virtual FCM::Result _FCMCALL CreateFontTable(
                    const PIFLADocument& pFlaDoc, 
                    FCM::CStringRep16 pCharacterString, 
                    FCM::CStringRep16 pFontName,
                    FCM::CStringRep8 pFontStyle, 
                    PIFontTable& pFontTable) = 0;
            

                /**
                 * @brief  This function generates font table for embedded chars and embedded 
                 *         char ranges in the font item.
                 *
                 * @param  pFontItem (IN)
                 *         The font item for which font table needs to be generated.    
                 *          
                 * @param  pFontTable (OUT)
                 *         The font table for embedded chars and embedded char ranges in 
                 *         the font item is returned.
                 *
                 * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned. 
                 *
                 * @see    IFontTable
                 *
                 * @see    IFontItem                 
                 */
                virtual FCM::Result _FCMCALL CreateFontTableForFontItem(
                    const LibraryItem::PIFontItem& pFontItem, 
                    PIFontTable& pFontTable) = 0;
                
            END_DECLARE_INTERFACE 
        }
    }
}


/* -------------------------------------------------- Inline / Functions */


#include "FCMPostConfig.h"

#endif // IFONT_TABLE_GENERATOR_H_

