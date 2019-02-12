#ifndef BCOM_H
#define BCOM_H
#include <objbase.h>
#include "butils.h"
#include "bexcept.h"

// Forward declaration
class TComException;

namespace b {

// Defines
#define MKIID(a)  IID_##a
#define TCOMPTR(a, b) TComPtr<a, b, IID_##b>

template <class T> class TIPtr
{
protected:
	T* pInterface;
public:
	TIPtr() : pInterface(0) {}
	TIPtr(T* ptr)
	{
		pInterface = ptr;
		if(pInterface)
			pInterface->AddRef();
	}
	TIPtr(TIPtr& e)
	{
		pInterface = e.pInterface;
		if(pInterface)
			pInterface->AddRef();
	}
	virtual ~TIPtr()
	{
		if(pInterface)
			pInterface->Release();
	}
	TIPtr& operator = (T* ptr)
	{
		if(pInterface)
			pInterface->Release();
		pInterface = ptr;
		if(pInterface)
			pInterface->AddRef();
		return *this;
	}
	T** operator & ()
	{
		if(pInterface)
			pInterface->Release();
		return &pInterface;
	}
	operator bool ()
	{
		return pInterface != 0;
	}
	operator T* ()
	{
		if(!pInterface)
			throw TException("Null interface pointer");
		return pInterface;
	}
	bool operator == (TIPtr& b)
	{
		return pInterface == b.pInterface;
	}
	T* operator -> ()
	{
		if(!pInterface) {
			TException e("Null interface pointer");
			throw e;
		}
		return pInterface;
	}
};

template<class T> class TComPtr : public TIPtr<T>
{
public:
	TComPtr(REFCLSID rclsid, REFIID riid)
	{
		if(FAILED(CoCreateInstance(rclsid, NULL, CLSCTX_INPROC_SERVER,
											riid, (void**)&TComPtr::pInterface)))
		{
			throw TComException("Cannot create COM instance");
		}
	}
};

class TCoInitialize
{
	bool bCoInitialized;
public:
	TCoInitialize()
	{
		HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
		bCoInitialized = (hr == S_OK || hr == S_FALSE);
	}
	~TCoInitialize()
	{
		if(bCoInitialized)
			CoUninitialize();
	}
	inline operator bool()
	{
		return bCoInitialized;
	}
};

class TBStr
{
	BSTR bstr;
public:
	TBStr()
	{
		bstr = 0;
	}
	TBStr(const wchar_t* src)
	{
		bstr = SysAllocString(src);
	}
	TBStr(const char* src)
	{
		int cCount = -1;
		wchar_t* wide = b::AnsiToWide(src, cCount);
		bstr = SysAllocString(wide);
		delete[] wide;
	}
	TBStr(const TBStr& obj)
	{
		bstr = SysAllocString(obj.bstr);
	}
	~TBStr()
	{
		SysFreeString(bstr);
	}
	unsigned int length()
	{
		return SysStringLen(bstr);
	}
	void clear()
	{
		SysFreeString(bstr);
		bstr = 0;
	}
	TBStr& operator = (const TBStr& obj)
	{
		SysFreeString(bstr);
		bstr = SysAllocString(obj.bstr);
		return *this;
	}
	TBStr& operator = (const wchar_t* str)
	{
		SysFreeString(bstr);
		bstr = SysAllocString(str);
		return *this;
	}
	BSTR* operator & ()
	{
		return &bstr;
	}
	operator BSTR()
	{
		return bstr;
	}
};

class TVariant : public VARIANT
{
public:
	TVariant()
	{
		VariantInit(this);
	}
	TVariant(BSTR bstr)
	{
		V_VT(this) = VT_BSTR;
		V_BSTR(this) = bstr;
	}
	TVariant(bool boolean)
	{
		V_VT(this) = VT_BOOL;
		V_BOOL(this) = (boolean) ? VARIANT_TRUE : VARIANT_FALSE;
	}
	TVariant(IUnknown* pUnknown)
	{
		V_VT(this) = VT_UNKNOWN;
		V_UNKNOWN(this) = pUnknown;
	}
};

} // End of namespace b

class TComException : public TException
{
public:
	TComException(const char* msg) : TException(msg)
	{
		b::TIPtr<IErrorInfo> ErrorInfo;
		if(SUCCEEDED(GetErrorInfo(0, &ErrorInfo)))
		{
			b::TBStr bstr;
			ErrorInfo->GetDescription(&bstr);
			AddInfo(bstr);
		}
		else
		{
			AddInfo(UNKNOWN_ERROR);
		}
	}
	TComException(const wchar_t* msg) : TException(msg)
	{
		b::TIPtr<IErrorInfo> ErrorInfo;
		if(SUCCEEDED(GetErrorInfo(0, &ErrorInfo)))
		{
			b::TBStr bstr;
			ErrorInfo->GetDescription(&bstr);
			AddInfo(bstr);
		}
		else
		{
			AddInfo(UNKNOWN_ERROR);
		}
	}
};
#endif //BCOM_H
