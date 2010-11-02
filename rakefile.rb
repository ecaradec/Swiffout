task :all, [:version]=> [:engine, :chromeExt, :firefoxExt, :nsis, :copy] do
    puts "done"
end

task :copy, [:version] do |t,args|
    FileUtils.mkdir_p "build/#{args.version}"
    FileUtils.cp "build/SwiffOut Setup.exe", "build/#{args.version}/SwiffOut Setup.exe"
    FileUtils.cp "build/SwiffOut Desktop Setup.exe", "build/#{args.version}/SwiffOut Desktop Setup.exe"
    FileUtils.cp "build/swiffout_gc.crx", "build/#{args.version}/swiffout_gc.crx"
    FileUtils.cp "build/swiffout_gc.zip", "build/#{args.version}/swiffout_gc.zip"
    FileUtils.cp "build/swiffout_ff.xpi", "build/#{args.version}/swiffout_ff.xpi"
end

task :nsis, [:version] do |t,args|
    puts "building setup..."

    File.open("setup/VersionNb.nsh","w+") do |f|
        f << "VIAddVersionKey \"FileVersion\" \"#{args.version}\"\n"
        f << "VIProductVersion \"#{args.version}\"\n"        
        f  << "!define productVersion \"#{args.version}\""
        f  << "!define chromeExtVersion \"#{args.version}\""
    end

    sh '"c:\Program Files\NSIS\makensis.exe" setup/setup.nsi'
    FileUtils.mv "SwiffOut Setup.exe","build/SwiffOut Desktop Setup.exe"

    sh '"c:\Program Files\NSIS\makensis.exe" /DADDONS setup/setup.nsi'
    FileUtils.mv "SwiffOut Setup.exe","build/SwiffOut Setup.exe"
end

task :engine, [:version] do |t,args|
    puts "building engine..."

    File.open("VersionNb.h","w+") do |f|
        f << "#define _PRODUCTVERSIONSTR_  \"#{args.version}\"\n"
        f << "#define _FILEVERSIONSTR_ \"#{args.version}\"\n"
        f << "#define _PRODUCTVERSION_ #{args.version.gsub(/\./,",")}\n"
        f << "#define _FILEVERSION_ #{args.version.gsub(/\./,",")}\n"
        f << "#define _RELEASEDATESTR_ \"#{Time.now.strftime("%d %b %y")}\"\n"        
    end

    sh '"C:/Program Files/Microsoft Visual Studio .NET 2002/Common7/IDE/devenv.com" swiffout.sln /Rebuild Release'
end

task :firefoxExt, [:version] do |t,args|
    puts "building firefox ext..."
    
    FileUtils.rm_r "tmp/firefoxExt", :force=>true
    FileUtils.mkdir_p "tmp/firefoxExt/chrome"
    FileUtils.cp_r "firefoxExt/defaults","tmp/firefoxExt/"
    FileUtils.mkdir_p "firefoxExt/plugins"
    FileUtils.cp "release/swiffoutrunner.exe", "firefoxExt/plugins/swiffoutrunner.exe"
    FileUtils.cp_r "firefoxExt/plugins","tmp/firefoxExt/"
    FileUtils.cp "firefoxExt/install.rdf","tmp/firefoxExt/install.rdf"

    File.open("tmp/firefoxExt/install.rdf","w") do |f| f << <<-HEREDOC
<?xml version="1.0"?>
<RDF:RDF xmlns:em="http://www.mozilla.org/2004/em-rdf#"
         xmlns:NC="http://home.netscape.com/NC-rdf#"
         xmlns:RDF="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
  <RDF:Description RDF:about="urn:mozilla:install-manifest"
                   em:id="swiffout@grownsoftware.com"
                   em:name="SwiffOut"
                   em:version="#{args.version}"
                   em:creator="GrownSoftware"
                   em:description="Full screen flash gaming without horrible lag">
    <em:targetApplication RDF:resource="rdf:#$mdrO.2"/>
  </RDF:Description>
  <RDF:Description RDF:about="rdf:#$mdrO.2"
                   em:id="{ec8030f7-c20a-464f-9b0e-13a3a9e97384}"
                   em:minVersion="1.5"
                   em:maxVersion="4.0.*" />
</RDF:RDF>
    HEREDOC
    end

    File.open("tmp/firefoxExt/chrome.manifest","w") do |f| f << <<-HEREDOC
content swiffout jar:chrome/swiffout.jar!/content/
locale swiffout en-US jar:chrome/swiffout.jar!/locale/en-US/
skin swiffout classic/1.0 jar:chrome/swiffout.jar!/skin/
overlay	chrome://browser/content/browser.xul	chrome://swiffout/content/firefoxOverlay.xul
style	chrome://global/content/customizeToolbar.xul	chrome://swiffout/skin/overlay.css
    HEREDOC
    end

    FileUtils.cd("firefoxExt") {
        `7z a -tzip "swiffout.jar" content locale skin -r -mx=0`    
    }

    FileUtils.mv "firefoxExt/swiffout.jar","tmp/firefoxExt/chrome/"

    FileUtils.cd("tmp/firefoxExt") {
        `7z a -tzip "swiffout_ff.xpi" * -r -mx=0`    
    }

    FileUtils.cp "tmp/firefoxExt/swiffout_ff.xpi", "build/swiffout_ff.xpi"
end

task :chromeExt, [:version] do |t,args|
    require 'crxmake'

    puts "building chrome ext..."
    
    File.open("chromeExt/manifest.json","w+") do |f| f<<
%{
{
  "name": "SwiffOut",
  "version": "__VERSION__",
  "description": "Full screen flash gaming without horrible lag",
  "browser_action": {
    "default_icon": "icon.png"
  },
  "icons": { "48":"icon48.png", "128":"icon128.png" },
  "permissions": ["tabs","http://swiffout.com/*"],
  "content_scripts": [ {
      "js": [ "swiffout.js"],
      "matches": [ "http://*/*"],
      "run_at": "document_start",
      "all_frames": true
  }, {
      "js": [ "addswiffoutelement.js"],
      "matches": [ "http://*/*"],
      "run_at": "document_end",
      "all_frames": true
  }
  ],
  "plugins": [ {
      "path": "plugin/npprotocol.dll",
      "public": true
   } ],
  "background_page": "background.html"
}
}.gsub(/__VERSION__/,args.version)
    end

    FileUtils.mkdir_p "chromeExt/plugin"
    
    FileUtils.cp "release/regProtocol.dll", "chromeExt/plugin/npprotocol.dll"
    FileUtils.cp "release/swiffoutrunner.exe", "chromeExt/plugin/swiffoutrunner.exe"

    # all in one version (for google website )
    FileUtils.cp "chromeExt/background.html", "background.html.bak1"
    txt=File.read("chromeExt/background.html");

    CrxMake.zip(
        :ex_dir => "chromeExt",
        :zip_output => "build/swiffout_gc.zip"
    )

    # embeded version
    CrxMake.make(
        :ex_dir => "chromeExt",
        :pkey   => "chromeExt.pem",
        :crx_output => "build/swiffout_gc.crx"
    )
end
