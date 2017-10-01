var csInterface, globalColor = "colorThemeCSS", panel_state = "importAS2", r = /[^\/]*$/, awayjsfl_script_path = document.URL.replace(r, "") + "AwayJSFL.jsfl";
$(document).ready(function () {
    csInterface = new CSInterface;
    var a = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
    ChangePanelTheme(a);
    csInterface.addEventListener(CSInterface.THEME_COLOR_CHANGED_EVENT, onAppThemeColorChanged);
    //csInterface.addEventListener("com.adobe.events.flash.documentChanged", changed_document);
    //csInterface.addEventListener("com.adobe.events.flash.documentSaved", changed_document);
    //csInterface.addEventListener("com.adobe.events.flash.documentOpened", changed_document);
    //csInterface.addEventListener("com.adobe.events.flash.documentClosed", changed_document);
    //csInterface.addEventListener("com.adobe.events.flash.documentNew", changed_document);
    //evalScript("fl.runScript('" + awayjsfl_script_path + "', 'hasValidDoc');", hasDocumentCallback);
    document.getElementById("scrollable_content").addEventListener("scroll", function (a) {
        var f = 0, f = document.getElementById("scrollable_content");
        a = document.getElementById("logo");
        var d = document.getElementById("header"), f = f.scrollTop;
        0 < f ? (a.className =
            "smaller", d.className = "smaller") : (a.className = "", d.className = "")
    });
    set_panelstate()
});
function openConverter() {
    panel_state = "importAS2";
    set_panelstate()
}
function openBatchExporter() {
    panel_state = "batchExporter";
    set_panelstate()
}
function set_panelstate() {
    var a = "", c = document.getElementById("panel_converter"), f = document.getElementById("panel_batch_export");
    c.className = "awayjs_button";
    f.className = "awayjs_button";
    "importAS2" == panel_state ? (c.className = "awayjs_button_active", a += '<div style=\'width:100%;height:85px;\'></div><div class="ui_comp_double">            <label class="myCHBXLabel2" for="inputFileAS2">Input XFL File:</label>        <input type="text" id="inputFileAS2">            <button class="awayjs_button" id="inputFileAS2Browse" onclick="opsd1()"><span class="glyphicon glyphicon-folder-open"></span></button>        </div>', a +=
        '<div class="ui_comp_double">            <label class="myCHBXLabel2" for="outputFileAS2">Output Folder:</label>        <input type="text" id="outputFileAS2">            <button class="awayjs_button" id="outputFileAS2Browse" onclick="opsd2()"><span class="glyphicon glyphicon-folder-open"></span></button>        </div>', a += '<div class="clearer"><div class="ui_comp_single2"><button class="awayjs_button" onclick="onCS6_AS2_XFL()">Import CS6 AS2 xfl</button></div></div>', a += "<div class='clearer'>Input is expected to be a xfl file created with Flash Pro CS6 containing AS2 FrameCode.</div>",
        a += "<div class='clearer'>Output is optional. If left empty, the converted output will be saved with a '_AwayJS'-suffix.</div>") : "batchExporter" == panel_state && (f.className = "awayjs_button_active", a += "<div style='width:100%;height:85px;'></div>", a += '<div class="clearer"><div class="ui_comp_single2"><button class="awayjs_button" onclick="onStartBatchExport()">Start Batch Export</button></div></div>', a += "<div class='clearer'>All FLA-docs found in selected folder will be exported as AWD.<br>FLA documets located at subfolders will be ignored.<br>Output AWD-files will inherit the filename from their FLA origin.</div>");
    document.getElementById("scrollable_content_inner").innerHTML = a
}
function onStartBatchExport() {
    document.getElementById("overlay_loading").className = "overlay_active";
    document.getElementById("overlay_loading").innerHTML = '<img id="loader" src="./ressources/loading_spinner.gif" alt="Loading...">';
    evalScript("fl.runScript('" + awayjsfl_script_path + "', 'batchExport');", batchExportCallback)
}
function batchExportCallback() {
    document.getElementById("overlay_loading").className = "overlayactive";
    document.getElementById("overlay_loading").innerHTML = ""
}
function onCS6_AS2_XFL() {
    function a() {
        window.cep.process.stdout(k, function (a) {
            a = a.split(/\r?\n|\r/g);
            var b = 0;
            "" != l && (evalScript("fl.trace('" + l + a[0].replace(/\\/g, " | ") + "');"), b += 1, l = "");
            for (; b < a.length; b++)"C" == a[b][0] ? "E" == a[b][a[b].length - 1] ? evalScript("fl.trace('" + a[b].replace(/\\/g, " | ") + "');") : l = a[b].replace(/\\/g, " | ") : evalScript("fl.trace('" + a[b].replace(/\\/g, " | ") + "');")
        });
        !0 == window.cep.process.isRunning(k).data && setTimeout(a, 1E3)
    }

    var c = "/usr/bin/java";
    if ("Win32" == navigator.platform ||
        "Windows" == navigator.platform)c = "C:/ProgramData/Oracle/Java/javapath/java.exe";
    var f = csInterface.getSystemPath(SystemPath.EXTENSION) + "/AS2JS.jar", d = document.getElementById("inputFileAS2").value;
    if (5 > d.length)alert("No valid path"); else {
        for (var b = d.split("\\"), h = "", e = 0; e < b.length; e++)h += b[e], e < b.length - 1 && (h += "/");
        var e = document.getElementById("outputFileAS2").value, g = "", b = "";
        if (0 == e.length) {
            b = d.split("\\");
            for (e = 0; e < b.length - 1; e++)g += b[e], e == b.length - 2 && (g += "_AwayJS"), e < b.length - 1 && (g += "/");
            b = b[b.length -
                1].split(".")[0] + "_AwayJS.xfl";
            openFilePath = g + b
        } else {
            b = e.split("\\");
            for (e = 0; e < b.length; e++)console.log(), evalScript("fl.trace('testthis2[i]=" + b[e] + "=');"), g += b[e], g += "/";
            openFilePath = g + b[b.length - 1] + ".xfl"
        }
        evalScript("fl.trace('javaURL " + c + "');");
        evalScript("fl.trace('jarURL " + f + "');");
        evalScript("fl.trace('inURL " + h + "');");
        evalScript("fl.trace('outURL " + g + "');");
        evalScript("fl.trace('openFilePath " + openFilePath + "');");
        var c = window.cep.process.createProcess(c, "-jar", f, "-d", g, d), k = c.data, l = "";
        a();
        window.cep.process.onquit(k, function () {
            evalScript("fl.openDocument(FLfile.platformPathToURI('" + openFilePath + "'));", loading_done_callback)
        });
        c = window.cep.process.isRunning(k);
        !0 == c.data ? (document.getElementById("overlay_loading").className = "overlay_active", document.getElementById("overlay_loading").innerHTML = '<img id="loader" src="./ressources/loading_spinner.gif" alt="Loading...">') : alert("Could not start the java-process")
    }
}
function loading_done_callback() {
    document.getElementById("overlay_loading").className = "overlayactive";
    document.getElementById("overlay_loading").innerHTML = "";
    //evalScript("fl.runScript('" + awayjsfl_script_path + "', 'hasValidDoc');", hasDocumentCallback)
}
function hasDocumentCallback(a) {
    "true" == a ? ("importAS2" == panel_state && (document.getElementById("overlay_no_doc").innerHTML = "", document.getElementById("scrollable_content").style.opacity = 1), document.getElementById("publish_panel").disabled = !1, document.getElementById("publish_panel").style.opacity = 1, document.getElementById("ok_panel").disabled = !1, document.getElementById("ok_panel").style.opacity = 1) : ("importAS2" != panel_state && (document.getElementById("overlay_no_doc").innerHTML = "No valid Away Document opened",
        document.getElementById("scrollable_content").style.opacity = 0.3), document.getElementById("publish_panel").disabled = !0, document.getElementById("publish_panel").style.opacity = 0.3, document.getElementById("ok_panel").disabled = !0, document.getElementById("ok_panel").style.opacity = 0.3)
}
function changed_document(a) {
    //evalScript("fl.runScript('" + awayjsfl_script_path + "', 'hasValidDoc');", hasDocumentCallback)
}
function populate_textfield1(a) {
    document.getElementById("inputFileAS2").value = a
}
function populate_textfield2(a) {
    document.getElementById("outputFileAS2").value = a
}
function evalScript(a, c) {
    csInterface.evalScript(a, c)
}
function open_callback1(a) {
    evalScript("FLfile.uriToPlatformPath('" + a + "');", populate_textfield1)
}
function open_callback2(a) {
    evalScript("FLfile.uriToPlatformPath('" + a + "');", populate_textfield2)
}
function ChangePanelTheme(a) {
    var c = 128 > a.appBarBackgroundColor.color.blue, f = document.getElementsByTagName("head")[0], d = document.getElementById(globalColor);
    d && d.parentNode.removeChild(d);
    d = document.createElement("link");
    d.rel = "stylesheet";
    d.type = "text/css";
    d.media = "all";
    d.href = c ? "css/mkDroverPanel_D.css" : "css/mkDroverPanel_L.css";
    d.id = globalColor;
    f.appendChild(d);
    c = document.styleSheets;
    if (0 < c.length)for (f = 0; f < c.length; f++)c.item(f).addRule("*", 'font-family: "' + a.baseFontFamily + '";font-size:11px',
        0)
}
function onAppThemeColorChanged(a) {
    a = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
    ChangePanelTheme(a)
}
function opsd1() {
    evalScript("fl.browseForFileURL('open','Select a CS6 AS2 XFL','XFL','xfl');", open_callback1)
}
function opsd2() {
    evalScript("fl.browseForFolderURL('Save as CC AwayDocument XFL');", open_callback2)
}
function onPublish() {
    document.getElementById("overlay_loading").className = "overlay_active";
    document.getElementById("overlay_loading").innerHTML = '<img id="loader" src="./ressources/loading_spinner.gif" alt="Loading...">';
    evalScript("fl.getDocumentDOM().publish();", loading_done_callback)
}
function openPublisher() {
    csInterface.requestOpenExtension("com.away.AwayExtensionsAnimateCC.PublishSettings", null)
};
