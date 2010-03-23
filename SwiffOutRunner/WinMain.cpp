#define _WIN32_WINNT 0x0400
#include "resource.h"

#include <afxwin.h>
#include <Ole2.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#import "c:\\windows\\system32\\macromed\\flash\\flash.ocx" named_guids no_auto_exclude

// sample licence key
// {4BA4A9AD-5751-672B-C6B8-FC387F0E1CDC}

// TODO : directdraw is not queried
__forceinline bool CheckLicKey(char *lic, long N1, long N2)
{   
    long t;   memcpy(&t, lic, 4);
    short k1; memcpy(&k1, lic+4, 2);
    short k2; memcpy(&k2, lic+6, 2);

    long n1; memcpy(&n1, lic+8, 4);
    long n2; memcpy(&n2, lic+12, 4);

    short t1=t&0xFFFF;
    short t2=t>>16;

    n1^=(k2<<16)|k1;
    n2^=t;

    return n1==N1 && n2==N2;
}

__forceinline bool checkSwiffOutKey() {
    CRegKey r;
    r.Create(HKEY_CURRENT_USER, L"Software\\SwiffOut");  
    CLSID licence;
    r.QueryGUIDValue(L"licence",licence);
    return CheckLicKey((char*)&licence, 0x5fd7ef97, 0x97b8a7d2);
}

#define CHECKLICKEY checkSwiffOutKey()

// host should implement direct draw for better performances

    //include direct draw header (do not link ddraw.lib)
#include <ddraw.h>
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
        }
    }
};

BEGIN_MESSAGE_MAP(FlashWndSubclass,CWnd)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

struct SwiffOutWnd : CWnd,
                  IOleClientSite,
                  IOleInPlaceSiteWindowless,
                  IOleInPlaceFrame,
                  IStorage,
                  ShockwaveFlashObjects::IServiceProvider 
{
    CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> pSF;
    CComPtr<IOleObject>                     pOO;
    CComQIPtr<IViewObjectEx>                pVOE;
    CComQIPtr<IViewObject>                  pVO;
    CComQIPtr<IOleInPlaceObjectWindowless>  pOIPOW;
    CComQIPtr<IOleInPlaceObject>            pOIPO;
    RECT                                    m_rSwf;
    RECT                                    m_rWin;
    HRESULT                                 hr;
    void                                   *m_lpBitsOnly;
    HDC                                     m_hdcBack;
    HBITMAP                                 m_bmpBack;
    CComPtr<IDirectDraw4>                   m_lpDD4;
    FlashWndSubclass                       *clss;

    DECLARE_MESSAGE_MAP()
public:
    SwiffOutWnd() {
    }
    ~SwiffOutWnd() {
    }
    //the implementation of IServiceProvider interface
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE raw_RemoteQueryService( 
            /* [in] */ GUID *guidService,
            /* [in] */ GUID *riid,
            /* [out] */ IUnknown **ppvObject)
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
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
        if(nChar==VK_ESCAPE) {            
            pOO->Close(OLECLOSE_NOSAVE);
            DestroyWindow();
        }
    }
    /*static LRESULT __stdcall FlashWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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

    void Create(CHAR *swf, RECT &rWin, RECT &rSwf) {
        DWORD   readBytes=0;
        HRESULT hr;
        m_lpBitsOnly=0;
        m_hdcBack=0;
        m_bmpBack=0;

        m_rSwf=rSwf;
        m_rWin=rWin;

        CreateEx(WS_EX_TOPMOST, AfxRegisterWndClass(0), L"SwiffOutRunner", WS_VISIBLE, 0, 0, rWin.right-rWin.left, rWin.bottom-rWin.top, 0, 0);
        ModifyStyle(WS_CAPTION,WS_POPUP);
        SetWindowPos(0,0,0,m_rWin.right-m_rWin.left,m_rWin.bottom-m_rWin.top,SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOZORDER);

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
	    hr=pSF->put_Scale(L"showAll");
	    hr=pSF->put_BackgroundColor(0x00000000);

        hr=pOO->DoVerb(OLEIVERB_SHOW, NULL, (IOleClientSite *)this, 0, NULL, NULL);
        
        pOIPO->SetObjectRects(&rSwf, &rSwf);
        pOIPOW->SetObjectRects(&rSwf, &rSwf);

        HWND flashHWND;
        pOIPO->GetWindow(&flashHWND);
        clss=new FlashWndSubclass;
        clss->SubclassWindow(flashHWND);
        
        hr=pSF->LoadMovie(0, _bstr_t(swf));
        hr=pSF->Play();
    }

    void SetFullscreen(bool b) {
        if(b) {
            LONG style=GetWindowLong(GetHWND(),GWL_STYLE);
            style&=~WS_CAPTION;
            style|=WS_POPUP;
            SetWindowLong(GetHWND(), GWL_STYLE, style);
            SetWindowPos(0,0,0,m_rWin.right-m_rWin.left,m_rWin.bottom-m_rWin.top,SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOZORDER);
            ::SetFocus(GetHWND());
        } else {
            LONG style=GetWindowLong(GetHWND(),GWL_STYLE);
            style|=WS_CAPTION;
            style&=~WS_POPUP;
            SetWindowLong(GetHWND(), GWL_STYLE, style);
            SetWindowPos(0,0,0,m_rWin.right-m_rWin.left+10,m_rWin.bottom-m_rWin.top+20,SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOZORDER);
        }
    }
    
    //
    // Event handlers
    //
    void OnPaint() {
        PAINTSTRUCT ps={0};
        CDC *pDC=BeginPaint(&ps);
        pDC->FillSolidRect(&m_rWin,0);
        
        OleDraw(pVO, DVASPECT_CONTENT, pDC->GetSafeHdc(), &m_rSwf);
        EndPaint(&ps);
    }

    void OnDestroy() {
        CWnd::OnDestroy();
        delete clss;
    }

    //
    // COM Interfaces
    //

    HWND GetHWND() {
        return GetSafeHwnd();
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject) {
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
    ULONG STDMETHODCALLTYPE AddRef() {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release() {
        return 1;
    }

    //IOleClientSite
    STDMETHOD(SaveObject)() { return E_NOTIMPL; }
    STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk) { return E_NOTIMPL; }
    STDMETHOD(GetContainer)(LPOLECONTAINER FAR* ppContainer)
    {
	    *ppContainer = 0;
  	    return E_NOINTERFACE;
    }
    STDMETHOD(ShowObject)() 
    {
	    return S_OK;
    }
    STDMETHOD(OnShowWindow)(BOOL fShow) { return E_NOTIMPL; }
    STDMETHOD(RequestNewObjectLayout)() { return E_NOTIMPL; }

    //IOleInPlaceSite
    STDMETHOD(GetWindow)(HWND FAR* lphwnd) {
	    *lphwnd = GetHWND();
	    return S_OK;
    }
    STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode) { return E_NOTIMPL; }
    STDMETHOD(CanInPlaceActivate)() { return(S_OK); }
    STDMETHOD(OnInPlaceActivate)() { return(S_OK); }
    STDMETHOD(OnUIActivate)() { return(S_OK); }
    STDMETHOD(GetWindowContext)(LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
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
    STDMETHOD(Scroll)(SIZE scrollExtent) { return E_NOTIMPL; }
    STDMETHOD(OnUIDeactivate)(BOOL fUndoable) { return(S_OK); }
    STDMETHOD(OnInPlaceDeactivate)() { return(S_OK); }
    STDMETHOD(DiscardUndoState)() { return E_NOTIMPL; }
    STDMETHOD(DeactivateAndUndo)() { return E_NOTIMPL; }
    STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect) { return(S_OK); }


    //IOleInPlaceSiteEx
    STDMETHOD(OnInPlaceActivateEx)(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags)
    {
	    if (pfNoRedraw)
		    *pfNoRedraw = FALSE;
	    return S_OK;
    }
    STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw) { return S_FALSE; }
    STDMETHOD(RequestUIActivate)(void) { return S_FALSE; }


    //IOleInPlaceSiteWindowless
    STDMETHOD(CanWindowlessActivate)( void) { return S_OK; }
    STDMETHOD(GetCapture)( void) { return S_FALSE; } 
    STDMETHOD(SetCapture)(  BOOL fCapture) { return S_FALSE; }	 
    STDMETHOD(GetFocus)( void) { return S_OK; } 
    STDMETHOD(SetFocus)(  BOOL fFocus) { return S_OK; }	  
    STDMETHOD(GetDC)(  LPCRECT pRect,  DWORD grfFlags,  HDC __RPC_FAR *phDC) { 
        return S_FALSE;
    }
    STDMETHOD(ReleaseDC)(  HDC hDC) { return S_FALSE; } 
    STDMETHOD(InvalidateRect)( 
         LPCRECT pRect,
         BOOL fErase)
    {
	    Draw(NULL, pRect, fErase);
	    return S_OK;
    } 
    STDMETHOD(InvalidateRgn)(  HRGN hRGN,  BOOL fErase) { return S_OK; } 
    STDMETHOD(ScrollRect)(  INT dx,  INT dy,  LPCRECT pRectScroll,  LPCRECT pRectClip) { return E_NOTIMPL; } 
    STDMETHOD(AdjustRect)(  LPRECT prc) { return S_FALSE; }
    STDMETHOD(OnDefWindowMessage)(  UINT msg,  WPARAM wParam,  LPARAM lParam,  LRESULT __RPC_FAR *plResult) { return S_FALSE; } 

    void Draw(HDC hdcDraw, const RECT *rcDraw, BOOL bErase) {
	    RECT rWin;
        ::GetClientRect(GetHWND(), &rWin);

        hdcDraw = ::GetDC(GetHWND());        
	    hr = OleDraw(pVO, DVASPECT_CONTENT, hdcDraw, &m_rSwf);
        ::ReleaseDC(GetHWND(), hdcDraw);
    }

    //IStorage
    STDMETHOD(CreateStream)(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
    STDMETHOD(OpenStream)(const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm) { return E_NOTIMPL; }
    STDMETHOD(CreateStorage)(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg) { return E_NOTIMPL; }
    STDMETHOD(OpenStorage)(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg) { return E_NOTIMPL; }
    STDMETHOD(CopyTo)(DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest) { return E_NOTIMPL; }
    STDMETHOD(MoveElementTo)(const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags) { return E_NOTIMPL; }
    STDMETHOD(Commit)(DWORD grfCommitFlags) { return E_NOTIMPL; }
    STDMETHOD(Revert)() { return E_NOTIMPL; }
    STDMETHOD(EnumElements)(DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum) { return E_NOTIMPL; }
    STDMETHOD(DestroyElement)(const OLECHAR *pwcsName) { return E_NOTIMPL; }
    STDMETHOD(RenameElement)(const WCHAR *pwcsOldName, const WCHAR *pwcsNewName) { return E_NOTIMPL; }
    STDMETHOD(SetElementTimes)(const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime) { return E_NOTIMPL; }
    STDMETHOD(SetClass)(REFCLSID clsid) { return(S_OK); }
    STDMETHOD(SetStateBits)(DWORD grfStateBits, DWORD grfMask) { return E_NOTIMPL; }
    STDMETHOD(Stat)(STATSTG * pstatstg, DWORD grfStatFlag) { return E_NOTIMPL; }

    //IOleInPlaceFrame
    STDMETHOD(GetBorder)(LPRECT lprectBorder) { return E_NOTIMPL; }
    STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
    STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS pborderwidths) { return E_NOTIMPL; }
    STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName) { return(S_OK); }
    STDMETHOD(InsertMenus)(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) { return E_NOTIMPL; }
    STDMETHOD(SetMenu)(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject) { return(S_OK); }
    STDMETHOD(RemoveMenus)(HMENU hmenuShared) { return E_NOTIMPL; }
    STDMETHOD(SetStatusText)(LPCOLESTR pszStatusText) { return(S_OK); }
    STDMETHOD(EnableModeless)(BOOL fEnable) { return(S_OK); }
    STDMETHOD(TranslateAccelerator)(LPMSG lpmsg, WORD wID) { return E_NOTIMPL; }

    //IDispatch
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT  *pctinfo) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo  * *ppTInfo) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR  *rgszNames, UINT cNames, LCID lcid, DISPID  *rgDispId) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS  *pDispParams, VARIANT  *pVarResult, EXCEPINFO  *pExcepInfo, UINT  *puArgErr) { return E_NOTIMPL; }
};

BEGIN_MESSAGE_MAP(SwiffOutWnd, CWnd)
    ON_WM_KEYDOWN()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
END_MESSAGE_MAP()

struct CLicenceDlg : CDialog {
    enum { IDD = IDD_LICENCE_DIALOG };
    CLicenceDlg() : CDialog(IDD) {}
    BOOL OnInitDialog() {        
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

        unsigned __int64 daysLeft=(ftNow-ftInstall) / 100 / 100 / 1000 / 60 / 60 / 24;

        if(daysLeft > 15) {
            CString daysLeftStr; daysLeftStr.Format(L"%d", daysLeft);
            GetDlgItem(IDC_STATIC)->SetWindowText(L"Oh no !!! The trial period for SwiffOut has ended. "
                                                  L"As a faithful SwiffOut user, you should consider buying a licence. "
                                                  L"It'll allow you to skip this annoooying dialog and enjoy SwiffOut for as long as you want\n"
                                                  L"Now BUY a licence and enjoy a great game.");

            SetWindowPos(0,0,0,510,175,SWP_NOMOVE);
        } else {
            CString daysLeftStr; daysLeftStr.Format(L"%d", daysLeft);
            GetDlgItem(IDC_STATIC)->SetWindowText(L"You only have "+daysLeftStr+L" days left on your trial period. "
                                                  L"If you like SwiffOut, you should consider buying a licence. "
                                                  L"It'll allow you to skip this annoooying dialog and enjoy SwiffOut for as long as you want.\n\n"
                                                  L"Now click BUY or SKIP and enjoy a great game.");
        }

        return CDialog::OnInitDialog();
    }
    void DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_LIC1, m_licence);
    }
    void OnBnBuy() {
        ShellExecute(0,0,L"http://www.grownsoftware.com/swiffout", 0, 0, SW_NORMAL);
    }
    void OnBnConfirm() {
        CString lic;
        m_licence.GetWindowText(lic);

        CLSID clsidLic;
        CLSIDFromString(CComBSTR(lic),&clsidLic);
        if(CheckLicKey((char*)&clsidLic, 0x5fd7ef97, 0x97b8a7d2)) {                        
            MessageBox(L"Thank you",L"Licence is correct", MB_OK);

            CRegKey r;
            r.Create(HKEY_CURRENT_USER, L"Software\\SwiffOut");
            r.SetStringValue(L"licence", lic);

            EndDialog(0);
        } else {
            MessageBox(L"This licence is not valid. Try to copy paste it to avoid errors.",L"Ooops, invalid licence", MB_OK);
        }

        //MessageBox(lic);
        //EndDialog(0);
    }
    void OnBnSkip() {
        EndDialog(0);
    }

    DECLARE_MESSAGE_MAP()

    HBRUSH m_hRedBrush;
    CEdit  m_licence;
};

BEGIN_MESSAGE_MAP(CLicenceDlg, CDialog)
    ON_BN_CLICKED(IDC_BUY_LICENCE, &CLicenceDlg::OnBnBuy)
    ON_BN_CLICKED(IDC_CONFIRM, &CLicenceDlg::OnBnConfirm)
    ON_BN_CLICKED(IDC_SKIP, &CLicenceDlg::OnBnSkip)    
END_MESSAGE_MAP()

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
            CLicenceDlg licenceDlg;
            licenceDlg.DoModal();
            
            if(!CHECKLICKEY)
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
        dm.dmPelsWidth=400;
        dm.dmPelsHeight=300;
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