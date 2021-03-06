/*************************************************************************
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright [2014] Adobe Systems Incorporated
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

/*
 * PLUGIN DEVELOPERS MUST CHANGE VALUES OF ALL THE MACROS AND CONSTANTS IN THIS FILE 
 * IN ORDER TO AVOID ANY CLASH WITH OTHER PLUGINS.
 */


#ifndef _PLUGIN_CONFIGURATION_H_
#define _PLUGIN_CONFIGURATION_H_

#define PUBLISHER_NAME						"AwayExtensionsAnimateCC"
#define PUBLISHER_UNIVERSAL_NAME			"AwayExtensionsAnimateCC"

/* The value of the PUBLISH_SETTINGS_UI_ID has to be the HTML extension ID used for Publish settings dialog*/
#define PUBLISH_SETTINGS_UI_ID				"com.away.AwayExtensionsAnimateCC.PublishSettings"

#define DOCTYPE_NAME						"Away Document"
#define DOCTYPE_UNIVERSAL_NAME				"AwayDocument"
#define DOCTYPE_DESCRIPTION					"An Away Document can be exported into an AWD file, ready to use with open source playback resources from The Away Foundation including AwayJS for WebGL playback"


	
namespace AwayJS
{
	// {2D999492-5132-4C5C-8066-3ABA1DF6DB6C}
    const FCM::FCMCLSID CLSID_DocType =
		{ 0x2d999492, 0x5132, 0x4c5c, { 0x80, 0x66, 0x3a, 0xba, 0x1d, 0xf6, 0xdb, 0x6c } };

	// {C37D8A04-CECD-4BBD-9A14-34A9E0D82B29}
    const FCM::FCMCLSID CLSID_FeatureMatrix =
		{ 0xc37d8a04, 0xcecd, 0x4bbd, { 0x9a, 0x14, 0x34, 0xa9, 0xe0, 0xd8, 0x2b, 0x29 } };
	
	// {D6863765-2532-47E6-83DA-BA7B575EAD27}
    const FCM::FCMCLSID CLSID_Publisher =
		{ 0xd6863765, 0x2532, 0x47e6, { 0x83, 0xda, 0xba, 0x7b, 0x57, 0x5e, 0xad, 0x27 } };	

	// {1D69F2D7-4CAA-4B17-81D6-6CACF09BBE2C}
    const FCM::FCMCLSID CLSID_ResourcePalette =
		{ 0x1d69f2d7, 0x4caa, 0x4b17, { 0x81, 0xd6, 0x6c, 0xac, 0xf0, 0x9b, 0xbe, 0x2c } };

	// {E07B04A6-435E-4113-B902-B42A88A80AC4}
    const FCM::FCMCLSID CLSID_TimelineBuilder =
		{ 0xe07b04a6, 0x435e, 0x4113, { 0xb9, 0x2, 0xb4, 0x2a, 0x88, 0xa8, 0xa, 0xc4 } };

	// {0AA1EBCE-E6EA-4138-B45A-3EB58594A9CC}
    const FCM::FCMCLSID CLSID_TimelineBuilderFactory =
		{ 0xaa1ebce, 0xe6ea, 0x4138, { 0xb4, 0x5a, 0x3e, 0xb5, 0x85, 0x94, 0xa9, 0xcc } };
		
}


#endif // _PLUGIN_CONFIGURATION_H_