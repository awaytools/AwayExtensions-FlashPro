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
 * @file  IMetaData.h
 *
 * @brief This file contains the interface for IMetaData. 
 */

#ifndef IMETADATA_H_
#define IMETADATA_H_

#include "FCMPreConfig.h"
#include "FCMPluginInterface.h"


/* -------------------------------------------------- Forward Decl */

namespace FCM
{
    FORWARD_DECLARE_INTERFACE(IFCMList);
}


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros / Constants */

namespace DOM 
{
    /**
     * @brief Defines the Interface ID (which is universally unique) for 
     *        IMetaData
     *
     * @note  Textual Representation: {A62847AC-C02B-4d4a-95DB-19B161D3EC91}
     */
    FCM::ConstFCMIID IID_IMETA_DATA =
        {0xa62847ac, 0xc02b, 0x4d4a, {0x95, 0xdb, 0x19, 0xb1, 0x61, 0xd3, 0xec, 0x91}};
}


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

namespace DOM 
{
    /**
     * @class IMetaData
     *
     * @brief This interface represents meta data such as properties stores in the 
     *        form of dictionary.
     */
    BEGIN_DECLARE_INTERFACE(IMetaData, IID_IMETA_DATA)

       /**
        * @brief  This function returns a set of property names and their values
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

#endif // IMETADATA_H_
