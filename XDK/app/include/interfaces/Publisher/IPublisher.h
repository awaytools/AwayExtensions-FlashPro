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
 * @file  IPublisher.h
 *
 * @brief This file contains the interface for IPublisher. The IPublisher 
 *        interface needs to be implemented by the "Publisher" plugin-developer.
 *        It contains methods to publish a FLA document and a single timeline.
 *        FlashPro would invoke methods in this interface in the test-movie 
 *        and publish workflows.
 */

#ifndef IPUBLISHER_H_
#define IPUBLISHER_H_

#include "FCMPreConfig.h"
#include "FCMPluginInterface.h"

#include "FCMPreConfig.h"
#include "FCMTypes.h"
#include "FCMPluginInterface.h"
#include "IFCMDictionary.h"
#include "ITimeline.h"
#include "IFLADocument.h"
#include "Exporter/Service/IFrameCommandGenerator.h"


/* -------------------------------------------------- Forward Decl */


/* -------------------------------------------------- Macros / Constants */

/**
 * @brief Defines the Interface ID (which is universally unique) for 
 *        IPublisher
 *
 * @note Textual Representation: {3ACF3427-4FD-44EF-9FBA-3EBB62F044A2}
 */
FCM::ConstFCMIID FCMIID_IPublisher =
    {0x3acf3427, 0x4fd, 0x44ef, {0x9f, 0xba, 0x3e, 0xbb, 0x62, 0xf0, 0x44, 0xa2}};


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Structs / Unions */


/* -------------------------------------------------- Class Decl */

namespace Publisher
{
    /**
     * @class IPublisher
     *
     * @brief Defines an interface that represents publisher.
     */
    BEGIN_DECLARE_INTERFACE(IPublisher, FCMIID_IPublisher)

        /**
         * @brief  Publishes the entire Fla Document
         *
         * @param  pFlaDocument 
         *         Document to be published.
         *
         * @param  pDictPublishSettings (IN) 
         *         Publish settings.
         *
         * @param  pDictConfig (IN) 
         *         The publish configuration parameters. For e.g.,
         *           pDictConfig.preViewNeeded = true/false;
         *           pDictConfig.IsTestMovie = true/false;
         *
         * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
         */
        virtual FCM::Result _FCMCALL Publish(
            DOM::PIFLADocument pFlaDocument, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig) = 0;
        
        
        /**
         * @brief  Publishes one single timeline.
         *
         * @param  pFlaDocument (IN)
         *         Document containing the timeline.
         *
         * @param  pTimeline (IN)
         *         Timeline to be published. It may correspond to a movie-clip or
         *         a scene itself. This will be never be NULL.
         *
         * @param  frameRange (IN) 
         *         Range of frames to be published.
         *
         * @param  pDictPublishSettings (IN) 
         *         Publish settings.
         *
         * @param  pDictConfig (IN) 
         *         Publish configuration parameters. This are controlled by 
         *         FlashPro and plugins hae no control on it. For e.g.,
         *           pDictConfig["PublishSettings.PreviewNeeded"] = "true" or "false";
         *
         * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
         */
        virtual FCM::Result _FCMCALL Publish(
            DOM::PIFLADocument pFlaDocument, 
            DOM::PITimeline pTimeline, 
            const Exporter::Service::RANGE& frameRange, 
            const PIFCMDictionary pDictPublishSettings, 
            const PIFCMDictionary pDictConfig) = 0;
        

        /**
         * @brief  Publisher must clear its cache if it maintains. 
         *
         * @return On success, FCM_SUCCESS is returned; otherwise an error code is returned.
         */
        virtual FCM::Result _FCMCALL ClearCache() = 0;

    END_DECLARE_INTERFACE
};


/* -------------------------------------------------- Inline / Functions */

#include "FCMPostConfig.h"

#endif //IPUBLISHER_H_

