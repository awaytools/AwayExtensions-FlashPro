

AwayExtensionsFlashPro 0.9.5 is a WIN ONLY update. It was tested in  flash professional 14.1.0.97


The MAC version is still the same as in 0.9.3.


was tested on flash professional 14.1.0.97 both on Mac and Windows.

work in progress...


# Changelog 0.9.5:

* fix publishing from publish settings. This was broken in 0.9.4


# Changelog 0.9.4 (Windows  Only):

* WIN ONLY UPDATE. Tested on flash professional 14.1.0.97 windows.

* MAC version is still the same as version 0.9.3

* Refactored geometry export. The plugin now makes use of the libTess2 library, instead of the poly2tri. This loosens a lot if restrictions of the data to tesselate, and can handle self-intersecting pathes (ODD_WINDINGS).

* Major improvemts in exporter speed

* Memory leaks should have been elimitnated. I still see the memory that is used by flash rise with every export process, but the used memory is even smaller than the memory used by flash when exporting the same scene as a swf (from a as3 document)


* Improvements to preview-option "Open in Browser":
	* no need to ceater the shapoes at the flash-origin no more, the width and height of the stage is passed to the Viewer, and the shapes are centered.
	* new camera mode: "panning camera" (orthogonal view). we know can view shapes in plain 2d.
	* traces to the output console of the browser.
	* speed improvements to the renderer

* Demo / example scenes: 

	* both demo-scenes provided with version 0.9.3 are still provided, and updates with 0.9.4
	* we included a new demo-scene "01_demo_js.fla". this demo is designed to show the power of our rendering in regards of range of scale of shapes.
Be sure use the camera zoom on this project.



## Preview Controls (controlling the camera in the demo)

By default a panning camera will be active allowing the user to pan around the scene using the mouse by clicking and dragging. (using the mouse wheel can enable zooming in and out).

As the scene is essentially 2d in a 3d environment it is possible to switch to an orbit camera to loop. Press the “z” key to enter orbit mode.

To re-enter panning-mode press the \ (backslash) key. 

There is also a fly-cam available via the c key.

Summary
z - orbit camera  (mouse wheel to zoom)
\ - pan camera    (mouse wheel to zoom)
c - fly camera    ( WASD - hold shift or ctrl to alter the speed)


Troubleshooting the Demo:

Make sure to use Flashplayer 15.0.0.152 or above

Make sure that you use the correct FlashPlayer for your Browser.

Tested on windows with this FlashPlayers:


http://labsdownload.adobe.com/pub/labs/flashruntimes/flashplayer/flashplayer15_install_win_ax.exe


http://labsdownload.adobe.com/pub/labs/flashruntimes/flashplayer/flashplayer15_install_win_pi.exe




## Installation AwayExtensionsFlashPro

Install ```AwayExtensionsFlashPro.zxp``` via Adobe Extension Mangager CC (v7.2.1.6 or newer)

Open Flash.

You should be able to create a new Project of Type "AwayJS".




## What is supported by the plugin ?

At the moment, the plguin should be able to export shapes and timeline information into AWD files.
This AWD-files can be played back in as3/stage3D. 
We are working on runtime for webGL, but for now, we are using stage3D.

On windows, the plugin will copy the needed files for the runtime ("awd_renderer.swf" / "js/swfObject.swf") into the output-directory.
On Mac this functionallity has not yet been included. 
The user  needs to take care, that the files are located in the output-directory, in order to have the "open in Preview in browser" option working correctly.
You can find the needed files in the folder "as3_awd_renderer"


For now, all features are enabled for the AwayJS-document-type. Most features are not supported on export yet, but i am hopeing that we will be able to implement most of them eventually.

Font export is disabled in this version, because the output cannot be previewed with our current runtime.
I didnt disabled Text in the Features.xml, because we can use text, and convert it to shapes in Flash-Pro before export.
Font export will be included soon.

Textures and Sounds should get exported, but will cause errors in the runtime.

Mask-Layers will crash the exporter. Do not use them.



## About the Runtime ("awd_renderer.swf"):

The runtime is able to play back the first root-timeline (scene) found in a AWD-file.


The timeline should only contain shapes or other timelines. No sound / text / bitmaps are supported yet.

The timeline will loop the animation. scripting is not yet supported.
















