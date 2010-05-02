

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

    void Create(CHAR *swf, CHAR *flashVars, RECT &rWin, RECT &rSwf) {
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