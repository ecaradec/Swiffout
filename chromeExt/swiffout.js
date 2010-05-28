chrome.extension.onRequest.addListener(function(request, sender, sendResponse) {
    if(request.msg=="findSwf") {
        var href="swiffout:";
        var swfList=[];


        function getEmbedFlashVars(o) {
            var v=o.getAttribute("flashvars")
            var m1=o.innerHTML.match(/flashVars="(.*?)"/i);
            var m2=o.innerHTML.match(/flashVars='(.*?)'/i);

            if(!v) {
                if(m1) {
                    v=m1[1];
                } else if(m2) {
                    v=m2[1];
                }
            }

            v=v.replace(/&amp;/g,"&");
            return v;
        }

        function getObjectFlashVars(o) {
            var v=o.getAttribute("flashvars")
            var m1=o.innerHTML.match(/name="flashVars" +value="([^<>"]*?)"/i);
            var m2=o.innerHTML.match(/value="([^<>"]*?)" +name="flashVars"/i);
            var m3=o.innerHTML.match(/name='flashVars' +value='([^<>"]*?)'/i);
            var m4=o.innerHTML.match(/value='([^<>"]*?)' +name='flashVars'/i);

            if(!v) {
                if(m1) {
                    v=m1[1];
                } else if(m2) {
                    v=m2[1];
                } else if(m3) {
                    v=m3[1];
                } else if(m4) {
                    v=m4[1];
                }
            }

            v=v.replace(/&amp;/g,"&").replace(/&gt;/g,">").replace(/&lt;/g,"<");
            return v;
        }

        var embedList=document.getElementsByTagName('embed');
        for(var i=0;i<embedList.length;i++) {
            swfList.push({id:embedList.item(i).id, src:embedList.item(i).src || embedList.item(i).data || embedList.item(i).Movie, width: embedList.item(i).width, height: embedList.item(i).height, flashVars:getEmbedFlashVars(embedList.item(i)) });
        }

        var objectList=document.getElementsByTagName('object');    
        for(var i=0;i<objectList.length;i++)
            if(objectList.item(i).src || objectList.item(i).data || objectList.item(i).Movie)
                swfList.push({id:objectList.item(i).id, src:objectList.item(i).data || objectList.item(i).Movie, width: objectList.item(i).width, height: objectList.item(i).height, flashVars:getObjectFlashVars(objectList.item(i)) });


        for(var i=0;i<swfList.length;i++) {
            var src=swfList[i].src;
            
            var l=location;
            if(/^\//.test(src))
                src=l.protocol+"//"+l.hostname+src;
            else if(/^http:/.test(src) || /^https:/.test(src))
                src=src;
            else
                src=l.href.substring(0,l.href.lastIndexOf("/"))+"/"+src;

            swfList[i].src=src;
            sendResponse({});
            chrome.extension.sendRequest(swfList[i]);

            //alert("swiffout://swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height+",swiffout_flashvars="+swfList[i].flashVars);
        }    
    } else if(request.msg=="runSwf") {
        //alert("swiffout://swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height);

        window.location="swiffout:swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height+",swiffout_flashvars="+request.flashVars;
        window.setTimeout(function() {
           window.location="http://www.grownsoftware.com/swiffout/cpu-preservation.html";
        }, 1000);

        //console.log("swiffout:swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height);
        sendResponse({});
    }
});

// swiffout://swiffout_href=http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525
//http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525
