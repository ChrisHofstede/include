#ifndef FILEDLG_H
#define FILEDLG_H
#include <windows.h>
#include "bstring.h"

namespace b {

template <class T> class TFileDialog
{
   LPOPENFILENAMEW pOpenFileName;
protected:
   TString<T> FileName;
   TString<T> Filter;
   TString<T> DefExt;
public:
   TFileDialog() :pOpenFileName(0)
   {
	  pOpenFileName = new OPENFILENAMEW;
	  ZeroMemory(pOpenFileName, sizeof(OPENFILENAMEW));
      pOpenFileName->lStructSize = sizeof(OPENFILENAMEW);
   }
   ~TFileDialog()
   {
      delete pOpenFileName;
   }
   void Init(const TString<T>& filtername, const TString<T>& filterext, const TString<T>& defext)
   {
      Filter = filtername;
	  Filter += T('\0');
      Filter += filterext;
      Filter += T('\0');
      DefExt = defext;
      pOpenFileName->lpstrFilter = Filter.c_str();
      pOpenFileName->lpstrDefExt = DefExt.c_str();
      pOpenFileName->nMaxFile = MAX_PATH;
   }
   bool GetOpenFileName(HWND hParent, const T* Title = 0)
   {
      pOpenFileName->Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_FILEMUSTEXIST;
      pOpenFileName->lpstrTitle = Title;
      pOpenFileName->hwndOwner = hParent;
      int l = FileName.length();
      pOpenFileName->lpstrFile = FileName.get_buffer(MAX_PATH);
      FileName[l] = 0;
      BOOL bRet = ::GetOpenFileNameW(pOpenFileName);
      FileName.fix_buffer();
      return bRet;
   }
   bool GetSaveFileName(HWND hParent, const T* Title = 0)
   {
      pOpenFileName->Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
      pOpenFileName->lpstrTitle = Title;
      pOpenFileName->hwndOwner = hParent;
      int l = FileName.length();
      pOpenFileName->lpstrFile = FileName.get_buffer(MAX_PATH);
      FileName[l] = 0;
	  BOOL bRet = ::GetSaveFileNameW(pOpenFileName);
      FileName.fix_buffer();
      return bRet;
   }
   const TString<T>& GetFileName()
   {
      return FileName;
   }
};
} // End Namespace
#endif //FILEDLG_H
