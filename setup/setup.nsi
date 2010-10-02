;nsis modern user interface
;basic example script
;written by joost verburg

Function GetTime
	!define GetTime `!insertmacro GetTimeCall`
 
	!macro GetTimeCall _FILE _OPTION _R1 _R2 _R3 _R4 _R5 _R6 _R7
		Push `${_FILE}`
		Push `${_OPTION}`
		Call GetTime
		Pop ${_R1}
		Pop ${_R2}
		Pop ${_R3}
		Pop ${_R4}
		Pop ${_R5}
		Pop ${_R6}
		Pop ${_R7}
	!macroend
 
	Exch $1
	Exch
	Exch $0
	Exch
	Push $2
	Push $3
	Push $4
	Push $5
	Push $6
	Push $7
	ClearErrors
 
	StrCmp $1 'L' gettime
	StrCmp $1 'A' getfile
	StrCmp $1 'C' getfile
	StrCmp $1 'M' getfile
	StrCmp $1 'LS' gettime
	StrCmp $1 'AS' getfile
	StrCmp $1 'CS' getfile
	StrCmp $1 'MS' getfile
	goto error
 
	getfile:
	IfFileExists $0 0 error
	System::Call /NOUNLOAD '*(i,l,l,l,i,i,i,i,&t260,&t14) i .r6'
	System::Call /NOUNLOAD 'kernel32::FindFirstFileA(t,i)i(r0,r6) .r2'
	System::Call /NOUNLOAD 'kernel32::FindClose(i)i(r2)'
 
	gettime:
	System::Call /NOUNLOAD '*(&i2,&i2,&i2,&i2,&i2,&i2,&i2,&i2) i .r7'
	StrCmp $1 'L' 0 systemtime
	System::Call /NOUNLOAD 'kernel32::GetLocalTime(i)i(r7)'
	goto convert
	systemtime:
	StrCmp $1 'LS' 0 filetime
	System::Call /NOUNLOAD 'kernel32::GetSystemTime(i)i(r7)'
	goto convert
 
	filetime:
	System::Call /NOUNLOAD '*$6(i,l,l,l,i,i,i,i,&t260,&t14)i(,.r4,.r3,.r2)'
	System::Free /NOUNLOAD $6
	StrCmp $1 'A' 0 +3
	StrCpy $2 $3
	goto tolocal
	StrCmp $1 'C' 0 +3
	StrCpy $2 $4
	goto tolocal
	StrCmp $1 'M' tolocal
 
	StrCmp $1 'AS' tosystem
	StrCmp $1 'CS' 0 +3
	StrCpy $3 $4
	goto tosystem
	StrCmp $1 'MS' 0 +3
	StrCpy $3 $2
	goto tosystem
 
	tolocal:
	System::Call /NOUNLOAD 'kernel32::FileTimeToLocalFileTime(*l,*l)i(r2,.r3)'
	tosystem:
	System::Call /NOUNLOAD 'kernel32::FileTimeToSystemTime(*l,i)i(r3,r7)'
 
	convert:
	System::Call /NOUNLOAD '*$7(&i2,&i2,&i2,&i2,&i2,&i2,&i2,&i2)i(.r5,.r6,.r4,.r0,.r3,.r2,.r1,)'
	System::Free $7
 
	IntCmp $0 9 0 0 +2
	StrCpy $0 '0$0'
	IntCmp $1 9 0 0 +2
	StrCpy $1 '0$1'
	IntCmp $2 9 0 0 +2
	StrCpy $2 '0$2'
	IntCmp $6 9 0 0 +2
	StrCpy $6 '0$6'
 
	StrCmp $4 0 0 +3
	StrCpy $4 Sunday
	goto end
	StrCmp $4 1 0 +3
	StrCpy $4 Monday
	goto end
	StrCmp $4 2 0 +3
	StrCpy $4 Tuesday
	goto end
	StrCmp $4 3 0 +3
	StrCpy $4 Wednesday
	goto end
	StrCmp $4 4 0 +3
	StrCpy $4 Thursday
	goto end
	StrCmp $4 5 0 +3
	StrCpy $4 Friday
	goto end
	StrCmp $4 6 0 error
	StrCpy $4 Saturday
	goto end
 
	error:
	SetErrors
	StrCpy $0 ''
	StrCpy $1 ''
	StrCpy $2 ''
	StrCpy $3 ''
	StrCpy $4 ''
	StrCpy $5 ''
	StrCpy $6 ''
 
	end:
	Pop $7
	Exch $6
	Exch
	Exch $5
	Exch 2
	Exch $4
	Exch 3
	Exch $3
	Exch 4
	Exch $2
	Exch 5
	Exch $1
	Exch 6
	Exch $0
FunctionEnd

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
    requestexecutionlevel highest

;--------------------------------
;interface settings

    !define mui_abortwarning

;--------------------------------
;pages    
    !insertmacro mui_page_welcome
    !insertmacro mui_page_license "license.txt"
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
    setoutpath "$INSTDIR\${chromeExtVersion}"

    writeregstr hkcu "Software\SwiffOut" "InstallDir" $INSTDIR

    file ..\release\swiffoutrunner.exe

    ;ieExt
    file ..\release\ieExt.dll
    RegDLL  "$OUTDIR\ieExt.dll"

    ; firefoxExt
    RMDir /r "$OUTDIR\firefoxExt" ; to clean folder modification time
    file /r ..\firefoxExt
    writeregstr hkcu "Software\Mozilla\Firefox\Extensions" "swiffout@grownsoftware.com" "$OUTDIR\firefoxExt"

    ; chromeExt
    file ..\chromeExt.crx

    writeregstr hkcu "Software\Google\Chrome\Extensions\kfbkdggicneacfcmlnnnaipijjabecmd" "path" "$OUTDIR\chromeExt.crx"
    writeregstr hkcu "Software\Google\Chrome\Extensions\kfbkdggicneacfcmlnnnaipijjabecmd" "version" ${chromeExtVersion}

    createdirectory "$smprograms\swiffout"
    createshortcut "$smprograms\swiffout\swiffout uninstall.lnk" "$INSTDIR\uninstall.exe"
    createshortcut "$smprograms\swiffout\swiffout help.lnk" "http://swiffout.com/help.html" "" "$OUTDIR\swiffoutrunner.exe"

    ;store installation folder
    writeregstr hkcu "software\classes\swiffout" "url protocol" ""
    writeregstr hkcu "software\classes\swiffout\shell\open\command" "" '"$OUTDIR\swiffoutrunner.exe" "%1"'
    
    ;create uninstaller
    writeuninstaller "$INSTDIR\uninstall.exe"
    writeregstr hkcu "software\microsoft\windows\currentversion\uninstall\swiffout" "displayname" "swiffout"
    writeregstr hkcu "software\microsoft\windows\currentversion\uninstall\swiffout" "uninstallstring" "$INSTDIR\uninstall.exe"

    ;write the installation date	
	; $0="01"      day
	; $1="04"      month
	; $2="2005"    year
	; $3="Friday"  day of week name
	; $4="11"      hour
	; $5="05"      minute
	; $6="50"      seconds    
    ClearErrors
    EnumRegKey $0 hkcu "software\Classes\CLSID\{1F2285D5-05F4-40ab-BFC2-BF3B9B7B1F50}" 0
    IfErrors 0 keyexist
        ${GetTime} "" "LS" $0 $1 $2 $3 $4 $5 $6
        writeregstr hkcu "software\Classes\CLSID\{1F2285D5-05F4-40ab-BFC2-BF3B9B7B1F50}" "" "$0$1$2" 
    keyexist:

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

    deleteregkey hkcu "software\classes\swiffout"
    deleteregkey hkcu "software\swiffout"
    
    ;chromeExt
    deleteregkey hkcu "Software\Google\Chrome\Extensions\kfbkdggicneacfcmlnnnaipijjabecmd"

    ;firefoxExt
    deleteregvalue hkcu "Software\Mozilla\Firefox\Extensions" "swiffout@grownsoftware.com"

    ;ieExt
    UnRegDLL $INSTDIR\ieExt.dll"
sectionend
