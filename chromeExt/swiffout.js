

chrome.extension.onRequest.addListener(function(request, sender, sendResponse) {
    if(request.msg=="findSwf") {
        var href="swiffout:";
        var swfList=[];
        var embedList=document.getElementsByTagName('embed');
        for(var i=0;i<embedList.length;i++) {
            swfList.push({id:embedList.item(i).id, src:embedList.item(i).src || embedList.item(i).data || embedList.item(i).Movie, width: embedList.item(i).width, height: embedList.item(i).height, flashVars:embedList.item(i).flashVars });
        }

        var objectList=document.getElementsByTagName('object');    
        for(var i=0;i<objectList.length;i++)
            if(objectList.item(i).src || objectList.item(i).data || objectList.item(i).Movie)
            swfList.push({id:objectList.item(i).id, src:objectList.item(i).data || objectList.item(i).Movie, width: objectList.item(i).width, height: objectList.item(i).height, flashVars:objectList.item(i).flashVars });

        for(var i=0;i<swfList.length;i++) {
            var swf=swfList[i].src;
            if(!/^http:\/\//.test(swf))
                swfList[i]=location.protocol+"//"+location.hostname+swf;
            
            sendResponse({});
            chrome.extension.sendRequest(swfList[i]);
        }    
    } else if(request.msg=="runSwf") {
        //alert("swiffout://swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height);
        window.location="swiffout:swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height;
        window.setTimeout(function() {
           window.location="http://www.grownsoftware.com/swiffout/cpu-preservation.html";
        }, 1000);

        console.log("swiffout:swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height);
        sendResponse({});
    }
});

// swiffout://swiffout_href=http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525
//http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525

