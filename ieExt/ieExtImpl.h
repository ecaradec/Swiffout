// ieExtImpl.h : Declaration of the CieExtImpl

#pragma once
#include "resource.h"       // main symbols

#include "ieExt.h"
#include <shlguid.h>     // IID_IWebBrowser2, DIID_DWebBrowserEvents2, etc.
#include <Mshtml.h>
#include <Shobjidl.h>
#include <vector>
#include <atlstr.h>
#include <algorithm>
#include <exdispid.h>
#include "..\versionNb.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

struct CWebBrowser2EventsBase
{
 static ATL::_ATL_FUNC_INFO StatusTextChangeStruct;
 static ATL::_ATL_FUNC_INFO TitleChangeStruct;
 static ATL::_ATL_FUNC_INFO PropertyChangeStruct;
 static ATL::_ATL_FUNC_INFO OnQuitStruct;
 static ATL::_ATL_FUNC_INFO OnToolBarStruct;
 static ATL::_ATL_FUNC_INFO OnMenuBarStruct;
 static ATL::_ATL_FUNC_INFO OnStatusBarStruct;
 static ATL::_ATL_FUNC_INFO OnFullScreenStruct;
 static ATL::_ATL_FUNC_INFO OnTheaterModeStruct;
 static ATL::_ATL_FUNC_INFO DownloadBeginStruct;
 static ATL::_ATL_FUNC_INFO DownloadCompleteStruct;
 static ATL::_ATL_FUNC_INFO NewWindow2Struct; 
 static ATL::_ATL_FUNC_INFO CommandStateChangeStruct;
 static ATL::_ATL_FUNC_INFO BeforeNavigate2Struct;
 static ATL::_ATL_FUNC_INFO ProgressChangeStruct;
 static ATL::_ATL_FUNC_INFO NavigateComplete2Struct;
 static ATL::_ATL_FUNC_INFO DocumentComplete2Struct;
 static ATL::_ATL_FUNC_INFO OnVisibleStruct;
 static ATL::_ATL_FUNC_INFO SetSecureLockIconStruct;
 static ATL::_ATL_FUNC_INFO NavigateErrorStruct;
 static ATL::_ATL_FUNC_INFO PrivacyImpactedStateChangeStruct;
};

struct FullscreenHost {
    virtual ~FullscreenHost() = 0 {};
    virtual void GoFullscreen(IDispatch *pElt, IHTMLDocument2 *pDoc2)=0;
};

struct DispatchFunction : CComObjectRootEx<CComSingleThreadModel>,
                          IDispatchImpl<ICallbackFunction, &IID_ICallbackFunction, &LIBID_ieExtLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
    BEGIN_COM_MAP(DispatchFunction)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()
    void FinalRelease() {
    }
    HRESULT STDMETHODCALLTYPE onInvoke(VARIANT e) {        
        CComPtr<IHTMLWindow2> pW2;
        m_pDoc2->get_parentWindow(&pW2);
        if(!pW2)
            return S_FALSE;

        CComPtr<IHTMLEventObj> pE;
        pW2->get_event(&pE);
        if(!pE)
            return S_FALSE;
        
        CComPtr<IHTMLElement> pElt;
        pE->get_srcElement(&pElt);
        if(!pElt)
            return S_FALSE;

        CComVariant eventType;
        CComQIPtr<IDispatch>(pE).GetPropertyByName(L"eventType", &eventType);

        if(CString(eventType)!=CString("swiffout.fullscreen"))
            return S_FALSE;
        
        m_pHost->GoFullscreen(pElt,m_pDoc2);
        return S_OK;
    }

    FullscreenHost          *m_pHost;
    CComPtr<IHTMLDocument2>  m_pDoc2;
};

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

CString GetModuleDirectory()
{
    CString filename;
    GetModuleFileName(HINST_THISCOMPONENT, filename.GetBufferSetLength(MAX_PATH), MAX_PATH); filename.ReleaseBuffer();
    int lastSlash=filename.ReverseFind('\\');
    return filename.Left(lastSlash);
}

// HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\Main\TabProcGrowth=0 is required to debug IE since IE8 spawn processes to control access rights
// and secure the tabs and addons

// CieExtImpl
class ATL_NO_VTABLE CieExtImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CieExtImpl, &CLSID_ieExtImpl>,
	public IObjectWithSiteImpl<CieExtImpl>,
	public IDispatchImpl<IieExtImpl, &IID_IieExtImpl, &LIBID_ieExtLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IOleCommandTarget,
    public IDispEventImpl<1, CieExtImpl, &__uuidof(DWebBrowserEvents2)>,
    public FullscreenHost
{
public:
	CieExtImpl()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_IEEXTIMPL)

BEGIN_COM_MAP(CieExtImpl)
	COM_INTERFACE_ENTRY(IieExtImpl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
    COM_INTERFACE_ENTRY(IOleCommandTarget)
END_COM_MAP()

BEGIN_SINK_MAP(CieExtImpl)
    SINK_ENTRY_INFO(1, __uuidof(DWebBrowserEvents2), DISPID_DOCUMENTCOMPLETE, OnDocumentComplete, &CWebBrowser2EventsBase::DocumentComplete2Struct)
END_SINK_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{        
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pSite)
    {
        if(pSite){
            DispEventAdvise(pSite, &__uuidof(DWebBrowserEvents2));
        }/* else {
            DispEventUnadvise(pSite, &__uuidof(DWebBrowserEvents2));
        }*/ //???
        
        IObjectWithSiteImpl<CieExtImpl>::SetSite(pSite);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
    {
        return S_OK;
    }

    /*void __stdcall OnCommandStateChange(long lCommand, VARIANT_BOOL bEnable);
    void __stdcall OnBeforeNavigate(IDispatch *pDisp,VARIANT *URL, VARIANT *Flags,VARIANT *TargetFrameName,VARIANT *PostData,VARIANT *Headers, VARIANT_BOOL *Cancel);    
	void __stdcall OnNewWindow3(IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext, BSTR bstrUrl);
    void __stdcall OnDownloadBegin();
    void __stdcall OnNavigateComplete(IDispatch* pDisp, VARIANT* URL);
    void __stdcall OnNavigateProgress(long Progress,long ProgressMax);
    void __stdcall OnNavigateError(IDispatch *pDisp,VARIANT *URL,VARIANT *TargetFrameName, VARIANT *StatusCode,VARIANT_BOOL *Cancel);
	void __stdcall OnTitleChange(BSTR Text);
	void __stdcall OnStatusTextChange(BSTR Text);*/
    void __stdcall OnDocumentComplete(IDispatch* pDisp, VARIANT* URL)
    {
        CComQIPtr<IWebBrowser2> pWB2(pDisp);        
        CComPtr<IDispatch> pDispDoc;
        pWB2->get_Document(&pDispDoc);
        CComQIPtr<IHTMLDocument2> pDOM2(pDispDoc);

        CComPtr<IHTMLElement> pEDivSWO;
        pDOM2->createElement(CComBSTR("div"), &pEDivSWO);
        pEDivSWO->setAttribute(CComBSTR("id"), CComVariant("swiffout"));
        pEDivSWO->setAttribute(CComBSTR("version"), CComVariant(_PRODUCTVERSIONSTR_));

        CComPtr<IHTMLElement> pBody;        
        pDOM2->get_body(&pBody);

        if(pBody) {
            CComQIPtr<IHTMLDOMNode> pNode(pBody);

            CComQIPtr<IHTMLDOMNode> pDivNode(pEDivSWO);

            CComObject<DispatchFunction> *pDispFunc;
            CComObject<DispatchFunction>::CreateInstance(&pDispFunc);
            pDispFunc->m_pDoc2=pDOM2;
            pDispFunc->m_pHost=this;            
            pNode->appendChild(pDivNode,0);

            VARIANT_BOOL b;
            CComQIPtr<IHTMLElement2> pBody2(pBody);
            pBody2->attachEvent(CComBSTR("onclick"), pDispFunc, &b);
        }
    }    

    struct Embed {            
        int                width;
        int                height;
        CComVariant        src;
        CComVariant        flashVars;

        static bool isbiggest(const Embed& e1, const Embed& e2)
        {
            return e1.width*e1.height > e2.width*e2.height;
        }
    };

    void GoFullscreen(IDispatch *pElt, IHTMLDocument2 *pDoc2)
    {
        CComPtr<IHTMLLocation> pLoc;
        pDoc2->get_location(&pLoc);
        CComBSTR hostnameBSTR, protocolBSTR, hrefBSTR;
        pLoc->get_hostname(&hostnameBSTR);
        pLoc->get_protocol(&protocolBSTR);
        pLoc->get_href(&hrefBSTR);

        CString hostname(hostnameBSTR), protocol(protocolBSTR), href(hrefBSTR);
        if(CString(hostname).Left(17)!=L"swiffoutgames.com")
            return;

        Embed e;
        GetEmbedData(e,CComPtr<IDispatch>(pElt),hostname,protocol,href);
        LaunchFullscreen(e);
    }

    void GetEmbedData(Embed &e, CComPtr<IDispatch> &pDispE, CString &hostname, CString &protocol, CString &href)
    {
        CComQIPtr<IHTMLElement> pElt(pDispE);

        HRESULT hr=pDispE.GetPropertyByName(CComBSTR("data"), &e.src);
        if(hr!=S_OK || e.src.bstrVal==0) {
            hr=pDispE.GetPropertyByName(CComBSTR("src"), &e.src);
            if(hr!=S_OK || e.src.bstrVal==0) {
                hr=pDispE.GetPropertyByName(CComBSTR("Movie"), &e.src);
            }
        }

        CString swf(e.src);
        if(swf.Left(1)=="/") {
            swf=protocol+L"//"+hostname+swf;
        } else if(swf.Left(7)=="http://" || swf.Left(8)=="https://")
            ;
        else
            swf=href.Left(href.ReverseFind(L'/')+1)+swf;

        e.src=swf;

        CComVariant widthBSTR, heightBSTR;
        pDispE.GetPropertyByName(CComBSTR("width"), &widthBSTR)==S_OK ? e.width=_wtoi(CStringW(widthBSTR)) : 800;
        pDispE.GetPropertyByName(CComBSTR("height"), &heightBSTR)==S_OK ? e.height=_wtoi(CStringW(heightBSTR)) : 600;

        CComQIPtr<IHTMLElement2> pElt3(pDispE);
        CComPtr<IHTMLElementCollection> params;
        pElt3->getElementsByTagName(CComBSTR("param"), &params);
        long nbParams=0; params->get_length(&nbParams);
        
        for(int p=0;p<nbParams;p++) {
            CComPtr<IDispatch> pParamDisp;
            params->item(CComVariant(p), CComVariant(p), &pParamDisp);
            CComQIPtr<IHTMLElement> pParamElt(pParamDisp);
            CComVariant name, value;
            pParamElt->getAttribute(CComBSTR("name"), 2, &name);

            if(CString(name).MakeUpper()==_T("FLASHVARS")) {
                pParamElt->getAttribute(CComBSTR("value"), 2, &e.flashVars);
                break;
            }
        }

        if(e.flashVars.vt!=VT_BSTR) {
            e.flashVars=CComBSTR("");
        }
    }

    void LaunchFullscreen(Embed &e)
    {
        CString cmd; cmd.Format(L"swiffout:swiffout_href=%s,swiffout_width=%d,swiffout_height=%d,swiffout_flashvars=%s", CString(e.src), e.width, e.height, CString(e.flashVars));
        //OutputDebugString(cmd+L"\n");
        ShellExecute(0, 0, GetModuleDirectory()+L"\\swiffoutrunner.exe", cmd, 0, SW_SHOWNORMAL);
    }

    void CollectEmbeds(const TCHAR *tag, IDispatch *pDispDoc, std::vector<Embed> *embeds)
    {
        int nbObjects=0;

        CComQIPtr<IHTMLDocument3> pDoc3(pDispDoc);
        CComPtr<IHTMLElementCollection> elements;
        HRESULT hr=pDoc3->getElementsByTagName(CComBSTR(tag), &elements);
        LONG nEmbeds=0;
        hr=elements->get_length(&nEmbeds);
        nbObjects+=nEmbeds;


        CComQIPtr<IHTMLDocument2> pDoc2(pDispDoc);
        CComPtr<IHTMLLocation> pLoc;
        pDoc2->get_location(&pLoc);
        CComBSTR hostnameBSTR, protocolBSTR, hrefBSTR;
        pLoc->get_hostname(&hostnameBSTR);
        pLoc->get_protocol(&protocolBSTR);
        pLoc->get_href(&hrefBSTR);

        CString hostname(hostnameBSTR), protocol(protocolBSTR), href(hrefBSTR);

        for(int i=0;i<nEmbeds;i++) {
            Embed e;
            CComPtr<IDispatch> pDispE;
            elements->item(CComVariant(i), CComVariant(i), &pDispE);
            GetEmbedData(e, pDispE, hostname, protocol, href);
            embeds->push_back(e);
        }
    }
    void CollectEmbeds(IDispatch *pDispDoc, std::vector<Embed> *embeds)
    {
        CollectEmbeds(L"object", pDispDoc, embeds);
        CollectEmbeds(L"embed", pDispDoc, embeds);
    }

    void EnumFrames(IDispatch *pDispDoc, std::vector<Embed> *embeds)
    {
        HRESULT hr;
        //
        // read embeds
        //
        CollectEmbeds(pDispDoc, embeds);

        //
        // search subframes
        //
        CComQIPtr<IOleContainer> pContainer(pDispDoc);
        if(!pContainer)
            return;

        CComPtr<IEnumUnknown> pEnumerator;

        // Get an enumerator for the frames
        hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
        if(!pEnumerator)
            return;

        // Enumerate and refresh all the frames
        while(1)
        {
            CComPtr<IUnknown> pUnk;
            ULONG uFetched;

            if(pEnumerator->Next(1, &pUnk, &uFetched)!=S_OK)
                break;

            //CComQIPtr<IHTMLElement> pElt(pUnk);
            //CComVariant name;
            //pElt->getAttribute(CComBSTR("id"), 2, &name);

            CComQIPtr<IWebBrowser2> pBrowser(pUnk);
            if(!pBrowser)
                continue;

            CComPtr<IDispatch> pDispDocFrame;

            pBrowser->get_Document(&pDispDocFrame); 
            if(!pDispDocFrame)
                continue;

            EnumFrames(pDispDocFrame, embeds);
        }
    }

    HRESULT STDMETHODCALLTYPE Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
    {
        HRESULT hr;

        std::vector<Embed> embeds;

        CComPtr<IWebBrowser2> pWB2;
        if (m_spUnkSite== NULL)
            return E_FAIL;
        
        CComPtr<IServiceProvider> spIServiceProvider;
        hr = m_spUnkSite->QueryInterface(IID_IServiceProvider, (void **)&spIServiceProvider); 
        if(!spIServiceProvider)
            return E_FAIL;

        spIServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void **) &pWB2);
        if(!pWB2)
            return E_FAIL;

        CComPtr<IDispatch> pDispDoc;
        pWB2->get_Document(&pDispDoc);
        EnumFrames(pDispDoc, &embeds);

        std::sort(embeds.begin(), embeds.end(), Embed::isbiggest);

        if(embeds.size()>0) {
            LaunchFullscreen(embeds[0]);            
            pWB2->Navigate(CComBSTR(L"http://swiffout.com/cpu-preservation.html"), 0, 0, 0, 0);
        }

        return S_OK;
    }    
};

OBJECT_ENTRY_AUTO(__uuidof(ieExtImpl), CieExtImpl)