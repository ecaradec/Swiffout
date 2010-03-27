#define _WIN32_WINNT 0x0400
#include "resource.h"

#include <afxwin.h>
#include <Ole2.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#import "c:\\windows\\system32\\macromed\\flash\\flash.ocx" named_guids no_auto_exclude
#include "CheckLicKey.h"
#include "SwiffOutWnd.h"
#include "LicenceDlg.h"

// sample licence key
// {4BA4A9AD-5751-672B-C6B8-FC387F0E1CDC}

// swiffout:swiffout_href=http://armorgames.com/files/games/zoo-transport-5289.swf,swiffout_width=780,swiffout_height=450
// javascript:location.href="bgamefu:http://armorgames.com/files/games/civilizations-wars-5151.swf"
struct SwiffOut : CWinApp {
    DEVMODE      defaultDM;
    bool         setResolution;
    SwiffOutWnd *flashWnd;

    BOOL InitInstance() {
        //_CrtSetBreakAlloc(71);
        OleInitialize(0);
        // parse params
        CString cmdline(m_lpCmdLine);

        //CreateDialog(0, MAKEINTRESOURCE(IDD_LICENCE_DIALOG), 0, (DLGPROC)DefDlgProc);
        if(!CHECKLICKEY) {
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

            int trialPeriodLength=15;
            unsigned __int64 daysLeft=trialPeriodLength - (ftNow-ftInstall) / 100 / 100 / 1000 / 60 / 60 / 24;

            CLicenceDlg licenceDlg(daysLeft); // could be two dialogs
            licenceDlg.DoModal();
            
            if(!CHECKLICKEY && daysLeft<=0)
                return FALSE;
        }

        setResolution=true;
        if(cmdline.Find(L"/setresolution=0")!=-1)
            setResolution=false;

        CStringA swf;
        
        int hrefIndex=cmdline.Find(L"swiffout_href=");
        int widthIndex=cmdline.Find(L",swiffout_width=");
        int heightIndex=cmdline.Find(L",swiffout_height=");
        
        swf=cmdline.Mid(hrefIndex+14, widthIndex-hrefIndex-14);    
        int width=_ttoi(cmdline.Mid(widthIndex+16));
        int height=_ttoi(cmdline.Mid(heightIndex+17));

        memset(&defaultDM,0,sizeof(defaultDM));
        // set resolution        
        defaultDM.dmSize=sizeof(DEVMODE);
        
        EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &defaultDM);

        DEVMODE dm={0};
        dm.dmSize=sizeof(DEVMODE);
        dm.dmBitsPerPel = 32;
        dm.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
        dm.dmPelsWidth=800;
        dm.dmPelsHeight=600;
        dm.dmDisplayFrequency=60;

        CRegKey r;
        r.Create(HKEY_CURRENT_USER, L"Software\\SwiffOut");  
        r.QueryDWORDValue(L"width",dm.dmPelsWidth);
        r.QueryDWORDValue(L"height",dm.dmPelsHeight);
        r.QueryDWORDValue(L"freq",dm.dmDisplayFrequency);

        if(setResolution) {    
            LONG l=ChangeDisplaySettingsEx(0, &dm, 0, CDS_FULLSCREEN, 0);
        }

        flashWnd=new SwiffOutWnd;
        m_pMainWnd=flashWnd;

        float wRatio=(float)dm.dmPelsWidth/width;
        float hRatio=(float)dm.dmPelsHeight/height;

        float ratio=min(wRatio,hRatio);

        RECT rWin;
        rWin.top=0;
        rWin.left=0;
        rWin.right=dm.dmPelsWidth;
        rWin.bottom=dm.dmPelsHeight;

        int diffH=dm.dmPelsHeight-height*ratio;
        int diffW=dm.dmPelsWidth-width*ratio;

        RECT rSwf;
        rSwf.top=diffH/2;
        rSwf.left=diffW/2;
        rSwf.right=width*ratio+diffW/2;
        rSwf.bottom=height*ratio+diffH/2;

        flashWnd->Create((CHAR*)swf.GetString(), rWin, rSwf);
        
        return TRUE;
    }
    int ExitInstance()
    {
        delete flashWnd;

        if(setResolution)
            ChangeDisplaySettingsEx(0, &defaultDM, 0, CDS_FULLSCREEN, 0);
        
        OleUninitialize();
        return 0;
    }
} theApp;