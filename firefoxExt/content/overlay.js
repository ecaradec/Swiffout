swiffout = {
    onLoad: function() {
        // initialization code
        this.initialized = true;
        this.strings = document.getElementById("swiffout-strings");

       // Read preferences
       var nsIPrefServiceObj = Components.classes["@mozilla.org/preferences-service;1"].getService(Components.interfaces.nsIPrefService);
       var nsIPrefBranchObj = nsIPrefServiceObj.getBranch("swiffout.");

       //First Run?
       if(!nsIPrefBranchObj.prefHasUserValue("toolbarbutton-initialized")) {
            nsIPrefBranchObj.setBoolPref("toolbarbutton-initialized",true);
            try {
              var firefoxnav = document.getElementById("nav-bar");
              var curSet = firefoxnav.currentSet;
              if (curSet.indexOf("swiffout-button") == -1)
              {
                var set;
                // Place the button before the urlbar
                if (curSet.indexOf("urlbar-container") != -1)
                  set = curSet.replace(/urlbar-container/, "swiffout-button,urlbar-container");
                else  // at the end
                  set = curSet + ",swiffout-button";
                firefoxnav.setAttribute("currentset", set);
                firefoxnav.currentSet = set;
                document.persist("nav-bar", "currentset");
                // If you don't do the following call, funny things happen
                try {
                  BrowserToolboxCustomizeDone(true);
                }
                catch (e) { }
              }
            }
            catch(e) { }
        }
    },
    onMenuItemCommand: function(e) {        
        function log(msg) {
            var consoleService = Components.classes["@mozilla.org/consoleservice;1"].getService(Components.interfaces.nsIConsoleService);
            consoleService.logStringMessage(""+msg);
        }

        function getSrc(e) {
            var src=e.getAttribute("data") || e.src || e.getAttribute("Movie");
            
            var l=gBrowser.contentDocument.location;
            if(/^\//.test(src))
                src=l.protocol+"//"+l.hostname+src;
            else if(/^http:/.test(src) || /^https:/.test(src))
                src=src;
            else
                src=l.href.substring(0,l.href.lastIndexOf("/"))+"/"+src;

            return src;
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

        var swfList=[];
        function parseDocument(d) {
            var items=d.getElementsByTagName("embed");            
            for(var e=0;e<items.length;e++) {
                swfList.push({
                    src:getSrc(items[e]),
                    orgSrc:items[e].getAttribute("data") || items[e].src || items[e].getAttribute("Movie"),
                    width:items[e].width,
                    height:items[e].height,
                    flashvars:getEmbedFlashVars(items[e])
                });
            }
            var items=d.getElementsByTagName("object");
            for(var e=0;e<items.length;e++) {
                swfList.push({
                    src:getSrc(items[e]),
                    orgSrc:items[e].getAttribute("data") || items[e].src || items[e].getAttribute("Movie"),
                    width:items[e].width,
                    height:items[e].height,
                    flashvars:getObjectFlashVars(items[e])
                });
            }
        }

        parseDocument(gBrowser.contentDocument);

        var iframes=gBrowser.contentDocument.getElementsByTagName("iframe");
        for(var i=0;i<iframes.length;i++) {
            parseDocument(iframes[i].contentDocument);
        }

        swfList.sort(function(a,b) { return (a.width*a.height)<(b.width*b.height); });

        //for(var i=swfList.length-1;i>=0;i--) {
        //    log("orgSrc :"+swfList[i].orgSrc+",src : "+swfList[i].src+",width : "+swfList[i].width+",height : "+swfList[i].height+",flashvars : "+swfList[i].flashvars);
        //}
        //alert("swiffout:swiffout_href="+swfList[0].src+",swiffout_width="+swfList[0].width+",swiffout_height="+swfList[0].height+",swiffout_flashvars="+swfList[0].flashvars);
        gBrowser.contentDocument.location.href="swiffout:swiffout_href="+swfList[0].src+",swiffout_width="+swfList[0].width+",swiffout_height="+swfList[0].height+",swiffout_flashvars="+swfList[0].flashvars;

        setTimeout(function() {
            gBrowser.contentDocument.location.href="http://grownsoftware.com/swiffout/cpu-preservation.html";
        }, 100);
    }
};
window.addEventListener("load", function(e) { swiffout.onLoad(e); }, false);
