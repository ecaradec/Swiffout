chrome.extension.onRequest.addListener(function(request, sender, sendResponse) {
    if(request.msg=="findSwf") {
        var href="swiffout:";
        var swfList=[];

        // getParam(o,'movie|data|src|code|filename|url')
        function getParam(e, n){
			var v = '', r = new RegExp('^('+n+')$', 'i');
			var param = e.getElementsByTagName('param');
			for(var i = 0, p; p = param[i]; i++){
				if(p.hasAttribute('name') && p.getAttribute('name').match(r)){v = p.getAttribute('value'); break};
			}
			return v;
		}

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

            if(v)
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

            if(v)
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

//            alert("swiffout://swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height+",swiffout_flashvars="+swfList[i].flashVars);
        }    
    } else if(request.msg=="runSwf") {
        //alert("swiffout://swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height);

        window.location="swiffout:swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height+",swiffout_flashvars="+request.flashVars;
        window.setTimeout(function() {
           window.location="http://swiffout.com/cpu-preservation.html";
        }, 1000);

        //console.log("swiffout:swiffout_href="+request.src+",swiffout_width="+request.width+",swiffout_height="+request.height);
        sendResponse({});
    }
});

//swiffout://swiffout_href=undefined,swiffout_width=undefined,swiffout_height=undefined,swiffout_flashvars=chat_ip=216.246.59.237&chat_host=of1.kongregate.com&chat_port=5222&chat_bosh_host=bosh1.kongregate.com&chat_bosh_port=80&chat_username=ecaradec&chat_password=3383486_8bdbda08059047cd1ce0661d3753c1c85baf208d&api_host=http%3A%2F%2Fapi.kongregate.com&base_domain=kongregate.com&connect=true&enable_bosh=true&enable_compression=true&game_id=55550&game_permalink=frantic-2&game_title=Frantic%202&game_url=http%3A%2F%2Fwww.kongregate.com%2Fgames%2Fpolymerrabbit%2Ffrantic-2&javascript_listener=konduitToHolodeck&channel_id=1284313653858&debug_level=undefined&game_auth_token=2e2c1155cdfa4b60d1e2701cc67d7073aec6f97a53a40af50bf8f750bfa6dfc4&user_id=3383486&permission_slk=false&permission_mtx_api=false&permission_chat_api=false&permission_sc_api=true&user_vars=%5B%22ecaradec%22%2C%22%22%2C2%2C%22cdn2%3A%2Fassets%2Favatars%2Fdefaults_chat_sized%2Fduck.jpg%22%2C%22Frantic%202%22%2C%22%2Fgames%2Fpolymerrabbit%2Ffrantic-2%22%5D&user_vars_sig=vGiOERzw3NDsoiUnL0iA0w%3D%3D
//swiffout://swiffout_href=undefined,swiffout_width=undefined,swiffout_height=undefined,swiffout_flashvars=kongregate=true&kongregate_username=ecaradec&kongregate_user_id=3383486&kongregate_game_id=55550&kongregate_game_version=1253280582&kongregate_host=http%3A%2F%2Fwww.kongregate.com&kongregate_game_url=http%3A%2F%2Fwww.kongregate.com%2Fgames%2Fpolymerrabbit%2Ffrantic-2&kongregate_api_host=http%3A%2F%2Fapi.kongregate.com&kongregate_game_auth_token=2e2c1155cdfa4b60d1e2701cc67d7073aec6f97a53a40af50bf8f750bfa6dfc4&kongregate_stamp=2e2c1155cdfa4b60d1e2701cc67d7073aec6f97a53a40af50bf8f750bfa6dfc4&api_path=http%3A%2F%2Fchat.kongregate.com%2Fflash%2FAPI_AS3_deecb15432160a09ae1dcd62af00a0f9.swf&game_url=http%3A%2F%2Fwww.kongregate.com%2Fgames%2Fpolymerrabbit%2Ffrantic-2&kongregate_api_path=http%3A%2F%2Fchat.kongregate.com%2Fflash%2FAPI_AS3_deecb15432160a09ae1dcd62af00a0f9.swf&kongregate_channel_id=1284313653858
// swiffout://swiffout_href=http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525
//http://chat.kongregate.com/gamez/0006/6688/live/Creeper-kong-0325.swf?kongregate_game_version=1261618203,swiffout_width=700,swiffout_height=525
