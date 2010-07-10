#include "stdafx.h"

#define _WIN32_WINNT 0x0400
#include "resource.h"
#include "CheckLicKey.h"
#include "SwiffOutWnd.h"
#include "LicenceDlg.h"


// sample licence key
// {4BA4A9AD-5751-672B-C6B8-FC387F0E1CDC}

// swiffout:swiffout_href=http://armorgames.com/files/games/zoo-transport-5289.swf,swiffout_width=780,swiffout_height=450
// javascript:location.href="bgamefu:http://armorgames.com/files/games/civilizations-wars-5151.swf"
struct SwiffOut : CWinApp {
    DEVMODE      defaultDM;
    SwiffOutWnd *flashWnd;

    BOOL InitInstance() {
        //_CrtSetBreakAlloc(71);
        OleInitialize(0);
        // parse params
        CString cmdline(m_lpCmdLine);

        //CComPtr<IUnknown> p;
        //CLSID clsid;
        //CLSIDFromString(L"{9BA9E4C9-1147-4C81-B45E-DF966D095543}", &clsid);
        //p.CoCreateInstance(clsid);
        //p.CreateInstance(


        bool setResolution=true;
        if(cmdline.Find(L"/setresolution=0")!=-1)
            setResolution=false;

        CStringA swf, flashvars;
        
        int hrefIndex=cmdline.Find(L"swiffout_href=");
        int widthIndex=cmdline.Find(L",swiffout_width=");
        int heightIndex=cmdline.Find(L",swiffout_height=");
        int flashVarsIndex=cmdline.Find(L",swiffout_flashvars=");
        
        swf=cmdline.Mid(hrefIndex+14, widthIndex-hrefIndex-14);    
        int width=_ttoi(cmdline.Mid(widthIndex+16));
        int height=_ttoi(cmdline.Mid(heightIndex+17));
        flashvars=cmdline.Mid(flashVarsIndex+20);

        //CreateDialog(0, MAKEINTRESOURCE(IDD_LICENCE_DIALOG), 0, (DLGPROC)DefDlgProc);
		if(!CHECKLICKEY || swf.Find("http://swiffoutgames.com/")!=0) {
            CString dateStr;
            CRegKey k;
            k.Create(HKEY_LOCAL_MACHINE, L"Software\\Classes\\CLSID\\{1F2285D5-05F4-40ab-BFC2-BF3B9B7B1F50}");
            ULONG len=256;
            k.QueryStringValue(0, dateStr.GetBufferSetLength(len), &len); dateStr.ReleaseBuffer();
        

            unsigned __int64 ftInstall=0;
            SYSTEMTIME stInstall={0};
            stInstall.wYear=_ttoi(dateStr.Mid(4,4));
            stInstall.wMonth=_ttoi(dateStr.Mid(2,2));
            stInstall.wDay=_ttoi(dateStr.Mid(0,2));

            SystemTimeToFileTime(&stInstall,(FILETIME*)&ftInstall);

            unsigned __int64 ftNow=0;
            SYSTEMTIME stNow;
            GetSystemTime(&stNow);
            SystemTimeToFileTime(&stNow,(FILETIME*)&ftNow);

            int trialPeriodLength=30;
            __int64 daysLeft=trialPeriodLength - (ftNow-ftInstall) / 100 / 100 / 1000 / 60 / 60 / 24;
            
            CLicenceDlg licenceDlg(daysLeft); // could be two dialogs
            licenceDlg.DoModal();        
            
            if(!CHECKLICKEY && daysLeft<=0)
                return FALSE;
        }
        
        flashWnd=new SwiffOutWnd;
        m_pMainWnd=flashWnd;

        flashWnd->Create((CHAR*)swf.GetString(), (CHAR*)flashvars.GetString(), width, height, setResolution);
        flashWnd->m_cmdLine=cmdline;
        
        return TRUE;
    }
    int ExitInstance()
    {
        delete flashWnd;
        
        OleUninitialize();
        return 0;
    }
} theApp;