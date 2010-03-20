;nsis modern user interface
;basic example script
;written by joost verburg

;--------------------------------
;include modern ui

    !include "mui2.nsh"

;--------------------------------
;general

    ;name and file
    name "swiffout setup"
    outfile "..\swiffout setup.exe"

    ;request application privileges for windows vista
    requestexecutionlevel user

;--------------------------------
;interface settings

    !define mui_abortwarning

;--------------------------------
;pages    
    !insertmacro mui_page_welcome
    !insertmacro mui_page_license "license.txt"
    !insertmacro mui_page_instfiles
    
    !insertmacro mui_unpage_confirm
    !insertmacro mui_unpage_instfiles
    
;--------------------------------
;languages
 
    !insertmacro mui_language "english"

;--------------------------------
;installer sections

section
    setoutpath "$programfiles\swiffout"

    file ..\release\swiffoutrunner.exe
    file ..\release\swiffoutsettings.exe
    file /r help

    createdirectory "$smprograms\swiffout"
    createshortcut "$smprograms\swiffout\swiffout uninstall.lnk" "$programfiles\swiffout\uninstall.exe"
    createshortcut "$smprograms\swiffout\swiffout settings.lnk" "$programfiles\swiffout\swiffoutsettings.exe"
    createshortcut "$smprograms\swiffout\swiffout help.lnk" "$programfiles\swiffout\help\help.html"

    ;store installation folder
    writeregstr hkcr "swiffout" "url protocol" ""
    writeregstr hkcr "swiffout\shell\open\command" "" "$programfiles\swiffout\swiffoutrunner.exe %1"
    
    ;create uninstaller
    writeuninstaller "$programfiles\swiffout\uninstall.exe"
    writeregstr hklm "software\microsoft\windows\currentversion\uninstall\swiffout" "displayname" "swiffout"
    writeregstr hklm "software\microsoft\windows\currentversion\uninstall\swiffout" "uninstallstring" "$programfiles\swiffout\uninstall.exe"

    ; open help
    execshell "open" "$programfiles\swiffout\help\help.html"
sectionend

;--------------------------------
;uninstaller section

section "uninstall"

    ;add your own files here...

    delete "$programfiles\swiffout\uninstall.exe"

    rmdir "$programfiles\swiffout"

    deleteregkey hkcr "swiffout"
    deleteregkey hkcu "software\swiffout"

sectionend
