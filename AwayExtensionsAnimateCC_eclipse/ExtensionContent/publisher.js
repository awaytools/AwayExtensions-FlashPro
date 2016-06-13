var csInterface;
var globalColor = 'colorThemeCSS';

var r = /[^\/]*$/;
var awayjsfl_script_path=document.URL.replace(r, '')+"AwayJSFL.jsfl";

var preview_custom_source = '';
var preview_output_path = '';
var preview_append_name = false;
var file_name = '';

$( document ).ready(function() {

    csInterface = new CSInterface();

/*
    document.getElementById("includeVisiblelayers").checked=true;
    document.getElementById("usePreview").checked=true;
    document.getElementById("exportShapesInDebugMode").checked=false;

    document.getElementById("exportLibFonts").checked=true;
    document.getElementById("embbed_sounds").checked=true;
    document.getElementById("embbed_textures").checked=true;
    document.getElementById("exportFrameScript").checked=true;
    document.getElementById("appendFilename").checked=false;
    document.getElementById("exportLibBitmaps").checked=true;
    document.getElementById("exportLibSounds").checked=true;
    document.getElementById("printExportLog").checked=true;
    document.getElementById("sound_type").selectedIndex = "2";
    document.getElementById("radialGradientSize").selectedIndex = "2";
    document.getElementById("timelineDepth").selectedIndex = "0";
    document.getElementById("fontData").selectedIndex = "0";
    document.getElementById("geometryData").selectedIndex = "0";
*/

    var skinInfo = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
    var styleSheets= document.styleSheets;
    ChangePanelTheme(skinInfo);
    document.title="AWD Publish Settings";
    //Light and dark theme change
    csInterface.addEventListener(CSInterface.THEME_COLOR_CHANGED_EVENT, onAppThemeColorChanged);

    csInterface.addEventListener("com.adobe.events.flash.extension.setstate", setUI);
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'getPublisherSettings');", getPublishSettingsCallback);


    var event = new CSEvent();
    event.scope = "APPLICATION";
    event.type = "com.adobe.events.flash.extensionLoaded";
    event.data = "Test Event";
    event.extensionId = "com.away.AwayExtensionsAnimateCC.PublishSettings";
    csInterface.dispatchEvent(event);
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
});


function getPublishSettingsCallback(result) {
    var data={};
    var allprops=result.split("###");
    for(var i=0; i<allprops.length-1; i++){
        var oneProp=allprops[i].split("=");
        if(oneProp.length==2){
            data[oneProp[0]]=oneProp[1];
        }
    }
    setUIFromData(data);
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


    writeCheckboxToSettings("usePreview", "PublishSettings.OpenPreview", pubSettings);
    writeCheckboxToSettings("copyRuntime", "PublishSettings.CopyRuntime", pubSettings);
    writeCheckboxToSettings("createLogfile", "PublishSettings.PrintExportLog", pubSettings);

    //writeCheckboxToSettings("exportTimelines", "PublishSettings.ExportTimelines", pubSettings);
    writeCheckboxToSettings("includeVisiblelayers", "PublishSettings.IncludeInvisibleLayer", pubSettings);
    writeCheckboxToSettings("exportFrameScript", "PublishSettings.ExportFrameScript", pubSettings);
    writeCheckboxToSettings("exportExternalFrameScript", "PublishSettings.ExternalScripts", pubSettings);
    writeCheckboxToSettings("printExportLogTimeline", "PublishSettings.PrintExportLogTimelines", pubSettings);

    //writeCheckboxToSettings("exportGeometries", "PublishSettings.ExportGeometries", pubSettings);
    writeCheckboxToSettings("exportShapesInDebugMode", "PublishSettings.ExportShapesInDebugMode", pubSettings);

    //writeCheckboxToSettings("exportFonts", "PublishSettings.ExportFonts", pubSettings);
    writeCheckboxToSettings("exportLibFonts", "PublishSettings.ExportLibFonts", pubSettings);
    writeCheckboxToSettings("embbedAllChars", "PublishSettings.EmbbedAllChars", pubSettings);

    //writeCheckboxToSettings("exportBitmaps", "PublishSettings.ExportBitmaps", pubSettings);
    writeCheckboxToSettings("forceTextureOverwrite", "PublishSettings.ForceTextureOverwrite", pubSettings);
    writeCheckboxToSettings("embbed_textures", "PublishSettings.EmbbedTextures", pubSettings);
    writeCheckboxToSettings("exportLibBitmaps", "PublishSettings.ExportLibBitmaps", pubSettings);

    //writeCheckboxToSettings("exportSounds", "PublishSettings.ExportSounds", pubSettings);
    writeCheckboxToSettings("forceSoundOverwrite", "PublishSettings.ForceSoundOverwrite", pubSettings);
    writeCheckboxToSettings("embbed_sounds", "PublishSettings.EmbbedSounds", pubSettings);
    writeCheckboxToSettings("exportLibSounds", "PublishSettings.ExportLibSounds", pubSettings);
    writeCheckboxToSettings("exportTimelineSounds", "PublishSettings.ExportTimelineSounds", pubSettings);
    writeCheckboxToSettings("createAudioMap", "PublishSettings.CreateAudioMap", pubSettings);

    pubSettings["PublishSettings.SaveSoundsAs"] = document.getElementById("sound_type").selectedIndex;

    //pubSettings["PublishSettings.PreviewPath"] = document.getElementById("preview_path").value.toString();
    //pubSettings["PublishSettings.PreviewPathSource"] = document.getElementById("preview_source_path").value.toString();


    event.scope = "APPLICATION";
    event.type = "com.adobe.events.flash.extension.savestate";
    event.data = JSON.stringify(pubSettings);
    event.extensionId = "com.away.AwayExtensionsAnimateCC.PublishSettings";
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

    setCheckBoxFromSettings("usePreview", "PublishSettings.OpenPreview", uiState, true);
    setCheckBoxFromSettings("copyRuntime", "PublishSettings.CopyRuntime", uiState, true);
    setCheckBoxFromSettings("createLogfile", "PublishSettings.PrintExportLog", uiState, false);

    //setCheckBoxFromSettings("exportTimelines", "PublishSettings.ExportTimelines", uiState, true);
    setCheckBoxFromSettings("includeVisiblelayers", "PublishSettings.IncludeInvisibleLayer", uiState, true);
    setCheckBoxFromSettings("exportFrameScript", "PublishSettings.ExportFrameScript", uiState, true);
    setCheckBoxFromSettings("exportExternalFrameScript", "PublishSettings.ExternalScripts", uiState, true);
    setCheckBoxFromSettings("printExportLogTimeline", "PublishSettings.PrintExportLogTimelines", uiState, false);

    //setCheckBoxFromSettings("exportGeometries", "PublishSettings.ExportGeometries", uiState, true);
    setCheckBoxFromSettings("exportShapesInDebugMode", "PublishSettings.ExportShapesInDebugMode", uiState, false);

    //setCheckBoxFromSettings("exportFonts", "PublishSettings.ExportFonts", uiState, true);
    setCheckBoxFromSettings("exportLibFonts", "PublishSettings.ExportLibFonts", uiState, true);
    setCheckBoxFromSettings("embbedAllChars", "PublishSettings.EmbbedAllChars", uiState, true);


    //setCheckBoxFromSettings("exportBitmaps", "PublishSettings.ExportBitmaps", uiState, true);
    setCheckBoxFromSettings("forceTextureOverwrite", "PublishSettings.ForceTextureOverwrite", uiState, true);
    setCheckBoxFromSettings("embbed_textures", "PublishSettings.EmbbedTextures", uiState, true);
    setCheckBoxFromSettings("exportLibBitmaps", "PublishSettings.ExportLibBitmaps", uiState, true);

    //setCheckBoxFromSettings("exportSounds", "PublishSettings.ExportSounds", uiState, true);
    setCheckBoxFromSettings("forceSoundOverwrite", "PublishSettings.ForceSoundOverwrite", uiState, true);
    setCheckBoxFromSettings("embbed_sounds", "PublishSettings.EmbbedSounds", uiState, true);
    setCheckBoxFromSettings("exportLibSounds", "PublishSettings.ExportLibSounds", uiState, true);
    setCheckBoxFromSettings("exportTimelineSounds", "PublishSettings.ExportTimelineSounds", uiState, true);
    setCheckBoxFromSettings("createAudioMap", "PublishSettings.CreateAudioMap", uiState, false);

    if(uiState["PublishSettings.SaveSoundsAs"] != null && uiState["PublishSettings.SaveSoundsAs"] != "undefined")
    {
        document.getElementById("sound_type").selectedIndex = uiState["PublishSettings.SaveSoundsAs"];
    }



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
        document.getElementById('of').value="";
        //alert ('Output file path cannot be empty');
    }
    return true;

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
function publish_callback()
{
    document.getElementById("overlay_loading").className="overlayactive";
    document.getElementById("overlay_loading").innerHTML="";
    csInterface.closeExtension();
}

function populate_textfield(val)
{
    document.getElementById("of").value = val;
    set_fileName(val);
}
function populate_textfield2(val)
{
    document.getElementById("preview_source_path").value = val;
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
function opcustum_prev() {
    evalScript("fl.browseForFolderURL('Select folder that contains the publisher.html');",open_custom_preview_source_callback);
}


function open_panel() {
    csInterface.requestOpenExtension("com.away.AwayExtensionsAnimateCC.Panel", null);
    csInterface.closeExtension();


}
function open_help() {
    var testURL=csInterface.getSystemPath(SystemPath.EXTENSION)+"/help.html";
    cep.util.openURLInDefaultBrowser("file:///"+testURL);
}
function open_github() {
    cep.util.openURLInDefaultBrowser("https://github.com/awaytools/AwayExtensions-FlashPro");
}
function onPublish() {
    var retVal = checkvalue();
    if(retVal){
        document.getElementById("overlay_loading").className="overlay_active";
        document.getElementById("overlay_loading").innerHTML='<img id="loader" src="./ressources/loading_spinner.gif" alt="Loading...">';
        serializeUI();
        evalScript("fl.getDocumentDOM().publish();", publish_callback);
    }
}
function onOK()  {

    serializeUI();
    csInterface.closeExtension();
}
function onCancel()  {
    csInterface.closeExtension();
}