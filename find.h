#ifndef FIND_H
#define FIND_H
#include <windows.h>

template<class T> class TFind {
	HANDLE hFind;
	bool IsDotDirectory();
protected:
	WIN32_FIND_DATA* FindData;
public:
	enum TFindResult {
		Invalid = 0, IsDirectory, IsFile
	};
	TFind();
	~TFind();
	TFindResult FindFirst(const T* pattern) {
		TFindResult Result = Invalid;
		if (hFind != INVALID_HANDLE_VALUE) {
			::FindClose(hFind);
		}
		if ((hFind = ::FindFirstFile(pattern, FindData)) != INVALID_HANDLE_VALUE) {
			while (IsDotDirectory()) {
				if (!::FindNextFile(hFind, FindData)) {
					goto stop;
				}
			}
			Result
					= (FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? IsDirectory
							: IsFile;
		}
		stop: return Result;
	}
	TFindResult FindNext() {
		TFindResult Result = Invalid;
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!::FindNextFile(hFind, FindData)) {
					goto stop;
				}
			} while (IsDotDirectory());
			Result
					= (FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? IsDirectory
							: IsFile;
		}
		stop: return Result;
	}
	FILETIME* LastWriteTime() {
		return &FindData->ftLastWriteTime;
	}
	const T* GetFileName() {
		return FindData->cFileName;
	}
};

template<class T> TFind<T>::TFind() {
	hFind = INVALID_HANDLE_VALUE;
	FindData = new WIN32_FIND_DATA;
}

template<class T> TFind<T>::~TFind() {
	if (hFind != INVALID_HANDLE_VALUE) {
		::FindClose(hFind);
	}
	delete FindData;
}

template<class T> bool TFind<T>::IsDotDirectory() {
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

#ifdef UNICODE
typedef TFind<wchar_t> Find;
#else
typedef TFind<char> Find;
#endif
#endif //FIND_H
