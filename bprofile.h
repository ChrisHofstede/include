#ifndef BPROFILE_H
#define BPROFILE_H
#include "bstring.h"

#define DOTINI       TEXT(".ini")
#define BUF_INC   40

template<class T> class TProfile {
	T* FileName;
	const static T* Empty;
public:
	TProfile(const T* filename) {
		const T* ptr = b::strrchr<T>(filename, (T) ('.'));
		int len = (ptr) ? ptr - filename : b::strlen<T>(filename);
		FileName = new T[len + sizeof(DOTINI) / sizeof(T)];
		b::strncpy(FileName, filename, len);
		b::strcpy(FileName + len, DOTINI);
	}
	~TProfile() {
		delete[] FileName;
	}
	bool WriteProfileString(const T* Section, const T* Key, const T* String) const {
		return ::WritePrivateProfileString(Section, // address of section name
				Key, // address of key name
				String, // address of string to add
				FileName); // address of initialization filename
	}
	bool WriteProfileString(const T* Section, const T* Key,
			const b::TString<T>& Value) const {
		return ::WritePrivateProfileString(Section, // address of section name
				Key, // address of key name
				Value.c_str(), // address of string to add
				FileName); // address of initialization filename
	}
	void GetProfileString(const T* Section, const T* Key, const T* Default,
			T** string) const;
	b::TString<T> GetProfileString(const T* Section, const T* Key,
			const T* Default) const;
	int GetProfileInt(const T* Section, const T* Key, int Default) const {
		return ::GetPrivateProfileInt(Section, // address of section name
				Key, // address of key name
				Default, // return value if key name is not found
				FileName); // address of initialization filename
	}
	bool WriteProfileInt(const T* Section, const T* Key, int Value) const {
		b::TNrToString<T> NrToString;
		return WriteProfileString(Section, Key, NrToString.itostr(Value));
	}
	void GetKeyNames(const T* Section, T** string) const {
		GetProfileString(Section, 0, Empty, string);
	}
	void GetSectionNames(T** string) const {
		GetProfileString(0, 0, Empty, string);
	}
	bool DeleteKey(const T* Section, const T* Key) const {
		return WriteProfileString(Section, Key, 0);
	}
	bool DeleteSection(const T* Section) const {
		return WriteProfileString(Section, 0, 0);
	}
	const T* GetFileName() const {
		return FileName;
	}
};

template<class T> const T* TProfile<T>::Empty = TEXT("");

template<class T> void TProfile<T>::GetProfileString(const T* Section,
		const T* Key, const T* Default, T** string) const {
	T* buf = 0;
	try {
		int buf_size = 0;
		long ret;
		do {
			buf_size += BUF_INC;
			delete[] buf;
			buf = new T[buf_size];
			ret = ::GetPrivateProfileString(Section, // points to section name
					Key, // points to key name
					Default, // points to default string
					buf, // points to destination buffer
					(DWORD) buf_size, // size of destination buffer
					FileName); // points to initialization filename
		} while ((ret == buf_size - 1) || (ret == buf_size - 2));
		*string = new T[ret + 1];
		b::copy(*string, buf, ret);
		(*string)[ret] = 0;
		delete[] buf;
	} catch (...) {
		delete[] buf;
		throw;
	}
}

template<class T> b::TString<T> TProfile<T>::GetProfileString(const T* Section,
		const T* Key, const T* Default) const {
	b::TString<T> tmp;
	T* buf = 0;
	try {
		int buf_size = 0;
		long ret;
		do {
			buf_size += BUF_INC;
			delete[] buf;
			buf = new T[buf_size];
			ret = ::GetPrivateProfileString(Section, // points to section name
					Key, // points to key name
					Default, // points to default string
					buf, // points to destination buffer
					(DWORD) buf_size, // size of destination buffer
					FileName); // points to initialization filename
		} while ((ret == buf_size - 1) || (ret == buf_size - 2));
		tmp = buf;
		delete[] buf;
	} catch (...) {
		delete[] buf;
		throw;
	}
	return tmp;
}
#endif //BPROFILE_H
