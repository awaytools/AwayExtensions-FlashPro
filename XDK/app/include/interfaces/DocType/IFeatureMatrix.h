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
 * @file  IFeatureMatrix.h
 *
 * @brief This file contains interface for IFeatureMatrix. 
 *        IFeatureMatrix provides methods to query the capabilities
 *        of a particular doc type.
 */

#ifndef IFEATURE_MATRIX_H_
#define IFEATURE_MATRIX_H_

#include "FCMPreConfig.h"
#include "FCMPluginInterface.h"


/* -------------------------------------------------- Forward Decl */


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros/Constants */

namespace DocType
{
    /**
     * @brief Defines the Interface ID (which is universally unique) for 
     *        IFeatureMatrix
     *
     * @note  Textual Representation:  {745F0985-43A9-4F11-BB66-6021D52223D7}
     */
    FCM::ConstFCMIID ID_IFEATURE_MATRIX =
        {0x745f0985, 0x43a9, 0x4f11, {0xbb, 0x66, 0x60, 0x21, 0xd5, 0x22, 0x23, 0xd7}};
}


/* -------------------------------------------------- Structs/Unions */


/* -------------------------------------------------- Class Decl */

namespace DocType
{

	/**
     * @class IFeatureMatrix
     *
     * @brief Defines an interface that represents the feature matrix for the assosciated 
     *        document type.
     */
    BEGIN_DECLARE_INTERFACE(IFeatureMatrix, ID_IFEATURE_MATRIX)

         /**  
          * @brief This function indicates whether a feature is supported or not
          *
          * @param inFeatureName (IN) 
          *        Name of the feature
          *
          * @param isSupported (OUT) 
          *        This param is updated based on whether the feature is supported or not
          *
          * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
          */
         virtual FCM::Result _FCMCALL IsSupported(
             CStringRep16 inFeatureName, 
             FCM::Boolean& isSupported) = 0;


        /**  
         * @brief This function indicates whether a property of a feature is supported or not
         *
         * @param inFeatureName (IN) 
         *        Name of the feature
         *
         * @param inPropName (IN)
         *        Name of the property related to feature(inFeatureName)
         *
         * @param isSupported (OUT) 
         *        This param is updated based on whether feature is supported or not
         *
         * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
         */
        virtual FCM::Result _FCMCALL IsSupported(
            CStringRep16 inFeatureName, 
            CStringRep16 inPropName, 
            FCM::Boolean& isSupported) = 0;


        /**  
         * @brief This function indicates whether a value is supported or not
         *
         * @param inFeatureName (IN) 
         *        Name of the feature
         *
         * @param inPropName (IN)
         *        Name of the property related to feature(inFeatureName)
         *
         * @param inValName (IN)
         *        Name of the value assosciated with a property(inPropName)
         *
         * @param isSupported (OUT)
         *        This param is updated according to whether value for the property is supported
         *
         * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
         */
        virtual FCM::Result _FCMCALL IsSupported(
            CStringRep16 inFeatureName, 
            CStringRep16 inPropName, 
            CStringRep16 inValName, 
            FCM::Boolean& isSupported) = 0;


        /**  
         * @brief This function gives default value for an unsupported property
         *
         * @param inFeatureName (IN) 
         *        Name of the feature
         *
         * @param inPropName (IN)
         *        Name of the property related to feature(inFeatureName)
         *
         * @param outDefVal (OUT) 
         *        This param is updated with default value of unsupported property
         *
         * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
         */
        virtual FCM::Result _FCMCALL GetDefaultValue(
            CStringRep16 inFeatureName, 
            CStringRep16 inPropName,
            FCM::VARIANT& outDefVal) = 0;

    END_DECLARE_INTERFACE 
} 

#include "FCMPostConfig.h"

#endif // IFEATURE_MATRIX_H_

