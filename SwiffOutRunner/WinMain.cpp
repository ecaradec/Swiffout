#include "stdafx.h"

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
        
        CStringA swf, flashvars;
        
        int hrefIndex=cmdline.Find(L"swiffout_href=");
        int widthIndex=cmdline.Find(L",swiffout_width=");
        int heightIndex=cmdline.Find(L",swiffout_height=");
        int flashVarsIndex=cmdline.Find(L",swiffout_flashvars=");
        if(hrefIndex==-1 || widthIndex==-1 || heightIndex==-1 || flashVarsIndex==-1)
        {
            MessageBox(0, L"Oups, bad parameters !! SwiffOut didn't recognized parameters given.\nIt might be a website error or a SwiffOut error.\nTry some other games and contact me at contact@grownsoftware.com if it still doesn't work.", L"SwiffOut Error", MB_OK|MB_ICONEXCLAMATION);
            return FALSE;
        }

        
        swf=cmdline.Mid(hrefIndex+14, widthIndex-hrefIndex-14);    
        int width=_ttoi(cmdline.Mid(widthIndex+16));
        int height=_ttoi(cmdline.Mid(heightIndex+17));
        flashvars=cmdline.Mid(flashVarsIndex+20);

		/*if(!CHECKLICKEY && swf.Find("http://swiffoutgames.com/")!=0) {
            CRegKey k;
            k.Create(HKEY_LOCAL_MACHINE, L"Software\\Classes\\CLSID\\{1F2285D5-05F4-40ab-BFC2-BF3B9B7B1F50}");
			DWORD nbTry=10^0x53;
			k.QueryDWORDValue(L"try", nbTry);
            nbTry^=0x53;
            if(nbTry>10)
                nbTry=0;
            if(nbTry>=1) {
                nbTry--;
                k.SetDWORDValue(L"try",nbTry^0x53);
            }

            CLicenceDlg licenceDlg(nbTry); // could be two dialogs
            INT_PTR r=licenceDlg.DoModal();        
            
            bool allowed = checkSwiffOutKey();
            if(r==2 || (!allowed && nbTry<=0))
                return FALSE;
        }*/
        
        flashWnd=new SwiffOutWnd;
        m_pMainWnd=flashWnd;

        flashWnd->Create((CHAR*)swf.GetString(), (CHAR*)flashvars.GetString(), width, height);
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