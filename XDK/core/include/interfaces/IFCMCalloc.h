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
 * @file  IFCMCalloc.h
 *
 * @brief This file contains interface for IFCMCalloc. 
 *        The IFCMCalloc interface provides a mechanism for allocating and deallocating memory.
 *        This interface is used when there is shared memory between Plugin and FCM Application and
 *        the memory being allocated by Plugin is freed by FCM Application (or) viceversa.
 */

#ifndef IFCM_CALLOC_H_
#define IFCM_CALLOC_H_

#include "FCMPreConfig.h"
#include "FCMTypes.h"
#include "IFCMUnknown.h"


/* -------------------------------------------------- Forward Decl */


/* -------------------------------------------------- Macros / Constants */

namespace FCM 
{
    /**
     * @brief Defines the Interface ID (which is universally unique) for IFCMCalloc.
     *
     * @note  Textual Representation:  {1920B8DD-6CD8-4D9A-8272-4CBA3F29086F}
     */
    FCM::ConstFCMIID FCMIID_IFCMCalloc =
        {0x1920b8dd, 0x6cd8, 0x4d9a, {0x82, 0x72, 0x4c, 0xba, 0x3f, 0x29, 0x8, 0x6f}};
}


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

 namespace FCM
 { 
     /**
     * @class IFCMCalloc
     *
     * @brief Defines the interface that represents the calloc.
     */
	BEGIN_DECLARE_INTERFACE(IFCMCalloc , FCMIID_IFCMCalloc )
	
    /**  
	 * @brief  Allocate memory of size requested by param cb
     *
	 * @param  cb (IN)
     *         size of memory to be allocated.
     *
	 * @return returns allocated memory pointer.
	 */
	virtual FCM::PVoid _FCMCALL Alloc(FCM::U_Int32 cb) = 0;


	/**  
	* @brief  Free memory pointed by param pv.
    *
	* @param  pv (IN)
    *         pointer to memory to be freed.
	*/
	virtual void _FCMCALL Free(FCM::PVoid pv) = 0;

	END_DECLARE_INTERFACE

};  // namespace FCM


/* -------------------------------------------------- Inline / Functions */


#include "FCMPostConfig.h"

#endif //IFCM_CALLOC_H_