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
            gBrowser.addTab("http://swiffout.com/welcome.html");
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

       var self=this;
       gBrowser.addEventListener("swiffout.fullscreen", function(ev)  {
           var e=ev.target;
           var src=self.getSrc(e);
           var flashvars=e.getAttribute('flashvars');
           
           if(/^http:\/\/(www\.|)swiffoutgames\.com\//.test(src))
               self.runSwf(src,flashvars);
       },false,true);

       gBrowser.addEventListener("load", function(ev)  {
           if(ev.target.body) {
                var swiffoutNode=ev.target.createElement("div");
                swiffoutNode.setAttribute("id", "swiffout");
                ev.target.body.appendChild(swiffoutNode);
           }
       },true);
    },
    runSwf:function(src,flashvars) {
        var extensionPath = Components.classes["@mozilla.org/extensions/manager;1"].  
            getService(Components.interfaces.nsIExtensionManager).  
            getInstallLocation("swiffout@grownsoftware.com"). // guid of extension  
            getItemLocation("swiffout@grownsoftware.com"); 

        // create an nsILocalFile for the executable
        var file = Components.classes["@mozilla.org/file/local;1"]
                             .createInstance(Components.interfaces.nsILocalFile);
        file.initWithPath(extensionPath.path+"\\plugins\\swiffoutrunner.exe");

        // create an nsIProcess
        var process = Components.classes["@mozilla.org/process/util;1"]
                                .createInstance(Components.interfaces.nsIProcess);
        process.init(file);

        // Run the process.
        // If first param is true, calling thread will be blocked until
        // called process terminates.
        // Second and third params are used to pass command-line arguments
        // to the process.
        var args = ["swiffout:swiffout_href="+src+",swiffout_width=640,swiffout_height=480,swiffout_flashvars="+flashvars];
        process.run(false, args, args.length);
    },
    onMenuItemCommand: function(e) {        
        var self=this;
        function log(msg) {
            var consoleService = Components.classes["@mozilla.org/consoleservice;1"].getService(Components.interfaces.nsIConsoleService);
            consoleService.logStringMessage(""+msg);
        }

        function getEltDesc(e) {
            var cs=window.getComputedStyle(e,null);
            var w=parseInt(cs.width,10);
            var h=parseInt(cs.height,10);

            return {
                src:self.getSrc(e),
                width:w,
                height:h,
                flashvars:self.getParam(e, "flashvars")
            }
        }

        var swfList=[];
        function parseDocument(d) {
            var items=d.getElementsByTagName("embed");            
            for(var e=0;e<items.length;e++) {
                swfList.push(getEltDesc(items[e]));
            }
            var items=d.getElementsByTagName("object");
            for(var e=0;e<items.length;e++) {
                swfList.push(getEltDesc(items[e]));
            }
        }

        parseDocument(gBrowser.contentDocument);

        var iframes=gBrowser.contentDocument.getElementsByTagName("iframe");
        for(var i=0;i<iframes.length;i++) {
            parseDocument(iframes[i].contentDocument);
        }

        swfList.sort(function(a,b) { return (a.width*a.height)<(b.width*b.height); });

        this.runSwf(swfList[0].src, swfList[0].flashvars);
        gBrowser.contentDocument.location.href="http://swiffout.com/cpu-preservation.html";
    },
    getSrc:function(e) {
        var src=e.getAttribute('data') || e.getAttribute('src') || this.getParam(e,'movie|data|src|code|filename|url');
        
        var l=e.ownerDocument.location;
        if(/^\//.test(src))
            src=l.protocol+"//"+l.hostname+src;
        else if(/^http:/.test(src) || /^https:/.test(src))
            src=src;
        else
            src=l.href.substring(0,l.href.lastIndexOf("/"))+"/"+src;

        return src;
    },
    getParam:function(e,n) {
        var v = '', r = new RegExp('^('+n+')$', 'i');
        var param = e.getElementsByTagName('param');
        for(var i = 0, p; p = param[i]; i++){
            if(p.hasAttribute('name') && p.getAttribute('name').match(r)){v = p.getAttribute('value'); break};
        }
        return v;
    }
};
window.addEventListener("load", function(e) { swiffout.onLoad(e); }, false);
