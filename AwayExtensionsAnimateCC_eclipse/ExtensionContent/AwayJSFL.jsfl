
function batchExport() {
	var folder = fl.browseForFolderURL("Choose a input folder");
	var files = FLfile.listFolder(folder + "/*.fla", "files");
	for (file in files) {
		var curFile = files[file];
		// open document, update publisher settings, publish, discard changes, close
		fl.openDocument(folder + "/" + curFile);
		updatePublishSettingsForBatchExport();
		fl.getDocumentDOM().publish();
		fl.getDocumentDOM().revert();
		fl.closeDocument(fl.getDocumentDOM());
	}
}

function updatePublishSettingsForBatchExport() {
	var profileXML = new XML(fl.getDocumentDOM().exportPublishProfileString());
	for(var i=0; i<profileXML.PublishProperties.length(); i++){
		if(profileXML.PublishProperties[i].@name=="AwayExtensionsAnimateCC"){
			for(var k=0;k<profileXML.PublishProperties[i].Property.length();k++){
				if(profileXML.PublishProperties[i].Property[k].@name=="out_file"){
					profileXML.PublishProperties[i].Property[k]="";
				}
				else if(profileXML.PublishProperties[i].Property[k].@name=="OpenPreview"){
					profileXML.PublishProperties[i].Property[k]=false;
				}
				else if(profileXML.PublishProperties[i].Property[k].@name=="CopyRuntime"){
					profileXML.PublishProperties[i].Property[k]=false;
				}
				else if(profileXML.PublishProperties[i].Property[k].@name=="PrintExportLog"){
					profileXML.PublishProperties[i].Property[k]=false;
				}
				else if(profileXML.PublishProperties[i].Property[k].@name=="PrintExportLogTimelines"){
					profileXML.PublishProperties[i].Property[k]=false;
				}
			}
		}
	}
	fl.getDocumentDOM().importPublishProfileString(profileXML.toXMLString());
}

function hasValidDoc() {
	// check if the current doc is a Away Document
	if(fl.getDocumentDOM()){
		var profileXML =  new XML(fl.getDocumentDOM().exportPublishProfileString());
		for(var i=0; i<profileXML.PublishProperties.length(); i++){
			fl.trace(profileXML.PublishProperties[i].@name);
			if(profileXML.PublishProperties[i].@name=="AwayExtensionsAnimateCC"){
				return true;
			}
		}
	}
	return false;
}

function getPublisherSettings() {
	var profileXML = new XML(fl.getDocumentDOM().exportPublishProfileString('Default'));
	var settings_str="";

	for(var i=0; i<profileXML.PublishProperties.length(); i++){
		if(profileXML.PublishProperties[i].@name=="AwayExtensionsAnimateCC"){
			for(var k=0;k<profileXML.PublishProperties[i].Property.length();k++){
				settings_str+=profileXML.PublishProperties[i].Property[k].@name+"="+profileXML.PublishProperties[i].Property[k]+"###";
			}
		}
	}
	return settings_str;

}

function importFrameScriptsFromJS(){
	var uri = fl.browseForFileURL("open", "Select a JS file", "Javascript file (*.js)", "js");
	var str = FLfile.read( uri);
	if (str) {
		var functions_ls=str.split("//AnimateCC: Symbol: ");
		if(functions_ls.length<=1){
			alert("The selected File contains no scripts that can be applied to document");
		}
		else{
			var i=1;
			var lastMC=undefined;
			var lastMC_name="";
			var currentdoc_lib = fl.getDocumentDOM().library;
			var lib_items = currentdoc_lib.items;
			var mcs_found=[];// aray of arrays - one array for each MC that has been found
			var mcs_not_found=[];// aray of arrays - one array for each MC that has been found
			for(i=1;i<functions_ls.length; i++){
				var splitted=functions_ls[i].split("\n");
				if(splitted.length<=2){
					alert("Error when parsing the script");
				}
				else{
					var name_and_frame=splitted[0].split("  #frame: ");
					if(lastMC_name!=name_and_frame[0]){
						lastMC_name=name_and_frame[0];
						var idx=currentdoc_lib.findItemIndex(lastMC_name);
						lastMC=null;
						if(idx!=""){
							if((lib_items[idx].itemType=="movie clip")||(lib_items[idx].itemType=="graphic")){
								lastMC=lib_items[idx];
								mcs_found.push([lastMC.timeline, [], []]);
							}
						}
						else{
							var t = 0;
							var curTimelines = fl.getDocumentDOM().timelines;
							while(t < fl.getDocumentDOM().timelines.length){
								if(curTimelines[t].name==lastMC_name){
									lastMC=curTimelines[t];
									mcs_found.push([lastMC, [], []]);
									t=fl.getDocumentDOM().timelines.length;
								};
								++t;
							}
						}
						if(lastMC==null){
							mcs_not_found.push(name_and_frame[0]);
						}
					}
					if(lastMC!=null){
						var k=2;
						var thisscript="";
						for(k=2;k<splitted.length-1; k++){
							thisscript+=splitted[k]+"\n";
						}
						mcs_found[mcs_found.length-1][1].push(thisscript);
						mcs_found[mcs_found.length-1][2].push(name_and_frame[1]);
					}
				}
			}
			var doit=true;
			if(mcs_not_found.length>0){
				doit=false;
				if(mcs_found.length>0){
					var questionstring="Found framescript for "+mcs_not_found.length+" MovieClip or Graphic that could not be found in the current Document.\n";
					questionstring+="The names of those Movieclips have been printed to the console.\n";
					questionstring+="Still apply scripts of MovieClips that could be found ?";
					for(i=0;i<mcs_not_found.length; i++){
						fl.trace("Did not found any MovieClip or Graphic with name '"+mcs_not_found[i]+"'");
					}
					doit = confirm(questionstring);
				}
				else{
					confirm("Found framescript for "+mcs_not_found.length+ " Movieclips, but none of those MovieClips could be found in the current Document.");
				}
			}
			if((mcs_found.length>0)&&(doit)){
				var done_count = 0;
				// apply the stuff to the movieclips we found
				for(i=0;i<mcs_found.length; i++){
					var timeline=mcs_found[i][0];
					var layerCount = timeline.layerCount;
					var k=0;
					for(k=0; k<mcs_found[i][1].length; k++){
						var frameIdx=mcs_found[i][2][k];
						var framescript=mcs_found[i][1][k];
						layerCount = timeline.layerCount;
						if(layerCount>0){
							var awayjs_code_layer=-1;
							var framescriptAssigned=false;
							while (layerCount--){
								var layer = timeline.layers[layerCount];
								var frame = layer.frames[frameIdx-1];
								if ( frame == undefined){
									continue;
								}
								if( frame.startFrame==frameIdx-1){
									if(!framescriptAssigned){
										awayjs_code_layer=layerCount;
									}
									if(frame.actionScript!=""){
										if(!framescriptAssigned){
											framescriptAssigned=true;
											done_count++;
											frame.actionScript=framescript;
										}
										else{
											frame.actionScript="";
										}
									};
								}
							}
							if(!framescriptAssigned){
								if(awayjs_code_layer!=-1){
									var frame = timeline.layers[awayjs_code_layer].frames[frameIdx-1];
									if ( frame == undefined){
										continue;
									}
									if( frame.startFrame==frameIdx-1){
										done_count++;
										frame.actionScript=framescript;
									}
								}
								else{
									fl.trace("Error when trying to set framescript for frame "+frameIdx+" on Movieclip '"+mcs_found[i][0].name+"'. No keyframe exists for this frame");
								}
							}
						}
					}
				}
				alert("Imported "+done_count+" framescripts into "+mcs_found.length+" Movieclips");

			}
		}
	}
	else{
		alert("The selected File contains no scripts that can be applied to document");
	}
}

function getFrameScript() {
	// check if the current doc is a Away Document
	if(fl.getDocumentDOM()){
		var profileXML =  fl.getDocumentDOM().exportPublishProfileString('Default');
		var findString = "AwayExtensionsAnimateCC";
		var startIndex = profileXML.indexOf(findString);
		if(startIndex>0){
			var timeline=fl.getDocumentDOM().getTimeline();
			if(timeline){
				var scripts="";
				layerCount = timeline.layerCount;
				if(layerCount>0) {
					var awayjs_code_layer = -1;
					var framescriptAssigned = false;
					while (layerCount--) {
						var layer = timeline.layers[layerCount];
						var frame = layer.frames[timeline.currentFrame];
						if (frame == undefined) {
							continue;
						}
						if (frame.startFrame == timeline.currentFrame) {
							scripts += frame.actionScript;
						}
					}
				}
				var script_lines=scripts.split("\n");
				scripts="";
				for(var i=0; i<script_lines.length; i++){
					scripts+=script_lines[i];
					if(i<script_lines.length-1){
						scripts+="<br>"
					}
				}
				//fl.trace(scripts);
				return scripts;
			}
		}
	}
	return "";
}





// stuff that might be used later:

function showXMLPanel(xmlString){
  var tempUrl = fl.configURI + "Commands/test.xml"
  fl.trace("myText2: " + tempUrl);
  
  FLfile.write(tempUrl, xmlString);
  var xmlPanelOutput = fl.getDocumentDOM().xmlPanel(tempUrl);
  FLfile.remove(tempUrl);
  return xmlPanelOutput;
}
function opcustum_prev() {
  fl.trace("clicked");
}
function change_preview_settings(source_path, preview_path, add_name) {

	// create an XUL with JSFL
	var dialogXML = "";
	dialogXML += "<dialog title='Preview options' buttons=''>";
	dialogXML +=   "<vbox>";
	dialogXML +=   "<flash width='450'  src='test.swf'  height='200'  id='settings' />";
	dialogXML +=    "<property id='file' />";

	dialogXML +=     "<label value='Custom source path.'/>";
	dialogXML +=     "<label value='All files and folders inside this folder will be copied to output-folder.'/>";
	dialogXML +=     "<hbox>";
	dialogXML +=    	"<textbox id='myText1' value='"+source_path+"'/>";
	dialogXML +=     	"<button label='' press='click()' />";
	dialogXML +=     "</hbox>";
	dialogXML +=     "<label value='Custom URL. If this is set, the preview will open the browser with this URL.'/>";
	dialogXML +=     "<textbox id='myText2' value='"+preview_path +"'/>";
	dialogXML +=     "<checkbox label='append name to output-path' id='myText3' value='"+add_name +"'/>";
	dialogXML +=   "</vbox>";
    dialogXML += 	"<script>";
    dialogXML += 		"function click(){  " ;
    dialogXML += 		"fl.xmlui.set( 'myText1', '#FF6600' ); "  ;
    dialogXML += 		"}";
    dialogXML += "</script> ";
	dialogXML += "</dialog>";

	var xmlPanelOutput = showXMLPanel(dialogXML);
	if (xmlPanelOutput.dismiss == "accept") // user confirms dialog
	{
		return "save"+"#AWD#"+xmlPanelOutput.myText1 + "#AWD#"+xmlPanelOutput.file;
	}
	return "cancel"+"#AWD#"+xmlPanelOutput.myText1 + "#AWD#"+xmlPanelOutput.file;
	
}
function documentInfos() {
	var doc = fl.getDocumentDOM(); 
	var collectallMotionsXML="All Motions";
	var animCounts=0;
	for(var i=0; i<doc.timelines.length;i++){
		var my_tl = doc.timelines[i]; 
		var countNum = my_tl.frameCount;
		fl.trace(countNum); 
		for(var l=0;l<my_tl.layerCount;l++){
			var my_ly =my_tl.layers[l];
			var fcountNum =my_ly.frameCount;
			if(my_ly.animationType=="none"){// this still can be classic tween
				for(var t=0;t<fcountNum;t++){
					var my_tw=my_ly.frames[t];
					if(my_tw.tweenType=="motion"){
						if(my_tw.duration>1){
							fl.trace("tween-type = "+my_tw.tweenType);
							//if(!doc.timelines[i].layers[l].frames[t].isEmpty){ 
							if(my_tw.elements.length==1){
									var waslocked=my_ly.locked;
									my_ly.locked = false;
									my_tl.setSelectedLayers(l);
									
									my_tl.setSelectedFrames(t, t+my_tw.duration); // 'f' is the frame number, start & end
									var file = fl.configURI + 'Javascript/MotionXML.jsfl';
									
									var newMotion=fl.runScript(file, 'copyMotion');
									collectallMotionsXML+="#$TWEEN$#CLASSICTWEEN TimeLine "+my_tl.name+" LayerIdx "+l+" frame "+t+ " duration "+my_tw.duration+"\n";
									//fl.trace(newMotion);
									collectallMotionsXML+=newMotion+"\n";
									animCounts++;
									my_ly.locked = waslocked;
							}
						}
					}
					if(my_tw.tweenType=="shape"){
						fl.trace("found shape");
					}
					else{
						// no tween found						
					}
						
					t+=my_tw.duration-1;
				}
				
			}
			if(my_ly.animationType=="motion object"){
				
				for(var t=0;t<fcountNum;t++){
					var my_tw=my_ly.frames[t];
					//fl.trace(my_tw.duration);
					//fl.trace(my_tw.hasMotionPath());
					if(my_tw.isMotionObject()){
						fl.trace("fcountNum = "+fcountNum+" t="+t);		
						collectallMotionsXML+="#$TWEEN$#MOTIONOBJECT TimeLine "+my_tl.name+" LayerIdx "+l+" frame "+t+ " duration "+my_tw.duration+"\n";
						//fl.trace(newMotion);
						motionXML=doc.timelines[i].layers[l].frames[t].getMotionObjectXML();
						//fl.trace("nodes num: " + motionXML.PropertyContainer.length());
						collectallMotionsXML+=motionXML+"\n";					
						//fl.trace("node 2 name: " + motionXML.;
						animCounts++;
									
					}
					t+=my_tw.duration-1;
					//fl.trace("fcountNum = "+fcountNum+" t="+t);
				}
				
			}
			
		}
	}
	return collectallMotionsXML;
}
function exportMotions() {
	var doc = fl.getDocumentDOM(); 
	var collectallMotionsXML="All Motions";
	var animCounts=0;
	for(var i=0; i<doc.timelines.length;i++){
		var my_tl = doc.timelines[i]; 
		var countNum = my_tl.frameCount;
		fl.trace(countNum); 
		for(var l=0;l<my_tl.layerCount;l++){
			var my_ly =my_tl.layers[l];
			var fcountNum =my_ly.frameCount;
			//fl.trace(fcountNum); 
			//fl.trace(my_ly.frames.length); 
			//fl.trace("Layer-Animation = "+my_ly.animationType); 
			if(my_ly.animationType=="none"){// this still can be classic tween
				for(var t=0;t<fcountNum;t++){
					var my_tw=my_ly.frames[t];
					if(my_tw.tweenType=="motion"){
						if(my_tw.duration>1){
							fl.trace("tween-type = "+my_tw.tweenType);
							//if(!doc.timelines[i].layers[l].frames[t].isEmpty){ 
							if(my_tw.elements.length==1){
									var waslocked=my_ly.locked;
									my_ly.locked = false;
									my_tl.setSelectedLayers(l);
									
									my_tl.setSelectedFrames(t, t+my_tw.duration); // 'f' is the frame number, start & end
									var file = fl.configURI + 'Javascript/MotionXML.jsfl';
									
									var newMotion=fl.runScript(file, 'copyMotion');
									collectallMotionsXML+="#$TWEEN$#CLASSICTWEEN TimeLine "+my_tl.name+" LayerIdx "+l+" frame "+t+ " duration "+my_tw.duration+"\n";
									//fl.trace(newMotion);
									collectallMotionsXML+=newMotion+"\n";
									animCounts++;
									my_ly.locked = waslocked;
							}
						}
					}
					if(my_tw.tweenType=="shape"){
						fl.trace("found shape");
					}
					else{
						// no tween found						
					}
						
					t+=my_tw.duration-1;
				}
				
			}
			if(my_ly.animationType=="motion object"){
				
				for(var t=0;t<fcountNum;t++){
					var my_tw=my_ly.frames[t];
					//fl.trace(my_tw.duration);
					//fl.trace(my_tw.hasMotionPath());
					if(my_tw.isMotionObject()){
						fl.trace("fcountNum = "+fcountNum+" t="+t);		
						collectallMotionsXML+="#$TWEEN$#MOTIONOBJECT TimeLine "+my_tl.name+" LayerIdx "+l+" frame "+t+ " duration "+my_tw.duration+"\n";
						//fl.trace(newMotion);
						motionXML=doc.timelines[i].layers[l].frames[t].getMotionObjectXML();
						//fl.trace("nodes num: " + motionXML.PropertyContainer.length());
						collectallMotionsXML+=motionXML+"\n";					
						//fl.trace("node 2 name: " + motionXML.;
						animCounts++;
									
					}
					t+=my_tw.duration-1;
					//fl.trace("fcountNum = "+fcountNum+" t="+t);
				}
				
			}
			
		}
	}
	return collectallMotionsXML;
}

