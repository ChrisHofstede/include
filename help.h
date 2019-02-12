#ifndef HELP_H
#define HELP_H
#include "htmlhelp.h"

// Helper class
class THtmlHelpLibLoader
{
   typedef HWND WINAPI (*THtmlHelpPtrA)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData);
   typedef HWND WINAPI (*THtmlHelpPtrW)(HWND hwndCaller, LPCWSTR pszFile, UINT uCommand, DWORD_PTR dwData);
   HINSTANCE hInst;
   THtmlHelpPtrA HelpA;
   THtmlHelpPtrW HelpW;
   void Init()
   {
      if(!hInst)
      {
         hInst = LoadLibrary(TEXT("hhctrl.ocx"));
      }
   }
public:
   THtmlHelpLibLoader() : hInst(0), HelpA(0), HelpW(0) {}
   ~THtmlHelpLibLoader()
   {
      if(hInst)
      {
         FreeLibrary(hInst);
      }
   }
   HWND HtmlHelp(HWND hwndCaller, const char* pszFile, UINT uCommand, DWORD_PTR dwData)
   {
      if(!HelpA)
      {
         Init();
         HelpA = (THtmlHelpPtrA)GetProcAddress(hInst, "HtmlHelpA");
      }
      return (HelpA) ? HelpA(hwndCaller, pszFile, uCommand, dwData) : 0;
   }
   HWND HtmlHelp(HWND hwndCaller, const wchar_t* pszFile, UINT uCommand, DWORD_PTR dwData)
   {
      if(!HelpW)
      {
         Init();
         HelpW = (THtmlHelpPtrW)GetProcAddress(hInst, "HtmlHelpW");
      }
      return (HelpW) ? HelpW(hwndCaller, pszFile, uCommand, dwData) : 0;
   }
};

template <class T> class THtmlHelp : THtmlHelpLibLoader
{
   const T* HelpFile;
   void ShowError(HWND hwnd, TCHAR* msg)
   {
      MessageBox(hwnd, msg, TEXT("Help Error"), MB_ICONERROR | MB_OK);
   }
public:
   THtmlHelp(const T* helpfile) : HelpFile(helpfile) {}
   ~THtmlHelp()
   {
      Close();
   }
   void ShowTopic(HWND hwnd, DWORD topic_id);
   void ShowPopup(HWND hwnd, DWORD control_id, DWORD context_id);
   void ShowPopup(LPHELPINFO hi);
   void ShowToc(HWND hwnd);
   void Close()
   {
      HtmlHelp(0, (T*)0, HH_CLOSE_ALL, 0);
   }
};

template <class T> void THtmlHelp<T>::ShowTopic(HWND hwnd, DWORD topic_id)
{
   if(HtmlHelp(hwnd, HelpFile, HH_HELP_CONTEXT, topic_id) == 0)
   {
      ShowError(hwnd, TEXT("Cannot show topic"));
   }
}

template <class T> void THtmlHelp<T>::ShowPopup(HWND hwnd, DWORD control_id, DWORD context_id)
{
   DWORD ids[3];
   ids[0] = control_id;
   ids[1] = context_id;
   ids[2] = 0;
   HWND hControl = GetDlgItem(hwnd, control_id);
   if(HtmlHelp((hControl) ? hControl : hwnd, HelpFile, HH_TP_HELP_WM_HELP, (DWORD)ids) == 0)
   {
      ShowError(hwnd, TEXT("Cannot show popup"));
   }
}

template <class T> void THtmlHelp<T>::ShowPopup(LPHELPINFO hi)
{
   HH_POPUP* popup = 0;
   try
   {
      popup = new HH_POPUP;
      ZeroMemory(popup, sizeof(HH_POPUP));
      popup->cbStruct = sizeof(HH_POPUP);
      popup->idString = hi->dwContextId;
      popup->pt = hi->MousePos;
      popup->clrForeground = -1;
      popup->clrBackground = -1;
      popup->rcMargins.left = -1; // RECT
      popup->rcMargins.top = -1;
      popup->rcMargins.right = -1;
      popup->rcMargins.bottom = -1;
      //popup->pszFont = TEXT("MS Sans Serif, 10");
      if(HtmlHelp((HWND)hi->hItemHandle, HelpFile, HH_DISPLAY_TEXT_POPUP, (DWORD)popup) == 0)
      {
         ShowError((HWND)hi->hItemHandle, TEXT("Cannot show popup"));
      }
   }
   __finally
   {
      delete popup;
   }
}

template <class T> void THtmlHelp<T>::ShowToc(HWND hwnd)
{
   if(HtmlHelp(hwnd, HelpFile, HH_DISPLAY_TOC, 0) == 0)
   {
      ShowError(hwnd, TEXT("Cannot show table of contents"));
   }
}
#endif //HELP_H
