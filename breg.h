#ifndef BREG_H
#define BREG_H

namespace b {

template <class T, HKEY hRootKey /*= HKEY_CURRENT_USER*/> class TRegistry
{
	const T* szKey;
	bool WriteKey(HKEY& hKey);
	bool ReadKey(HKEY& hKey);
	bool SetValue(HKEY hKey, LPCTSTR lpValueName, DWORD dwType,
					  CONST BYTE *lpData, DWORD cbData);
	bool QueryValue(HKEY hKey, LPCTSTR lpValueName, LPBYTE lpData, LPDWORD lpcbData);
public:
	static const T separator[];
	TRegistry() : szKey(0) {}
	TRegistry(const T* key)
	{
		szKey = strnewdup(key);
	}
	TRegistry(const TRegistry& e)
	{
		szKey = strnewdup(e.szKey);
	}
	~TRegistry()
	{
		delete[] szKey;
	}
	TRegistry& operator = (const TRegistry& e)
	{
		delete[] szKey;
		szKey = strnewdup(e.szKey);
		return *this;
	}
	operator bool ()
	{
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
	const T* GetKeyName()
	{
		return szKey;
	}
	TRegistry GetSubKey(const T* subkey);
};

template <class T, HKEY hRootKey> const T TRegistry<T, hRootKey>::separator[] = { T('\\'), T('\0') };

template <class T, HKEY hRootKey> bool TRegistry<T, hRootKey>::WriteKey(HKEY& hKey)
{
	return RegCreateKeyEx(hRootKey, szKey, 0, 0, 0, KEY_WRITE, 0, &hKey, 0) == ERROR_SUCCESS;
}

template <class T, HKEY hRootKey> bool TRegistry<T, hRootKey>::ReadKey(HKEY& hKey)
{
	return RegOpenKeyEx(hRootKey, szKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS;
}

template <class T, HKEY hRootKey> bool TRegistry<T, hRootKey>::SetValue(HKEY hKey,
																		LPCTSTR lpValueName,
																		DWORD dwType,
																		CONST BYTE *lpData,
																		DWORD cbData)
{
	return RegSetValueEx(hKey, lpValueName, 0, dwType, lpData, cbData) == ERROR_SUCCESS;
}

template <class T, HKEY hRootKey> bool TRegistry<T, hRootKey>::QueryValue(HKEY hKey,
																		  LPCTSTR lpValueName,
																		  LPBYTE lpData,
																		  LPDWORD lpcbData)
{
	return RegQueryValueEx(hKey, lpValueName, 0, 0, lpData, lpcbData) == ERROR_SUCCESS;
}

template <class T, HKEY hRootKey> bool TRegistry<T, hRootKey>::IsValue(const T* key)
{
	HKEY hKey;
	bool RetVal = false;
	if(ReadKey(hKey))
	{
		if(RegQueryValueEx(hKey, key, 0, 0, 0, 0) == ERROR_SUCCESS)
		{
			RetVal = true;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template <class T, HKEY hRootKey> bool TRegistry<T, hRootKey>::DeleteValue(const T* key)
{
	HKEY hKey;
	bool RetVal = false;
	if(WriteKey(hKey))
	{
		if(RegDeleteValue(hKey, key) == ERROR_SUCCESS)
		{
			RetVal = true;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template <class T, HKEY hRootKey> bool TRegistry<T, hRootKey>::DeleteKey(const T* key)
{
	HKEY hKey;
	bool RetVal = false;
	if(WriteKey(hKey))
	{
		if(RegDeleteKey(hKey, key) == ERROR_SUCCESS)
		{
			RetVal = true;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template <class T, HKEY hRootKey> void TRegistry<T, hRootKey>::SetLong(const T* key, DWORD value)
{
	HKEY hKey;
	if(WriteKey(hKey))
	{
		SetValue(hKey, key, REG_DWORD, (BYTE *)&value, sizeof(value));
		RegCloseKey(hKey);
	}
}

template <class T, HKEY hRootKey> DWORD TRegistry<T, hRootKey>::GetLong(const T* key)
{
	HKEY hKey;
	DWORD RetVal = 0;
	if(ReadKey(hKey))
	{
		DWORD dwVal;
		DWORD dwSize = sizeof(dwVal);
		if(QueryValue(hKey, key, (BYTE *)&dwVal, &dwSize))
		{
			RetVal = dwVal;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template <class T, HKEY hRootKey> void TRegistry<T, hRootKey>::SetBool(const T* key, bool value)
{
	HKEY hKey;
	if(WriteKey(hKey))
	{
		SetValue(hKey, key, REG_BINARY, (BYTE *)&value, sizeof(value));
		RegCloseKey(hKey);
	}
}

template <class T, HKEY hRootKey> bool TRegistry<T, hRootKey>::GetBool(const T* key)
{
	HKEY hKey;
	bool RetVal = false;
	if(ReadKey(hKey))
	{
		bool dwVal;
		DWORD dwSize = sizeof(dwVal);
		if(QueryValue(hKey, key, (BYTE *)&dwVal, &dwSize))
		{
			RetVal = dwVal;
		}
		RegCloseKey(hKey);
	}
	return RetVal;
}

template <class T, HKEY hRootKey> void TRegistry<T, hRootKey>::SetString(const T* key, const T* value)
{
	HKEY hKey;
	if(value && WriteKey(hKey))
	{
		SetValue(hKey, key, REG_SZ, (BYTE*)value, (b::strlen<T>(value) + 1) * sizeof(T));
		RegCloseKey(hKey);
	}
}

template <class T, HKEY hRootKey> void TRegistry<T, hRootKey>::SetStringExpand(const T* key, const T* value)
{
	HKEY hKey;
	if(value && WriteKey(hKey))
	{
		SetValue(hKey, key, REG_EXPAND_SZ, (BYTE*)value, (b::strlen<T>(value) + 1) * sizeof(T));
		RegCloseKey(hKey);
	}
}

template <class T, HKEY hRootKey> void TRegistry<T, hRootKey>::SetString(const T* key, const TString<T>& value)
{
	HKEY hKey;
	if(value == true && WriteKey(hKey))
	{
		SetValue(hKey, key, REG_SZ, (BYTE*)value.c_str(), (value.length() + 1) * sizeof(T));
		RegCloseKey(hKey);
	}
}

template <class T, HKEY hRootKey> void TRegistry<T, hRootKey>::SetStringExpand(const T* key, const TString<T>& value)
{
	HKEY hKey;
	if(value == true && WriteKey(hKey))
	{
		SetValue(hKey, key, REG_EXPAND_SZ, (BYTE*)value.c_str(), (value.length() + 1) * sizeof(T));
		RegCloseKey(hKey);
	}
}

template <class T, HKEY hRootKey> void TRegistry<T, hRootKey>::GetString(const T* key, T** value)
{
	HKEY hKey;
	*value = 0;
	if(ReadKey(hKey))
	{
		T* szString = 0;
		try
		{
			DWORD dwSize;
			if(QueryValue(hKey, key, 0, &dwSize))
			{
				szString = new T[dwSize / sizeof(T)];
				if(QueryValue(hKey, key, (BYTE*)szString, &dwSize))
				{
					*value = szString;
					szString = 0;
				}
			}
		}
		__finally
		{
			delete[] szString;
			RegCloseKey(hKey);
		}
	}
}

template <class T, HKEY hRootKey> TString<T> TRegistry<T, hRootKey>::GetString(const T* key)
{
	TString<T> tmp;
	HKEY hKey;
	if(ReadKey(hKey))
	{
		T* szString = 0;
		try
		{
			DWORD dwSize;
			if(QueryValue(hKey, key, 0, &dwSize))
			{
				szString = new T[dwSize / sizeof(T)];
				if(QueryValue(hKey, key, (BYTE*)szString, &dwSize))
				{
					tmp = szString;
				}
			}
		}
		__finally
		{
			delete[] szString;
			RegCloseKey(hKey);
		}
	}
	return tmp;
}

template <class T, HKEY hRootKey> TRegistry<T, hRootKey> TRegistry<T, hRootKey>::GetSubKey(const T* subkey)
{
	TRegistry<T, hRootKey> registry;
	if(szKey && subkey)
	{
		T* szSubKey = new T[b::strlen(szKey) + b::strlen(subkey) + 2];
		if(szSubKey)
		{
			b::strcpy(szSubKey, szKey);
			b::strcat(szSubKey, separator);
			b::strcat(szSubKey, subkey);
			registry = TRegistry<T, hRootKey>(szSubKey);
			delete[] szSubKey;
		}
	}
	return registry;
}

template <class T, HKEY hRootKey /*= HKEY_CURRENT_USER*/> class TRegEnumValue
{
	HKEY hKey;
	bool bKeyValid;
	T* szBuffer;
	DWORD dwBufferSize;
	DWORD dwValues;
	DWORD dwIndex;
public:
	TRegEnumValue()
	{
		bKeyValid = false;
		szBuffer = 0;
		dwBufferSize = 0;
		dwValues = 0;
		dwIndex = 0;
	}
	~TRegEnumValue()
	{
		if(bKeyValid)
			RegCloseKey(hKey);
		delete[] szBuffer;
	}
	const T* First(const T* SubKey);
	const T* Next();
};

template <class T, HKEY hRootKey> const T* TRegEnumValue<T, hRootKey>::First(const T* SubKey)
{
	T* RetVal = 0;
	dwIndex = 0;
	dwValues = 0;
	if(bKeyValid)
		RegCloseKey(hKey);
	bKeyValid = RegOpenKeyEx(hRootKey, SubKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS;
	if(bKeyValid)
	{
		DWORD cbMaxValueNameLen;
		if(RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, &dwValues, &cbMaxValueNameLen,	0,	0,	0) == ERROR_SUCCESS)
		{
			delete[] szBuffer;
			dwBufferSize = ++cbMaxValueNameLen;
			szBuffer = new T[dwBufferSize];
			if(RegEnumValue(hKey, dwIndex, szBuffer, &cbMaxValueNameLen, 0, 0, 0, 0) == ERROR_SUCCESS)
			{
				dwIndex++;
				RetVal = szBuffer;
			}
		}
	}
	return RetVal;
}

template <class T, HKEY hRootKey> const T* TRegEnumValue<T, hRootKey>::Next()
{
	T* RetVal = 0;
	if(bKeyValid && dwIndex < dwValues)
	{
		DWORD cbMaxValueNameLen = dwBufferSize;
		if(RegEnumValue(hKey, dwIndex, szBuffer, &cbMaxValueNameLen, 0, 0, 0, 0) == ERROR_SUCCESS)
		{
			dwIndex++;
			RetVal = szBuffer;
		}
	}
	return RetVal;
}

template <class T, HKEY hRootKey /*= HKEY_CURRENT_USER*/> class TRegEnumKey
{
	HKEY hKey;
	bool bKeyValid;
	T* szBuffer;
	DWORD dwBufferSize;
	DWORD dwKeys;
	DWORD dwIndex;
public:
	TRegEnumKey()
	{
		bKeyValid = false;
		szBuffer = 0;
		dwBufferSize = 0;
		dwKeys = 0;
		dwIndex = 0;
	}
	~TRegEnumKey()
	{
		if(bKeyValid)
			RegCloseKey(hKey);
		delete[] szBuffer;
	}
	const T* First(const T* SubKey);
	const T* Next();
};

template <class T, HKEY hRootKey> const T* TRegEnumKey<T, hRootKey>::First(const T* SubKey)
{
	T* RetVal = 0;
	dwIndex = 0;
	dwKeys = 0;
	if(bKeyValid)
		RegCloseKey(hKey);
	bKeyValid = RegOpenKeyEx(hRootKey, SubKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS;
	if(bKeyValid)
	{
		DWORD cbMaxKeyNameLen;
		if(RegQueryInfoKey(hKey, 0, 0, 0, &dwKeys, &cbMaxKeyNameLen, 0, 0, 0, 0, 0, 0) == ERROR_SUCCESS)
		{
			delete[] szBuffer;
			dwBufferSize = ++cbMaxKeyNameLen;
			szBuffer = new T[dwBufferSize];
			//FILETIME ftLastWriteTime;
			if(RegEnumKeyEx(hKey, dwIndex, szBuffer, &cbMaxKeyNameLen, 0, 0, 0, 0/*&ftLastWriteTime*/) == ERROR_SUCCESS)
			{
				dwIndex++;
				RetVal = szBuffer;
			}
		}
	}
	return RetVal;
}

template <class T, HKEY hRootKey> const T* TRegEnumKey<T, hRootKey>::Next()
{
	T* RetVal = 0;
	if(bKeyValid && dwIndex < dwKeys)
	{
		DWORD cbMaxKeyNameLen = dwBufferSize;
		//FILETIME ftLastWriteTime;
		if(RegEnumKeyEx(hKey, dwIndex, szBuffer, &cbMaxKeyNameLen, 0, 0, 0, 0/*&ftLastWriteTime*/) == ERROR_SUCCESS)
		{
			dwIndex++;
			RetVal = szBuffer;
		}
	}
	return RetVal;
}


#ifdef UNICODE
typedef TRegistry<wchar_t, HKEY_CURRENT_USER> Registry;
#else
typedef TRegistry<char, HKEY_CURRENT_USER> Registry;
#endif

typedef TRegistry<char, HKEY_CURRENT_USER> RegistryA;
typedef TRegistry<wchar_t, HKEY_CURRENT_USER> RegistryW;

}
#endif // BREG_H

