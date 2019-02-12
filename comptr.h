#ifndef COMPTR_H
#define COMPTR_H
#include <windows.h>
#include "wexcept.h"

//defines
#define MKIID(a)  IID_##a
#define TCOMPTR(a) TComPtr<a, IID_##a>

template <class T> class TIPtr
{
protected:
   T* pInterface;
public:
   TIPtr() : pInterface(0)
   {
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
      return pInterface;
   }
   bool operator == (TIPtr& b)
   {
      return pInterface == b.pInterface;
   }
   T* operator -> ()
   {
      if(!pInterface)
         throw TComException("Null interface pointer");
      return pInterface;
   }
};

template <class T, REFIID riid> class TComPtr : public TIPtr<T>
{
public:
   TComPtr(REFCLSID rclsid)
   {
      IUnknown* pUnknown;
      if(FAILED(CoCreateInstance(rclsid, NULL,
                                 CLSCTX_INPROC_SERVER,
                                 IID_IUnknown, (void**)&pUnknown)))
      {
         throw TComException("Cannot create instance");
      }
      if(FAILED(pUnknown->QueryInterface(riid, (void**)&pInterface)))
      {
         throw TComException("Cannot query interface");
      }
      pUnknown->Release();
   }
   TComPtr& operator = (T* ptr)
   {
      return (TComPtr&)TIPtr<T>::operator = (ptr);
   }
};

class TCoInitialize
{
	bool bCoInitialized;
public:
	TCoInitialize()
   {
      //HRESULT hr = CoInitialize(0);
      HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
   	bCoInitialized = (hr == S_OK || hr == S_FALSE);
   }
	~TCoInitialize()
   {
   	if(bCoInitialized)
      	CoUninitialize();
   }
};
#endif //COMPTR_H
