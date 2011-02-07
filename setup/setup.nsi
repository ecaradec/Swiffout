;--------------------------------
;include version number
    !include "VersionNb.nsh"

;--------------------------------
;include modern ui

    !include "mui2.nsh"

;--------------------------------
;general

    ;name and file
    name "SwiffOut"
    outfile "..\SwiffOut Setup.exe"

    ;request application privileges for windows vista
    requestexecutionlevel user

;--------------------------------
;interface settings

    !define mui_abortwarning

;--------------------------------
;pages    
    !insertmacro mui_page_welcome
    ;!insertmacro mui_page_license "license.txt"
    ;!insertmacro mui_page_components
    !insertmacro mui_page_instfiles
    
    !insertmacro mui_unpage_confirm
    !insertmacro mui_unpage_instfiles
    
;--------------------------------
;languages
 
    !insertmacro mui_language "english"

;--------------------------------
;installer sections

installdir "$APPDATA\swiffout"
installdirregkey HKCU "Software\SwiffOut" ""

section
    ; setoutpath "$INSTDIR\${productVersion}"
    setoutpath "$INSTDIR"

    writeregstr hkcu "Software\SwiffOut" "InstallDir" $INSTDIR

    file ..\release\swiffoutrunner.exe

    file ..\release\ieExt.dll
    RegDLL  "$OUTDIR\ieExt.dll"

    createdirectory "$smprograms\swiffout"
    createshortcut "$smprograms\swiffout\swiffout uninstall.lnk" "$INSTDIR\uninstall.exe"
    createshortcut "$smprograms\swiffout\swiffout help.lnk" "http://swiffout.com/help.html" "" "$OUTDIR\swiffoutrunner.exe"
    
    ;create uninstaller
    writeuninstaller "$INSTDIR\uninstall.exe"
    writeregstr hkcu "software\microsoft\windows\currentversion\uninstall\swiffout" "displayname" "swiffout"
    writeregstr hkcu "software\microsoft\windows\currentversion\uninstall\swiffout" "uninstallstring" "$INSTDIR\uninstall.exe"

    ; open help
    execshell "open" "http://swiffout.com/welcome.html"
sectionend

;--------------------------------
;uninstaller section

section "uninstall"    
    setoutpath "$INSTDIR"

    ;add your own files here...

    delete "$INSTDIR\uninstall.exe"

    rmdir /r "$INSTDIR"

    ;ieExt
    UnRegDLL $INSTDIR\ieExt.dll
sectionend
