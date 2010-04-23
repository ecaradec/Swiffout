// ieExtImpl.h : Declaration of the CieExtImpl

#pragma once
#include "resource.h"       // main symbols

#include "ieExt.h"
#include <shlguid.h>     // IID_IWebBrowser2, DIID_DWebBrowserEvents2, etc.
#include <Mshtml.h>
#include <Shobjidl.h>
#include <vector>

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

    HRESULT STDMETHODCALLTYPE Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
    {
        HRESULT hr;
        struct Embed {            
            int                width;
            int                height;
            CComBSTR            src;
            CComBSTR            flashVars;
        };
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

        int nbObjects=0;

        CComQIPtr<IHTMLDocument3> pDoc3(pDispDoc);
        CComPtr<IHTMLElementCollection> elements;
        hr=pDoc3->getElementsByTagName(CComBSTR("object"), &elements);
        LONG nEmbeds=0;
        hr=elements->get_length(&nEmbeds);
        nbObjects+=nEmbeds;

        for(int i=0;i<nEmbeds;i++) {
            Embed e;
            CComPtr<IDispatch> pDispE;
            elements->item(CComVariant(i), CComVariant(i), &pDispE);
            CComVariant srcVt;
            if(pDispE.GetPropertyByName(CComBSTR("src"), &srcVt)==S_OK || 
               pDispE.GetPropertyByName(CComBSTR("Movie"), &srcVt)==S_OK || 
               pDispE.GetPropertyByName(CComBSTR("Data"), &srcVt)==S_OK) {
            }

            CComVariant flashVarVt;
            if(pDispE.GetPropertyByName(CComBSTR("flashVars"), &flashVarVt)==S_OK) {
            }
        }

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

            CComQIPtr<IHTMLDocument3> pDoc3(pDispDocFrame);
            CComPtr<IHTMLElementCollection> elements;
            hr=pDoc3->getElementsByTagName(CComBSTR("object"), &elements);
            LONG nEmbeds=0;
            hr=elements->get_length(&nEmbeds);
            nbObjects+=nEmbeds;


            for(int i=0;i<nEmbeds;i++) {
                Embed e;
                CComPtr<IDispatch> pDispE;
                elements->item(CComVariant(i), CComVariant(i), &pDispE);
                CComVariant srcVt;
                if(pDispE.GetPropertyByName(CComBSTR("src"), &srcVt)==S_OK || 
                   pDispE.GetPropertyByName(CComBSTR("Movie"), &srcVt)==S_OK || 
                   pDispE.GetPropertyByName(CComBSTR("Data"), &srcVt)==S_OK) {
                }

                CComVariant flashVarVt;
                if(pDispE.GetPropertyByName(CComBSTR("flashVars"), &flashVarVt)==S_OK) {
                }
            }     
        }

        TCHAR buff[256]={0};
        _stprintf(buff, _T("NbObjects : %d"), nbObjects);
        MessageBox(0, buff, 0, MB_OK);

        //CComQIPtr<IShellBrowser>  pShellBrowser(m_spUnkSite);

        //CComPtr<IShellView> pSV;
        //pShellBrowser->QueryActiveShellView(&pSV);

        //CComQIPtr<IServiceProvider> pSP(pShellBrowser);

        //CComPtr<IWebBrowserApp> pWB2;
        //HRESULT hr=pSP->QueryInterface(IID_IWebBrowserApp, (void**)&pWB2);

        //m_spUnkSite->QueryInterface(IID_


        //pSV->
        

        return S_OK;
    }    
};

OBJECT_ENTRY_AUTO(__uuidof(ieExtImpl), CieExtImpl)


/*
Beasto:
I was having exactly the same problem today. I wanted to recursively find a named tag amongst a bunch of frames, but couldn't access the IHTMLDocument2 from a IHTMLWindow2. I found this question from your comment in:

http://www.experts-exchange.com/Programming/Programming_Languages/Cplusplus/Q_20118715.html

which i had found via google.

ANYWAY, the answer provided for that thread actually worked for me. I had implemented a function that tried to get the IHTMLDocument2 from the IHTMLWindow2 made available via 'frames' in the root document, but for some reason was getting the same 'access denied' error.

Anyway, since the link accepted as the correct answer for that question is now dead, here is an updated link:

http://support.microsoft.com/default.aspx?scid=kb;en-us;196340

Below is my recursive function that digs up a named element (returns first match in case of duplicate names). It works by coming up with an IWebBrowser2 for each frame (or IFrame as well, i think, haven't tested) which is much more forthcoming with an IHTMLDocument2 than the irritating IHTMLWindow2.

Hope this helps with an extremely irritating problem!

David Hogan

IHTMLElement *findNamedTag(VARIANT name, IHTMLDocument2 *doc)
{
      if(doc == NULL) return NULL;
      IHTMLElementCollection *all;
      doc->get_all(&all);
      if(all == NULL) return NULL;

      VARIANT index;
      index.vt = VT_I4;
      index.lVal = 0;

      IDispatch *dispatch;
      all->item(name, index, &dispatch);
      all->Release();

      if(dispatch == NULL) 
      {
            //Then recursively check child frames ...
            IOleContainer* pContainer;

            // Get the container
            HRESULT hr = doc->QueryInterface(IID_IOleContainer, (void**)&pContainer);
            
            if (FAILED(hr))
            {
                  _com_error e(hr); MBW(CComBSTR(e.ErrorMessage()));
                  return NULL;
            }

            IEnumUnknown* pEnumerator;

            // Get an enumerator for the frames
            hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
            pContainer->Release();

            if (FAILED(hr))
            {
                  _com_error e(hr); MBW(CComBSTR(e.ErrorMessage()));
                  return NULL; 
            }

            IUnknown* pUnk;
            ULONG uFetched;

            // Enumerate and refresh all the frames
            for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
            {
                  // QI for IWebBrowser here to see if we have an embedded browser
                  IWebBrowser2* pBrowser;

                  hr = pUnk->QueryInterface(IID_IWebBrowser2, (void**)&pBrowser);
                  pUnk->Release();

                  if (SUCCEEDED(hr))
                  {
                        IDispatch *pDisp;
                        hr = pBrowser->get_Document(&pDisp);
                        pBrowser->Release();

                        if(FAILED(hr))
                        {
                              _com_error e(hr);
                              MBW(CComBSTR(e.ErrorMessage()));
                              continue;
                        }

                        IHTMLDocument2 *frameDoc;
                        hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&frameDoc);
                        
                        if(FAILED(hr))
                        {
                              _com_error e(hr);
                              MBW(CComBSTR(e.ErrorMessage()));
                              continue;
                        }
                        
                        IHTMLElement *element = findNamedTag(name, frameDoc);
                        frameDoc->Release();
                        if(element != NULL) 
                        {
                              pEnumerator->Release();
                              return element;
                        }
                  }
            }

            pEnumerator->Release();

            return NULL;
      }

      IHTMLElement *tag;
      dispatch->QueryInterface(IID_IHTMLElement, (void**)&tag);
      dispatch->Release();

      return tag;
}
*/