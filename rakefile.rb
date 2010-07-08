task :all, [:version]=> [:chromeExt, :firefoxExt, :engine, :nsis] do
    puts "done"
end

task :nsis, [:version] do |t,args|
    puts "building setup..."

    File.open("setup/VersionNb.nsh","w+") do |f|
        f << "VIAddVersionKey \"FileVersion\" \"#{args.version}\"\n"
        f << "VIProductVersion \"#{args.version}\"\n"        
        f  << "!define chromeExtVersion \"#{args.version}\""
    end
    sh '"c:\Program Files\NSIS\makensis.exe" setup/setup.nsi'
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

    sh '"C:\Program Files\Microsoft Visual Studio .NET 2002\Common7\IDE\devenv.exe" swiffout.sln /Build Release'
end

task :firefoxExt, [:version] do |t,args|
    puts "building firefox ext..."
    
    File.open("firefoxExt/install.rdf","w+") do |f| f<<
%{<?xml version="1.0" encoding="UTF-8"?>
<RDF xmlns="http://www.w3.org/1999/02/22-rdf-syntax-ns#" 
 xmlns:em="http://www.mozilla.org/2004/em-rdf#">
  <Description about="urn:mozilla:install-manifest">
    <em:id>swiffout@grownsoftware.com</em:id>
    <em:name>SwiffOut</em:name>
    <em:version>__VERSION__</em:version>
    <em:creator>GrownSoftware</em:creator>
    <em:description>Play flash games real fullscreen</em:description>
    <em:targetApplication>
      <Description>
        <em:id>\{ec8030f7-c20a-464f-9b0e-13a3a9e97384\}</em:id> <!-- firefox -->
        <em:minVersion>1.5</em:minVersion>
        <em:maxVersion>3.6.*</em:maxVersion>
      </Description>
    </em:targetApplication>
  </Description>
</RDF>}.gsub(/__VERSION__/,args.version)
    end
end

task :chromeExt, [:version] do |t,args|
    puts "building chrome ext..."

    File.open("chromeExt/manifest.json","w+") do |f| f<<
%{
{
  "name": "SwiffOut",
  "version": "__VERSION__",
  "description": "SwiffOut - Play flash games real fullscreen.",
  "browser_action": {
    "default_icon": "icon.png"
  },
  "icons": { "48":"icon48.png", "128":"icon128.png" },
  "permissions": ["tabs","http://grownsoftware.com/*"],
  "content_scripts": [ {
      "js": [ "swiffout.js"],
      "matches": [ "http://*/*"],
      "run_at": "document_start",
      "all_frames": true
  } ],
  "background_page": "background.html"
}
}.gsub(/__VERSION__/,args.version)
    end

    require 'crxmake'
    # create crx
    CrxMake.make(
        :ex_dir => "chromeExt",
        :pkey   => "chromeExt.pem",
        :crx_output => "chromeExt.crx"
    )    
end
