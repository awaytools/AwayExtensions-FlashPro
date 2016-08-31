var csInterface, globalColor = "colorThemeCSS", r = /[^\/]*$/, awayjsfl_script_path = document.URL.replace(r, "") + "AwayJSFL.jsfl", preview_custom_source = "", preview_output_path = "", preview_append_name = !1, file_name = "";
$(document).ready(function () {
    csInterface = new CSInterface;
    var a = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
    ChangePanelTheme(a);
    document.title = "AWD Publish Settings";
    csInterface.addEventListener(CSInterface.THEME_COLOR_CHANGED_EVENT, onAppThemeColorChanged);
    csInterface.addEventListener("com.adobe.events.flash.extension.setstate", setUI);
    evalScript("fl.runScript('" + awayjsfl_script_path + "', 'getPublisherSettings');", getPublishSettingsCallback);
    a = new CSEvent;
    a.scope = "APPLICATION";
    a.type = "com.adobe.events.flash.extensionLoaded";
    a.data = "Test Event";
    a.extensionId = "com.away.AwayExtensionsAnimateCC.PublishSettings";
    csInterface.dispatchEvent(a);
    document.getElementById("scrollable_content").addEventListener("scroll", function (a) {
        var c = 0, c = document.getElementById("scrollable_content");
        a = document.getElementById("logo");
        var d = document.getElementById("header"), c = c.scrollTop;
        0 < c ? (a.className = "smaller", d.className = "smaller") : (a.className = "", d.className = "")
    })
});
function getPublishSettingsCallback(a) {
    var b = {};
    a = a.split("###");
    for (var c = 0; c < a.length - 1; c++) {
        var d = a[c].split("=");
        2 == d.length && (b[d[0]] = d[1])
    }
    setUIFromData(b)
}
function setCheckBoxFromSettings(a, b, c, d) {
    document.getElementById(a).checked = d;
    null != c[b] && "undefined" != c[b] && c[b] !== "" + d && (document.getElementById(a).checked = !d)
}
function enableUIElements(a) {
    var b = [];
    b.push("exportTimelines");
    b.push("exportFonts");
    b.push("exportBitmaps");
    b.push("exportSounds");
    var c = document.getElementById(a.id + "_panel");
    a.checked && "panel-collapse collapse in" != c.className ? document.getElementById(a.id + "_open").click() : !a.checked && "panel-collapse collapse" != c.className && document.getElementById(a.id + "_open").click();
    for (a = 0; a < b.length; a++)document.getElementById(b[a]).checked ? document.getElementById(b[a] + "_settings").className = "panel-body" :
        document.getElementById(b[a] + "_settings").className = "panel-body disabled"
}
function writeCheckboxToSettings(a, b, c) {
    c[b] = "false";
    document.getElementById(a).checked && (c[b] = "true")
}
function serializeUI() {
    var a = new CSEvent, b = {};
    b.out_file = document.getElementById("of").value.toString();
    writeCheckboxToSettings("usePreview", "PublishSettings.OpenPreview", b);
    writeCheckboxToSettings("copyRuntime", "PublishSettings.CopyRuntime", b);
    //writeCheckboxToSettings("createLogfile", "PublishSettings.PrintExportLog", b);
    writeCheckboxToSettings("includeVisiblelayers", "PublishSettings.IncludeInvisibleLayer", b);
    writeCheckboxToSettings("exportFrameScript", "PublishSettings.ExportFrameScript", b);
    writeCheckboxToSettings("exportLibFonts",
        "PublishSettings.ExportLibFonts", b);
    writeCheckboxToSettings("embbedAllChars", "PublishSettings.EmbbedAllChars", b);
    writeCheckboxToSettings("exportEmptyFontsForFNT", "PublishSettings.ExportEmptyFontsForFNT", b);
    writeCheckboxToSettings("forceTextureOverwrite", "PublishSettings.ForceTextureOverwrite", b);
    writeCheckboxToSettings("embbed_textures", "PublishSettings.EmbbedTextures", b);
    writeCheckboxToSettings("exportLibBitmaps", "PublishSettings.ExportLibBitmaps", b);
    writeCheckboxToSettings("forceSoundOverwrite", "PublishSettings.ForceSoundOverwrite", b);
    writeCheckboxToSettings("embbed_sounds", "PublishSettings.EmbbedSounds",
        b);
    writeCheckboxToSettings("exportLibSounds", "PublishSettings.ExportLibSounds", b);
    writeCheckboxToSettings("exportTimelineSounds", "PublishSettings.ExportTimelineSounds", b);
    b["PublishSettings.SaveSoundsAs"] = document.getElementById("sound_type").selectedIndex;
    a.scope = "APPLICATION";
    a.type = "com.adobe.events.flash.extension.savestate";
    a.data = JSON.stringify(b);
    a.extensionId = "com.away.AwayExtensionsAnimateCC.PublishSettings";
    csInterface.dispatchEvent(a)
}
function setUI(a) {
    setUIFromData(a.data)
}
function setUIFromData(a) {
    null != a.out_file && void 0 != a.out_file && (document.getElementById("of").value = a.out_file);
    setCheckBoxFromSettings("usePreview", "PublishSettings.OpenPreview", a, !0);
    setCheckBoxFromSettings("copyRuntime", "PublishSettings.CopyRuntime", a, !0);
    //setCheckBoxFromSettings("createLogfile", "PublishSettings.PrintExportLog", a, !1);
    setCheckBoxFromSettings("includeVisiblelayers", "PublishSettings.IncludeInvisibleLayer", a, !0);
    setCheckBoxFromSettings("exportFrameScript", "PublishSettings.ExportFrameScript",
        a, !0);
    setCheckBoxFromSettings("exportLibFonts", "PublishSettings.ExportLibFonts", a, !0);
    setCheckBoxFromSettings("embbedAllChars", "PublishSettings.EmbbedAllChars", a, !0);
    setCheckBoxFromSettings("exportEmptyFontsForFNT", "PublishSettings.ExportEmptyFontsForFNT", a, !1);
    setCheckBoxFromSettings("forceTextureOverwrite", "PublishSettings.ForceTextureOverwrite", a, !0);
    setCheckBoxFromSettings("embbed_textures", "PublishSettings.EmbbedTextures", a, !0);
    setCheckBoxFromSettings("exportLibBitmaps", "PublishSettings.ExportLibBitmaps", a, !0);
    setCheckBoxFromSettings("forceSoundOverwrite", "PublishSettings.ForceSoundOverwrite",
        a, !0);
    setCheckBoxFromSettings("embbed_sounds", "PublishSettings.EmbbedSounds", a, !0);
    setCheckBoxFromSettings("exportLibSounds", "PublishSettings.ExportLibSounds", a, !0);
    setCheckBoxFromSettings("exportTimelineSounds", "PublishSettings.ExportTimelineSounds", a, !0);
    null != a["PublishSettings.SaveSoundsAs"] && "undefined" != a["PublishSettings.SaveSoundsAs"] && (document.getElementById("sound_type").selectedIndex = a["PublishSettings.SaveSoundsAs"]);
    enableUIElements()
}
function read_preview_settings(a) {
    evalScript("fl.trace('" + a + "');")
}
function importFrameScriptsFromJS(a) {
    evalScript("fl.runScript('" + awayjsfl_script_path + "', 'importFrameScriptsFromJS');")
}
function open_advanced(a) {
    evalScript("fl.runScript('" + awayjsfl_script_path + "', 'change_preview_settings', 'test1', 'test2', 'test3', 'test4');", read_preview_settings)
}
function show_open_preview_settings(a) {
    evalScript("fl.runScript('" + awayjsfl_script_path + "', 'change_preview_settings', 'test1', 'test2', 'test3', 'test4');", read_preview_settings)
}
function isNumber(a) {
    return a && (a = a.which ? a.which : a.keyCode, 190 != a && 31 < a && (48 > a || 57 < a) && (96 > a || 105 < a) && (37 > a || 40 < a) && (188 > a || 190 < a) && 110 != a && 8 != a && 46 != a) ? !1 : !0
}
function minmax(a, b, c) {
    return parseFloat(a) < b || isNaN(a) ? b : parseFloat(a) > c ? c : a
}
function checkvalue() {
    document.getElementById("of").value.match(/\S/) || (document.getElementById("of").value = "");
    return !0
}
function onJSFLFinish(a) {
    evalScript("fl.trace('" + a.toString() + "');")
}
function testUIResult(a) {
    evalScript("fl.trace('" + a + "');")
}
function onGettestUI() {
    var a = document.URL.replace(/[^\/]*$/, "") + "test_ui.xml";
    evalScript("var xpanel = fl.getDocumentDOM().xmlPanel('" + a + "'); fl.xmlui.get('separator');", testUIResult)
}
function onGetTimeLineInfos() {
    var a = document.URL.replace(/[^\/]*$/, "") + "AwayJSFL.jsfl";
    evalScript("fl.trace('" + a + "');");
    evalScript("fl.runScript('" + a + "', 'exportMotions');", onJSFLFinish)
}
function onGetDocInfos() {
    var a = document.URL.replace(/[^\/]*$/, "") + "AwayJSFL.jsfl";
    evalScript("fl.trace('" + a + "');");
    evalScript("fl.runScript('" + a + "', 'exportMotions');", onJSFLFinish)
}
function onGetSceneInfos() {
    var a = document.URL.replace(/[^\/]*$/, "") + "AwayJSFL.jsfl";
    evalScript("fl.runScript('" + a + "', 'exportMotions');", onJSFLFinish)
}
function onHelp() {
    alert("No Help document is provided yet.")
}
function publish_callback() {
    document.getElementById("overlay_loading").className = "overlayactive";
    document.getElementById("overlay_loading").innerHTML = "";
    csInterface.closeExtension()
}
function populate_textfield(a) {
    document.getElementById("of").value = a;
    set_fileName(a)
}
function populate_textfield2(a) {
    document.getElementById("preview_source_path").value = a
}
function set_fileName(a) {
    a = a.replace(/^.*[\\\/]/, "");
    a = a.replace("awd", "html");
    document.getElementById("previewPathName").value = "/" + a
}
function evalScript(a, b) {
    csInterface.evalScript(a, b)
}
function open_callback(a) {
    evalScript("FLfile.uriToPlatformPath('" + a + "');", populate_textfield)
}
function ChangePanelTheme(a) {
    var b = 128 > a.appBarBackgroundColor.color.blue, c = document.getElementsByTagName("head")[0], d = document.getElementById(globalColor);
    d && d.parentNode.removeChild(d);
    d = document.createElement("link");
    d.rel = "stylesheet";
    d.type = "text/css";
    d.media = "all";
    d.href = b ? "css/mkDroverPanel_D.css" : "css/mkDroverPanel_L.css";
    d.id = globalColor;
    c.appendChild(d);
    b = document.styleSheets;
    if (0 < b.length)for (c = 0; c < b.length; c++)b.item(c).addRule("*", 'font-family: "' + a.baseFontFamily + '";font-size:11px',
        0)
}
function onAppThemeColorChanged(a) {
    a = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
    ChangePanelTheme(a)
}
function open_custom_preview_source_callback(a) {
    evalScript("FLfile.uriToPlatformPath('" + a + "');", populate_textfield2)
}
function getPublisherSettings() {
    evalScript("fl.runScript('" + awayjsfl_script_path + "', 'getPublisherSettings');")
}
function opsd() {
    evalScript("fl.browseForFileURL('save','Publish to AWD','AWD','awd');", open_callback)
}
function opcustum_prev() {
    evalScript("fl.browseForFolderURL('Select folder that contains the publisher.html');", open_custom_preview_source_callback)
}
function open_panel() {
    csInterface.requestOpenExtension("com.away.AwayExtensionsAnimateCC.Panel", null);
    csInterface.closeExtension()
}
function open_help() {
    var a = csInterface.getSystemPath(SystemPath.EXTENSION) + "/help.html";
    cep.util.openURLInDefaultBrowser("file:///" + a)
}
function open_github() {
    cep.util.openURLInDefaultBrowser("https://github.com/awaytools/AwayExtensions-FlashPro")
}
function onPublish() {
    checkvalue() && (document.getElementById("overlay_loading").className = "overlay_active", document.getElementById("overlay_loading").innerHTML = '<img id="loader" src="./ressources/loading_spinner.gif" alt="Loading...">', serializeUI(), evalScript("fl.getDocumentDOM().publish();", publish_callback))
}
function onOK() {
    serializeUI();
    csInterface.closeExtension()
}
function onCancel() {
    csInterface.closeExtension()
};
