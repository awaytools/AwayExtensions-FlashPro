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
 * @file  ILibraryItem.h
 *
 * @brief This file contains the interface for ILibraryItem. 
 */

#ifndef ILIBRARY_ITEM_H_
#define ILIBRARY_ITEM_H_

#include "FCMPreConfig.h"
#include "FCMPluginInterface.h"


/* -------------------------------------------------- Forward Decl */

namespace FCM
{
    FORWARD_DECLARE_INTERFACE(IFCMDictionary);
};


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros / Constants */

// LibraryItem prop Key and type
#define kLibProp_LinkageClass_DictKey                   "LinkageClass"
#define kLibProp_LinkageClass_DictType                  FCM::kFCMDictType_StringRep8

#define kLibProp_LinkageBaseClass_DictKey               "LinkageBaseClass"
#define kLibProp_LinkageBaseClass_DictType              FCM::kFCMDictType_StringRep8

#define kLibProp_LinkageIdentifier_DictKey              "LinkageIdentifier"
#define kLibProp_LinkageIdentifier_DictType             FCM::kFCMDictType_StringRep8

#define kLibProp_LinkageURL_DictKey                     "LinkageURL"
#define kLibProp_LinkageURL_DictType                    FCM::kFCMDictType_StringRep8

#define kLibProp_LinkageExportForRS_DictKey             "LinkageExportForRS"
#define kLibProp_LinkageExportForRS_DictType            FCM::kFCMDictType_Bool

#define kLibProp_LinkageImportForRS_DictKey             "LinkageImportForRS"
#define kLibProp_LinkageImportForRS_DictType            FCM::kFCMDictType_Bool

#define kLibProp_LinkageExportInFirstFrame_DictKey      "LinkageExportInFirstFrame"
#define kLibProp_LinkageExportInFirstFrame_DictType     FCM::kFCMDictType_Bool

#define kLibProp_LinkageExportForAS_DictKey             "LinkageExportForAS"
#define kLibProp_LinkageExportForAS_DictType            FCM::kFCMDictType_Bool


namespace DOM
{
    /**
     * @brief Defines the Interface ID (which is universally unique) for 
     *        ILibraryItem
     *
     * @note  Textual Representation: {2EA5B91D-26C8-45A4-8BDC-41AD1F2C7B73}
     */
    FCM::ConstFCMIID IID_ILIBRARY_ITEM =
        {0x2ea5b91d, 0x26c8, 0x45a4, {0x8b, 0xdc, 0x41, 0xad, 0x1f, 0x2c, 0x7b, 0x73}};
}


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

namespace DOM
{
    /**
    * @class ILibraryItem
    *
    * @brief Each Library item is a reusable resource residing in the library. 
    *        The frame contains instances of this resource.
    */
    BEGIN_DECLARE_INTERFACE(ILibraryItem, IID_ILIBRARY_ITEM)
        
        /**
         * @brief  Returns the name of the item.
         * 
         * @param  ppName (OUT)
         *         Name of the library item with the full path in Library.
         *
         * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
         */
        virtual FCM::Result _FCMCALL GetName(FCM::StringRep16* ppName) = 0;


       /**
        * @brief  GetProperties returns a set of property names and their values
        *
        * @param  pPropDict (OUT)
        *         IFCMDictionary object
        *
        * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
        */    
        virtual FCM::Result _FCMCALL GetProperties(PIFCMDictionary& pPropDict) = 0;
    
    END_DECLARE_INTERFACE
};


/* -------------------------------------------------- Inline / Functions */


#include "FCMPostConfig.h"

#endif // ILIBRARY_ITEM_H_

