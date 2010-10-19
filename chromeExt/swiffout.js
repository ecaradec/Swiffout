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

    return {method:m, src:src, flashVars:o.getAttribute('flashvars'), width:o.width, height:o.height };
}

chrome.extension.onRequest.addListener(function(request, sender, sendResponse) {
    if(request.msg=="findSwf") {
        var href="swiffout:";
        var swfList=[];

        var embedList=document.getElementsByTagName('embed');
        for(var i=0;i<embedList.length;i++) {
            swfList.push(makeMsg("collect",embedList[i]));
        }

        var objectList=document.getElementsByTagName('object');    
        for(var i=0;i<objectList.length;i++) {
            swfList.push(makeMsg("collect",objectList[i]));
        }

        for(var i=0;i<swfList.length;i++) {
            sendResponse({});
            chrome.extension.sendRequest(swfList[i]);
        }    
    } else if(request.msg=="navigate") {
       window.location=request.href;
    }
});

document.addEventListener("fullscreen", function(e) {
    chrome.extension.sendRequest(makeMsg("run", e.srcElement));
});

// swiffout://swiffout_href=http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525
//http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525
