// Registry.cpp
#include <windows.h>
#include <objbase.h>
#include "butils.h"
#include "registry.h"

// Set the given key and its value.
BOOL setKeyAndValue(const TCHAR* pszPath, const TCHAR* szSubkey,
		const TCHAR* szValue);

// Open a key and set a value.
BOOL setValueInKey(const TCHAR* szKey, const TCHAR* szNamedValue,
		const TCHAR* szValue);

// Convert a CLSID into a char string.
void CLSIDtochar(REFCLSID clsid, TCHAR* szCLSID, int length);

// Delete szKeyChild and all of its descendents.
LONG recursiveDeleteKey(HKEY hKeyParent, const TCHAR* szKeyChild);

// Size of a CLSID as a string
#define CLSID_STRING_SIZE 39

// String Constants
const TCHAR szcCLSID[] = TEXT("CLSID");
const TCHAR szcSlash[] = TEXT("\\");
const TCHAR szcInprocServer32[] = TEXT("InprocServer32");
const TCHAR szcThreadingModel[] = TEXT("ThreadingModel");
const TCHAR szcLocalServer32[] = TEXT("LocalServer32");
const TCHAR szcProgID[] = TEXT("ProgID");
const TCHAR szcVersionIndependentProgID[] = TEXT("VersionIndependentProgID");
const TCHAR szcCurVer[] = TEXT("CurVer");

// Register the component in the registry.
HRESULT RegisterServer(const TCHAR* szModuleName, // DLL module handle
		REFCLSID clsid, // Class ID
		const TCHAR* szFriendlyName, // Friendly Name
		const TCHAR* szVerIndProgID, // Programmatic
		const TCHAR* szProgID, // IDs
		const TCHAR* szThreadingModel) // ThreadingModel
{
	// Get server location.
	TCHAR szModule[MAX_PATH];
	HMODULE hModule = GetModuleHandle(szModuleName);
	if (!GetModuleFileName(hModule, szModule, sizeof(szModule) / sizeof(TCHAR)))
		return S_FALSE;

	// Convert the CLSID into a char.
	TCHAR szCLSID[CLSID_STRING_SIZE];
	CLSIDtochar(clsid, szCLSID, sizeof(szCLSID) / sizeof(TCHAR));

	// Build the key CLSID\\{...}
	TCHAR* szKey = new TCHAR[sizeof(szcCLSID) + sizeof(szcSlash) + b::strlen(
			szCLSID) - 1];
	b::strcpy(szKey, szcCLSID);
	b::strcat(szKey, szcSlash);
	b::strcat(szKey, szCLSID);

	// Add the CLSID to the registry.
	setKeyAndValue(szKey, NULL, szFriendlyName);

	// Add the server filename subkey under the CLSID key.
	if (b::strstr(szModuleName, TEXT(".exe")) == NULL) {
		setKeyAndValue(szKey, szcInprocServer32, szModule);
		TCHAR* szInproc = new TCHAR[b::strlen(szKey) + sizeof(szcSlash)
				+ sizeof(szcInprocServer32) - 1];
		b::strcpy(szInproc, szKey);
		b::strcat(szInproc, szcSlash);
		b::strcat(szInproc, szcInprocServer32);
		setValueInKey(szInproc, szcThreadingModel, szThreadingModel);
		delete[] szInproc;
	} else
		setKeyAndValue(szKey, szcLocalServer32, szModule);

	// Add the ProgID subkey under the CLSID key.
	setKeyAndValue(szKey, szcProgID, szProgID);

	// Add the version-independent ProgID subkey under CLSID key.
	setKeyAndValue(szKey, szcVersionIndependentProgID, szVerIndProgID);

	// Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT.
	setKeyAndValue(szVerIndProgID, NULL, szFriendlyName);
	setKeyAndValue(szVerIndProgID, szcCLSID, szCLSID);
	setKeyAndValue(szVerIndProgID, szcCurVer, szProgID);

	// Add the versioned ProgID subkey under HKEY_CLASSES_ROOT.
	setKeyAndValue(szProgID, NULL, szFriendlyName);
	setKeyAndValue(szProgID, szcCLSID, szCLSID);

	delete[] szKey;
	return S_OK;
}

// Remove the component from the registry.
LONG UnregisterServer(REFCLSID clsid, // Class ID
		const TCHAR* szVerIndProgID, // Programmatic
		const TCHAR* szProgID) // IDs
{
	LONG lRet = S_FALSE;
	// Convert the CLSID into a char.
	TCHAR szCLSID[CLSID_STRING_SIZE];
	CLSIDtochar(clsid, szCLSID, sizeof(szCLSID));

	// Build the key CLSID\\{...}
	TCHAR* szKey = new TCHAR[sizeof(szcCLSID) + sizeof(szcSlash) + b::strlen(
			szCLSID) - 1];
	b::strcpy(szKey, szcCLSID);
	b::strcat(szKey, szcSlash);
	b::strcat(szKey, szCLSID);

	// Delete the CLSID Key - CLSID\{...}
	LONG lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szKey);
	if ((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND)) // Subkey may not exist.
	{
		// Delete the version-independent ProgID Key.
		lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szVerIndProgID);
		if ((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND)) // Subkey may not exist.
		{
			// Delete the ProgID key.
			lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szProgID);
			if ((lResult == ERROR_SUCCESS) || (lResult == ERROR_FILE_NOT_FOUND)) // Subkey may not exist.
				lRet = S_OK;
		}
	}
	delete[] szKey;
	return lRet;
}

// Convert a CLSID to a char string.
void CLSIDtochar(REFCLSID clsid, TCHAR* szCLSID, int length) {
	if (length < CLSID_STRING_SIZE)
		return;
	// Get CLSID
	LPOLESTR wszCLSID = NULL;
	if (FAILED(::StringFromCLSID(clsid, &wszCLSID)))
		return;

#ifdef UNICODE
	b::strncpy(szCLSID, wszCLSID, length);
#else
	// Convert from wide characters to non-wide.
	//wcstombs(szCLSID, wszCLSID, length);
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, wszCLSID, -1, szCLSID,
			length, 0, 0);
#endif

	// Free memory.
	::CoTaskMemFree(wszCLSID);
}

// Delete a key and all of its descendents.
LONG recursiveDeleteKey(HKEY hKeyParent, // Parent of key to delete
		const TCHAR* lpszKeyChild) // Key to delete
{
	// Open the child.
	HKEY hKeyChild;
	LONG lRes = RegOpenKeyEx(hKeyParent, lpszKeyChild, 0, KEY_ALL_ACCESS,
			&hKeyChild);
	if (lRes != ERROR_SUCCESS)
		return lRes;

	// Enumerate all of the decendents of this child.
	FILETIME time;
	TCHAR szBuffer[MAX_PATH];
	DWORD dwSize = sizeof(szBuffer);
	while (RegEnumKeyEx(hKeyChild, 0, szBuffer, &dwSize, NULL, NULL, NULL,
			&time) == ERROR_SUCCESS) {
		// Delete the decendents of this child.
		lRes = recursiveDeleteKey(hKeyChild, szBuffer);
		if (lRes != ERROR_SUCCESS) {
			// Cleanup before exiting.
			RegCloseKey(hKeyChild);
			return lRes;
		}
		dwSize = sizeof(szBuffer);
	}

	// Close the child.
	RegCloseKey(hKeyChild);

	// Delete this child.
	return RegDeleteKey(hKeyParent, lpszKeyChild);
}

// Create a key and set its value.
BOOL setKeyAndValue(const TCHAR* szKey, const TCHAR* szSubkey,
		const TCHAR* szValue) {
	HKEY hKey;
	BOOL bRet = FALSE;
	TCHAR* szKeyBuf = new TCHAR[b::strlen(szKey) + sizeof(szcSlash)
			+ ((szSubkey) ? b::strlen(szSubkey) : 0)];

	// Copy keyname into buffer.
	b::strcpy(szKeyBuf, szKey);

	// Add subkey name to buffer.
	if (szSubkey) {
		b::strcat(szKeyBuf, szcSlash);
		b::strcat(szKeyBuf, szSubkey);
	}

	// Create and open key and subkey.
	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, szKeyBuf, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL)
			== ERROR_SUCCESS) {
		// Set the Value.
		if (szValue != NULL)
			RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE *) szValue, (b::strlen(
					szValue) + 1) * sizeof(TCHAR));

		RegCloseKey(hKey);
		bRet = TRUE;
	}
	delete[] szKeyBuf;
	return bRet;
}

// Open a key and set a value.
BOOL setValueInKey(const TCHAR* szKey, const TCHAR* szNamedValue,
		const TCHAR* szValue) {
	HKEY hKey;
	BOOL bRet = FALSE;
	// Open key and subkey.
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szKey, 0, KEY_SET_VALUE, &hKey)
			== ERROR_SUCCESS) {
		// Set the Value.
		if (szValue != NULL)
			RegSetValueEx(hKey, szNamedValue, 0, REG_SZ, (BYTE*) szValue,
					(b::strlen(szValue) + 1) * sizeof(TCHAR));

		RegCloseKey(hKey);
		bRet = TRUE;
	}
	return bRet;
}
