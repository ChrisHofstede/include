#ifndef BROWSE_H
#define BROWSE_H
#include <shlobj.h>

template<class T> class TBrowseForFolder {
	LPMALLOC pMalloc;
	BROWSEINFO bi;
	int static WINAPI BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam,
			LPARAM lpData);
public:
	TBrowseForFolder() :
		pMalloc(0) {
		SHGetMalloc(&pMalloc);
	}
	~TBrowseForFolder() {
		if (pMalloc)
			pMalloc->Release();
	}
	bool Browse(HWND hParent, b::TString<T>& Folder);
};

template<class T> int WINAPI TBrowseForFolder<T>::BrowseCallbackProc(HWND hwnd,
		UINT uMsg, LPARAM lParam, LPARAM lpData) {
	switch (uMsg) {
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	case BFFM_SELCHANGED: {
		T* Path = new T[MAX_PATH];
		const T* ptr;
		if (!SHGetPathFromIDList((LPCITEMIDLIST) lParam, Path)) {
			const T NotValid[] = { 'N', 'o', 't', ' ', 'a', ' ', 'v', 'a', 'l',
					'i', 'd', ' ', 'f', 'o', 'l', 'd', 'e', 'r', '\0' };
			ptr = NotValid;
		} else {
			ptr = b::strrchr(Path, (T) ('\\'));
			if (!ptr || ptr[1] == 0) {
				ptr = Path;
			} else {
				ptr++;
			}
		}
		SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM) ptr);
		delete[] Path;
		break;
	}
	}
	return 0;
}

template<class T> bool TBrowseForFolder<T>::Browse(HWND hParent,
		b::TString<T>& Folder) {
	T* lpBuffer = 0;
	LPITEMIDLIST pidlDrives = 0; // PIDL for Programs folder
	LPITEMIDLIST pidlBrowse = 0; // PIDL selected by user
	bool RetVal = false;
	if (pMalloc) {
		try {
			// Allocate a buffer to receive browse information.
			if ((lpBuffer = (T*) pMalloc->Alloc(MAX_PATH)) != NULL) {
				// Get the PIDL for the Programs folder.
				if (SUCCEEDED(SHGetSpecialFolderLocation(hParent,
								CSIDL_DESKTOP, &pidlDrives))) {
					const T ChooseFolder[] = { 'C', 'h', 'o', 'o', 's', 'e',
							' ', 'a', ' ', 'f', 'o', 'l', 'd', 'e', 'r', '\0' };

					// Fill in the BROWSEINFO structure.
					bi.hwndOwner = hParent;
					bi.pidlRoot = pidlDrives;
					bi.pszDisplayName = lpBuffer;
					bi.lpszTitle = ChooseFolder;
					bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
					bi.lpfn = BrowseCallbackProc;
					bi.lParam = (LPARAM) Folder.c_str();

					// Browse for a folder and return its PIDL.
					pidlBrowse = SHBrowseForFolder(&bi);
					if (pidlBrowse != NULL) {
						if (SHGetPathFromIDList(pidlBrowse, lpBuffer)) {
							Folder = lpBuffer;
							RetVal = true;
						}
					}
				}
			}

			// Clean up.
			pMalloc->Free(pidlBrowse);
			pMalloc->Free(pidlDrives);
			pMalloc->Free(lpBuffer);
		} catch (...) {

			// Clean up.
			pMalloc->Free(pidlBrowse);
			pMalloc->Free(pidlDrives);
			pMalloc->Free(lpBuffer);
			throw;
		}
	}
	return RetVal;
}

#endif //BROWSE_H
