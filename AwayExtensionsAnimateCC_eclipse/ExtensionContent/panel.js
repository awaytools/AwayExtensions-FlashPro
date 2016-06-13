var csInterface;
var globalColor = 'colorThemeCSS';
var doc_is_valid=true;

var panel_state="importAS2";

var r = /[^\/]*$/;
var awayjsfl_script_path=document.URL.replace(r, '')+"AwayJSFL.jsfl";

var preview_custom_source = '';
var preview_output_path = '';
var preview_append_name = false;
var file_name = '';

$( document ).ready(function() {

    csInterface = new CSInterface();


    var skinInfo = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
    var styleSheets= document.styleSheets;
    ChangePanelTheme(skinInfo);

    //Light and dark theme change
    csInterface.addEventListener(CSInterface.THEME_COLOR_CHANGED_EVENT, onAppThemeColorChanged);

    csInterface.addEventListener("com.adobe.events.flash.documentChanged",  changed_document);
    csInterface.addEventListener("com.adobe.events.flash.timelineChanged",  changed_timeline);
    //csInterface.addEventListener("com.adobe.events.flash.documentSaved",  changed_savedoc);
    //csInterface.addEventListener("com.adobe.events.flash.documentOpened",  changed_opendoc);
    csInterface.addEventListener("com.adobe.events.flash.documentClosed",  changed_closedoc);
    //csInterface.addEventListener("com.adobe.events.flash.documentNew",  changed_newdoc);
    csInterface.addEventListener("com.adobe.events.flash.layerChanged",  changed_layer);
    csInterface.addEventListener("com.adobe.events.flash.frameChanged",  changed_frame);
    csInterface.addEventListener("com.adobe.events.flash.selectionChanged",  changed_selection);
    csInterface.addEventListener("com.adobe.events.flash.mouseMove",  changed_mouse);

    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'hasValidDoc');", hasDocumentCallback);
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'getFrameScript');", getFrameScriptCallback);

    // smaller header when scrolling:
    document.getElementById('scrollable_content').addEventListener('scroll', function(e){
        var distanceY = 0,
            shrinkOn = 0,
            scrollable_content = document.getElementById("scrollable_content"),
            logo = document.getElementById("logo"),
            header = document.getElementById("header");
        distanceY = scrollable_content.scrollTop;
        if (distanceY > shrinkOn) {
            logo.className="smaller";
            header.className="smaller";
        } else {
            logo.className="";
            header.className="";
        }
    });
    set_panelstate();
});

function openConverter(){
    panel_state="importAS2";
    set_panelstate();
}
function openBatchExporter(){
    panel_state="batchExporter";
    set_panelstate();
}
function set_panelstate(){
    var html_str="";
    var btn_converter=document.getElementById("panel_converter");
    var btn_batch_export=document.getElementById("panel_batch_export");
    btn_converter.className="awayjs_button";
    btn_batch_export.className="awayjs_button";
    if(panel_state=="importAS2"){
        btn_converter.className="awayjs_button_active";
        html_str+="<div style='width:100%;height:85px;'></div>";
        html_str+='<div class="ui_comp_double">\
            <label class="myCHBXLabel2" for="inputFileAS2">Input XFL File:</label>\
        <input type="text" id="inputFileAS2">\
            <button class="awayjs_button" id="inputFileAS2Browse" onclick="opsd1()"><span class="glyphicon glyphicon-folder-open"></span></button>\
        </div>';
        html_str+='<div class="ui_comp_double">\
            <label class="myCHBXLabel2" for="outputFileAS2">Output Folder:</label>\
        <input type="text" id="outputFileAS2">\
            <button class="awayjs_button" id="outputFileAS2Browse" onclick="opsd2()"><span class="glyphicon glyphicon-folder-open"></span></button>\
        </div>';
        html_str+='<div class="clearer"><div class="ui_comp_single2"><button class="awayjs_button" onclick="onCS6_AS2_XFL()">Import CS6 AS2 xfl</button></div></div>';
    }
    else if(panel_state=="batchExporter"){
        btn_batch_export.className="awayjs_button_active";
        html_str+="<div style='width:100%;height:85px;'></div>";
        html_str+='<div class="clearer"><div class="ui_comp_single2"><button class="awayjs_button" onclick="onStartBatchExport()">Start Batch Export</button></div></div>';


    }
    document.getElementById("scrollable_content_inner").innerHTML=html_str;
}
function onStartBatchExport(){
    document.getElementById("overlay_loading").className="overlay_active";
    document.getElementById("overlay_loading").innerHTML='<img id="loader" src="./ressources/loading_spinner.gif" alt="Loading...">';

    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'batchExport');", batchExportCallback);
}
function batchExportCallback(){

    document.getElementById("overlay_loading").className="overlayactive";
    document.getElementById("overlay_loading").innerHTML="";
}

function onCS6_AS2_XFL()
{
    var javaURL = "/usr/bin/java";
    if ("Win32" == navigator.platform || "Windows" == navigator.platform) {

        javaURL="C:/ProgramData/Oracle/Java/javapath/java.exe";
    }

    var jarURL=csInterface.getSystemPath(SystemPath.EXTENSION)+"/AS2JS.jar";

    var inURL=document.getElementById("inputFileAS2").value;
    if(inURL.length<5){
        alert("No valid path");
        return;
    }
    var outURL=document.getElementById("outputFileAS2").value;
    if(outURL.length<5){
        var allpathes=inURL.split("/");
        outURL="";
        for(var i=0; i<allpathes.length-2;i++){
            outURL+=allpathes[i]+"/";
        }
        outURL+=allpathes[allpathes.length-2]+"_AwayJS";
    }
    var outPathes=outURL.split("/");
    var filename=outPathes[outPathes.length-1];
    evalScript("fl.trace('javaURL " + javaURL +"');");
    evalScript("fl.trace('jarURL " + jarURL +"');");
    evalScript("fl.trace('inURL " + inURL +"');");
    evalScript("fl.trace('outURL " + outURL +"');");
    //var check=testURL4.find(".xfl");
    //var testURL4=csInterface.getSystemPath(SystemPath.EXTENSION)+"/Icycle1_AwayJS";
    //var testURL5=csInterface.getSystemPath(SystemPath.EXTENSION)+"/Icycle1/Icycle1.xfl";
    //var result = window.cep.process.createProcess("C:/eclipse-inst-win64.exe");
    //evalScript("fl.trace('" + result.data+" / "+result.err+"');");
    var result = window.cep.process.createProcess(javaURL, "-jar", jarURL, "-d", outURL, inURL);
    evalScript("fl.trace('" + result.data+" / "+result.err+"');");

    var pid = result.data;

    var stringer = "";
    function getSTDOutput()
    {
        window.cep.process.stdout(pid, function(output) {
            var allstr=output.split(/\r?\n|\r/g);
            var startIdx=0;
            if(stringer!=""){
                evalScript("fl.trace('" + stringer+allstr[0].replace(/\\/g, " | ") + "');");
                startIdx+=1;
                stringer="";
            }
            for(var i=startIdx; i<allstr.length;i++){
                if(allstr[i][0]=="C"){
                    if(allstr[i][allstr[i].length-1]=="E") {
                        evalScript("fl.trace('" + allstr[i].replace(/\\/g, " | ") + "');");
                    }
                    else{
                        stringer=allstr[i].replace(/\\/g, " | ");
                    }
                }
                else{
                    evalScript("fl.trace('" + allstr[i].replace(/\\/g, " | ") + "');");
                }

            }
        });
        var result = window.cep.process.isRunning(pid);
        if (result.data == true)
        {
            setTimeout(getSTDOutput, 1000);
        }
    }
    getSTDOutput();
    window.cep.process.onquit(pid,function(){
        evalScript("fl.openDocument(FLfile.platformPathToURI('"+outURL+"/"+filename+".xfl'));", loading_done_callback);

    });
    var result = window.cep.process.isRunning(pid);
    if (result.data == true)
    {
        document.getElementById("overlay_loading").className="overlay_active";
        document.getElementById("overlay_loading").innerHTML='<img id="loader" src="./ressources/loading_spinner.gif" alt="Loading...">';
    }
    else{
        alert("Could not start the java-process");
        return;
    }


}


function hasDocumentCallback(e)
{
    //evalScript("fl.trace('hasDocumentCallback "+e+"');");
    if(e=="true"){
        if(panel_state=="importAS2") {
            document.getElementById("overlay_no_doc").innerHTML = "";
            document.getElementById("scrollable_content").style.opacity = 1;
        }
        document.getElementById("publish_panel").disabled = false;
        document.getElementById("publish_panel").style.opacity = 1;
        document.getElementById("ok_panel").disabled = false;
        document.getElementById("ok_panel").style.opacity = 1;
    }
    else{
        if(panel_state!="importAS2") {
            document.getElementById("overlay_no_doc").innerHTML = "No valid Away Document opened";
            document.getElementById("scrollable_content").style.opacity = 0.3;
        }
        document.getElementById("publish_panel").disabled = true;
        document.getElementById("publish_panel").style.opacity = 0.3;
        document.getElementById("ok_panel").disabled = true;
        document.getElementById("ok_panel").style.opacity = 0.3;
    }
}
function keyEvent(event) {
    var key = event.keyCode || event.which;
    var keychar = String.fromCharCode(key);
    evalScript("fl.trace('Combination of metaKey + " + keychar+"');");
}

function metaKeyUp (event) {
    var key = event.keyCode || event.which;
    evalScript("fl.trace('Combination of metaKey + " + key+"');");
}
function getFrameScriptCallback(e)
{
    if(e==""){
        document.getElementById("script_container").innerHTML="";
    }
    else{
        document.getElementById("script_container").innerHTML=e;
        hljs.highlightBlock(document.getElementById("script_container"))
    }
}
function changed_document(e)
{
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'hasValidDoc');", hasDocumentCallback);
}
function changed_closedoc(e)
{
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'hasValidDoc');", hasDocumentCallback);
}
function changed_timeline(e)
{
    evalScript("fl.trace('changed_timeline "+e+"');");
}
function changed_layer(e)
{
    evalScript("fl.trace('changed_layer "+e+"');");
}
function changed_frame(e)
{
    evalScript("fl.trace('changed_frame "+e+"');");
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'getFrameScript');", getFrameScriptCallback);
}
function changed_selection(e)
{
    evalScript("fl.trace('changed_selection "+e+"');");
}
function changed_mouse(e)
{
    //evalScript("fl.trace('changed_mouse "+e+"');");
}
function setCheckBoxFromSettings(checkBoxName, settingsName, settings, defaultvalue)
{
    document.getElementById(checkBoxName).checked = defaultvalue;
    var defaultValueStr=""+defaultvalue;
    if(settings[settingsName] != null && settings[settingsName] != "undefined")
    {
        if(settings[settingsName]!== defaultValueStr){
            document.getElementById(checkBoxName).checked = !defaultvalue;
        }
    }
}
function enableUIElements(event_obj)
{
    var panel_names=[];
    panel_names.push("exportTimelines");
    panel_names.push("exportGeometries");
    panel_names.push("exportFonts");
    panel_names.push("exportBitmaps");
    panel_names.push("exportSounds");

    var target_panel=document.getElementById(event_obj.id+"_panel");
    if(event_obj.checked && target_panel.className!="panel-collapse collapse in"){
        document.getElementById(event_obj.id+"_open").click();
    }
    else if(!event_obj.checked && target_panel.className!="panel-collapse collapse"){
        document.getElementById(event_obj.id+"_open").click();
    }
    for(var i=0; i<panel_names.length; i++) {
        if (document.getElementById(panel_names[i]).checked) {
            document.getElementById(panel_names[i]+"_settings").className = "panel-body";
        }
        else {
            document.getElementById(panel_names[i]+"_settings").className = "panel-body disabled";
        }
    }
}

function writeCheckboxToSettings(checkBoxName, settingsName, settings)
{
    settings[settingsName] = "false";
    if(document.getElementById(checkBoxName).checked)
        settings[settingsName] = "true";
}
function serializeUI()
{
    var event = new CSEvent();
    var pubSettings = new Object();

    // general settings:
    pubSettings["out_file"] = document.getElementById("of").value.toString();

    writeCheckboxToSettings("exportTimelines", "PublishSettings.ExportTimelines", pubSettings);
    writeCheckboxToSettings("exportGeometries", "PublishSettings.ExportGeometries", pubSettings);
    writeCheckboxToSettings("exportFonts", "PublishSettings.ExportFonts", pubSettings);
    writeCheckboxToSettings("exportBitmaps", "PublishSettings.ExportBitmaps", pubSettings);
    writeCheckboxToSettings("exportSounds", "PublishSettings.ExportSounds", pubSettings);

    writeCheckboxToSettings("usePreview", "PublishSettings.OpenPreview", pubSettings);
    writeCheckboxToSettings("copyRuntime", "PublishSettings.CopyRuntime", pubSettings);
    writeCheckboxToSettings("createLogfile", "PublishSettings.PrintExportLog", pubSettings);

    //pubSettings["PublishSettings.PreviewPath"] = document.getElementById("preview_path").value.toString();
    //pubSettings["PublishSettings.PreviewPathSource"] = document.getElementById("preview_source_path").value.toString();

    /*
     if(document.getElementById("includeVisiblelayers").checked == true)
     {
     pubSettings["PublishSettings.IncludeInvisibleLayer"] = "true";
     }
     else
     {
     pubSettings["PublishSettings.IncludeInvisibleLayer"] = "false";
     }
     */
    /*
     if(document.getElementById("appendFilename").checked == true)
     {
     pubSettings["PublishSettings.AppendFilename"] = "true";
     }
     else
     {
     pubSettings["PublishSettings.AppendFilename"] = "false";
     }
     */
    /*
     if(document.getElementById("exportLibSounds").checked == true)
     {
     pubSettings["PublishSettings.ExportLibSounds"] = "true";
     }
     else
     {
     pubSettings["PublishSettings.ExportLibSounds"] = "false";
     }

     if(document.getElementById("exportLibBitmaps").checked == true)
     {
     pubSettings["PublishSettings.ExportLibBitmaps"] = "true";
     }
     else
     {
     pubSettings["PublishSettings.ExportLibBitmaps"] = "false";
     }


     if(document.getElementById("embbed_sounds").checked == true)
     {
     pubSettings["PublishSettings.EmbbedSounds"] = "true";
     }
     else
     {
     pubSettings["PublishSettings.EmbbedSounds"] = "false";
     }


     if(document.getElementById("exportFrameScript").checked == true)
     {
     pubSettings["PublishSettings.ExportFrameScript"] = "true";
     }
     else
     {
     pubSettings["PublishSettings.ExportFrameScript"] = "false";
     }
     if(document.getElementById("embbed_textures").checked == true)
     {
     pubSettings["PublishSettings.EmbbedTextures"] = "true";
     }
     else
     {
     pubSettings["PublishSettings.EmbbedTextures"] = "false";
     }
     if(document.getElementById("exportLibFonts").checked == true)
     {
     pubSettings["PublishSettings.ExportLibFonts"] = "true";
     }
     else
     {
     pubSettings["PublishSettings.ExportLibFonts"] = "false";
     }


     if(document.getElementById("exportShapesInDebugMode").checked == true)
     {
     pubSettings["PublishSettings.ExportShapesInDebugMode"] = "true";
     }
     else
     {
     pubSettings["PublishSettings.ExportShapesInDebugMode"] = "false";
     }

     pubSettings["PublishSettings.SaveSoundsAs"] = document.getElementById("sound_type").selectedIndex;
     pubSettings["PublishSettings.RadialGradientSize"] = document.getElementById("radialGradientSize").selectedIndex;
     pubSettings["PublishSettings.TimelineDepth"] = document.getElementById("timelineDepth").selectedIndex;
     pubSettings["PublishSettings.FontData"] = document.getElementById("fontData").selectedIndex;
     pubSettings["PublishSettings.GeometryData"] = document.getElementById("geometryData").selectedIndex;
     */
    event.scope = "APPLICATION";
    event.type = "com.adobe.events.flash.extension.savestate";
    event.data = JSON.stringify(pubSettings);
    event.extensionId = "AwayExtensionsAnimateCC.PublishSettings";
    csInterface.dispatchEvent(event);

}

function setUI(uiState)
{
    setUIFromData(uiState.data);
}
function setUIFromData(uiState)
{
    // general settings:
    if(uiState.out_file !=null && uiState.out_file != undefined)
    {
        document.getElementById("of").value = uiState.out_file;
        //set_fileName(uiState.out_file);
    }
    setCheckBoxFromSettings("exportTimelines", "PublishSettings.ExportTimelines", uiState, true);
    setCheckBoxFromSettings("exportGeometries", "PublishSettings.ExportGeometries", uiState, true);
    setCheckBoxFromSettings("exportFonts", "PublishSettings.ExportFonts", uiState, true);
    setCheckBoxFromSettings("exportBitmaps", "PublishSettings.ExportBitmaps", uiState, true);
    setCheckBoxFromSettings("exportSounds", "PublishSettings.ExportSounds", uiState, true);

    setCheckBoxFromSettings("usePreview", "PublishSettings.OpenPreview", uiState, true);
    setCheckBoxFromSettings("copyRuntime", "PublishSettings.CopyRuntime", uiState, true);
    setCheckBoxFromSettings("createLogfile", "PublishSettings.PrintExportLog", uiState, false);


    /*
     if(uiState.data["PublishSettings.PreviewPath"] != null && uiState.data["PublishSettings.PreviewPath"] != "undefined")
     {
     document.getElementById("preview_path").value = uiState.data["PublishSettings.PreviewPath"];

     }
     if(uiState.data["PublishSettings.PreviewPathSource"] != null && uiState.data["PublishSettings.PreviewPathSource"] != "undefined")
     {
     document.getElementById("preview_source_path").value = uiState.data["PublishSettings.PreviewPathSource"];

     }*/
/*
    if(uiState.data["PublishSettings.IncludeInvisibleLayer"] != null && uiState.data["PublishSettings.IncludeInvisibleLayer"] != "undefined")
    {
        if(uiState.data["PublishSettings.IncludeInvisibleLayer"] == "true")
        {
            document.getElementById("includeVisiblelayers").checked = true;
        }else if (uiState.data["PublishSettings.IncludeInvisibleLayer"] == "false")
        {
            document.getElementById("includeVisiblelayers").checked = false;
        }
    }

    if(uiState.data["PublishSettings.SaveSoundsAs"] != null && uiState.data["PublishSettings.SaveSoundsAs"] != "undefined")
    {
        document.getElementById("sound_type").selectedIndex = uiState.data["PublishSettings.SaveSoundsAs"];
    }
    if (uiState.data["PublishSettings.GeometryData"] != null && uiState.data["PublishSettings.GeometryData"] != "undefined") {
        document.getElementById("geometryData").selectedIndex = uiState.data["PublishSettings.GeometryData"];
    }
    if (uiState.data["PublishSettings.FontData"] != null && uiState.data["PublishSettings.FontData"] != "undefined") {
        document.getElementById("fontData").selectedIndex = uiState.data["PublishSettings.FontData"];
    }
    if (uiState.data["PublishSettings.TimelineDepth"] != null && uiState.data["PublishSettings.TimelineDepth"] != "undefined") {
        document.getElementById("timelineDepth").selectedIndex = uiState.data["PublishSettings.TimelineDepth"];
    }

    switchPreviewOnOff(document.getElementById("usePreview"));

    if(uiState.data["PublishSettings.DistinglishExteriorAndinterior"] != null && uiState.data["PublishSettings.DistinglishExteriorAndinterior"] != "undefined")
    {
        if(uiState.data["PublishSettings.DistinglishExteriorAndinterior"] == "true")
        {
            document.getElementById("distinglishExteriorAndinterior").checked = true;
        }else
        {
            document.getElementById("distinglishExteriorAndinterior").checked = false;
        }
    }

    if(uiState.data["PublishSettings.ExportShapesInDebugMode"] != null && uiState.data["PublishSettings.ExportShapesInDebugMode"] != "undefined")
    {
        if(uiState.data["PublishSettings.ExportShapesInDebugMode"] == "true")
        {
            document.getElementById("exportShapesInDebugMode").checked = true;
        }else
        {
            document.getElementById("exportShapesInDebugMode").checked = false;
        }
    }
*/
    /*
     if(uiState.data["PublishSettings.AppendFilename"] != null && uiState.data["PublishSettings.AppendFilename"] != "undefined")
     {
     if(uiState.data["PublishSettings.AppendFilename"] == "true")
     {
     document.getElementById("appendFilename").checked = true;
     }else
     {
     document.getElementById("appendFilename").checked = false;
     }
     }
     */
    /*
    if(uiState.data["PublishSettings.ExportLibSounds"] != null && uiState.data["PublishSettings.ExportLibSounds"] != "undefined")
    {
        if(uiState.data["PublishSettings.ExportLibSounds"] == "true")
        {
            document.getElementById("exportLibSounds").checked = true;
        }else
        {
            document.getElementById("exportLibSounds").checked = false;
        }
    }
    if(uiState.data["PublishSettings.ExportLibBitmaps"] != null && uiState.data["PublishSettings.ExportLibBitmaps"] != "undefined")
    {
        if(uiState.data["PublishSettings.ExportLibBitmaps"] == "true")
        {
            document.getElementById("exportLibBitmaps").checked = true;
        }else
        {
            document.getElementById("exportLibBitmaps").checked = false;
        }
    }
    if(uiState.data["PublishSettings.EmbbedSounds"] != null && uiState.data["PublishSettings.EmbbedSounds"] != "undefined")
    {
        if(uiState.data["PublishSettings.EmbbedSounds"] == "true")
        {
            document.getElementById("embbed_sounds").checked = true;
        }else
        {
            document.getElementById("embbed_sounds").checked = false;
        }
    }
    if(uiState.data["PublishSettings.ExportFrameScript"] != null && uiState.data["PublishSettings.ExportFrameScript"] != "undefined")
    {
        if(uiState.data["PublishSettings.ExportFrameScript"] == "true")
        {
            document.getElementById("exportFrameScript").checked = true;
        }else
        {
            document.getElementById("exportFrameScript").checked = false;
        }
    }
    if(uiState.data["PublishSettings.EmbbedTextures"] != null && uiState.data["PublishSettings.EmbbedTextures"] != "undefined")
    {
        if(uiState.data["PublishSettings.EmbbedTextures"] == "true")
        {
            document.getElementById("embbed_textures").checked = true;
        }else
        {
            document.getElementById("embbed_textures").checked = false;
        }
    }
    */

    //var test = csInterface.closeExtension();
    //evalScript("fl.runScript('"+awayjsfl_script_path+"', 'change_preview_settings', 'test1', 'test2', 'test3', 'test4');", read_preview_settings);
    enableUIElements();
}


function read_preview_settings(val) {

    evalScript("fl.trace('"+val+"');")
}

function importFrameScriptsFromJS(event) {
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'importFrameScriptsFromJS');");
}
function open_advanced(event) {
    // create an XUL with JSFL
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'change_preview_settings', 'test1', 'test2', 'test3', 'test4');", read_preview_settings);
}
function show_open_preview_settings(event) {
    // create an XUL with JSFL
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'change_preview_settings', 'test1', 'test2', 'test3', 'test4');", read_preview_settings);
}
function isNumber(event) {
    if (event) {
        var charCode = (event.which) ? event.which : event.keyCode;
        if (charCode != 190 && charCode > 31 &&
            (charCode < 48 || charCode > 57) &&
            (charCode < 96 || charCode > 105) &&
            (charCode < 37 || charCode > 40) &&
            (charCode < 188 || charCode > 190) &&
            charCode != 110 && charCode != 8 && charCode != 46 )
            return false
    }
    return true;
}

function minmax(value, min, max)
{
    if(parseFloat(value) < min || isNaN(value))
        return min;
    else if(parseFloat(value) > max)
        return max;
    else return value;
}


function checkvalue() {
    var mystring = document.getElementById('of').value;
    if(!mystring.match(/\S/)) {
        alert ('Output file path cannot be empty');
        return false;
    } else {
        return true;
    }
}

function onJSFLFinish(val)  {
    //alert (val);
    evalScript("fl.trace('"+val.toString()+"');");
}
function testUIResult(val)  {
    //alert (val);
    evalScript("fl.trace('"+val+"');");
}

function onGettestUI()  {
    var r = /[^\/]*$/;
    var filePath=document.URL;
    var test=filePath.replace(r, '');
    var test2=test+"test_ui.xml";

    evalScript("var xpanel = fl.getDocumentDOM().xmlPanel('"+test2+"'); fl.xmlui.get('separator');", testUIResult);
    // evalScript("fl.runScript('"+test2+"', 'exportMotions');", onJSFLFinish);
}
function onGetTimeLineInfos()  {
    var r = /[^\/]*$/;
    var filePath=document.URL;
    var test=filePath.replace(r, '');
    var test2=test+"AwayJSFL.jsfl";
    evalScript("fl.trace('"+test2+"');");
    evalScript("fl.runScript('"+test2+"', 'exportMotions');", onJSFLFinish);
}
function onGetDocInfos()  {
    var r = /[^\/]*$/;
    var filePath=document.URL;
    var test=filePath.replace(r, '');
    var test2=test+"AwayJSFL.jsfl";
    evalScript("fl.trace('"+test2+"');");
    evalScript("fl.runScript('"+test2+"', 'exportMotions');", onJSFLFinish);
}
function onGetSceneInfos()  {
    var r = /[^\/]*$/;
    var filePath=document.URL;
    var test=filePath.replace(r, '');
    var test2=test+"AwayJSFL.jsfl";
    //evalScript("fl.trace('"+test2+"');");
    evalScript("fl.runScript('"+test2+"', 'exportMotions');", onJSFLFinish);
}

function onHelp()  {
    alert ('No Help document is provided yet.');
}
function populate_textfield(val)
{
    document.getElementById("of").value = val;
    set_fileName(val);
}
function populate_textfield1(val)
{
    document.getElementById("inputFileAS2").value = val;
}
function populate_textfield2(val)
{
    document.getElementById("outputFileAS2").value = val;
}


function set_fileName(val)
{
    var filename = val.replace(/^.*[\\\/]/, '');
    filename = filename.replace("awd", "html");
    document.getElementById("previewPathName").value = "/"+filename;
}






function evalScript(script, callback) {
    csInterface.evalScript(script, callback);
}
function open_callback(pathVal)
{
    evalScript("FLfile.uriToPlatformPath('"+pathVal+"');",populate_textfield);
}
function open_callback1(pathVal)
{
    evalScript("FLfile.uriToPlatformPath('"+pathVal+"');",populate_textfield1);
}
function open_callback2(pathVal)
{
    evalScript("FLfile.uriToPlatformPath('"+pathVal+"');",populate_textfield2);
}

function ChangePanelTheme(skinInfo){
    var darkTheme = (skinInfo.appBarBackgroundColor.color.blue < 128)
    var head  = document.getElementsByTagName('head')[0];
    //load the CSS for App theme
    var loadedCSS = document.getElementById(globalColor);
    if (loadedCSS)
        loadedCSS.parentNode.removeChild(loadedCSS);
    var link  = document.createElement('link');
    link.rel  = 'stylesheet';
    link.type = 'text/css';
    link.media = 'all';
    if(darkTheme)
        link.href = 'css/mkDroverPanel_D.css';
    else
        link.href = 'css/mkDroverPanel_L.css';
    link.id = globalColor;
    head.appendChild(link);
    var styleSheets= document.styleSheets;
    if(styleSheets.length > 0)
    {
        for(var i=0;i<styleSheets.length;i++)
        {
            styleSheets.item(i).addRule("*",'font-family: "' + skinInfo.baseFontFamily + '";font-size:11px',0);
        }
    }

}

function onAppThemeColorChanged(event) {
    // Should get a latest HostEnvironment object from application.
    //var skinInfo = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
    // Gets the style information such as color info from the skinInfo,
    // and redraw all UI controls of your extension according to the style info.
    var skinInfo = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
    ChangePanelTheme(skinInfo);
}


function open_custom_preview_source_callback(pathVal)
{

    evalScript("FLfile.uriToPlatformPath('"+pathVal+"');",populate_textfield2);

}

function getPublisherSettings() {
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'getPublisherSettings');");
}
function opsd() {
    evalScript("fl.browseForFileURL('save','Publish to AWD','AWD','awd');",open_callback);
}
function opsd1() {
    evalScript("fl.browseForFileURL('open','Select a CS6 AS2 XFL','XFL','xfl');",open_callback1);
}
function opsd2() {
    evalScript("fl.browseForFolderURL('Save as CC AwayDocument XFL');",open_callback2);
}
function openBatchBrowser() {
    evalScript("fl.browseForFolderURL('Input directory');",openBatchBrowser_callback);
}
function openBatchBrowser_callback(pathVal)
{
    evalScript("FLfile.uriToPlatformPath('"+pathVal+"');",openBatchBrowser_populate_textfield);
}
function openBatchBrowser_populate_textfield(val)
{
    document.getElementById("inputBatchFolder").value = val;
}
function openBatchBrowserOutput() {
    evalScript("fl.browseForFolderURL('Output directory');",openBatchBrowseroutput_callback);
}
function openBatchBrowseroutput_callback(pathVal)
{
    evalScript("FLfile.uriToPlatformPath('"+pathVal+"');",openBatchBrowseroutput_populate_textfield);
}
function openBatchBrowseroutput_populate_textfield(val)
{
    document.getElementById("outputBatchFolder").value = val;
}

function opcustum_prev() {
    evalScript("fl.browseForFolderURL('Select folder that contains the publisher.html');",open_custom_preview_source_callback);
}

function loading_done_callback()
{
    document.getElementById("overlay_loading").className="overlayactive";
    document.getElementById("overlay_loading").innerHTML="";
    //csInterface.closeExtension();
}


function onPublish() {

    document.getElementById("overlay_loading").className="overlay_active";
    document.getElementById("overlay_loading").innerHTML='<img id="loader" src="./ressources/loading_spinner.gif" alt="Loading...">';
    //serializeUI();
    evalScript("fl.getDocumentDOM().publish();", loading_done_callback);

}
function openPublisher()  {
    //if(!doc_is_valid){
        csInterface.requestOpenExtension("com.away.AwayExtensionsAnimateCC.PublishSettings", null);
   // }
}
function onCancel()  {
    csInterface.closeExtension();
}