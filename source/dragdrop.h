// Copyright (c) 1992-2004. Ken Kahn, Animated Programs, All rights reserved.

#if TT_ENHANCED_DRAG_AND_DROP

#if !defined(__TT_DRAGDROP_H)

#define __TT_DRAGDROP_H

#include <shlobj.h>

class CIDropTarget : public IDropTarget
{
	DWORD m_cRefCount;
	bool m_bAllowDrop;
//	struct IDropTargetHelper *m_pDropTargetHelper;
//	CComPtr<IDropTargetHelper> *m_pDropTargetHelper;
	// rewritten after reading http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnwui/html/ddhelp_pt1.asp
	CSimpleArray<FORMATETC> m_formatetc;
	FORMATETC* m_pSupportedFrmt;
protected:
	HWND m_hTargetWnd;
public:
	
	CIDropTarget(HWND m_hTargetWnd);
	virtual ~CIDropTarget();
	void AddSuportedFormat(FORMATETC& ftetc) { m_formatetc.Add(ftetc); }
	
	//return values: true - release the medium. false - don't release the medium 
//	virtual
	bool OnDrop(FORMATETC* pFmtEtc, STGMEDIUM& medium,DWORD *pdwEffect, POINTL point); // = 0;

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef( void) { 
//		ATLTRACE("CIDropTarget::AddRef\n"); // commented out on 100205 but probably only happens when debugging anyway
		return ++m_cRefCount; 
	}
	virtual ULONG STDMETHODCALLTYPE Release( void);

    bool QueryDrop(DWORD grfKeyState, LPDWORD pdwEffect);
	virtual HRESULT STDMETHODCALLTYPE DragEnter(
        /* [unique][in] */ IDataObject __RPC_FAR *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD __RPC_FAR *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragOver( 
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD __RPC_FAR *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragLeave( void);    
    virtual HRESULT STDMETHODCALLTYPE Drop(
        /* [unique][in] */ IDataObject __RPC_FAR *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD __RPC_FAR *pdwEffect);
};

void register_drag_drop(HWND window_handle);
void release_drag_drop(HWND window_handle);

#endif

#endif
