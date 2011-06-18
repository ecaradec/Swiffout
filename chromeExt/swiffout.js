var flashCSSQuery='embed[type*="application/x-shockwave-flash"],embed[src*=".swf"],object[type*="application/x-shockwave-flash"],object[codetype*="application/x-shockwave-flash"],object[src*=".swf"],object[codebase*="swflash.cab"],object[classid*="D27CDB6E-AE6D-11cf-96B8-444553540000"],object[classid*="d27cdb6e-ae6d-11cf-96b8-444553540000"],object[classid*="D27CDB6E-AE6D-11cf-96B8-444553540000"]';

function getParam(e, n){
    var v = '', r = new RegExp('^('+n+')$', 'i');
    var param = e.getElementsByTagName('param');
    for(var i = 0, p; p = param[i]; i++){
        if(p.hasAttribute('name') && p.getAttribute('name').match(r)){v = p.getAttribute('value'); break};
    }
    return v;
}

function makeMsg(m, o) {
    var src=o.getAttribute('data') || o.getAttribute('src') || getParam(o,'movie|data|src|code|filename|url');
    var l=location;
    if(/^\//.test(src))
        src=l.protocol+"//"+l.hostname+src;
    else if(/^http:/.test(src) || /^https:/.test(src))
        src=src;
    else
        src=l.href.substring(0,l.href.lastIndexOf("/"))+"/"+src;

    var cs=window.getComputedStyle(o);
    var w=parseInt(cs.width,10);
    var h=parseInt(cs.height,10);
    return {method:m, src:src, flashVars:getParam(o, 'flashvars'), width:w, height:h };
}

chrome.extension.onRequest.addListener(function(request, sender, sendResponse) {
    if(request.msg=="findSwf") {
        var href="swiffout:";
        var swfList=[];

        sendResponse({});
        var embedList=document.querySelectorAll(flashCSSQuery);
        for(var i=0;i<embedList.length;i++) {
            chrome.extension.sendRequest(makeMsg("collect",embedList[i]));
        }
    } else if(request.msg=="navigate") {
       window.location=request.href;
    }
});

document.addEventListener("swiffout.fullscreen", function(e) {
    var msg=makeMsg("run", e.srcElement);
    if(/^http:\/\/(www\.|)swiffoutgames\.com\//.test(msg.src)) {
        chrome.extension.sendRequest(msg);
    }
});

// swiffout://swiffout_href=http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525
//http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525
