#ifndef FINDFILE_H
#define FINDFILE_H
#include <windows.h>

template <class T> class TFindFile
{
	HANDLE hFindFile;
protected:
	WIN32_FIND_DATA* FindData;
public:
   TFindFile();
   ~TFindFile();
   bool FindFirst(const T* pattern);
   bool FindNext();
   FILETIME* LastWriteTime()
   {
      return &FindData->ftLastWriteTime;
   }
   const T* GetFileName()
   {
      return FindData->cFileName;
   }
};

template <class T> TFindFile<T>::TFindFile()
{
   hFindFile = INVALID_HANDLE_VALUE;
   FindData = new WIN32_FIND_DATA;
}

template <class T> TFindFile<T>::~TFindFile()
{
   if(hFindFile != INVALID_HANDLE_VALUE)
   {
      ::FindClose(hFindFile);
   }
   delete FindData;
}

template <class T> bool TFindFile<T>::FindFirst(const T* pattern)
{
   bool bRet = false;
   if(hFindFile != INVALID_HANDLE_VALUE)
   {
      ::FindClose(hFindFile);
   }
   if((hFindFile = ::FindFirstFile(pattern, FindData)) != INVALID_HANDLE_VALUE)
   {
      while(FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         if(!::FindNextFile(hFindFile, FindData))
         {
            goto stop;
         }
      }
      bRet = true;
	}
stop:
   return bRet;
}

template <class T> bool TFindFile<T>::FindNext()
{
	bool bRet = false;
   if(hFindFile != INVALID_HANDLE_VALUE)
   {
   	do
      {
         if(!::FindNextFile(hFindFile, FindData))
         {
            goto stop;
         }
      }
      while(FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
      bRet = true;
	}
stop:
   return bRet;
}

#ifdef UNICODE
typedef TFindFile<wchar_t> FindFile;
#else
typedef TFindFile<char> FindFile;
#endif
#endif //FINDFILE_H

