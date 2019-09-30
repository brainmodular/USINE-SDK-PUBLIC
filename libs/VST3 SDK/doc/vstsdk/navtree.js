var NAVTREE =
[
  [ "VST 3 SDK", "index.html", [
    [ "Introduction", "index.html", null ],
    [ "Frequently Asked Questions", "faq.html", [
      [ "Communication", "faq.html#faqCommunication", [
        [ "Q: How should I communicate between the 'Processing' and the 'User Interface'?", "faq.html#faqCommunication1", null ],
        [ "Q: I want to implement an audio meter in my user interface. How do I do this?", "faq.html#faqCommunication2", null ],
        [ "Q: How does the host send automation data to my VST 3 Plug-in?", "faq.html#faqCommunication3", null ],
        [ "Q: How report to the host that the Plug-in has new parameter titles?", "faq.html#faqCommunication4", null ],
        [ "Q: How receive MIDI Controllers from the host?", "faq.html#faqCommunication5", null ],
        [ "Q: How my parameter changes (from UI interaction) are send to the processor if the host does not process?", "faq.html#faqCommunication6", null ]
      ] ],
      [ "Processing", "faq.html#faqProcessing", [
        [ "Q: How does Audio Processing Bypass work?", "faq.html#faqProcessing1", null ],
        [ "Q: Must the host deliver valid initialized Audio buffers if the associated bus is deactivated?", "faq.html#faqProcessing2", null ],
        [ "Q: Can the maximum sample block size change while the Plug-in is processing?", "faq.html#faqProcessing3", null ],
        [ "Q: Can the sample rate change while the Plug-in is processing?", "faq.html#faqProcessing4", null ],
        [ "Q: Could the host call the process function without Audio buffers?", "faq.html#faqProcessing5", null ],
        [ "Q: What is a Side-chain?", "faq.html#faqProcessing6", null ],
        [ "Q: How can I implement a Side-chain path into my Plug-in?", "faq.html#faqProcessing7", null ],
        [ "Q: My Plug-in is capable of processing all possible channel configurations.", "faq.html#faqProcessing8", null ],
        [ "Q: How are speaker arrangement settings handled for FX Plug-ins?", "faq.html#faqProcessing9", null ],
        [ "Q: My Plug-in has mono input and stereo output. How does VST 3 handle this?", "faq.html#faqProcessing10", null ],
        [ "Q: How does it work with silence flags?", "faq.html#faqProcessing11", null ],
        [ "Q: How report to the host that the Plug-in latency has changed?", "faq.html#faqProcessing12", null ],
        [ "Q: How report to the host that the Plug-in Arrangement has changed?", "faq.html#faqProcessing13", null ],
        [ "Q: Can IAudioProcessor::setProcessing be called without any IAudioProcessor::process call?", "faq.html#faqProcessing14", null ],
        [ "Q: How to make sure that a plug-in is always processed?", "faq.html#faqProcessing15", null ],
        [ "Q: Can IComponent::getState()/setState() could be called during processing?", "faq.html#faqProcessing16", null ],
        [ "Q: How can a Plug-in be informed that it is currently processed in offline processing?", "faq.html#faqProcessing17", null ]
      ] ],
      [ "GUI-Editor", "faq.html#faqEditor", [
        [ "Q: The host doesn't open my Plug-in UI, why?", "faq.html#faqEditor1", null ]
      ] ],
      [ "Compatibility with VST 2.x or VST 1", "faq.html#faqVst2", [
        [ "Q: How can I update my VST 2 version of my Plug-in to a VST 3 version and be sure that Cubase will load it instead of my old one?", "faq.html#faqVst2q1", null ],
        [ "Q: How can I support projects which were saved with the VST 2 version of my Plug-in?", "faq.html#faqVst2q2", null ],
        [ "Q: In VST 2 the editor was able to access the processing part, named effect, directly. How can I do this in VST 3?", "faq.html#faqVst2q3", null ],
        [ "Q: Does VST 3 implement methods like beginEdit and endEdit known from VST 2?", "faq.html#faqVst2q4", null ],
        [ "Q: Does VST 3 include variable Input/Output processing like processVariableIo of VST 2?", "faq.html#faqVst2q5", null ],
        [ "Q: What is the equivalent to the VST 2 kPlugCategOfflineProcess?", "faq.html#faqVst2q6", null ]
      ] ],
      [ "Persistence", "faq.html#faqPersistence", [
        [ "Q: How does persistence work?", "faq.html#faqPersistence1", null ],
        [ "Q: What's the difference between IEditController::setComponentState and IEditController::setState?", "faq.html#faqPersistence2", null ]
      ] ],
      [ "Miscellaneous", "faq.html#faqMiscellaneous", [
        [ "Q: How is a normalized value converted to a discrete integer value in VST 3?", "faq.html#faqMiscellaneous1", null ],
        [ "Q: What is the return value tresult?", "faq.html#faqMiscellaneous2", null ],
        [ "Q: Which version of Steinberg Sequencers support VST 3?", "faq.html#faqMiscellaneous3", null ],
        [ "Q: How are the different Speakers located?", "faq.html#faqMiscellaneous4", null ],
        [ "Q: Why do Plug-ins need subcategories?", "faq.html#faqMiscellaneous5", null ],
        [ "Q: Is it possible to define a Plug-in as Fx and Instrument?", "faq.html#faqMiscellaneous6", null ],
        [ "Q: Is it possible to ask a Plug-in about which speaker arrangements are supported?", "faq.html#faqMiscellaneous7", null ],
        [ "Q: Which version of Steinberg Sequencers support VST 3 Note Expression?", "faq.html#faqMiscellaneous8", null ],
        [ "Q: When compiling for Mac AudioUnit, I have a compiler error in AUCarbonViewBase.cpp. What can i do?", "faq.html#faqMiscellaneous9", null ],
        [ "Q: How can i develop a SurroundPanner Plug-in which is integrated in Nuendo as Panner?", "faq.html#faqMiscellaneous10", null ],
        [ "Q: How can i validate my Plug-in?", "faq.html#faqMiscellaneous11", null ]
      ] ]
    ] ],
    [ "VST 3 Licensing Issues", "vst3License.html", [
      [ "Steinberg VST usage guidelines", "vst3License.html#guidelines", null ],
      [ "What are the licensing options for VST 3?", "vst3License.html#lic1", [
        [ "\"Proprietary Steinberg VST 3\" license", "vst3License.html#lic2", null ],
        [ "\"Open-source GPLv3\" license", "vst3License.html#lic3", null ]
      ] ],
      [ "Which files of the VST 3 SDK are under which license?", "vst3License.html#lic4", null ],
      [ "Developer use cases", "vst3License.html#lic5", [
        [ "I would like to share the source code of my plug-in/host on GitHub.", "vst3License.html#usecase1", null ],
        [ "I would like to distribute my plug-in/host as freeware.", "vst3License.html#usecase2", null ],
        [ "I would like to sell my plug-in/host in binary form.", "vst3License.html#usecase3", null ],
        [ "I would like to reuse a distributed modified version of the VST 3 SDK for my own plug-ins/host.", "vst3License.html#usecase4", null ],
        [ "I would like to distribute my plug-in/host in binary form based on a modified version of VST 3 SDK which is under GPLv3.", "vst3License.html#usecase5", null ]
      ] ]
    ] ],
    [ "Other Copyrights/Trademarks", "otherCopyright.html", null ],
    [ "Test Applications", "applications.html", [
      [ "Validator command line", "applications.html#validator", null ],
      [ "AudioHost - Cross-platform Standalone", "applications.html#audiohost", null ],
      [ "EditorHost - Cross-platform Standalone", "applications.html#editorhost", null ],
      [ "VST 3 Plug-in Test Host", "applications.html#vstTestHost", [
        [ "How to use it?", "applications.html#howtouse", null ],
        [ "VST Player Window", "applications.html#vstTestHostPlayer", [
          [ "Audio Input", "applications.html#audio", null ],
          [ "Event Input", "applications.html#input", null ],
          [ "VST Rack", "applications.html#rack", null ],
          [ "Info Window", "applications.html#info", null ],
          [ "Transport", "applications.html#transport", null ]
        ] ],
        [ "VST 3 Plug-ins Tests Window", "applications.html#vstTestHostTests", null ],
        [ "Preset Editor", "applications.html#vstTestHostPresets", null ]
      ] ]
    ] ],
    [ "VST 3 Version History", "vst3History.html", [
      [ "Version 3.6.9 (2018/03/01)", "vst3History.html#rev369", null ],
      [ "Version 3.6.8 (2017/11/08)", "vst3History.html#rev368", null ],
      [ "Version 3.6.7 (2017/03/03)", "vst3History.html#rev367", null ],
      [ "Version 3.6.6 (2016/06/17)", "vst3History.html#rev366", null ],
      [ "Version 3.6.5 (2015/08/28)", "vst3History.html#rev365", null ],
      [ "Version 3.6.0 (2013/11/22)", "vst3History.html#rev360", null ],
      [ "Version 3.5.2 (2012/09/25)", "vst3History.html#rev352", null ],
      [ "Version 3.5.1 (2011/11/11)", "vst3History.html#rev351", null ],
      [ "Version 3.5.0 (2011/02/04)", "vst3History.html#rev350", null ],
      [ "Version 3.1.0 (2010/06/25)", "vst3History.html#rev310", null ],
      [ "Version 3.0.2 (2009/01/15)", "vst3History.html#rev302", null ],
      [ "Version 3.0.1 (2008/03/19)", "vst3History.html#rev301", null ],
      [ "Version 3.0.0 (2008/01/17)", "vst3History.html#rev300", null ]
    ] ],
    [ "VST 3 - AAX Wrapper", "AAXWrapper.html", [
      [ "Introduction", "AAXWrapper.html#AAXIntroduction", null ],
      [ "How does it work?", "AAXWrapper.html#howtoAAX", null ]
    ] ],
    [ "VST 3 - AudioUnit Wrapper", "AUWrapper.html", [
      [ "Introduction", "AUWrapper.html#AUIntroduction", null ],
      [ "How does it work?", "AUWrapper.html#AUhowdoesitwork", null ]
    ] ],
    [ "VST 3 - Audio Unit v3 Wrapper", "AUv3Wrapper.html", [
      [ "Introduction", "AUv3Wrapper.html#AUv3Introduction", null ],
      [ "How does it work?", "AUv3Wrapper.html#howtoAUv3", null ]
    ] ],
    [ "iOS Inter-App Audio support", "interappaudio.html", [
      [ "Introduction", "interappaudio.html#iaa_introduction", null ],
      [ "How does it work?", "interappaudio.html#iaa_howto", [
        [ "Create and Setup the Xcode Project", "interappaudio.html#iaa_howto1", null ],
        [ "Add your Audio Component description to the Info.plist", "interappaudio.html#iaa_howto2", null ],
        [ "Add files to the project", "interappaudio.html#iaa_howto3", null ],
        [ "If using vstgui", "interappaudio.html#iaa_howto4", null ]
      ] ],
      [ "Code changes", "interappaudio.html#iaa_codechanges", null ],
      [ "UI Handling", "interappaudio.html#iaa_ui", [
        [ "Creating a different UI when running on iOS", "interappaudio.html#iaa_editor", null ],
        [ "Using VSTGUI", "interappaudio.html#iaa_ui_vstgui", null ],
        [ "Using a native UIView", "interappaudio.html#iaa_ui_native", null ],
        [ "Host UI Integration", "interappaudio.html#iaa_host_ui", null ]
      ] ]
    ] ],
    [ "VST 3 - VST 2.x Wrapper", "vst2xwrapper.html", [
      [ "Introduction", "vst2xwrapper.html#VST2Introduction", null ],
      [ "How does it work?", "vst2xwrapper.html#VST2howdoesitwork", null ]
    ] ],
    [ "Modules", "modules.html", "modules" ],
    [ "Namespaces", null, [
      [ "Namespace List", "namespaces.html", "namespaces" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ]
      ] ]
    ] ],
    [ "Classes", null, [
      [ "Class List", "annotated.html", "annotated" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ],
        [ "Properties", "functions_prop.html", null ]
      ] ]
    ] ],
    [ "Files", null, [
      [ "File List", "files.html", "files" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
".html",
"classSteinberg_1_1Vst_1_1ComponentBase.html#a60bbab8211ead54d50dbb672be410342",
"classSteinberg_1_1Vst_1_1ParameterChanges.html",
"classSteinberg_1_1Vst_1_1VSTGUIEditor.html#a9b6d44e6eb5f2b1b8ae9399dc56a6ad7",
"classVST3_1_1Hosting_1_1FactoryInfo.html#a92b852ccebf419816579c8f6877dbcd1",
"stringconvert_8cpp.html#ad0e56044e282dcac7890cbce544aa83f"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';
var navTreeSubIndices = new Array();

function getData(varName)
{
  var i = varName.lastIndexOf('/');
  var n = i>=0 ? varName.substring(i+1) : varName;
  return eval(n.replace(/\-/g,'_'));
}

function stripPath(uri)
{
  return uri.substring(uri.lastIndexOf('/')+1);
}

function stripPath2(uri)
{
  var i = uri.lastIndexOf('/');
  var s = uri.substring(i+1);
  var m = uri.substring(0,i+1).match(/\/d\w\/d\w\w\/$/);
  return m ? uri.substring(i-6) : s;
}

function hashValue()
{
  return $(location).attr('hash').substring(1).replace(/[^\w\-]/g,'');
}

function hashUrl()
{
  return '#'+hashValue();
}

function pathName()
{
  return $(location).attr('pathname').replace(/[^-A-Za-z0-9+&@#/%?=~_|!:,.;\(\)]/g, '');
}

function localStorageSupported()
{
  try {
    return 'localStorage' in window && window['localStorage'] !== null && window.localStorage.getItem;
  }
  catch(e) {
    return false;
  }
}


function storeLink(link)
{
  if (!$("#nav-sync").hasClass('sync') && localStorageSupported()) {
      window.localStorage.setItem('navpath',link);
  }
}

function deleteLink()
{
  if (localStorageSupported()) {
    window.localStorage.setItem('navpath','');
  }
}

function cachedLink()
{
  if (localStorageSupported()) {
    return window.localStorage.getItem('navpath');
  } else {
    return '';
  }
}

function getScript(scriptName,func,show)
{
  var head = document.getElementsByTagName("head")[0]; 
  var script = document.createElement('script');
  script.id = scriptName;
  script.type = 'text/javascript';
  script.onload = func; 
  script.src = scriptName+'.js'; 
  if ($.browser.msie && $.browser.version<=8) { 
    // script.onload does not work with older versions of IE
    script.onreadystatechange = function() {
      if (script.readyState=='complete' || script.readyState=='loaded') { 
        func(); if (show) showRoot(); 
      }
    }
  }
  head.appendChild(script); 
}

function createIndent(o,domNode,node,level)
{
  var level=-1;
  var n = node;
  while (n.parentNode) { level++; n=n.parentNode; }
  if (node.childrenData) {
    var imgNode = document.createElement("img");
    imgNode.style.paddingLeft=(16*level).toString()+'px';
    imgNode.width  = 16;
    imgNode.height = 22;
    imgNode.border = 0;
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() {
      if (node.expanded) {
        $(node.getChildrenUL()).slideUp("fast");
        node.plus_img.src = node.relpath+"ftv2pnode.png";
        node.expanded = false;
      } else {
        expandNode(o, node, false, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
    imgNode.src = node.relpath+"ftv2pnode.png";
  } else {
    var span = document.createElement("span");
    span.style.display = 'inline-block';
    span.style.width   = 16*(level+1)+'px';
    span.style.height  = '22px';
    span.innerHTML = '&#160;';
    domNode.appendChild(span);
  } 
}

var animationInProgress = false;

function gotoAnchor(anchor,aname,updateLocation)
{
  var pos, docContent = $('#doc-content');
  var ancParent = $(anchor.parent());
  if (ancParent.hasClass('memItemLeft') ||
      ancParent.hasClass('fieldname') ||
      ancParent.hasClass('fieldtype') ||
      ancParent.is(':header'))
  {
    pos = ancParent.position().top;
  } else if (anchor.position()) {
    pos = anchor.position().top;
  }
  if (pos) {
    var dist = Math.abs(Math.min(
               pos-docContent.offset().top,
               docContent[0].scrollHeight-
               docContent.height()-docContent.scrollTop()));
    animationInProgress=true;
    docContent.animate({
      scrollTop: pos + docContent.scrollTop() - docContent.offset().top
    },Math.max(50,Math.min(500,dist)),function(){
      if (updateLocation) window.location.href=aname;
      animationInProgress=false;
    });
  }
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  node.expanded = false;
  a.appendChild(node.label);
  if (link) {
    var url;
    if (link.substring(0,1)=='^') {
      url = link.substring(1);
      link = url;
    } else {
      url = node.relpath+link;
    }
    a.className = stripPath(link.replace('#',':'));
    if (link.indexOf('#')!=-1) {
      var aname = '#'+link.split('#')[1];
      var srcPage = stripPath(pathName());
      var targetPage = stripPath(link.split('#')[0]);
      a.href = srcPage!=targetPage ? url : "javascript:void(0)"; 
      a.onclick = function(){
        storeLink(link);
        if (!$(a).parent().parent().hasClass('selected'))
        {
          $('.item').removeClass('selected');
          $('.item').removeAttr('id');
          $(a).parent().parent().addClass('selected');
          $(a).parent().parent().attr('id','selected');
        }
        var anchor = $(aname);
        gotoAnchor(anchor,aname,true);
      };
    } else {
      a.href = url;
      a.onclick = function() { storeLink(link); }
    }
  } else {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() {
    if (!node.childrenUL) {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  (function (){ // retry until we can scroll to the selected item
    try {
      var navtree=$('#nav-tree');
      navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
    } catch (err) {
      setTimeout(arguments.callee, 0);
    }
  })();
}

function expandNode(o, node, imm, showRoot)
{
  if (node.childrenData && !node.expanded) {
    if (typeof(node.childrenData)==='string') {
      var varName    = node.childrenData;
      getScript(node.relpath+varName,function(){
        node.childrenData = getData(varName);
        expandNode(o, node, imm, showRoot);
      }, showRoot);
    } else {
      if (!node.childrenVisited) {
        getNode(o, node);
      } if (imm || ($.browser.msie && $.browser.version>8)) { 
        // somehow slideDown jumps to the start of tree for IE9 :-(
        $(node.getChildrenUL()).show();
      } else {
        $(node.getChildrenUL()).slideDown("fast");
      }
      if (node.isLast) {
        node.plus_img.src = node.relpath+"ftv2mlastnode.png";
      } else {
        node.plus_img.src = node.relpath+"ftv2mnode.png";
      }
      node.expanded = true;
    }
  }
}

function glowEffect(n,duration)
{
  n.addClass('glow').delay(duration).queue(function(next){
    $(this).removeClass('glow');next();
  });
}

function highlightAnchor()
{
  var aname = hashUrl();
  var anchor = $(aname);
  if (anchor.parent().attr('class')=='memItemLeft'){
    var rows = $('.memberdecls tr[class$="'+hashValue()+'"]');
    glowEffect(rows.children(),300); // member without details
  } else if (anchor.parent().attr('class')=='fieldname'){
    glowEffect(anchor.parent().parent(),1000); // enum value
  } else if (anchor.parent().attr('class')=='fieldtype'){
    glowEffect(anchor.parent().parent(),1000); // struct field
  } else if (anchor.parent().is(":header")) {
    glowEffect(anchor.parent(),1000); // section header
  } else {
    glowEffect(anchor.next(),1000); // normal member
  }
  gotoAnchor(anchor,aname,false);
}

function selectAndHighlight(hash,n)
{
  var a;
  if (hash) {
    var link=stripPath(pathName())+':'+hash.substring(1);
    a=$('.item a[class$="'+link+'"]');
  }
  if (a && a.length) {
    a.parent().parent().addClass('selected');
    a.parent().parent().attr('id','selected');
    highlightAnchor();
  } else if (n) {
    $(n.itemDiv).addClass('selected');
    $(n.itemDiv).attr('id','selected');
  }
  if ($('#nav-tree-contents .item:first').hasClass('selected')) {
    $('#nav-sync').css('top','30px');
  } else {
    $('#nav-sync').css('top','5px');
  }
  showRoot();
}

function showNode(o, node, index, hash)
{
  if (node && node.childrenData) {
    if (typeof(node.childrenData)==='string') {
      var varName    = node.childrenData;
      getScript(node.relpath+varName,function(){
        node.childrenData = getData(varName);
        showNode(o,node,index,hash);
      },true);
    } else {
      if (!node.childrenVisited) {
        getNode(o, node);
      }
      $(node.getChildrenUL()).css({'display':'block'});
      if (node.isLast) {
        node.plus_img.src = node.relpath+"ftv2mlastnode.png";
      } else {
        node.plus_img.src = node.relpath+"ftv2mnode.png";
      }
      node.expanded = true;
      var n = node.children[o.breadcrumbs[index]];
      if (index+1<o.breadcrumbs.length) {
        showNode(o,n,index+1,hash);
      } else {
        if (typeof(n.childrenData)==='string') {
          var varName = n.childrenData;
          getScript(n.relpath+varName,function(){
            n.childrenData = getData(varName);
            node.expanded=false;
            showNode(o,node,index,hash); // retry with child node expanded
          },true);
        } else {
          var rootBase = stripPath(o.toroot.replace(/\..+$/, ''));
          if (rootBase=="index" || rootBase=="pages" || rootBase=="search") {
            expandNode(o, n, true, true);
          }
          selectAndHighlight(hash,n);
        }
      }
    }
  } else {
    selectAndHighlight(hash);
  }
}

function removeToInsertLater(element) {
  var parentNode = element.parentNode;
  var nextSibling = element.nextSibling;
  parentNode.removeChild(element);
  return function() {
    if (nextSibling) {
      parentNode.insertBefore(element, nextSibling);
    } else {
      parentNode.appendChild(element);
    }
  };
}

function getNode(o, po)
{
  var insertFunction = removeToInsertLater(po.li);
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
      i==l);
  }
  insertFunction();
}

function gotoNode(o,subIndex,root,hash,relpath)
{
  var nti = navTreeSubIndices[subIndex][root+hash];
  o.breadcrumbs = $.extend(true, [], nti ? nti : navTreeSubIndices[subIndex][root]);
  if (!o.breadcrumbs && root!=NAVTREE[0][1]) { // fallback: show index
    navTo(o,NAVTREE[0][1],"",relpath);
    $('.item').removeClass('selected');
    $('.item').removeAttr('id');
  }
  if (o.breadcrumbs) {
    o.breadcrumbs.unshift(0); // add 0 for root node
    showNode(o, o.node, 0, hash);
  }
}

function navTo(o,root,hash,relpath)
{
  var link = cachedLink();
  if (link) {
    var parts = link.split('#');
    root = parts[0];
    if (parts.length>1) hash = '#'+parts[1].replace(/[^\w\-]/g,'');
    else hash='';
  }
  if (hash.match(/^#l\d+$/)) {
    var anchor=$('a[name='+hash.substring(1)+']');
    glowEffect(anchor.parent(),1000); // line number
    hash=''; // strip line number anchors
  }
  var url=root+hash;
  var i=-1;
  while (NAVTREEINDEX[i+1]<=url) i++;
  if (i==-1) { i=0; root=NAVTREE[0][1]; } // fallback: show index
  if (navTreeSubIndices[i]) {
    gotoNode(o,i,root,hash,relpath)
  } else {
    getScript(relpath+'navtreeindex'+i,function(){
      navTreeSubIndices[i] = eval('NAVTREEINDEX'+i);
      if (navTreeSubIndices[i]) {
        gotoNode(o,i,root,hash,relpath);
      }
    },true);
  }
}

function showSyncOff(n,relpath)
{
    n.html('<img src="'+relpath+'sync_off.png" title="'+SYNCOFFMSG+'"/>');
}

function showSyncOn(n,relpath)
{
    n.html('<img src="'+relpath+'sync_on.png" title="'+SYNCONMSG+'"/>');
}

function toggleSyncButton(relpath)
{
  var navSync = $('#nav-sync');
  if (navSync.hasClass('sync')) {
    navSync.removeClass('sync');
    showSyncOff(navSync,relpath);
    storeLink(stripPath2(pathName())+hashUrl());
  } else {
    navSync.addClass('sync');
    showSyncOn(navSync,relpath);
    deleteLink();
  }
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;
  o.node.expanded = false;
  o.node.isLast = true;
  o.node.plus_img = document.createElement("img");
  o.node.plus_img.src = relpath+"ftv2pnode.png";
  o.node.plus_img.width = 16;
  o.node.plus_img.height = 22;

  if (localStorageSupported()) {
    var navSync = $('#nav-sync');
    if (cachedLink()) {
      showSyncOff(navSync,relpath);
      navSync.removeClass('sync');
    } else {
      showSyncOn(navSync,relpath);
    }
    navSync.click(function(){ toggleSyncButton(relpath); });
  }

  $(window).load(function(){
    navTo(o,toroot,hashUrl(),relpath);
    showRoot();
  });

  $(window).bind('hashchange', function(){
     if (window.location.hash && window.location.hash.length>1){
       var a;
       if ($(location).attr('hash')){
         var clslink=stripPath(pathName())+':'+hashValue();
         a=$('.item a[class$="'+clslink.replace(/</g,'\\3c ')+'"]');
       }
       if (a==null || !$(a).parent().parent().hasClass('selected')){
         $('.item').removeClass('selected');
         $('.item').removeAttr('id');
       }
       var link=stripPath2(pathName());
       navTo(o,link,hashUrl(),relpath);
     } else if (!animationInProgress) {
       $('#doc-content').scrollTop(0);
       $('.item').removeClass('selected');
       $('.item').removeAttr('id');
       navTo(o,toroot,hashUrl(),relpath);
     }
  })
}

