
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Thu Jul 08 00:15:28 2010
 */
/* Compiler settings for ieExt.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ieExt_h__
#define __ieExt_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IieExtImpl_FWD_DEFINED__
#define __IieExtImpl_FWD_DEFINED__
typedef interface IieExtImpl IieExtImpl;
#endif 	/* __IieExtImpl_FWD_DEFINED__ */


#ifndef __ieExtImpl_FWD_DEFINED__
#define __ieExtImpl_FWD_DEFINED__

#ifdef __cplusplus
typedef class ieExtImpl ieExtImpl;
#else
typedef struct ieExtImpl ieExtImpl;
#endif /* __cplusplus */

#endif 	/* __ieExtImpl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IieExtImpl_INTERFACE_DEFINED__
#define __IieExtImpl_INTERFACE_DEFINED__

/* interface IieExtImpl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IieExtImpl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1C0D37DB-4646-4158-A876-1F748C3F53EC")
    IieExtImpl : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IieExtImplVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IieExtImpl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IieExtImpl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IieExtImpl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IieExtImpl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IieExtImpl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IieExtImpl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IieExtImpl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IieExtImplVtbl;

    interface IieExtImpl
    {
        CONST_VTBL struct IieExtImplVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IieExtImpl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IieExtImpl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IieExtImpl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IieExtImpl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IieExtImpl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IieExtImpl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IieExtImpl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IieExtImpl_INTERFACE_DEFINED__ */



#ifndef __ieExtLib_LIBRARY_DEFINED__
#define __ieExtLib_LIBRARY_DEFINED__

/* library ieExtLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ieExtLib;

EXTERN_C const CLSID CLSID_ieExtImpl;

#ifdef __cplusplus

class DECLSPEC_UUID("9BA9E4C9-1147-4C81-B45E-DF966D095543")
ieExtImpl;
#endif
#endif /* __ieExtLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


