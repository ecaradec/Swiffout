#include "stdafx.h"
#include "resource.h"
#include "SwiffOutWnd.h"
#include <afxdlgs.h>

// host should implement direct draw for better performances

// swiffout:swiffout_href=http://www.miniclip.com/games/final-ninja/pl/finalninja.swf,swiffout_width=550,swiffout_height=400,flashvars=undefined  /setresolution=0
// swiffout:swiffout_href=http://cdn.nitrome.com/games/bcbowcontest/bcbowcontest.swf,swiffout_width=550,swiffout_height=400,flashvars=undefined  /setresolution=0

//declare required GUIDs
#ifndef DEFINE_GUID2
#define DEFINE_GUID2(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const GUID name \
    = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }
#endif
    DEFINE_GUID2(CLSID_DirectDraw,0xD7B70EE0,0x4340,0x11CF,0xB0,0x63,0x00,0x20,0xAF,0xC2,0xCD,0x35);
    DEFINE_GUID2(CLSID_DirectDraw7,0x3c305196,0x50db,0x11d3,0x9c,0xfe,0x00,0xc0,0x4f,0xd9,0x30,0xc5);
    DEFINE_GUID2(IID_IDirectDraw,0x6C14DB80,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
    DEFINE_GUID2(IID_IDirectDraw3,0x618f8ad4,0x8b7a,0x11d0,0x8f,0xcc,0x0,0xc0,0x4f,0xd9,0x18,0x9d);
    DEFINE_GUID2(IID_IDirectDraw4,0x9c59509a,0x39bd,0x11d1,0x8c,0x4a,0x00,0xc0,0x4f,0xd9,0x30,0xc5);
    DEFINE_GUID2(IID_IDirectDraw7,0x15e65ec0,0x3b9c,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b);
    //declare smart pointer type for IDirectDraw4 interface
    _COM_SMARTPTR_TYPEDEF(IDirectDraw4, IID_IDirectDraw4);

struct FlashWndSubclass : CWnd {
    DECLARE_MESSAGE_MAP()
public:
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
        CWnd::OnKeyDown(nChar,nRepCnt,nFlags);
        if(nChar==VK_ESCAPE) {
            GetParent()->PostMessage(WM_KEYDOWN, nChar, nFlags);
        } else if(nChar==VK_F12 || nChar==VK_F11) {
            GetParent()->PostMessage(WM_KEYDOWN, nChar, nFlags);
        }
    }
    void OnRButtonDown(UINT nFlags, CPoint point) {
    }
    void OnRButtonUp(UINT nFlags, CPoint point) {        
        GetParent()->PostMessage(WM_RBUTTONUP, nFlags, (point.x|((point.y)<<16)));
    }
};

BEGIN_MESSAGE_MAP(FlashWndSubclass,CWnd)
    ON_WM_KEYDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


SwiffOutWnd::SwiffOutWnd()
{
}
SwiffOutWnd::~SwiffOutWnd() {
}
//the implementation of IServiceProvider interface
HRESULT SwiffOutWnd::raw_RemoteQueryService(GUID *guidService, GUID *riid, IUnknown **ppvObject)
{
    HRESULT hr;
    if (IsEqualGUID(*guidService, IID_IDirectDraw3))
    {
        if (!m_lpDD4)
        {
            //LPDIRECTDRAW lpDD;
            //DirectDrawCreate(0, &lpDD, 0);
            //lpDD->QueryInterface(IID_IDirectDraw4, (void**)&m_lpDD4);
            //m_lpDD4 = new IDirectDraw4Ptr;
            hr = CoCreateInstance(CLSID_DirectDraw, NULL, CLSCTX_INPROC_SERVER, IID_IDirectDraw4, (void**)&m_lpDD4);
            /*m_lpDD4->Initialize(NULL);
            
            hr=m_lpDD4->SetCooperativeLevel(GetHWND(), DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
            hr=m_lpDD4->SetDisplayMode(640,480,320,0,0);*/

            if (FAILED(hr))
            {
                delete m_lpDD4;
                m_lpDD4 = NULL;
                return E_NOINTERFACE;
            }
        }
        if (m_lpDD4)
        {
            *ppvObject = m_lpDD4;
            ((IUnknown*)*ppvObject)->AddRef();
            return S_OK;
        }
    }
    return E_NOINTERFACE;
}
void SwiffOutWnd::OnRButtonUp(UINT nFlags, CPoint point) {
    ClientToScreen(&point);
    m.TrackPopupMenu(0, point.x, point.y, this);
}
void SwiffOutWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    if(nChar==VK_ESCAPE) {
        DWORD enableEscKey=1;
        m_regkey.QueryDWORDValue(L"enableEscKey", enableEscKey);
        if(enableEscKey)
            Exit();
    } else if(nChar==VK_F12 || nChar==VK_F11) {
        CPoint p(m_rSwf.left, m_rSwf.top);
        ClientToScreen(&p);

        m.TrackPopupMenu(0, p.x, p.y, this);
    }
}
void SwiffOutWnd::Exit()
{
    ChangeDisplaySettingsEx(0, &defaultDM, 0, CDS_FULLSCREEN, 0);
    pOO->Close(OLECLOSE_NOSAVE);
    DestroyWindow();
}
/*void SwiffOutWnd::OnShowSettings(BOOL bShow, UINT nStatus) {
    if(bShow) {
        DWORD enableEscKey=BST_CHECKED;
        DWORD selectBestResolution=BST_CHECKED;
        DWORD startFullscreen=BST_CHECKED;

        CRegKey k;
        k.Open(HKEY_CURRENT_USER, L"Software\\SwiffOut");
        k.QueryDWORDValue(L"enableEscKey", enableEscKey);    
        k.QueryDWORDValue(L"selectBestResolution", selectBestResolution);
        k.QueryDWORDValue(L"startFullscreen", startFullscreen);

        m_pSettingsDlg->CheckDlgButton(IDC_ENABLE_ESC_CHECK, enableEscKey);
        m_pSettingsDlg->CheckDlgButton(IDC_BEST_RESOLUTION_CHECK, selectBestResolution);
        m_pSettingsDlg->CheckDlgButton(IDC_START_FULLSCREEN_CHECK, startFullscreen);

        CRect rSettings;
        m_pSettingsDlg->GetClientRect(&rSettings);


        CRect rClient;
        GetClientRect(&rClient);

        int flashWidth=m_rSwf.right-m_rSwf.left;
        float ratio = rSettings.Height()/flashWidth;
        CRect r(rClient.Width()/2-flashWidth/2, rSettings.Height(), rClient.Width()/2+flashWidth/2, m_rSwf.bottom);

        
        //r.MoveToY(rSettings.Height());
        //r.bottom=rSettings.bottom;
        //float ratio=rSettings.Height()/r.Height();
        //r.SetRect((int)r.Width()/2-r.Width()*ratio, (int)rSettings.Height(), (int)r.Width()/2+r.Width()*ratio, (int)r.Height());
        
        pOIPO->SetObjectRects(&r, &r);
        pOIPOW->SetObjectRects(&r, &r);
    } else {
        pOIPO->SetObjectRects(&m_rSwf, &m_rSwf);
        pOIPOW->SetObjectRects(&m_rSwf, &m_rSwf);
    }
}*/
/*static LRESULT __stdcall SwiffOutWnd::FlashWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    FlashWnd *thiz=(FlashWnd*)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    if(msg==WM_CREATE) {
		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lparam;
		SetWindowLong(hwnd, GWL_USERDATA, (long)lpcs->lpCreateParams);
		return 0;
	}

    if(msg==WM_CLOSE) {            
        DestroyWindow(hwnd);
        PostQuitMessage(0);
    }

    if(msg == WM_KEYDOWN && wparam==VK_ESCAPE) {
        thiz->SetFullscreen(false);
        //DestroyWindow(hwnd);
        //PostQuitMessage(0);
    }

    if(msg == WM_PAINT) {        
        HDC hdcDraw = ::GetDC(thiz->GetHWND());
        ::SetBkColor(hdcDraw,0);
        ExtTextOut(hdcDraw,0,0,ETO_OPAQUE,&thiz->m_rWin,0,0,0);

        RECT r;
        GetClientRect(thiz->GetHWND(),&r);
        r.left=thiz->m_rWin.right;
        ::SetBkColor(hdcDraw,GetSysColor(COLOR_3DFACE));
        ExtTextOut(hdcDraw,0,0,ETO_OPAQUE,&r,0,0,0);

        OleDraw(thiz->pVO, DVASPECT_CONTENT, hdcDraw, &thiz->m_rSwf);
        ::ReleaseDC(thiz->GetHWND(), hdcDraw);
    }

    if(msg == WM_COMMAND && wparam==1) {
        thiz->SetFullscreen(true);
    }

    // only use for windowless display
    if (thiz && thiz->pOIPOW)
    {
	    if (msg == WM_MOUSEMOVE || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_LBUTTONDBLCLK
		    || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP || msg == WM_RBUTTONDBLCLK
		    || msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP || msg == WM_MBUTTONDBLCLK
		    || msg == WM_MOUSEWHEEL 
		    || msg == WM_KEYDOWN || msg == WM_KEYUP || msg == WM_CHAR
		    || msg == WM_SETCURSOR)
	    {
		    HRESULT hr;
		    LRESULT res;
		    hr = thiz->pOIPOW->OnWindowMessage(msg, wparam, lparam, &res);
		    if (hr == S_OK)
			    return res;
	    }
    }

    return ::DefWindowProc(hwnd,msg,wparam,lparam);
}*/

void SwiffOutWnd::Create(CHAR *swf, CHAR *flashVars, int width, int height) {
    //
    // read registry
    //
    m_regkey.Open(HKEY_CURRENT_USER, L"Software\\SwiffOut");

    DWORD enableEscKey=BST_CHECKED;
    DWORD startFullscreen=BST_CHECKED;

    m_regkey.QueryDWORDValue(L"enableEscKey", enableEscKey);
    m_regkey.QueryDWORDValue(L"startFullscreen", startFullscreen);

    //
    // create popup
    //
    m.CreatePopupMenu();
    resMenu.CreatePopupMenu();

    m.AppendMenu(0, ID_TOGGLE_FULLSCREEN, L"Toggle Fullscreen");    
    m.AppendMenu(enableEscKey?MF_CHECKED:MF_UNCHECKED, ID_ENABLE_ESC, L"Use Esc to quit games");
    m.AppendMenu(startFullscreen?MF_CHECKED:MF_UNCHECKED, ID_START_FULLSCREEN, L"Start fullscreen");
    /*m.AppendMenu(MF_POPUP, (UINT_PTR)resMenu.GetSafeHmenu(), L"Resolution");*/
    m.AppendMenu(0, ID_CREATE_DESKTOP_SHORTCUT, L"Save a shortcut");
    m.AppendMenu(0, ID_EXIT, L"Quit game");

    //resMenu.AppendMenu(MF_CHECKED, ID_CLOSEST_RESOLUTION, L"Select closest resolution");

    //
    // scan for best resolution and populate menu
    //
    memset(&defaultDM,0,sizeof(defaultDM));
    // set resolution        
    defaultDM.dmSize=sizeof(DEVMODE);
    
    EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &defaultDM);

    // TODO: Add extra initialization here    
    memset(&dm,0,sizeof(dm));
    dm.dmSize=sizeof(DEVMODE);
    
    int resolutionNb=0;
    int resolutionCmd=ID_FIRST_RESOLUTION;
    int smallestDiff=0x07FFFFFF;
    int displayFreq=0;
    int bestMode=0;
    int mode=0;
    BOOL b;
    while(1) {
        dm.dmSize=sizeof(DEVMODE);
        if((b=EnumDisplaySettings(0, mode, &dm)) && dm.dmBitsPerPel==32) {

            CString str;
            //str.Format(L"%dx%d", dm.dmPelsWidth, dm.dmPelsHeight);
            //resMenu.AppendMenu(MF_UNCHECKED, resolutionCmd, str);
            resolutionNb++;
            resolutionCmd++;

            if( dm.dmPelsWidth >= width && dm.dmPelsHeight>= height) {
                int diff=dm.dmPelsWidth - width + dm.dmPelsHeight - height;
                if(diff<smallestDiff || (diff==smallestDiff && dm.dmDisplayFrequency>displayFreq)) {
                    smallestDiff=diff;
                    bestMode=mode;
                }
            }
        }
        if(!b)
            break;
        mode++;
    }

    EnumDisplaySettings(0, bestMode, &dm);

    m_fullscreen=startFullscreen;
    if(startFullscreen) {    
        LONG l=ChangeDisplaySettingsEx(0, &dm, 0, CDS_FULLSCREEN, 0);
    }

    //
    // resize the game, but keep a correct ratio 
    //
    float wRatio=(float)dm.dmPelsWidth/width;
    float hRatio=(float)dm.dmPelsHeight/height;

    float ratio=min(wRatio,hRatio);

    CRect rWin;
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


    DWORD   readBytes=0;
    HRESULT hr;
    m_lpBitsOnly=0;
    m_hdcBack=0;
    m_bmpBack=0;

    m_rSwf=rSwf;
    m_rWin=rWin;

    //
    // GUI stuff and flash initialisation
    //

    CreateEx(WS_EX_TOPMOST, AfxRegisterWndClass(0), L"SwiffOutRunner", WS_SYSMENU|WS_MINIMIZEBOX, 0, 0, rWin.right-rWin.left, rWin.bottom-rWin.top, 0, 0);

    CRect rClient;
    GetClientRect(&rClient);

    m_borderWidth=rWin.Width()-rClient.Width();
    m_borderHeight=rWin.Height()-rClient.Height();
        

    if(m_fullscreen) {
        ModifyStyle(WS_CAPTION,WS_POPUP);
        SetWindowPos(0,0,0,m_rWin.right-m_rWin.left,m_rWin.bottom-m_rWin.top,SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
    } else {
        CenterWindow();        
        SetWindowPos(&CWnd::wndNoTopMost,0,0,m_rWin.right-m_rWin.left+m_borderWidth,m_rWin.bottom-m_rWin.top+m_borderHeight,SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOMOVE);
    }


    hr=OleCreate(ShockwaveFlashObjects::CLSID_ShockwaveFlash, IID_IOleObject, OLERENDER_DRAW, 0, (IOleClientSite *)this, (IStorage *)this, (void **)&pOO);
    hr=OleSetContainedObject(pOO, TRUE);        

    pVOE=pOO;

    pVO=pOO;

    pOIPOW=pOO;

    pOIPO=pOO;

    pSF=pOO;

    // bizarrement en mode transparent c'est plus lent, mais ca consomme moins de CPU
    // apres verification : chrome consomme moins, parce qu'il donne moins de CPU a flash.
	//hr=pSF->put_WMode(L"transparent");
    //hr=pSF->put_Scale(L"exactfit");
    hr=pSF->put_Scale(L"showAll");
    hr=pSF->put_BackgroundColor(0x00000000);

    CComPtr<IDispatch> pDispSF(pSF);
    pDispSF.PutPropertyByName(L"flashVars", &CComVariant(flashVars));

    hr=pOO->DoVerb(OLEIVERB_SHOW, NULL, (IOleClientSite *)this, 0, NULL, NULL);
    
    pOIPO->SetObjectRects(&rSwf, &rSwf);
    pOIPOW->SetObjectRects(&rSwf, &rSwf);

    HWND flashHWND;
    pOIPO->GetWindow(&flashHWND);
    clss=new FlashWndSubclass;
    clss->SubclassWindow(flashHWND);
    
    hr=pSF->LoadMovie(0, _bstr_t(swf));
    hr=pSF->Play();

    SetCapture(TRUE);
}

void SwiffOutWnd::SetFullscreen(bool b) {
    m_fullscreen=b;
    if(b) {
        ChangeDisplaySettingsEx(0, &dm, 0, CDS_FULLSCREEN, 0);

        LONG style=GetWindowLong(GetHWND(),GWL_STYLE);
        style&=~WS_CAPTION;
        style|=WS_POPUP;
        SetWindowLong(GetHWND(), GWL_STYLE, style);
        SetWindowPos(&CWnd::wndTopMost,0,0,m_rWin.right-m_rWin.left,m_rWin.bottom-m_rWin.top,SWP_FRAMECHANGED);
        ::SetFocus(GetHWND());
    } else {
        ChangeDisplaySettingsEx(0, &defaultDM, 0, CDS_FULLSCREEN, 0);

        LONG style=GetWindowLong(GetHWND(),GWL_STYLE);
        style|=WS_CAPTION;
        style&=~WS_POPUP;
        SetWindowLong(GetHWND(), GWL_STYLE, style);
        SetWindowPos(&CWnd::wndNoTopMost,0,0,m_rWin.right-m_rWin.left+m_borderWidth,m_rWin.bottom-m_rWin.top+m_borderHeight,SWP_FRAMECHANGED|SWP_NOMOVE);
    }
}

//
// Event handlers
//
void SwiffOutWnd::OnPaint() {
    PAINTSTRUCT ps={0};
    CDC *pDC=BeginPaint(&ps);

    CRect r;
    GetClientRect(&r);
    pDC->FillSolidRect(&r,0x000000);
    
    OleDraw(pVO, DVASPECT_CONTENT, pDC->GetSafeHdc(), &m_rSwf);
    EndPaint(&ps);
}

void SwiffOutWnd::OnDestroy() {
    if(m_fullscreen)
        ChangeDisplaySettingsEx(0, &defaultDM, 0, CDS_FULLSCREEN, 0);

    CWnd::OnDestroy();
    delete clss;
}

void SwiffOutWnd::OnToggleFullscreen()
{
    SetFullscreen(!m_fullscreen);
}
void SwiffOutWnd::OnEnableEsc()
{
    m.CheckMenuItem(ID_ENABLE_ESC, m.GetMenuState(ID_ENABLE_ESC, MF_BYCOMMAND)==MF_CHECKED ? MF_UNCHECKED : MF_CHECKED);
    m_regkey.SetDWORDValue(L"enableEscKey", m.GetMenuState(ID_ENABLE_ESC, MF_BYCOMMAND)==MF_CHECKED);
}
void SwiffOutWnd::OnStartFullscreen()
{
    m.CheckMenuItem(ID_START_FULLSCREEN, m.GetMenuState(ID_START_FULLSCREEN, MF_BYCOMMAND)==MF_CHECKED ? MF_UNCHECKED : MF_CHECKED);
    m_regkey.SetDWORDValue(L"startFullscreen", m.GetMenuState(ID_START_FULLSCREEN, MF_BYCOMMAND)==MF_CHECKED);
}

void SwiffOutWnd::OnCreateDesktopShortcut()
{
    HRESULT hr;
    TCHAR desktopPath[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);

    CString filename;
    GetModuleFileName(0, filename.GetBufferSetLength(4096), 4096); filename.ReleaseBuffer();

    CComPtr<IShellLink> pLink;
    pLink.CoCreateInstance(CLSID_ShellLink);
    hr=pLink->SetPath(filename);
    hr=pLink->SetArguments(m_cmdLine);
    hr=pLink->SetDescription(L"SwiffOut Game Shortcut");
    
    CComQIPtr<IPersistFile> pPF(pLink);
    CFileDialog d(FALSE,L"lnk", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.lnk");
    d.DoModal();

    hr=pPF->Save(d.GetPathName(), TRUE);
}
void SwiffOutWnd::OnExit()
{
    Exit();
}
void SwiffOutWnd::OnClosestResolution()
{
    //ChangeDisplaySettingsEx(0, &selDM, 0, CDS_FULLSCREEN, 0);
}
void SwiffOutWnd::OnSelectResolution(UINT cmd)
{
    /*resMenu.GetMenuString(cmd, str, MF_BYCOMMAND);
    DWORD width=0, height=0;
    _snwscanf(str, 50, "%dx%d", &width, &height);

    DEVMODE selDM=dm;
    selDM.dmPelsWidth=width;
    selDM.dmPelsHeight=height;    
    ChangeDisplaySettingsEx(0, &selDM, 0, CDS_FULLSCREEN, 0);*/
}

//
// COM Interfaces
//

HWND SwiffOutWnd::GetHWND() {
    return GetSafeHwnd();
}

HRESULT __stdcall SwiffOutWnd::QueryInterface(REFIID riid, void ** ppvObject) {
    if (IsEqualGUID(riid, IID_IUnknown))
	    *ppvObject = (void*)(this);
    else if (IsEqualGUID(riid, IID_IOleInPlaceSite))
	    *ppvObject = (void*)dynamic_cast<IOleInPlaceSite *>(this);
    else if (IsEqualGUID(riid, IID_IOleInPlaceSiteEx))
	    *ppvObject = (void*)dynamic_cast<IOleInPlaceSiteEx *>(this);
    else if (IsEqualGUID(riid, IID_IOleInPlaceSiteWindowless))
	    *ppvObject = (void*)dynamic_cast<IOleInPlaceSiteWindowless *>(this);
    else if (IsEqualGUID(riid, IID_IOleInPlaceFrame))
	    *ppvObject = (void*)dynamic_cast<IOleInPlaceFrame *>(this);
    else if (IsEqualGUID(riid, IID_IStorage))
	    *ppvObject = (void*)dynamic_cast<IStorage *>(this);
    else if (IsEqualGUID(riid, ShockwaveFlashObjects::IID_IServiceProvider))
	    *ppvObject = (void*)dynamic_cast<ShockwaveFlashObjects::IServiceProvider *>(this);      
    else
    {
	    *ppvObject = 0;
	    return E_NOINTERFACE;
    }
    if (!(*ppvObject))
	    return E_NOINTERFACE; //if dynamic_cast returned 0
    //m_iRef++;
    return S_OK;
}
ULONG __stdcall SwiffOutWnd::AddRef() {
    return 1;
}
ULONG __stdcall SwiffOutWnd::Release() {
    return 1;
}

//IOleClientSite
HRESULT __stdcall SwiffOutWnd::SaveObject() { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::GetContainer(LPOLECONTAINER FAR* ppContainer)
{
    *ppContainer = 0;
    return E_NOINTERFACE;
}
HRESULT __stdcall SwiffOutWnd::ShowObject() 
{
    return S_OK;
}
HRESULT __stdcall SwiffOutWnd::OnShowWindow(BOOL fShow) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::RequestNewObjectLayout() { return E_NOTIMPL; }

//IOleInPlaceSite
HRESULT __stdcall SwiffOutWnd::GetWindow(HWND FAR* lphwnd) {
    *lphwnd = GetHWND();
    return S_OK;
}
HRESULT __stdcall SwiffOutWnd::ContextSensitiveHelp(BOOL fEnterMode) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::CanInPlaceActivate() { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::OnInPlaceActivate() { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::OnUIActivate() { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::GetWindowContext(LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    *lplpFrame = (LPOLEINPLACEFRAME)this;

    *lplpDoc = 0;

    lpFrameInfo->fMDIApp = FALSE;
    lpFrameInfo->hwndFrame = GetHWND();
    lpFrameInfo->haccel = 0;
    lpFrameInfo->cAccelEntries = 0;    	

    *lprcPosRect = m_rSwf;
    *lprcClipRect = m_rSwf;
    return(S_OK);
}
HRESULT __stdcall SwiffOutWnd::Scroll(SIZE scrollExtent) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::OnUIDeactivate(BOOL fUndoable) { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::OnInPlaceDeactivate() { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::DiscardUndoState() { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::DeactivateAndUndo() { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::OnPosRectChange(LPCRECT lprcPosRect) { return(S_OK); }


//IOleInPlaceSiteEx
HRESULT __stdcall SwiffOutWnd::OnInPlaceActivateEx(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags)
{
    if (pfNoRedraw)
	    *pfNoRedraw = FALSE;
    return S_OK;
}
HRESULT __stdcall SwiffOutWnd::OnInPlaceDeactivateEx(BOOL fNoRedraw) { return S_FALSE; }
HRESULT __stdcall SwiffOutWnd::RequestUIActivate(void) { return S_FALSE; }


//IOleInPlaceSiteWindowless
HRESULT __stdcall SwiffOutWnd::CanWindowlessActivate( void) { return S_OK; }
HRESULT __stdcall SwiffOutWnd::GetCapture( void) { return S_FALSE; } 
HRESULT __stdcall SwiffOutWnd::SetCapture(  BOOL fCapture) { return S_FALSE; }	 
HRESULT __stdcall SwiffOutWnd::GetFocus( void) { return S_OK; } 
HRESULT __stdcall SwiffOutWnd::SetFocus(  BOOL fFocus) { return S_OK; }	  
HRESULT __stdcall SwiffOutWnd::GetDC(  LPCRECT pRect,  DWORD grfFlags,  HDC __RPC_FAR *phDC) { 
    return S_FALSE;
}
HRESULT __stdcall SwiffOutWnd::ReleaseDC(  HDC hDC) { return S_FALSE; } 
HRESULT __stdcall SwiffOutWnd::InvalidateRect( 
     LPCRECT pRect,
     BOOL fErase)
{
    Draw(NULL, pRect, fErase);
    return S_OK;
} 
HRESULT __stdcall SwiffOutWnd::InvalidateRgn(  HRGN hRGN,  BOOL fErase) { return S_OK; } 
HRESULT __stdcall SwiffOutWnd::ScrollRect(  INT dx,  INT dy,  LPCRECT pRectScroll,  LPCRECT pRectClip) { return E_NOTIMPL; } 
HRESULT __stdcall SwiffOutWnd::AdjustRect(  LPRECT prc) { return S_FALSE; }
HRESULT __stdcall SwiffOutWnd::OnDefWindowMessage(  UINT msg,  WPARAM wParam,  LPARAM lParam,  LRESULT __RPC_FAR *plResult) { return S_FALSE; } 

void SwiffOutWnd::Draw(HDC hdcDraw, const RECT *rcDraw, BOOL bErase) {
    RECT rWin;
    ::GetClientRect(GetHWND(), &rWin);

    hdcDraw = ::GetDC(GetHWND());        
    hr = OleDraw(pVO, DVASPECT_CONTENT, hdcDraw, &m_rSwf);
    ::ReleaseDC(GetHWND(), hdcDraw);
}

//IStorage
HRESULT __stdcall SwiffOutWnd::CreateStream(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::OpenStream(const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::CreateStorage(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::OpenStorage(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::CopyTo(DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::MoveElementTo(const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::Commit(DWORD grfCommitFlags) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::Revert() { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::EnumElements(DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::DestroyElement(const OLECHAR *pwcsName) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::RenameElement(const WCHAR *pwcsOldName, const WCHAR *pwcsNewName) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::SetElementTimes(const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::SetClass(REFCLSID clsid) { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::SetStateBits(DWORD grfStateBits, DWORD grfMask) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::Stat(STATSTG * pstatstg, DWORD grfStatFlag) { return E_NOTIMPL; }

//IOleInPlaceFrame
HRESULT __stdcall SwiffOutWnd::GetBorder(LPRECT lprectBorder) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::SetBorderSpace(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName) { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject) { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::RemoveMenus(HMENU hmenuShared) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::SetStatusText(LPCOLESTR pszStatusText) { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::EnableModeless(BOOL fEnable) { return(S_OK); }
HRESULT __stdcall SwiffOutWnd::TranslateAccelerator(LPMSG lpmsg, WORD wID) { return E_NOTIMPL; }

//IDispatch
HRESULT __stdcall SwiffOutWnd::GetTypeInfoCount(UINT  *pctinfo) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo  * *ppTInfo) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::GetIDsOfNames(REFIID riid, LPOLESTR  *rgszNames, UINT cNames, LCID lcid, DISPID  *rgDispId) { return E_NOTIMPL; }
HRESULT __stdcall SwiffOutWnd::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS  *pDispParams, VARIANT  *pVarResult, EXCEPINFO  *pExcepInfo, UINT  *puArgErr) { return E_NOTIMPL; }

BEGIN_MESSAGE_MAP(SwiffOutWnd, CWnd)
    ON_WM_KEYDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_COMMAND(ID_TOGGLE_FULLSCREEN, OnToggleFullscreen)
    ON_COMMAND(ID_ENABLE_ESC, OnEnableEsc)
    ON_COMMAND(ID_START_FULLSCREEN, OnStartFullscreen)
    ON_COMMAND(ID_CREATE_DESKTOP_SHORTCUT, OnCreateDesktopShortcut)
    ON_COMMAND(ID_EXIT, OnExit)
    ON_COMMAND(ID_CLOSEST_RESOLUTION, OnClosestResolution)
    ON_COMMAND_RANGE(ID_FIRST_RESOLUTION, ID_LAST_RESOLUTION, OnSelectResolution)
END_MESSAGE_MAP()