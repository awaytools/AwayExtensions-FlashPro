AwayExtensions-FlashPro
=======================

Extension for exporting Flash pro content to Away3D &amp; AwayJS



## Installation AwayExtensionsFlashPro

Install ```AwayExtensionsFlashPro.zxp``` via Adobe Extension Mangager CC (v7.2.1.6 or newer)

Open Flash.

You should be able to create a new project of Type "AwayJS".




### changelog:

#### version 0.9.75:

* Big refactor. Most of the exporter and lib-awd have been revisited and updated.

* Source needs the dev-branch of [awd-sdk](https://github.com/awaytools/awd-sdk) to compile. 

* Needs FlashPro 15.0.0.74 

* Tested on mac and win

* As3 preview runtim is no longor available.

* Exports Colors as texture-atlas. Gradient and bitmap-fills are exported, but UV-Transform is not applied yet.

* No font / text is exported yet (convert text to shapes to test)






#### version 0.9.61:

* Fix commit 0.9.6. (correct sources)

* Added JS as preview option. Also included a optional "Path for Preview" to allow the user to change the path that will opened in the browser into a url.

* Known bugs: 

	* In the exporter-ui on mac, the handlers for the slider-elements are not shown. However when draging on the slider, the corresponding textfield is updated, so changing the sliders value is possible.

	* The resolving of the curve intersections needs another look. Something is not correct, and not all curves seem to be subdivided correctly.



#### version 0.9.6:

* This update brings multiple changes and is not tested much yet. Expect another update soon.

* Fixes for mac version.

* Still uses as3 preview, but the output is no longer 100% valid for the as3 runtime. (Might playback much faster)

* Added developer (tempor�ry) exporter options:

	* Resolve Curve Interesections: If this is enabled, the exporter will try to resolve curve intersections that would lead to render errors.

	* Curve Threshold: Curve intersections are resolved by subdividing curves. We calculate a value for the "curveiness" of each curve. If the value is smaller than "curve threshold", we will stop subdividing this curve-segment. A curviness of 1 means the 3 points that define the curve segment build a regular triangle. Very small curviness mean the curve segment is nearly a straight line.
	
	* Max iteration: The maximal number of iterations to do when resolving intersections

	* Use Adobe Frame Commands: If this is true, the frame commands will be generated by the function provided by Adobe. deselect this to test my own implentation. My implementation is work in progress, and not yet fully working. the as3 runtim doesnt show the results, but the awayjs runtime does.

* known bug: in the exporter-ui on mac, the handlers for the slider-elements are not shown. However when draging on the slider, the corresponding textfield is updated, so changing the sliders value is possible.


#### version 0.9.5:
* Fixes a bug that the exporter did not start exporting, on pressing the publish-button in the publish settings.

#### version 0.9.4:
* WIN ONLY UPDATE. Tested on flash professional 14.1.0.97 windows.

* MAC version is still the same as version 0.9.3

* Refactored geometry export. The plugin now makes use of the libTess2 library, instead of the poly2tri. This loosens a lot of restrictions on the data to tesselate, and can handle self-intersecting pathes (ODD_WINDINGS).

* Major improvments on exporter speed

* Improvements to preview-option "Open in Browser":
	* no need to ceater the shapoes at the flash-origin no more, the width and height of the stage is passed to the Viewer, and the shapes are centered.
	* Panning-cam / ortographic view. new camera mode. we know can view shapes in plain 2d.
	* traces to the output console of the browser.


* Memory leaks should have been elimitnated. I still see the memory that is used by flash rise with every export process, but the memory that is used, is even smaller than the memory used by flash when exporting the same scene as a swf (from a as3 document)




#### version 0.9.3:
* Tested on flash professional 14.1.0.97 both on mac and windows.

* Changed the GUID id for the Plugin, because it still was the same as the SamplePlugin from Adobe. Because of this change, existing AwayJS-Documents (*.fla) will no longer be recogniced as AwayJS-document.

* On Mac the files needed for preview ( ```awd_renderer.swf``` and ```js/swfobject.js``` ) are not copied to the output directory.

* "Open Preview" is now optional.

* Still MAsK-layers, will crash the exporter.

* Still Font export is disabled in this version, because the output cannot be previewed with our current awd_renderer. I didnt disabled Text in the Features.xml, because we can use text, and convert it to shapes in Flash-Pro before export.

* Textures and Sounds should get exported, but might cause errors in the awd_renderer

* The vertex-colors are now stored in range 0.0 - 1.0 instead of 0 - 255.

* All SubShapes of a Shape are merged now. Since we use VertexColors, the current runtime can benefit from this. Later we will not be able to merge all SubShapes, because subshapes will provide the information about Texture-Atlas.

* Infos about the awd_renderer
	* colors are fixed now.
	* displayTransform is set correctly.
	* some improvments for performance in runtime.

* there are still memory leaks in the code. I hope to fix this soon, but for now, big scenes will pile up used memory with every export.


#### version 0.9.2:
* Tested on flash professional 14.1.0.97 both on mac and windows.
* Implemented a swf-preview (awd_renderer). The ```awd_renderer.swf``` and the ```js/swfobject.js``` should get copied to the output directory.
* On Mac there is a bug, that the copied files (*.swf / *.js) are empty. That is the reason why these files are provided alongside the *.zxp

* Dont use MAsK-layers, because they will crash the exporter.

* Font export is disabled in this version, because the output cannot be previewed with our current awd_renderer. I didnt disabled Text in the Features.xml, because we can use text, and convert it to shapes in Flash-Pro before export.

* Textures and Sounds should get exported, but might cause errors in the awd_renderer

* The Vertex-data specs was changed, because 3 floats were not used. Because of this changes, the only compatible parser is the one contained in the awd_renderer.

* Infos about the awd_renderer
	* This is a temporary tool. We wanted a fast proof of concept. The Shader for stage3d already exisiting, we decided to makea fast prototyp in as3.
	* This awd_renderer only supports very basic shape and timeline-information.
	* Only supported fills are sold-colors.
	* Filters and colorTransform are not supported yet.
	* Display-matrix is supported, but the rotation is not set yet.
	* No keyframe duration is supported yet. Each Keyframe is considered to have a length of 1 frame.

* there are still memory leaks in the code. I hope to fix this soon, but for now big scenes will pile up used flash memory when exporting.



#### version 0.9.1:
* Tested on flash professional 14.1.0.88 and 14.1.0.97 both on mac and windows.
* Font is exported as shapes. At this point, a generated shape for a char can consit of multiple subShapes. this will be merged into one subshape in next version.
* Some of the shapes generated for fonts are making problems in the poly2Tri lib. (Self intersecting pathes). this chars will not get exported.
* there are memory leaks in the code. I hope to fix this soon, but for now big scenes will pile up used flash memory when exporting.
* lot of traces to the console. This is only temporary.
* lot of small fixes and improvments in code.


#### version 0.9.0:
* Started **versioning** (0.9.0) and **changelog**.<br/><br/>
* Compatible only to **Flash Pro 14.1.0.88** (and hopefully above)<br/><br/>
* Exporter compatible to latest update of AWD3Specs.<br/><br/>
* **The Pugin was compiled for mac, but i could not test it, because with the new update of Flash pro, the Extension manager stopped working on my MacBookPro.**<br/><br/>
* Only importer compatible to this output is [Bens as3 viewer](https://github.com/awaystudios/icycle_project/tree/master/swf_parser/bswf). On my system, i can compile the as3-viewer, but it fails when i try to package the actual air-app. So unless someone else (Ben) uploads a *.air with the latest changes, you have to compile the project yourself.<br/><br/>
* Exporter offers option to decide if outer triangles (not-curves) should get splitted, in order to minimize the area that needs to be rendered with anti-aliasing. This option is called **Outline-threshold**. The data type is float, and accepted range is 0.0 - 1.0. Values of 0.0 and 1.0 are both telling the exporter to completely  skip this calculation. Small values result in smaller outside-triangles. I am not sure if we want to make this option available in final release of plugin. It might be best to find the best setting, and not make it available.<br/><br/>
* The curves of shapes might overlap. On export, we have to make sure, that no overlapping curves exist. The exporter offers to choose between 3 different type of curve intersection-test-modes. This option is only included temporary, to test the different modes without recompiling the plugin.
	* **SaveSingle** Subdivide intersecting curves, until no curves intersect.
	* **SaveDouble** Double-Subdivide intersecting curves,  until no curves intersect.
	* **Fast** Double-Subdivide intersecting curves. Do not check for intersecting again.<br/><br/>
* Textures are saved as external files into a folder called "textures" that is located beside the awd-output. Embedding textures is not supported yet.<br/><br/>
* Sounds are exported as external files  into a folder called "sounds" that is located beside the awd-output. The format of the exported sound depends on the format of the sound. Later the Exporter will provide a option to choose a specific output-format for export. Embedding sounds is not supported yet. <br/><br/>
* Fonts are not getting exported as shapes yet. I am waiting for feedback from adobe, that will decide if i have to convert the current available data into fill-shapes manually, or if adobe will do this for me.<br/><br/>
* Storing gradients and colors into a texture-atlas is prepared in the code, but not yet finished implemented.(no information other than color is exported)<br/><br/>
* All available Timeline-information (including frame-script and frame-labels) should be exported as AWDTimeLineFrames (containing AWDFrameCommands). The only properties missing from this is "clip-depth" (information about mask-layers - bigger refactor needed), and filters (no specs / test done yet).<br/><br/>
* The Plugin can be used to export a complete scene ("Publish"), or to export only the current active Timeline incl. dependencies ("Control->Test Scene"). <br/><br/>
* To complete the "Test Scene" workflow, the Publish settings was changed, so that we can now save the settings without publishing.


## Compiling


### Windows

open and compile: ```cpp/AwayExtensionsFlashPro/project/win/AwayExtensionsFlashPro.sln```

This solution contains the main-plugin project and 2 other project that are included in the main project.

The 2 other projects are static libraries ```cpp/ThirdParty/Poly2Tri``` and ```cpp/ThirdParty/cpp-libawd```.


### Mac

open and compile: ```cpp/AwayExtensionsFlashPro/project/mac/AwayExtensionsFlashPro.mp.xcodeproj```

In the Mac version, only one project exists. 
The ```cpp/ThirdParty/Poly2Tri``` and ```cpp/ThirdParty/cpp-libawd``` are just added as sources to this project (not using static libraries).




## Packaging

To package ```AwayExtensionsFlashPro.zxp```, we need Eclipse with [Adobe Extension Builder 3](http://labs.adobe.com/technologies/extensionbuilder3/) installed.

The Adobe Extension Builder 3 must be [adjusted to support dev for CC2014 software](https://blogs.adobe.com/cssdk/2014/06/adobe-extension-builder-and-creative-cloud-2014.html).

Import the ```AwayExtensionsFlashPro_eclipse``` folder into Eclipse.

Export as ```Adobe Extension Builder 3/Application Extension``` to create the zxp.

Sign with certifacte found in project-folder (password: ```1234```), or use own certificate.


