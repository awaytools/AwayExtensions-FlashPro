var csInterface;
var globalColor = 'colorThemeCSS';
var panel_state="importAS2";

var r = /[^\/]*$/;
var awayjsfl_script_path=document.URL.replace(r, '')+"AwayJSFL.jsfl";


$( document ).ready(function() {

    csInterface = new CSInterface();


    var skinInfo = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
    var styleSheets= document.styleSheets;
    ChangePanelTheme(skinInfo);

    //Light and dark theme change
    csInterface.addEventListener(CSInterface.THEME_COLOR_CHANGED_EVENT, onAppThemeColorChanged);

    csInterface.addEventListener("com.adobe.events.flash.documentChanged",  changed_document);
    csInterface.addEventListener("com.adobe.events.flash.documentSaved",  changed_document);
    csInterface.addEventListener("com.adobe.events.flash.documentOpened",  changed_document);
    csInterface.addEventListener("com.adobe.events.flash.documentClosed",  changed_document);
    csInterface.addEventListener("com.adobe.events.flash.documentNew",  changed_document);

    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'hasValidDoc');", hasDocumentCallback);

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
        html_str+="<div class='clearer'>Input is expected to be a xfl file created with Flash Pro CS6 containing AS2 FrameCode.</div>";
        html_str+="<div class='clearer'>Output is optional. If left empty, the converted output will be saved with a '_AwayJS'-suffix.</div>";
    }
    else if(panel_state=="batchExporter"){
        btn_batch_export.className="awayjs_button_active";
        html_str+="<div style='width:100%;height:85px;'></div>";
        html_str+='<div class="clearer"><div class="ui_comp_single2"><button class="awayjs_button" onclick="onStartBatchExport()">Start Batch Export</button></div></div>';
        html_str+="<div class='clearer'>All FLA-docs found in selected folder will be exported as AWD.<br>FLA documets located at subfolders will be ignored.<br>Output AWD-files will inherit the filename from their FLA origin.</div>";


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
function onCS6_AS2_XFL(){
    var javaURL = "/usr/bin/java";
    if ("Win32" == navigator.platform || "Windows" == navigator.platform) {

        javaURL="C:/ProgramData/Oracle/Java/javapath/java.exe";
    }

    var jarURL=csInterface.getSystemPath(SystemPath.EXTENSION)+"/AS2JS.jar";

    var inURL=document.getElementById("inputFileAS2").value;
    if(inURL.length<5){
        // we  assume that a path with size smaller than 5 chars is not valid
        alert("No valid path");
        return;
    }

    var testthis=inURL.split("\\");
    var newInURL="";
    for(var i=0; i<testthis.length; i++){
        newInURL+=testthis[i];
        if(i<testthis.length-1)newInURL+="/";
    }

    var outURL=document.getElementById("outputFileAS2").value;

    var newOutURL="";
    var filename="";
    if(outURL.length==0){

        var testthis=inURL.split("\\");
        for(var i=0; i<testthis.length-1; i++){
            newOutURL+=testthis[i];
            if(i==testthis.length-2)newOutURL+="_AwayJS";
            if(i<testthis.length-1)newOutURL+="/";
        }
        var filename_ar=testthis[testthis.length-1].split(".");
        filename=filename_ar[0]+"_AwayJS.xfl";
        openFilePath=newOutURL + filename;
    }
    else{
        var testthis2=outURL.split("\\");
        for(var i=0; i<testthis2.length; i++){
            console.log();
            evalScript("fl.trace('testthis2[i]="+testthis2[i]+"=');");
            newOutURL+=testthis2[i];
            newOutURL+="/";
        }
        openFilePath=newOutURL + testthis2[testthis2.length-1]+".xfl";
    }

    //var outPathes=outURL.split("\\");
    //var filename=outPathes[outPathes.length-1];

    evalScript("fl.trace('javaURL " + javaURL +"');");
    evalScript("fl.trace('jarURL " + jarURL +"');");
    //evalScript("fl.trace('inURL " + inURL +"');");
    evalScript("fl.trace('inURL " + newInURL +"');");
    //evalScript("fl.trace('outURL " + outURL +"');");
    evalScript("fl.trace('outURL " + newOutURL +"');");
    evalScript("fl.trace('openFilePath " + openFilePath +"');");

    var result = window.cep.process.createProcess(javaURL, "-jar", jarURL, "-d", newOutURL, inURL);
    //evalScript("fl.trace('" + result.data+" / "+result.err+"');");

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
        evalScript("fl.openDocument(FLfile.platformPathToURI('"+openFilePath+"'));", loading_done_callback);

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
function loading_done_callback(){
    document.getElementById("overlay_loading").className="overlayactive";
    document.getElementById("overlay_loading").innerHTML="";
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'hasValidDoc');", hasDocumentCallback);
    //csInterface.closeExtension();
}
function hasDocumentCallback(e){
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
function changed_document(e){
    evalScript("fl.runScript('"+awayjsfl_script_path+"', 'hasValidDoc');", hasDocumentCallback);
}
function populate_textfield1(val){
    document.getElementById("inputFileAS2").value = val;
}
function populate_textfield2(val){
    document.getElementById("outputFileAS2").value = val;
}
function evalScript(script, callback) {
    csInterface.evalScript(script, callback);
}
function open_callback1(pathVal){
    evalScript("FLfile.uriToPlatformPath('"+pathVal+"');",populate_textfield1);
}
function open_callback2(pathVal){
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
function opsd1() {
    evalScript("fl.browseForFileURL('open','Select a CS6 AS2 XFL','XFL','xfl');",open_callback1);
}
function opsd2() {
    evalScript("fl.browseForFolderURL('Save as CC AwayDocument XFL');",open_callback2);
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