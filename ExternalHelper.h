#ifndef EXTERNALHELPER_H
#define EXTERNALHELPER_H


#include <fcntl.h>
#include "Settings.h"


extern CSettings _Settings;


class ExternalHelper :
  public CComObjectRoot,
  public IDispatchImpl<IExternalHelper, &IID_IExternalHelper>
{
public:

  DECLARE_NO_REGISTRY()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(ExternalHelper)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IExternalHelper)
  END_COM_MAP()

  // IExternalHelper
  STDMETHOD(BeginUndoUnit)(IDispatch *obj,BSTR name) {
    MSHTML::IMarkupServices   *srv;
    HRESULT hr=obj->QueryInterface(&srv);
    if (FAILED(hr))
      return hr;
    hr=srv->raw_BeginUndoUnit(name);
    srv->Release();
    return hr;
  }
  STDMETHOD(EndUndoUnit)(IDispatch *obj) {
    MSHTML::IMarkupServices   *srv;
    HRESULT hr=obj->QueryInterface(&srv);
    if (FAILED(hr))
      return hr;
    hr=srv->raw_EndUndoUnit();
    srv->Release();
    return hr;
  }
  STDMETHOD(get_inflateBlock)(IDispatch *obj,BOOL *ifb) {
    MSHTML::IHTMLElement3 *elem;
    HRESULT hr=obj->QueryInterface(&elem);
    if (FAILED(hr))
      return hr;
    VARIANT_BOOL vb;
    hr=elem->get_inflateBlock(&vb);
    *ifb=SUCCEEDED(hr) && vb==VARIANT_TRUE ? TRUE : FALSE;
    elem->Release();
    return hr;
  }
  STDMETHOD(put_inflateBlock)(IDispatch *obj,BOOL ifb) {
    MSHTML::IHTMLElement3 *elem;
    HRESULT hr=obj->QueryInterface(&elem);
    if (FAILED(hr))
      return hr;
    hr=elem->put_inflateBlock(ifb ? VARIANT_TRUE : VARIANT_FALSE);
    elem->Release();
    return hr;
  }
  STDMETHOD(GenrePopup)(IDispatch *obj,LONG x,LONG y,BSTR *name);
  STDMETHOD(DescShowMenu)(IDispatch *obj,LONG x,LONG y,BSTR *element_id);
  // Modification by Pilgrim
  /*STDMETHOD(LangPopup)(IDispatch *obj,LONG x,LONG y,BSTR *name);
  STDMETHOD(SrcLangPopup)(IDispatch *obj,LONG x,LONG y,BSTR *name);
  STDMETHOD(STILangPopup)(IDispatch *obj,LONG x,LONG y,BSTR *name);

  STDMETHOD(STISrcLangPopup)(IDispatch *obj,LONG x,LONG y,BSTR *name);*/


	STDMETHOD(GetStylePath)(BSTR *name)
	{
		wchar_t path[MAX_PATH + 1];
		GetModuleFileName(0, path, MAX_PATH);
		PathRemoveFileSpec(path);
		CString     us(path);
		*name = us.AllocSysString();
		
		return S_OK;
	}

	STDMETHOD(GetBinarySize)(BSTR data, int *length)
	{
		*length = SysStringByteLen(data);
		return S_OK;
	}  
	STDMETHOD(InflateParagraphs)(IDispatch *elem)
	{
			MSHTML::IHTMLElement2Ptr el;
			elem->QueryInterface(IID_IHTMLElement2,(void**)&el);
			MSHTML::IHTMLElementCollectionPtr   pp(el->getElementsByTagName(L"P"));
			for (long l=0;l<pp->length;++l)
			{
				MSHTML::IHTMLElement3Ptr(pp->item(l))->inflateBlock=VARIANT_TRUE;
			}
			return S_OK;
	}
	STDMETHOD(GetUUID)(BSTR *uid)
	{
			UUID	      uuid;
			unsigned char *str;
			if (UuidCreate(&uuid)==RPC_S_OK && UuidToStringA(&uuid,&str)==RPC_S_OK) 
			{
				CString     us(str);
				RpcStringFreeA(&str);
				us.MakeUpper();
				*uid = us.AllocSysString();
				return S_OK;
			}
			return S_FALSE;
	}
	STDMETHOD(MsgBox)(BSTR message)
	{
		wchar_t cpt[MAX_LOAD_STRING + 1];
		::LoadString(_Module.GetResourceInstance(), IDS_SCRIPT_MSG_CPT, cpt, MAX_LOAD_STRING);
		MessageBoxW(GetActiveWindow(), message, cpt, MB_ICONINFORMATION|MB_OK);
		return S_OK;
	}

	STDMETHOD(AskYesNo)(BSTR message, BOOL *pVal)
	{
		wchar_t cpt[MAX_LOAD_STRING + 1];
		::LoadString(_Module.GetResourceInstance(), IDS_SCRIPT_MSG_CPT, cpt, MAX_LOAD_STRING);
		if (IDYES == MessageBoxW(GetActiveWindow(), message, cpt, MB_ICONQUESTION|MB_YESNO))
		{
			*pVal = true;
		}
		else
		{
			*pVal = false;
		}
		return S_OK;
	}

	STDMETHOD(SaveBinary)(BSTR path, BSTR data, BOOL* ret)
	{		
		int len = SysStringByteLen(data);
		void* buf = (void*)data;
		HANDLE file = CreateFile(path, GENERIC_WRITE | FILE_WRITE_DATA, FILE_SHARE_WRITE, 0, CREATE_NEW, 0, 0);
		if(INVALID_HANDLE_VALUE == file)
		{
			*ret = false;
			return S_OK;
		}
		DWORD writen = 0;
		WriteFile(file, buf, len, &writen, 0);
		CloseHandle(file);
		*ret = true;
		return S_OK;
	}

	STDMETHOD(GetExtendedStyle)(BSTR elem, BOOL* ext)
	{
		*ext = _Settings.GetExtElementStyle(elem);		
		return S_OK;
	}

  STDMETHOD(IsFastMode)(BOOL* ext)
  {
    *ext = _Settings.FastMode();	
    return S_OK;
  }

	STDMETHOD(DescShowElement)(BSTR elem, BOOL show)
	{
		_Settings.SetExtElementStyle(elem, show != 0);
		return S_OK;
	}

	STDMETHOD(SetStyleEx)(IDispatch* doc, IDispatch* elem, BSTR style)
	{
		MSHTML::IHTMLElementPtr el = elem;
		U::ChangeAttribute(el, L"class", style, doc);
		return S_OK;
	}

};

#endif
