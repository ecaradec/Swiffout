task :all, [:version]=> ["chromeExt.crx",:engine, :nsis] do
    puts "done"
end

task :nsis, [:version] do |t,args|
    puts "building setup..."

    File.open("setup/VersionNb.nsh","w+") do |f|
        f << "VIAddVersionKey \"FileVersion\" \"#{args.version}\"\n"
        f << "VIProductVersion \"#{args.version}\"\n"
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

    sh '"c:\Program Files\Microsoft Visual Studio 8\Common7\IDE\devenv.com" swiffout.sln /Build Release'
end

task :chromeExt => ["chromeExt.crx"]
file "chromeExt.crx" => FileList["chromeExt/*"] do
    puts "building chrome ext..."
    require 'crxmake'
    # create crx
    CrxMake.make(
        :ex_dir => "chromeExt",
        :pkey   => "chromeExt.pem",
        :crx_output => "chromeExt.crx"
    )    
end

task :firefoxExt do
    sh "build.bat"
end
