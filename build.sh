#!/usr/bin/bash
version=$1
echo "#define _PRODUCTVERSIONSTR_ \"$version\"" > VersionNb.h
echo "#define _FILEVERSIONSTR_ \"$version\"" >> VersionNb.h
echo "#define _PRODUCTVERSION_ ${version//./,}" >> VersionNb.h
echo "#define _FILEVERSION_ ${version//./,}" >> VersionNb.h
echo "#define _RELEASEDATESTR_ \"`date +'%d %b %Y'`\"" >> VersionNb.h

echo "VIAddVersionKey \"FileVersion\" \"$version\"" > setup/VersionNb.nsh
echo "VIProductVersion \"$version\"" >> setup/VersionNb.nsh

/c/Program\ Files/Microsoft\ Visual\ Studio\ 8/Common7/IDE/devenv.com swiffout.sln /Rebuild Release\|Win32
/c/Program\ Files/NSIS/makensis.exe setup/setup.nsi

#cp release/swiffoutrunner.exe ../swiffout-builds/swiffoutrunner.$i.exe
#cp release/swiffoutsettings.exe ../swiffout-builds/swiffoutsettings.$i.exe

