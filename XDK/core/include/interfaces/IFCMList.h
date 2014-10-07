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
 * @file  IFCMList.h
 *
 * @brief This file contains interface for IFCMList. 
 *        List of pointer to IFCMUnknown objects.
*/ 

#ifndef IFCM_LIST_H_
#define IFCM_LIST_H_

#include "FCMPreConfig.h"
#include "FCMTypes.h"
#include "IFCMUnknown.h"


/* -------------------------------------------------- Forward Decl */


/* -------------------------------------------------- Macros / Constants */

namespace FCM 
{
    /**
     * @brief Defines the Interface ID (which is universally unique) for IFCMList.
     *
     * @note  Textual Representation:  {FEAF29A0-9A0D-4C4F-9E58-1BA7E279824B}
     */
    FCM::ConstFCMIID FCMIID_IFCMList =
        {0xfeaf29a0, 0x9a0d, 0x4c4f, {0x9e, 0x58, 0x1b, 0xa7, 0xe2, 0x79, 0x82, 0x4b}};
}


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

namespace FCM
{ 
    /**
     * @class IFCMList
     *
     * @brief Defines the interface that represents the List of pointer to IFCMUnknown objects.
     */
    BEGIN_DECLARE_INTERFACE(IFCMList, FCMIID_IFCMList)

        /**
         * @brief  Fetches the number of pointer to IFCMUnknown objects in the List.
         *
         * @param  count (OUT)
         *         Populated with the number of pointer to IFCMUnknown objects in the List.
         *
         * @return FCM_SUCCESS is returned for success. Otherwise an error code is returned.
         */
         virtual FCM::Result _FCMCALL Count(FCM::U_Int32& count) = 0;


        /**
         * @brief  Returns the IFCMUnknown pointer at given index.
         *
         * @param  index (IN)
         *         The location of the entry in List for which we need info.
         *        Index should range from 0 and Count-1.
         *
         * @return IFCMUnknown pointer is returned for success. NULL is returned for failure.
         */
         virtual PIFCMUnknown _FCMCALL operator [](FCM::U_Int32 index) = 0;

    END_DECLARE_INTERFACE

};  // namespace FCM


/* -------------------------------------------------- Inline / Functions */


#include "FCMPostConfig.h"

#endif //IFCM_LIST_H_