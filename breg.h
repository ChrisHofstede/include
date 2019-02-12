#ifndef BREG_H
#define BREG_H
#include "bstring.h"

namespace b {

class TKey {
protected:
	HKEY hKey;
public:
	TKey() :
			hKey((HKEY) 0) {
	}
	TKey(HKEY handle) :
			hKey(handle) {
	}
	TKey(const TKey& a) :
			hKey(a.hKey) {
	}
	~TKey() {
		Close();
	}
	void Close() {
		if (hKey != (HKEY) 0) {
			::RegCloseKey(hKey);
			hKey = (HKEY) 0;
		}
	}
	inline operator HKEY() const {
		return hKey;
	}
	inline operator HKEY&() {
		return hKey;
	}
	inline operator bool() const {
		return hKey != (HKEY) 0;
	}
};

template<class T> class TRegistry {
	const T* szKey;
	bool WriteKey(HKEY& hKey);
	bool ReadKey(HKEY& hKey);
	bool SetValue(HKEY hKey, LPCTSTR lpValueName, DWORD dwType,
	CONST BYTE *lpData, DWORD cbData);
	bool QueryValue(HKEY hKey, LPCTSTR lpValueName, LPBYTE lpData,
			LPDWORD lpcbData);
public:
	static const T separator[];
	TRegistry() :
			szKey(0) {
	}
	TRegistry(const T* key) {
		szKey = strnewdup(key);
	}
	TRegistry(const TRegistry& e) {
		szKey = strnewdup(e.szKey);
	}
	~TRegistry() {
		delete[] szKey;
	}
	TRegistry& operator =(const TRegistry& e) {
		delete[] szKey;
		szKey = strnewdup(e.szKey);
		return *this;
	}
	operator bool() {
		return szKey;
	}
	bool IsValue(const T* key);
	bool DeleteValue(const T* key);
	bool DeleteKey(const T* key);
	void SetLong(const T* key, DWORD value);
	DWORD GetLong(const T* key);
	void SetBool(const T* key, bool value);
	bool GetBool(const T* key);
	void SetString(const T* key, const T* value);
	void SetStringExpand(const T* key, const T* value);
	void SetString(const T* key, const TString<T>& value);
	void SetStringExpand(const T* key, const TString<T>& value);
	void GetString(const T* key, T** value);
	TString<T> GetString(const T* key);
	const T* GetKeyName() {
		return szKey;
	}
	TRegistry GetSubKey(const T* subkey);
};

template<class T> const T TRegistry<T>::separator[] = {
		T('\\'), T('\0') };

template<class T> bool TRegistry<T>::WriteKey(
		HKEY& hKey) {
	return RegCreateKeyEx(HKEY_CURRENT_USER, szKey, 0, 0, 0, KEY_WRITE, 0, &hKey, 0)
			== ERROR_SUCCESS;
}

template<class T> bool TRegistry<T>::ReadKey(
		HKEY& hKey) {
	return RegOpenKeyEx(HKEY_CURRENT_USER, szKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS;
}

template<class T> bool TRegistry<T>::SetValue(
		HKEY hKey, LPCTSTR lpValueName, DWORD dwType,
		CONST BYTE *lpData, DWORD cbData) {
	return RegSetValueEx(hKey, lpValueName, 0, dwType, lpData, cbData)
			== ERROR_SUCCESS;
}

template<class T> bool TRegistry<T>::QueryValue(
		HKEY hKey, LPCTSTR lpValueName, LPBYTE lpData, LPDWORD lpcbData) {
	return RegQueryValueEx(hKey, lpValueName, 0, 0, lpData, lpcbData)
			== ERROR_SUCCESS;
}

template<class T> bool TRegistry<T>::IsValue(
		const T* key) {
	HKEY hKey;
	bool RetVal = false;
	if (ReadKey(hKey)) {
		if (RegQueryValueEx(hKey, key, 0, 0, 0, 0) == ERROR_SUCCESS) {
			RetVal = true;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template<class T> bool TRegistry<T>::DeleteValue(
		const T* key) {
	HKEY hKey;
	bool RetVal = false;
	if (WriteKey(hKey)) {
		if (RegDeleteValue(hKey, key) == ERROR_SUCCESS) {
			RetVal = true;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template<class T> bool TRegistry<T>::DeleteKey(
		const T* key) {
	HKEY hKey;
	bool RetVal = false;
	if (WriteKey(hKey)) {
		if (RegDeleteKey(hKey, key) == ERROR_SUCCESS) {
			RetVal = true;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template<class T> void TRegistry<T>::SetLong(
		const T* key, DWORD value) {
	HKEY hKey;
	if (WriteKey(hKey)) {
		SetValue(hKey, key, REG_DWORD, (BYTE *) &value, sizeof(value));
		RegCloseKey(hKey);
	}
}

template<class T> DWORD TRegistry<T>::GetLong(
		const T* key) {
	HKEY hKey;
	DWORD RetVal = 0;
	if (ReadKey(hKey)) {
		DWORD dwVal;
		DWORD dwSize = sizeof(dwVal);
		if (QueryValue(hKey, key, (BYTE *) &dwVal, &dwSize)) {
			RetVal = dwVal;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template<class T> void TRegistry<T>::SetBool(
		const T* key, bool value) {
	HKEY hKey;
	if (WriteKey(hKey)) {
		SetValue(hKey, key, REG_BINARY, (BYTE *) &value, sizeof(value));
		RegCloseKey(hKey);
	}
}

template<class T> bool TRegistry<T>::GetBool(
		const T* key) {
	HKEY hKey;
	bool RetVal = false;
	if (ReadKey(hKey)) {
		bool dwVal;
		DWORD dwSize = sizeof(dwVal);
		if (QueryValue(hKey, key, (BYTE *) &dwVal, &dwSize)) {
			RetVal = dwVal;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template<class T> void TRegistry<T>::SetString(
		const T* key, const T* value) {
	HKEY hKey;
	if (value && WriteKey(hKey)) {
		SetValue(hKey, key, REG_SZ, (BYTE*) value,
				(b::strlen<T>(value) + 1) * sizeof(T));
		RegCloseKey(hKey);
	}
}

template<class T> void TRegistry<T>::SetStringExpand(
		const T* key, const T* value) {
	HKEY hKey;
	if (value && WriteKey(hKey)) {
		SetValue(hKey, key, REG_EXPAND_SZ, (BYTE*) value,
				(b::strlen<T>(value) + 1) * sizeof(T));
		RegCloseKey(hKey);
	}
}

template<class T> void TRegistry<T>::SetString(
		const T* key, const TString<T>& value) {
	HKEY hKey;
	if (value == true && WriteKey(hKey)) {
		SetValue(hKey, key, REG_SZ, (BYTE*) value.c_str(),
				(value.length() + 1) * sizeof(T));
		RegCloseKey(hKey);
	}
}

template<class T> void TRegistry<T>::SetStringExpand(
		const T* key, const TString<T>& value) {
	HKEY hKey;
	if (value == true && WriteKey(hKey)) {
		SetValue(hKey, key, REG_EXPAND_SZ, (BYTE*) value.c_str(),
				(value.length() + 1) * sizeof(T));
		RegCloseKey(hKey);
	}
}

template<class T> void TRegistry<T>::GetString(
		const T* key, T** value) {
	TKey hKey;
	*value = 0;
	if (ReadKey(hKey)) {
		TString<T> szString;
		DWORD dwSize;
		if (QueryValue(hKey, key, 0, &dwSize)) {
			if (QueryValue(hKey, key,
					(BYTE*) szString.get_buffer(dwSize / sizeof(T)), &dwSize)) {
				szString.fix_buffer();
				*value = strnewdup(szString);
			}
		}
	}
}

template<class T> TString<T> TRegistry<T>::GetString(
		const T* key) {
	TString<T> tmp;
	TKey hKey;
	if (ReadKey(hKey)) {
		TString<T> szString;
		DWORD dwSize;
		if (QueryValue(hKey, key, 0, &dwSize)) {
			if (QueryValue(hKey, key,
					(BYTE*) szString.get_buffer(dwSize / sizeof(T)), &dwSize)) {
				szString.fix_buffer();
				tmp = szString;
			}
		}
	}
	return tmp;
}

template<class T> TRegistry<T> TRegistry<T>::GetSubKey(
		const T* subkey) {
	TRegistry<T> registry;
	if (szKey && subkey) {
		T* szSubKey = new T[b::strlen(szKey) + b::strlen(subkey) + 2];
		if (szSubKey) {
			b::strcpy(szSubKey, szKey);
			b::strcat(szSubKey, separator);
			b::strcat(szSubKey, subkey);
			registry = TRegistry<T>(szSubKey);
			delete[] szSubKey;
		}
	}
	return registry;
}

template<class T> class TRegEnumValue {
	HKEY hKey;
	bool bKeyValid;
	T* szBuffer;
	DWORD dwBufferSize;
	DWORD dwValues;
	DWORD dwIndex;
public:
	TRegEnumValue() {
		hKey = 0;
		bKeyValid = false;
		szBuffer = 0;
		dwBufferSize = 0;
		dwValues = 0;
		dwIndex = 0;
	}
	~TRegEnumValue() {
		if (bKeyValid)
			RegCloseKey(hKey);
		delete[] szBuffer;
	}
	const T* First(const T* SubKey);
	const T* Next();
};

template<class T> const T* TRegEnumValue<T>::First(
		const T* SubKey) {
	T* RetVal = 0;
	dwIndex = 0;
	dwValues = 0;
	if (bKeyValid)
		RegCloseKey(hKey);
	bKeyValid = RegOpenKeyEx(HKEY_CURRENT_USER, SubKey, 0, KEY_READ,
			&hKey) == ERROR_SUCCESS;
	if (bKeyValid) {
		DWORD cbMaxValueNameLen;
		if (RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, &dwValues,
				&cbMaxValueNameLen, 0, 0, 0) == ERROR_SUCCESS) {
			delete[] szBuffer;
			dwBufferSize = ++cbMaxValueNameLen;
			szBuffer = new T[dwBufferSize];
			if (RegEnumValue(hKey, dwIndex, szBuffer, &cbMaxValueNameLen, 0, 0,
					0, 0) == ERROR_SUCCESS) {
				dwIndex++;
				RetVal = szBuffer;
			}
		}
	}
	return RetVal;
}

template<class T> const T* TRegEnumValue<T>::Next() {
	T* RetVal = 0;
	if (bKeyValid && dwIndex < dwValues) {
		DWORD cbMaxValueNameLen = dwBufferSize;
		if (RegEnumValue(hKey, dwIndex, szBuffer, &cbMaxValueNameLen, 0, 0, 0,
				0) == ERROR_SUCCESS) {
			dwIndex++;
			RetVal = szBuffer;
		}
	}
	return RetVal;
}

template<class T> class TRegEnumKey {
	HKEY hKey;
	bool bKeyValid;
	T* szBuffer;
	DWORD dwBufferSize;
	DWORD dwKeys;
	DWORD dwIndex;
public:
	TRegEnumKey() {
		hKey = 0;
		bKeyValid = false;
		szBuffer = 0;
		dwBufferSize = 0;
		dwKeys = 0;
		dwIndex = 0;
	}
	~TRegEnumKey() {
		if (bKeyValid)
			RegCloseKey(hKey);
		delete[] szBuffer;
	}
	const T* First(const T* SubKey);
	const T* Next();
};

template<class T> const T* TRegEnumKey<T>::First(
		const T* SubKey) {
	T* RetVal = 0;
	dwIndex = 0;
	dwKeys = 0;
	if (bKeyValid)
		RegCloseKey(hKey);
	bKeyValid = RegOpenKeyEx(HKEY_CURRENT_USER, SubKey, 0, KEY_READ,
			&hKey) == ERROR_SUCCESS;
	if (bKeyValid) {
		DWORD cbMaxKeyNameLen;
		if (RegQueryInfoKey(hKey, 0, 0, 0, &dwKeys, &cbMaxKeyNameLen, 0, 0, 0,
				0, 0, 0) == ERROR_SUCCESS) {
			delete[] szBuffer;
			dwBufferSize = ++cbMaxKeyNameLen;
			szBuffer = new T[dwBufferSize];
			//FILETIME ftLastWriteTime;
			if (RegEnumKeyEx(hKey, dwIndex, szBuffer, &cbMaxKeyNameLen, 0, 0, 0,
					0/*&ftLastWriteTime*/) == ERROR_SUCCESS) {
				dwIndex++;
				RetVal = szBuffer;
			}
		}
	}
	return RetVal;
}

template<class T> const T* TRegEnumKey<T>::Next() {
	T* RetVal = 0;
	if (bKeyValid && dwIndex < dwKeys) {
		DWORD cbMaxKeyNameLen = dwBufferSize;
//FILETIME ftLastWriteTime;
		if (RegEnumKeyEx(hKey, dwIndex, szBuffer, &cbMaxKeyNameLen, 0, 0, 0,
				0/*&ftLastWriteTime*/) == ERROR_SUCCESS) {
			dwIndex++;
			RetVal = szBuffer;
		}
	}
	return RetVal;
}

#ifdef UNICODE
typedef TRegistry<wchar_t> Registry;
#else
typedef TRegistry<char> Registry;
#endif

typedef TRegistry<char> RegistryA;
typedef TRegistry<wchar_t> RegistryW;

}
#endif // BREG_H

