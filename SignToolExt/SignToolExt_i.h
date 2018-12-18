

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Nov 04 21:23:24 2017
 */
/* Compiler settings for SignToolExt.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __SignToolExt_i_h__
#define __SignToolExt_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICHandleMenu_FWD_DEFINED__
#define __ICHandleMenu_FWD_DEFINED__
typedef interface ICHandleMenu ICHandleMenu;
#endif 	/* __ICHandleMenu_FWD_DEFINED__ */


#ifndef __CHandleMenu_FWD_DEFINED__
#define __CHandleMenu_FWD_DEFINED__

#ifdef __cplusplus
typedef class CHandleMenu CHandleMenu;
#else
typedef struct CHandleMenu CHandleMenu;
#endif /* __cplusplus */

#endif 	/* __CHandleMenu_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ICHandleMenu_INTERFACE_DEFINED__
#define __ICHandleMenu_INTERFACE_DEFINED__

/* interface ICHandleMenu */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICHandleMenu;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B69FBB6A-CD60-4CFE-9465-AB13BEFA5C8D")
    ICHandleMenu : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICHandleMenuVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICHandleMenu * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICHandleMenu * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICHandleMenu * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICHandleMenu * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICHandleMenu * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICHandleMenu * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICHandleMenu * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICHandleMenuVtbl;

    interface ICHandleMenu
    {
        CONST_VTBL struct ICHandleMenuVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICHandleMenu_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICHandleMenu_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICHandleMenu_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICHandleMenu_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICHandleMenu_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICHandleMenu_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICHandleMenu_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICHandleMenu_INTERFACE_DEFINED__ */



#ifndef __SignToolExtLib_LIBRARY_DEFINED__
#define __SignToolExtLib_LIBRARY_DEFINED__

/* library SignToolExtLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_SignToolExtLib;

EXTERN_C const CLSID CLSID_CHandleMenu;

#ifdef __cplusplus

class DECLSPEC_UUID("56BC0909-03F5-4EF9-AE7B-2846AB28B33E")
CHandleMenu;
#endif
#endif /* __SignToolExtLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


