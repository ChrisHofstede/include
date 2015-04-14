#ifndef SCANFILE_H
#define SCANFILE_H
#include <windows.h>
// Prototypes

template<class T> class TScanFile {
	HANDLE hFindFile;
	b::TStack<HANDLE> Stack;
	b::TString<T> Dir;
	static const T Mask[];
	bool bRecurse;
protected:
	WIN32_FIND_DATA *FindData;
public:
	enum TScanResult {
		Invalid = 0, IsDirectory, IsFile
	};
	TScanFile() :
		FindData(0) {
		hFindFile = INVALID_HANDLE_VALUE;
		FindData = new WIN32_FIND_DATA;
		Result = Invalid;
		bRecurse = true;
	}
	~TScanFile() {
		Flush();
		delete FindData;
	}
	TScanResult Init(const b::TString<T>& dir, bool recurse = true) {
		Result = Invalid;
		bRecurse = recurse;
		Dir = dir;
		int len = Dir.length() - 1;
		if (Dir[len] == T('\\')) {
			Dir.resize(len);
		}
		b::TString<T> File = Dir + Mask;
		Flush();
		if ((hFindFile = ::FindFirstFile(File.c_str(), FindData))
				!= INVALID_HANDLE_VALUE) {
			if (IsDotDirectory()) {
				Next();
			} else {
				Result = IsFileOrDirectory();
			}
		}
		return Result;
	}
	TScanResult Next() {
		if (Result == IsDirectory && bRecurse) {
			PushDirectory();
		} else {
			bool retval = false;
			if (hFindFile != INVALID_HANDLE_VALUE) {
				do {
					retval = ::FindNextFile(hFindFile, FindData);
				} while (retval && IsDotDirectory());
			}
			if (retval) {
				Result = IsFileOrDirectory();
			} else {
				PopDirectory();
			}
		}
		return Result;
	}
	TScanResult SkipDir() {
		bool retval = false;
		if (hFindFile != INVALID_HANDLE_VALUE) {
			do {
				retval = ::FindNextFile(hFindFile, FindData);
			} while (retval && IsDotDirectory());
		}
		if (retval) {
			Result = IsFileOrDirectory();
		} else {
			PopDirectory();
		}
		return Result;
	}
	const b::TString<T> GetFile();
	const b::TString<T> GetDir() {
		return Dir;
	}
	const T* GetFileName() {
		return FindData->cFileName;
	}
	const DWORD GetSize() {
		return FindData->nFileSizeLow;
	}
	const DWORD GetAttributes() {
		return FindData->dwFileAttributes;
	}
	bool GetFileTime(SYSTEMTIME& time);
private:
	bool IsDotDirectory();
	TScanResult IsFileOrDirectory() {
		return (FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? IsDirectory
				: IsFile;
	}
	void Flush();
	void PushDirectory();
	void PopDirectory();
	TScanResult Result;
};

template<class T> const T TScanFile<T>::Mask[] = { T('\\'), T('*'), T(0) };

template<class T> bool TScanFile<T>::IsDotDirectory() {
	bool retval = false;
	if (FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		const T* ptr = FindData->cFileName;
		if (ptr[0] == T('.')) {
			if (ptr[1] == T('\0')) {
				retval = true;
			} else {
				if (ptr[1] == T('.') && ptr[2] == T('\0')) {
					retval = true;
				}
			}
		}
	}
	return retval;
}

template<class T> void TScanFile<T>::Flush() {
	do {
		if (hFindFile != INVALID_HANDLE_VALUE)
			FindClose(hFindFile);
	} while (Stack.Pop(hFindFile));
}

template<class T> void TScanFile<T>::PushDirectory() {
	Result = Invalid;
	if (hFindFile != INVALID_HANDLE_VALUE) {
		if (Stack.Push(hFindFile)) {
			Dir = GetFile();
			b::TString<T> File = Dir + Mask;
			if ((hFindFile = ::FindFirstFile(File.c_str(), FindData))
					!= INVALID_HANDLE_VALUE) {
				if (IsDotDirectory()) {
					Next();
				} else {
					Result = IsFileOrDirectory();
				}
			} else {
				PopDirectory();
			}
		}
	}
}

template<class T> void TScanFile<T>::PopDirectory() {
	Result = Invalid;
	if (hFindFile != INVALID_HANDLE_VALUE) {
		FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	if (Stack.Pop(hFindFile)) {
		int pos = Dir.last_index(T('\\'));
		if (pos != b::npos) {
			Dir.resize(pos);
		}
		Next();
	}
}

template<class T> const b::TString<T> TScanFile<T>::GetFile() {
	b::TString<T> sReturn = Dir;

	// Fix for GNU compiler
	sReturn.operator+=(T('\\'));
	sReturn += FindData->cFileName;
	return sReturn;
}

template<class T> bool TScanFile<T>::GetFileTime(SYSTEMTIME& time) {
	bool retval = false;
	FILETIME LocalFileTime;
	if (FileTimeToLocalFileTime(&FindData->ftLastWriteTime, &LocalFileTime)) {
		if (FileTimeToSystemTime(&LocalFileTime, &time)) {
			retval = true;
		}
	}
	return retval;
}

#ifdef UNICODE
typedef TScanFile<wchar_t> ScanFile;
#else
typedef TScanFile<char> ScanFile;
#endif
#endif //SCANFILE_H
