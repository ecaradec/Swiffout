//include direct draw header (do not link ddraw.lib)
#include <ddraw.h>
#import "c:\\windows\\system32\\macromed\\flash\\flash.ocx" named_guids no_auto_exclude

struct FlashWndSubclass;

struct SwiffOutWnd : CWnd,
                  IOleClientSite,
                  IOleInPlaceSiteWindowless,
                  IOleInPlaceFrame,
                  IStorage,
                  ShockwaveFlashObjects::IServiceProvider,
                  ShockwaveFlashObjects::_IShockwaveFlashEvents
{
    CComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> pSF;
    CComPtr<IOleObject>                     pOO;
    CComQIPtr<IViewObjectEx>                pVOE;
    CComQIPtr<IViewObject>                  pVO;
    CComQIPtr<IOleInPlaceObjectWindowless>  pOIPOW;
    CComQIPtr<IOleInPlaceObject>            pOIPO;
    CRect                                   m_rSwf;
    CRect                                   m_rWin;
    HRESULT                                 hr;
    void                                   *m_lpBitsOnly;
    HDC                                     m_hdcBack;
    HBITMAP                                 m_bmpBack;
    CComPtr<IDirectDraw4>                   m_lpDD4;
    FlashWndSubclass                       *clss;

    DEVMODE                                 dm;
    DEVMODE                                 defaultDM;
    bool                                    m_fullscreen;
    CMenu                                   m;
    CMenu                                   resMenu;
    CRegKey                                 m_regkey;
    CString                                 m_cmdLine;
    
    int                                     m_borderWidth;
    int                                     m_borderHeight;

    int                                     m_width;
    int                                     m_height;

    DECLARE_MESSAGE_MAP()
public:
    SwiffOutWnd();
    ~SwiffOutWnd();
    

    STDMETHOD(OnReadyStateChange)(long newState)
    {
        return S_OK;
    }
    STDMETHOD(OnProgress)(long percentDone)
    {
        return S_OK;
    }
    STDMETHOD(FSCommand)(_bstr_t command, _bstr_t args )
    {
        return S_OK;
    }

    //the implementation of IServiceProvider interface
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE raw_RemoteQueryService( 
            /* [in] */ GUID *guidService,
            /* [in] */ GUID *riid,
            /* [out] */ IUnknown **ppvObject);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    /*static LRESULT __stdcall FlashWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);*/

    void Create(CHAR *swf, CHAR *flashVars, int width, int height);
    void SetFullscreen(bool b);
    void Exit();

    //
    // Event handlers
    //
    void OnPaint();
    void OnDestroy();
    void OnRButtonUp(UINT nFlags, CPoint point);
    void OnToggleFullscreen();
    void OnEnableEsc();
    void OnStartFullscreen();
    void OnCreateDesktopShortcut();
    void OnExit();
    void OnClosestResolution();
    void OnSelectResolution(UINT cmd);

    //
    // COM Interfaces
    //

    HWND GetHWND();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    //IOleClientSite
    STDMETHOD(SaveObject)();
    STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk);
    STDMETHOD(GetContainer)(LPOLECONTAINER FAR* ppContainer);
    STDMETHOD(ShowObject)();
    STDMETHOD(OnShowWindow)(BOOL fShow);
    STDMETHOD(RequestNewObjectLayout)();

    //IOleInPlaceSite
    STDMETHOD(GetWindow)(HWND FAR* lphwnd);
    STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);
    STDMETHOD(CanInPlaceActivate)();
    STDMETHOD(OnInPlaceActivate)();
    STDMETHOD(OnUIActivate)();
    STDMETHOD(GetWindowContext)(LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
    STDMETHOD(Scroll)(SIZE scrollExtent);
    STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
    STDMETHOD(OnInPlaceDeactivate)();
    STDMETHOD(DiscardUndoState)();
    STDMETHOD(DeactivateAndUndo)();
    STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);


    //IOleInPlaceSiteEx
    STDMETHOD(OnInPlaceActivateEx)(BOOL __RPC_FAR *pfNoRedraw, DWORD dwFlags);
    STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);
    STDMETHOD(RequestUIActivate)(void);


    //IOleInPlaceSiteWindowless
    STDMETHOD(CanWindowlessActivate)();
    STDMETHOD(GetCapture)( void);
    STDMETHOD(SetCapture)(  BOOL fCapture); 
    STDMETHOD(GetFocus)( void);
    STDMETHOD(SetFocus)(  BOOL fFocus);	  
    STDMETHOD(GetDC)(  LPCRECT pRect,  DWORD grfFlags,  HDC __RPC_FAR *phDC);
    STDMETHOD(ReleaseDC)(  HDC hDC);
    STDMETHOD(InvalidateRect)( 
         LPCRECT pRect,
         BOOL fErase);
    STDMETHOD(InvalidateRgn)(  HRGN hRGN,  BOOL fErase) ;
    STDMETHOD(ScrollRect)(  INT dx,  INT dy,  LPCRECT pRectScroll,  LPCRECT pRectClip);
    STDMETHOD(AdjustRect)(  LPRECT prc);
    STDMETHOD(OnDefWindowMessage)(  UINT msg,  WPARAM wParam,  LPARAM lParam,  LRESULT __RPC_FAR *plResult);

    void Draw(HDC hdcDraw, const RECT *rcDraw, BOOL bErase);

    //IStorage
    STDMETHOD(CreateStream)(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm);
    STDMETHOD(OpenStream)(const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm);
    STDMETHOD(CreateStorage)(const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg);
    STDMETHOD(OpenStorage)(const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg);
    STDMETHOD(CopyTo)(DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude,IStorage *pstgDest);
    STDMETHOD(MoveElementTo)(const OLECHAR *pwcsName,IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags);
    STDMETHOD(Commit)(DWORD grfCommitFlags);
    STDMETHOD(Revert)();
    STDMETHOD(EnumElements)(DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum);
    STDMETHOD(DestroyElement)(const OLECHAR *pwcsName);
    STDMETHOD(RenameElement)(const WCHAR *pwcsOldName, const WCHAR *pwcsNewName);
    STDMETHOD(SetElementTimes)(const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime);
    STDMETHOD(SetClass)(REFCLSID clsid);
    STDMETHOD(SetStateBits)(DWORD grfStateBits, DWORD grfMask);
    STDMETHOD(Stat)(STATSTG * pstatstg, DWORD grfStatFlag);

    //IOleInPlaceFrame
    STDMETHOD(GetBorder)(LPRECT lprectBorder);
    STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS pborderwidths);
    STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS pborderwidths);
    STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName);
    STDMETHOD(InsertMenus)(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
    STDMETHOD(SetMenu)(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
    STDMETHOD(RemoveMenus)(HMENU hmenuShared);
    STDMETHOD(SetStatusText)(LPCOLESTR pszStatusText);
    STDMETHOD(EnableModeless)(BOOL fEnable);
    STDMETHOD(TranslateAccelerator)(LPMSG lpmsg, WORD wID);

    //IDispatch
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT  *pctinfo);
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo  * *ppTInfo);
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR  *rgszNames, UINT cNames, LCID lcid, DISPID  *rgDispId);
    virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS  *pDispParams, VARIANT  *pVarResult, EXCEPINFO  *pExcepInfo, UINT  *puArgErr);
};