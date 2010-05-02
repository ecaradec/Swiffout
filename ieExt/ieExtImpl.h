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

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// HKEY_CURRENT_USER\Software\Microsoft\Internet Explorer\Main\TabProcGrowth=0 is required to debug IE since IE8 spawn processes to control access rights
// and secure the tabs and addons

// CieExtImpl
class ATL_NO_VTABLE CieExtImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CieExtImpl, &CLSID_ieExtImpl>,
	public IObjectWithSiteImpl<CieExtImpl>,
	public IDispatchImpl<IieExtImpl, &IID_IieExtImpl, &LIBID_ieExtLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IOleCommandTarget
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

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{        
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
    HRESULT STDMETHODCALLTYPE QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
    {
        return S_OK;
    }

    struct Embed {            
        int                width;
        int                height;
        CComVariant         src;
        CComVariant         flashVars;

        static bool isbiggest(const Embed& e1, const Embed& e2)
        {
            return e1.width*e1.height > e2.width*e2.height;
        }
    };

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

            CComQIPtr<IHTMLElement> pElt(pDispE);

            if(pDispE.GetPropertyByName(CComBSTR("Data"), &e.src)==S_OK ||
               pDispE.GetPropertyByName(CComBSTR("src"), &e.src)==S_OK || 
               pDispE.GetPropertyByName(CComBSTR("Movie"), &e.src)==S_OK) {
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

            embeds->push_back(e);
        }
    }
    void CollectEmbeds(IDispatch *pDispDoc, std::vector<Embed> *embeds)
    {
        CollectEmbeds(L"object", pDispDoc, embeds);
        CollectEmbeds(L"embed", pDispDoc, embeds);
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

        CollectEmbeds(pDispDoc, &embeds);

        CComQIPtr<IOleContainer> pContainer(pDispDoc);

        CComPtr<IEnumUnknown> pEnumerator;

        // Get an enumerator for the frames
        hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);

        // Enumerate and refresh all the frames
        while(1)
        {
            CComPtr<IUnknown> pUnk;
            ULONG uFetched;

            if(pEnumerator->Next(1, &pUnk, &uFetched)!=S_OK)
                break;

            CComQIPtr<IWebBrowser2> pBrowser(pUnk);
            if(!pBrowser)
                continue;

            CComPtr<IDispatch> pDispDocFrame;

            pBrowser->get_Document(&pDispDocFrame); 
            if(!pDispDocFrame)
                continue;

            CollectEmbeds(pDispDoc, &embeds);
        }

        std::sort(embeds.begin(), embeds.end(), Embed::isbiggest);

        if(embeds.size()>0) {
            CString cmd; cmd.Format(L"swiffout:swiffout_href=%s,swiffout_width=%d,swiffout_height=%d,swiffout_flashvars=%s", CString(embeds[0].src), embeds[0].width, embeds[0].height, CString(embeds[0].flashVars));
            ShellExecute(0, 0, cmd, 0, 0, SW_SHOWNORMAL);
            pWB2->Navigate(CComBSTR(L"http://www.grownsoftware.com/swiffout/cpu-preservation.html"), 0, 0, 0, 0);
        }

        return S_OK;
    }    
};

OBJECT_ENTRY_AUTO(__uuidof(ieExtImpl), CieExtImpl)