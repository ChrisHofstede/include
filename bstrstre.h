#ifndef BSTRSTRE_H
#define BSTRSTRE_H
#include "butils.h"
#include "bstring.h"

namespace b {

// Defines

// Prototypes

template<class T> class TStrStream: public TNrToString<T> , public TString<T> {
	int tptr;
	bool bNoEOF;
	void pushback();
	void puts(const T* string);
	void put_char(const T kar);
	T get_char();
	int getinteger();
	unsigned getunsigned();
public:
	typedef enum {
		Error, EndOfFile, NoCrLf, CrLf
	} TResult;
	static const T crlf[];
	TStrStream();
	TStrStream(const T* string);
	TStrStream(const TString<T>& string);
	operator bool() const {
		return bNoEOF && TString<T>::operator bool();
	}
	int getpos() {
		return tptr;
	}
	void setpos(int pos);
	TStrStream& operator =(const T* string);
	TStrStream& operator =(const TString<T>& string);
	TStrStream& operator <<(const TString<T>& string);
	TStrStream& operator <<(const T* string);
	TStrStream& operator <<(int number);
	TStrStream& operator <<(unsigned number);
	TStrStream& operator <<(long number);
	TStrStream& operator <<(unsigned long number);
	TStrStream& operator <<(T kar);
	void gets(T* buffer, int cCount);
	TResult getline(T *buffer, int cCount) {
		TResult RetValue = Error;
		cCount--; // Last character is always a zero
		// Buffer size is including a terminating zero
		if (buffer && cCount > 0) {
			int l = TString<T>::length() - tptr;
			int i;
			cCount--;
			for (i = 0; i < cCount && i < l; i++) {
				buffer[i] = TString<T>::operator[](tptr++);
				if (buffer[i] == crlf[0] || buffer[i] == crlf[1])
					break;
			}
			bNoEOF = (i == cCount);
			buffer[i] = 0;
			if (i == cCount || i == l) // CrLf not found
			{
				if (bNoEOF) {
					RetValue = NoCrLf;
					tptr -= i;
				} else {
					RetValue = EndOfFile;
				}
			} else {
				RetValue = CrLf;
				skip(crlf);
			}
		}
		return RetValue;
	}
	void skip(const T* tokens);
	void skip();
	TStrStream& operator >>(T& kar);
	TStrStream& operator >>(int& integer);
	TStrStream& operator >>(unsigned& integer);
	TStrStream& operator >>(short& integer);
	bool operator ==(const T* string);
	bool operator ==(T kar);
	TStrStream& operator <<(TStrStream& (*f)(TStrStream&)) {
		return f(*this);
	}
};

template<class T> const T TStrStream<T>::crlf[] = { T('\r'), T('\n'), T('\0') };

template<class T> void TStrStream<T>::pushback() {
	bNoEOF = true;
	if (tptr > 0) {
		tptr--;
	}
}

template<class T> TStrStream<T>::TStrStream() {
	tptr = 0;
	bNoEOF = true;
}

template<class T> TStrStream<T>::TStrStream(const T* string) :
	TString<T> (string) {
	tptr = TString<T>::length();
	bNoEOF = true;
}

template<class T> TStrStream<T>::TStrStream(const TString<T>& string) :
	TString<T> (string) {
	tptr = TString<T>::length();
	bNoEOF = true;
}

template<class T> void TStrStream<T>::setpos(int pos) {
	bNoEOF = true;
	if (pos >= 0 && pos <= TString<T>::length()) {
		tptr = pos;
	}
}

template<class T> TStrStream<T>& TStrStream<T>::operator =(const T* string) {
	TString<T>::operator =(string);
	tptr = TString<T>::length();
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator =(
		const TString<T>& string) {
	TString<T>::operator =(string);
	tptr = TString<T>::length();
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator <<(
		const TString<T>& string) {
	puts(string.c_str());
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator <<(const T* string) {
	puts(string);
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator <<(int number) {
	puts(TStrStream<T>::itostr(number));
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator <<(unsigned number) {
	puts(TStrStream<T>::utostr(number));
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator <<(long number) {
	puts(TStrStream<T>::itostr(number));
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator <<(unsigned long number) {
	puts(TStrStream<T>::utostr(number));
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator <<(T kar) {
	put_char(kar);
	return *this;
}

template<class T> void TStrStream<T>::puts(const T* string) {
	if (string) {
		int l = TString<T>::length();
		if (tptr > l)
			tptr = l;
		int len = b::strlen<T>(string);
		if (tptr + len > l) {
			TString<T>::resize(tptr + len);
		}
		for (int i = 0; i < len; i++) {
			TString<T>::operator[](tptr++) = string[i];
		}
	}
}

template<class T> void TStrStream<T>::put_char(T kar) {
	int l = TString<T>::length();
	if (tptr > l)
		tptr = l;
	if (tptr + 1 > l) {
		TString<T>::resize(tptr + 1);
	}
	TString<T>::operator[](tptr++) = kar;
}

template<class T> void TStrStream<T>::gets(T* buffer, int cCount) {
	if (buffer) {
		int l = TString<T>::length() - tptr;
		int i;
		cCount--;
		for (i = 0; i < cCount && i < l; i++) {
			buffer[i] = TString<T>::operator[](tptr++);
		}
		bNoEOF = (i == cCount);
		buffer[i] = 0;
	}
}

template<class T> void TStrStream<T>::skip(const T* tokens) {
	T kar;
	do {
		kar = get_char();
	} while (b::strchr<T>(tokens, kar) && bNoEOF);
	if (bNoEOF)
		pushback();
}

template<class T> void TStrStream<T>::skip() {
	get_char();
}

template<class T> TStrStream<T>& TStrStream<T>::operator >>(T& kar) {
	kar = get_char();
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator >>(int& integer) {
	integer = getinteger();
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator >>(unsigned& integer) {
	integer = getunsigned();
	return *this;
}

template<class T> TStrStream<T>& TStrStream<T>::operator >>(short& integer) {
	integer = static_cast<short> (getinteger());
	return *this;
}

template<class T> T TStrStream<T>::get_char() {
	T kar;
	bNoEOF = tptr < TString<T>::length();
	if (bNoEOF)
		kar = TString<T>::operator[](tptr++);
	return kar;
}

template<class T> int TStrStream<T>::getinteger() {
	int number = 0;
	bool bNegative = false;
	T kar;

	do {
		kar = get_char();
	} while (ISSPACE(kar) && bNoEOF);
	if (bNoEOF) {
		if (kar == (T) '-') {
			bNegative = true;
			kar = get_char();
		} else if (kar == (T) '+')
			kar = get_char();
		while (ISDIGIT(kar) && bNoEOF) {
			number = (number * 10) + (kar - (T) '0');
			kar = get_char();
		}
		if (bNoEOF) {
			pushback();
		}
	}
	return (bNegative) ? -number : number;
}

template<class T> unsigned TStrStream<T>::getunsigned() {
	unsigned number = 0;
	T kar;

	do {
		kar = get_char();
	} while (ISSPACE(kar) && bNoEOF);
	if (bNoEOF) {
		if (kar == (T) '+')
			kar = get_char();
		while (ISDIGIT(kar) && bNoEOF) {
			number = (number * 10) + (kar - (T) '0');
			kar = get_char();
		}
		if (bNoEOF) {
			pushback();
		}
	}
	return number;
}

template<class T> bool TStrStream<T>::operator ==(const T* string) {
	bool bRetVal = true;
	int pos = getpos();
	const T* ptr = string;
	while (*ptr && bNoEOF) {
		if (*ptr++ != get_char()) {
			bRetVal = false;
			if (bNoEOF)
				setpos(pos);
			break;
		}
	}
	return (bNoEOF) ? bRetVal : false;
}

template<class T> bool TStrStream<T>::operator ==(T kar) {
	bool bRetVal = true;
	if (kar != get_char()) {
		bRetVal = false;
		if (bNoEOF)
			pushback();
	}
	return (bNoEOF) ? bRetVal : false;
}

template<class T> TStrStream<T>& endl(TStrStream<T>& s) {
	return s << TStrStream<T>::crlf;
}

template<class T> TStrStream<T>& tab(TStrStream<T>& s) {
	return s << T('\t');
}

template<class T> TStrStream<T>& clear(TStrStream<T>& s) {
	s.clear();
	return s;
}

template<class T> TStrStream<T>& operator <<(TStrStream<T>& s, const TManip<
		TStrStream<T> >& m) {
	return m.f(s, m.value);
}

template<class T> TStrStream<T>& operator <<(TStrStream<T>& s, const TManipInt<
		TStrStream<T> >& m) {
	return m.f(s, m.value);
}

#ifdef UNICODE
typedef TStrStream<wchar_t> StrStream;
#else
typedef TStrStream<char> StrStream;
#endif
typedef TStrStream<char> StrStreamA;
typedef TStrStream<wchar_t> StrStreamW;
} //End Namespace
#endif //BSTRSTRE_H
