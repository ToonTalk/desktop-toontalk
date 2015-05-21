// Copyright (c) 1992-2005. Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif

#if TT_ENHANCED_DRAG_AND_DROP

#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif 
#if !defined(__TT_MAIN_H)   
#include "main.h"
#endif   
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif

#include <shlobj.h>
//#include <ole2.h>
//#include <shlguid.h>
//
//// copied from the above -- don't understand why I need to
//// {4657278A-411B-11d2-839A-00C04FD918D0}
//DEFINE_GUID(CLSID_DragDropHelper,   0x4657278a, 0x411b, 0x11d2, 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0);
//
//// {4657278B-411B-11d2-839A-00C04FD918D0}
//DEFINE_GUID(IID_IDropTargetHelper,  0x4657278b, 0x411b, 0x11d2, 0x83, 0x9a, 0x0, 0xc0, 0x4f, 0xd9, 0x18, 0xd0);

#if !defined(__TT_DRAGDROP_H)
#include "dragdrop.h"
#endif

// OLE drag and drop code largely based upon sample code of Author: Leon Finker  1/2001 -- http://www.codeproject.com/shell/dragdrop.asp

//////////////////////////////////////////////////////////////////////
// CIDropTarget Class
//////////////////////////////////////////////////////////////////////

CIDropTarget::CIDropTarget(HWND hTargetWnd): 
	m_hTargetWnd(hTargetWnd),
	m_cRefCount(0), m_bAllowDrop(false),
//	m_pDropTargetHelper(NULL), 
	m_pSupportedFrmt(NULL)
{ 
	//if(FAILED(CoCreateInstance(CLSID_DragDropHelper,NULL,CLSCTX_INPROC_SERVER,
 //                    IID_IDropTargetHelper,(LPVOID*)&m_pDropTargetHelper)))
//		m_pDropTargetHelper = NULL;
}

CIDropTarget::~CIDropTarget()
{
	//if(m_pDropTargetHelper != NULL)
	//{
	//	m_pDropTargetHelper->Release();
	//	m_pDropTargetHelper = NULL;
	//}
}

HRESULT STDMETHODCALLTYPE CIDropTarget::QueryInterface( /* [in] */ REFIID riid,
						/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
   *ppvObject = NULL;
   if (IID_IUnknown==riid || IID_IDropTarget==riid)
			 *ppvObject=this;

	if (*ppvObject != NULL)
	{
		((LPUNKNOWN)*ppvObject)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CIDropTarget::Release( void) {
   long nTemp;
   nTemp = --m_cRefCount;
   if(nTemp==0)
	  delete this;
   return nTemp;
}

bool CIDropTarget::QueryDrop(DWORD grfKeyState, LPDWORD pdwEffect) {  
	DWORD dwOKEffects = *pdwEffect; 
	if(!m_bAllowDrop)
	{
	   *pdwEffect = DROPEFFECT_NONE;
	   return false;
	}

//	*pdwEffect = DROPEFFECT_LINK; // DROPEFFECT_COPY;
	*pdwEffect = DROPEFFECT_COPY|DROPEFFECT_LINK; // rewritten on 010205 so works both from e-mail (copy) and a browser (link)

	return true;
}   

HRESULT STDMETHODCALLTYPE CIDropTarget::DragEnter(
    /* [unique][in] */ IDataObject __RPC_FAR *pDataObj,
    /* [in] */ DWORD grfKeyState,
    /* [in] */ POINTL pt,
    /* [out][in] */ DWORD __RPC_FAR *pdwEffect)
{
	if(pDataObj == NULL)
		return E_INVALIDARG;
	
	tt_dragging = TRUE; //  new on 020105

	//if(m_pDropTargetHelper)
	//	m_pDropTargetHelper->DragEnter(m_hTargetWnd, pDataObj, (LPPOINT)&pt, *pdwEffect);
	//IEnumFORMATETC* pEnum;
	//pDataObj->EnumFormatEtc(DATADIR_GET,&pEnum);
	//FORMATETC ftm;
	//for()
	//pEnum->Next(1,&ftm,0);
	//pEnum->Release();
	m_pSupportedFrmt = NULL;
	for(int i =0; i<m_formatetc.GetSize(); ++i)
	{
		m_bAllowDrop = (pDataObj->QueryGetData(&m_formatetc[i]) == S_OK)?true:false;
		if(m_bAllowDrop)
		{
			m_pSupportedFrmt = &m_formatetc[i];
			break;
		}
	}

	QueryDrop(grfKeyState, pdwEffect);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CIDropTarget::DragOver( 
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD __RPC_FAR *pdwEffect)
{
	//if(m_pDropTargetHelper)
	//	m_pDropTargetHelper->DragOver((LPPOINT)&pt, *pdwEffect);
	QueryDrop(grfKeyState, pdwEffect);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CIDropTarget::DragLeave( void)
{
	//if(m_pDropTargetHelper)
	//	m_pDropTargetHelper->DragLeave();
	
	m_bAllowDrop = false;
	m_pSupportedFrmt = NULL;
	return S_OK;
}

bool CIDropTarget::OnDrop(FORMATETC *pFmtEtc, STGMEDIUM &medium, DWORD *pdwEffect, POINTL point) {
	if (pFmtEtc->cfFormat == CF_TEXT && medium.tymed == TYMED_HGLOBAL) {
		TCHAR* pStr = (TCHAR *) GlobalLock(medium.hGlobal);
		if (pStr != NULL) {
			// simulate a drop
			tt_drop_files_x = ideal_horizontal_units(point.x);
			tt_drop_files_y = ideal_screen_height-ideal_vertical_units(point.y);
			tt_drop_files_count = 1;
			tt_drop_file_names = new string[tt_drop_files_count];
			tt_drop_file_hash_names = new string[tt_drop_files_count];			
	  		tt_drop_file_names[0] = copy_string(pStr);
			tt_drop_file_hash_names[0] = log_file_name(tt_drop_file_names[0]);
			if (tt_drop_file_hash_names[0] == NULL) { // pretend it didn't happen -- new on 150105
				tt_drop_files_count = 0; 
				delete [] tt_drop_file_names[0];
				delete [] tt_drop_file_names;
				tt_drop_file_names = NULL;
				delete [] tt_drop_file_hash_names;
				tt_drop_file_hash_names = NULL;
			};
		}
		GlobalUnlock(medium.hGlobal);
	}
	//if (pFmtEtc->cfFormat == CF_HDROP && medium.tymed == TYMED_HGLOBAL) {
	//	HDROP hDrop = (HDROP) GlobalLock(medium.hGlobal);
	//	if (hDrop != NULL) {
	//		TCHAR szFileName[MAX_PATH];
	//		UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); 
	//		for(UINT i = 0; i < cFiles; ++i) {
	//			DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
	//			// more to do -- or use other code in Winmain.cpp
	//		}  
	//		//DragFinish(hDrop); // base class calls ReleaseStgMedium
	//	}
	//	GlobalUnlock(medium.hGlobal);
	//}
	return true; //let base free the medium
}

HRESULT STDMETHODCALLTYPE CIDropTarget::Drop(
	/* [unique][in] */ IDataObject __RPC_FAR *pDataObj,
   /* [in] */ DWORD grfKeyState, 
	/* [in] */ POINTL pt, 
	/* [out][in] */ DWORD __RPC_FAR *pdwEffect) {
	tt_dragging = FALSE; // new on 020105
	if (pDataObj == NULL)
		return E_INVALIDARG;	

	//if (m_pDropTargetHelper)
	//	m_pDropTargetHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);

	if (QueryDrop(grfKeyState, pdwEffect)) {
		if (m_bAllowDrop && m_pSupportedFrmt != NULL) {
			STGMEDIUM medium;
			if (pDataObj->GetData(m_pSupportedFrmt, &medium) == S_OK) {
				POINT pt_as_point; // don't know why Microsoft has both POINT and POINTL -- programming around it
				pt_as_point.x = pt.x;
				pt_as_point.y = pt.y; 
				ScreenToClient(tt_main_window->get_handle(),&pt_as_point); // new on 301204 to convert to "client" coordinates
				pt.x = pt_as_point.x;
				pt.y = pt_as_point.y;
				if (OnDrop(m_pSupportedFrmt, medium, pdwEffect, pt)) //does derive class wants us to free medium?
					ReleaseStgMedium(&medium);
			}
		}
	}
	m_bAllowDrop=false;
	*pdwEffect = DROPEFFECT_NONE;
	m_pSupportedFrmt = NULL;
	return S_OK;
}

//IDropTargetHelper drop_target_helper = NULL;

CIDropTarget *drop_target = NULL;

void release_drag_drop(HWND window_handle) {
	if (drop_target == NULL) {
		return; // already done
	};
	RevokeDragDrop(window_handle);
	drop_target->Release();
	drop_target = NULL;
#if TT_DEBUG_ON
	if (tt_debug_mode == 20105) {
		tt_error_file() << "release_drag_drop on frame " << tt_frame_number << endl; 
	};
#endif
};

void register_drag_drop(HWND window_handle) {
	if (drop_target != NULL || !has_initialization_completed()) {
		return; // already done or too early
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(0,"before RegisterDragDrop");
#endif
	drop_target = new CIDropTarget(window_handle);
	drop_target->AddRef(); // I think RegisterDragDrop does it too -- should be released on exit anyway...
	HRESULT status = RegisterDragDrop(window_handle,drop_target);
	if (status != S_OK) { 
		tt_error_file() << "Failed to set up drag and drop from any program other than Windows Explorer. Sorry." << endl;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 20105) {
		tt_error_file() << "register_drag_drop succeeded on frame " << tt_frame_number << endl; // for now 301204
	};
#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(0,"after RegisterDragDrop");
#endif
	FORMATETC ftetc={0};
	ftetc.cfFormat = CF_TEXT;
	ftetc.dwAspect = DVASPECT_CONTENT;
	ftetc.lindex = -1;
	ftetc.tymed = TYMED_HGLOBAL;
	drop_target->AddSuportedFormat(ftetc);
	ftetc.cfFormat=CF_HDROP;
	drop_target->AddSuportedFormat(ftetc);
	//ftetc.cfFormat = CFSTR_INETURL; // new on 291204 for URLs
	//drop_target->AddSuportedFormat(ftetc);
//   return(status == S_OK);
};

//boolean do_drag_drop() {
//	IDataObject data_object;
//	IDropSource drop_source;
//	DWORD effect;
//	WINOLEAPI status = DoDragDrop(&data_object,&drop_source,DROPEFFECT_COPY,&effect);
//	return(status == DRAGDROP_S_DROP);
//};

#endif
