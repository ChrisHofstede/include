#ifndef BSTRING_H
#define BSTRING_H
#include "bexcept.h"

namespace b {
// Defines

// Prototypes
enum {
	npos = -1
};

template<class T> class TString {
	struct srep {
		T *s; // pointer to data
		int n; // reference count
		int l; // length of string
		int bl; // length of buffer
		srep() :
			s(0), n(1), bl(0) {
		}
	};
	srep *p;
	int compare(srep* s1, srep* s2) const; // equivalent to strcmp
	int compare_ignore_case(srep* s1, srep* s2) const; // equivalent to stricmp
public:
	TString(); // string x;
	TString(const T* s); // string x = "abc"
	TString(const T* s, int len);
	TString(const TString &x); // string x = string ...
	~TString();
	void resize(int size);
	TString& operator=(const T* s);
	TString& operator=(const TString &x);
	TString operator+(const T c) const;
	TString operator+(const T* s) const;
	TString operator+(const TString& x) const;
	TString& operator+=(const T c);
	TString& operator+=(const T* s);
	TString& operator+=(const TString& x);
	T& operator[](int i);
	const T& operator[](int i) const;
	inline int compare(const TString &x) const {
		return compare(p, x.p);
	}
	int compare(const T* s);
	inline int compare_ignore_case(const TString &x) const {
		return compare_ignore_case(p, x.p);
	}
	int compare_ignore_case(const T* s) const;
	bool operator ==(const TString &x) {
		return compare(p, x.p) == 0;
	}
	bool operator !=(const TString &x) {
		return compare(p, x.p) != 0;
	}
	bool operator ==(const T* s);
	bool operator !=(const T* s);
	operator bool() const {
		return p->l > 0;
	}
	bool operator !() const {
		return p->l == 0;
	}
	operator const T*() const {
		return p->s;
	}
	void set_buffer(int size); // Sets a buffer to prevent series of allocations
	T* get_buffer(int size); // Dangerous but convenient
	void fix_buffer(); // Fix the damage
	void clear();
	const T* c_str() const {
		return p->s;
	}
	int length() const {
		return p->l;
	}
	int first_index(T kar) const;
	int last_index(T kar) const;
	bool starts_with(const TString &x) const;
	bool starts_with(const T* s) const;
	bool ends_with(const TString &x) const;
	bool ends_with(const T* s) const;
	TString sub_string(int begin, int end) const;
	TString sub_string(int begin) const {
		return sub_string(begin, p->l);
	}
	int to_int();
	TString tolower() const;
	TString trim() const;
};

template<class T> TString<T>::TString() // string x;
{
	p = new srep;
	p->l = 0;
}

template<class T> TString<T>::TString(const T* s) // string x = "abc"
{
	p = new srep;
	p->l = b::strlen<T>(s);
	p->s = new T[p->bl = p->l + 1];
	b::strcpy<T>(p->s, s);
}

template<class T> TString<T>::TString(const T* s, int len) {
	p = new srep;
	p->l = len;
	p->s = new T[p->bl = len + 1];
	b::copy<T>(p->s, s, len);
	p->s[len] = 0;
}

template<class T> TString<T>::TString(const TString &x) // string x = string ...
{
	x.p->n++;
	p = x.p;
}

template<class T> TString<T>::~TString() {
	if (--p->n == 0) {
		delete[] p->s;
		delete p;
	}
}

template<class T> TString<T>& TString<T>::operator=(const T* s) {
	int l = b::strlen<T>(s);
	if (p->n > 1) // disconnect self
	{
		p->n--;
		p = new srep;
	}
	if (l >= p->bl) {
		delete[] p->s; // free old string
		p->s = new T[p->bl = l + 1];
	}
	p->l = l;
	b::strcpy<T>(p->s, s);
	return *this;
}

template<class T> TString<T>& TString<T>::operator=(const TString &x) {
	x.p->n++; // protect against ''st = st''
	if (--p->n == 0) {
		delete[] p->s;
		delete p;
	}
	p = x.p;
	return *this;
}

template<class T> TString<T> TString<T>::operator+(const T c) const {
	return TString<T> (*this) += c;
}

template<class T> TString<T> TString<T>::operator+(const T* s) const {
	return TString<T> (*this) += s;
}

template<class T> TString<T> TString<T>::operator+(const TString& x) const {
	return TString<T> (*this) += x;
}

template<class T> TString<T>& TString<T>::operator+=(const T c) {
	int l = p->l;
	resize(l + 1);
	p->s[l] = c;
	return *this;
}

template<class T> TString<T>& TString<T>::operator+=(const T* s) {
	int l = p->l;
	resize(l + b::strlen<T>(s));
	b::strcpy<T>(&p->s[l], s);
	return *this;
}

template<class T> TString<T>& TString<T>::operator+=(const TString& x) {
	int l = p->l;
	resize(l + x.p->l);
	b::copy<T>(&p->s[l], x.p->s, x.p->l);
	return *this;
}

template<class T> T& TString<T>::operator[](int i) {
	if (i < 0 || p->l < i) {
		TIndexOutOfRange e;
		throw e;
	}
	if (p->n > 1) // clone to maintain value semantics
	{
		srep* np = new srep;
		np->l = p->l;
		np->s = new T[np->bl = p->l + 1];
		b::copy<T>(np->s, p->s, np->bl);
		p->n--;
		p = np;
	}
	return p->s[i];
}

template<class T> const T& TString<T>::operator[](int i) const {
	if (i < 0 || p->l < i) {
		TIndexOutOfRange e;
		throw e;
	}
	return p->s[i];
}

template<class T> int TString<T>::compare(const T* s) {
	srep n;
	n.l = b::strlen<T>(s);
	n.s = (T*) s;
	return compare(p, &n);
}

template<class T> int TString<T>::compare_ignore_case(const T* s) const {
	srep n;
	n.l = b::strlen<T>(s);
	n.s = (T*) s;
	return compare_ignore_case(p, &n);
}

template<class T> bool TString<T>::operator ==(const T* s) {
	srep n;
	n.l = b::strlen<T>(s);
	n.s = (T*) s;
	return compare(p, &n) == 0;
}

template<class T> bool TString<T>::operator !=(const T* s) {
	srep n;
	n.l = b::strlen<T>(s);
	n.s = (T*) s;
	return compare(p, &n) != 0;
}

template<class T> void TString<T>::set_buffer(int size) { // size is including terminating zero
	int l = p->l;
	resize(((size) ? size : p->bl) - 1); // resize automatically adds one extra
	if (l < p->l) {
		p->s[l] = 0; // repair resize settings
		p->l = l;
	}
}

template<class T> T* TString<T>::get_buffer(int size) { // size is including terminating zero
	resize(((size) ? size : p->bl) - 1); // resize automatically adds one extra
	return p->s;
}

template<class T> void TString<T>::fix_buffer() {
	int i = 0;
	int l = p->bl - 1;
	while (i < l && p->s[i])
		i++;
	p->l = i;
	p->s[i] = 0;
}

template<class T> void TString<T>::clear() {
	if (p->n > 1) // disconnect self
	{
		p->n--;
		p = new srep;
	}
	if (p->bl < 1) {
		delete[] p->s; // free old string
		p->s = new T[p->bl = 1];
	}
	p->l = 0;
	p->s[0] = 0; // empty string
}

template<class T> int TString<T>::first_index(T kar) const {
	int i = 0;
	while (i < p->l && p->s[i] != kar)
		i++;
	return (i < p->l) ? i : npos;
}

template<class T> int TString<T>::last_index(T kar) const {
	int i = p->l - 1;
	while (i >= 0 && p->s[i] != kar)
		i--;
	return (i >= 0) ? i : npos;
}

template<class T> bool TString<T>::starts_with(const TString &x) const {
	int i = x.p->l;
	if (i > 0 && i <= p->l) {
		T* ptr1 = p->s;
		T* ptr2 = x.p->s;
		while (i) {
			if (*ptr1 != *ptr2) {
				break;
			}
			i--;
			ptr1++;
			ptr2++;
		}
		return i == 0;
	}
	return false;
}

template<class T> bool TString<T>::starts_with(const T* s) const {
	int i = b::strlen<T>(s);
	if (i > 0 && i <= p->l) {
		T* ptr = p->s;
		while (i) {
			if (*ptr != s) {
				break;
			}
			i--;
			ptr++;
			s++;
		}
		return i == 0;
	}
	return false;
}

template<class T> bool TString<T>::ends_with(const TString &x) const {
	int i = x.p->l;
	if (i > 0 && i <= p->l) {
		T* ptr1 = p->s + p->l;
		T* ptr2 = x.p->s + i;
		while (i) {
			ptr1--;
			ptr2--;
			if (*ptr1 != *ptr2) {
				break;
			}
			i--;
		}
		return i == 0;
	}
	return false;
}

template<class T> bool TString<T>::ends_with(const T* s) const {
	int i = b::strlen<T>(s);
	if (i > 0 && i <= p->l) {
		T* ptr = p->s;
		s += i;
		while (i) {
			ptr--;
			s--;
			if (*ptr != s) {
				break;
			}
			i--;
		}
		return i == 0;
	}
	return false;
}

template<class T> TString<T> TString<T>::sub_string(int begin, int end) const {
	// end is last character + 1
	TString<T> tmp;
	if (begin >= 0 && end > begin && begin < p->l && end <= p->l) {
		int size = end - begin;
		tmp.resize(size);
		b::copy<T>(tmp.p->s, &p->s[begin], size);
	}
	return tmp;
}

template<class T> int TString<T>::to_int() {
	int number = 0;
	bool bNegative = false;
	int i = 0;
	while (i < p->l && ISSPACE(p->s[i]))
		i++;
	if (i < p->l) {
		if (p->s[i] == '-') {
			bNegative = true;
			i++;
		} else if (p->s[i] == '+')
			i++;
		while (i < p->l && ISDIGIT(p->s[i])) {
			number = (number * 10) + (p->s[i++] - '0');
		}
	}
	return (bNegative) ? -number : number;
}

template<class T> TString<T> TString<T>::tolower() const {
	TString<T> tmp;
	if (p->l > 0) {
		tmp.resize(p->l);
		int i;
		for (i = 0; i < p->l; i++) {
			tmp.p->s[i] = b::tolower<T>(p->s[i]);
		}
	}
	return tmp;
}

template<class T> TString<T> TString<T>::trim() const {
	TString<T> tmp;
	if (p->l > 0) {
		int first;
		for (first = 0; first < p->l; first++) {
			if (!ISSPACE(p->s[first])) {
				break;
			}
		}
		int last;
		for (last = p->l; last > first; last--) {
			if (!ISSPACE(p->s[last - 1])) {
				break;
			}
		}
		int l = last - first;
		tmp.resize(l);
		b::copy<T>(tmp.p->s, &p->s[first], l);
	}
	return tmp;
}

template<class T> void TString<T>::resize(int size) { // size is without terminating zero
	if (size >= p->bl || p->n > 1) // Resizes the buffer if a new is created
	{
		T* ptr = new T[size + 1];
		b::copy<T>(ptr, p->s, (size > p->l) ? p->l : size); // preserve content
		if (p->n > 1) // disconnect self
		{
			p->n--;
			p = new srep;
		} else {
			delete[] p->s; // free old string
		}
		p->bl = size + 1;
		p->s = ptr;
	}
	p->l = size;
	p->s[size] = 0; // put an ending zero
}

template<class T> int TString<T>::compare(srep* s1, srep* s2) const // equivalent to strcmp
{
	int i = s1->l;
	int j = s2->l;
	int iRet = i - j;
	T* ptr1 = s1->s;
	T* ptr2 = s2->s;
	while (i && j) {
		if (*ptr1 != *ptr2) {
			iRet = *ptr1 - *ptr2;
			break;
		}
		i--;
		j--;
		ptr1++;
		ptr2++;
	}
	return iRet;
}

template<class T> int TString<T>::compare_ignore_case(srep* s1, srep* s2) const // equivalent to stricmp
{
	int i = s1->l;
	int j = s2->l;
	int iRet = i - j;
	T* ptr1 = s1->s;
	T* ptr2 = s2->s;
	while (i && j) {
		T dkar, skar;
		if ((dkar = b::tolower<T>(*ptr1)) != (skar = b::tolower<T>(*ptr2))) {
			iRet = dkar - skar;
			break;
		}
		i--;
		j--;
		ptr1++;
		ptr2++;
	}
	return iRet;
}

template<class W, class A> TString<A> WideToAnsi(const TString<W>& Wide) {
	TString<A> Ansi;
	const W* s = Wide.c_str();
	int cChars = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, s, -1, 0, 0, 0,
			0);
	if (WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, s, -1, Ansi.get_buffer(
			cChars), cChars, 0, 0)) {
		Ansi.fix_buffer();
	} else {
		Ansi.clear();
	}
	return Ansi;
}

template<class A, class W> TString<W> AnsiToWide(const TString<A>& Ansi) {
	TString<W> Wide;
	const A* s = Ansi.c_str();
	int cChars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s, -1, 0, 0);
	if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s, -1, Wide.get_buffer(
			cChars), cChars)) {
		Wide.fix_buffer();
	} else {
		Wide.clear();
	}
	return Wide;
}

#ifdef UNICODE
typedef TString<wchar_t> String;
#else
typedef TString<char> String;
#endif
typedef TString<char> StringA;
typedef TString<wchar_t> StringW;

} // End Namespace
#endif //BSTRING_H
