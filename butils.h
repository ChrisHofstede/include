#ifndef BUTILS_H
#define BUTILS_H
#include <new>
#include <shlwapi.h>

// Defines
#define MAXNRSPACE    12
// White space is:
// 0x20 SP
// 0x09 HT \t
// 0x0A LF \n
// 0x0B VT
// 0x0C FF
// 0x0D CR \r
#define ISSPACE(a)		((a) == ' ' || ((a) >= '\t' && (a) <= '\r'))
#define ISDIGIT(a)		((a) >= '0' && (a) <= '9')
#define ISLEADBYTE(a)	(((a) & 0xC0) == 0xC0)
#define ISTRAILBYTE(a)	(((a) & 0xC0) == 0x80)
#define MIN(a, b)		((a) < (b)) ? (a) : (b)
#define MAX(a, b)		((a) > (b)) ? (a) : (b)

// Profiles

namespace b {

// Function templates
template<class T> T tolower(T kar) {
	if (kar >= (T) 'A' && kar <= (T) 'Z') {
		kar = (T) ((T) 'a' + (kar - (T) 'A'));
	} else {
		if ((kar < (T) 'a' && kar > (T) 'z')
				&& (kar < (T) '0' && kar > (T) '9')) {
			switch (kar) {
			case (T) ('Á'):
				kar = (T) ('á');
				break;
			case (T) ('À'):
				kar = (T) ('à');
				break;
			case (T) ('Â'):
				kar = (T) ('â');
				break;
			case (T) ('Ä'):
				kar = (T) ('ä');
				break;
			case (T) ('Ã'):
				kar = (T) ('ã');
				break;
			case (T) ('Å'):
				kar = (T) ('å');
				break;
			case (T) ('Æ'):
				kar = (T) ('æ');
				break;
			case (T) ('Ç'):
				kar = (T) ('ç');
				break;
			case (T) ('É'):
				kar = (T) ('é');
				break;
			case (T) ('È'):
				kar = (T) ('è');
				break;
			case (T) ('Ê'):
				kar = (T) ('ê');
				break;
			case (T) ('Ë'):
				kar = (T) ('ë');
				break;
			case (T) ('Í'):
				kar = (T) ('í');
				break;
			case (T) ('Ì'):
				kar = (T) ('ì');
				break;
			case (T) ('Î'):
				kar = (T) ('î');
				break;
			case (T) ('Ï'):
				kar = (T) ('ï');
				break;
			case (T) ('Ñ'):
				kar = (T) ('ñ');
				break;
			case (T) ('Ó'):
				kar = (T) ('ó');
				break;
			case (T) ('Ò'):
				kar = (T) ('ò');
				break;
			case (T) ('Ô'):
				kar = (T) ('ô');
				break;
			case (T) ('Ö'):
				kar = (T) ('ö');
				break;
			case (T) ('Õ'):
				kar = (T) ('õ');
				break;
			case (T) ('Ø'):
				kar = (T) ('ø');
				break;
			case (T) ('Œ'):
				kar = (T) ('œ');
				break;
			case (T) ('Š'):
				kar = (T) ('š');
				break;
			case (T) ('þ'):
				kar = (T) ('Þ');
				break;
			case (T) ('Ú'):
				kar = (T) ('ú');
				break;
			case (T) ('Ù'):
				kar = (T) ('ù');
				break;
			case (T) ('Û'):
				kar = (T) ('û');
				break;
			case (T) ('Ü'):
				kar = (T) ('ü');
				break;
			case (T) ('Ý'):
				kar = (T) ('ý');
				break;
			case (T) ('Ÿ'):
				kar = (T) ('ÿ');
				break;
			case (T) ('Ž'):
				kar = (T) ('ž');
				break;
			}
		}
	}
	return kar;
}

template<class T> T* tolower(T* string) {
	T* ptr = string;
	while (*ptr) {
		*ptr = tolower<T> (*ptr);
		ptr++;
	}
	return string;
}

template<class T> void strcpy(T* dest, const T* src) {
	while ((*dest++ = *src++) != 0)
		;
}

template<class T> void strncpy(T* dest, const T* src, int limit) {
	while (limit-- > 0 && (*dest++ = *src++) != 0)
		;
}

template<class T> int strcmp(const T* dest, const T* src) {
	while (*dest == *src && *dest != 0) {
		dest++;
		src++;
	}
	return *dest - *src;
}

template<class T> int stricmp(const T* dest, const T* src) {
	T dkar, skar;
	while ((dkar = tolower<T> (*dest)) == (skar = tolower<T> (*src)) && *dest
			!= 0) {
		dest++;
		src++;
	}
	return dkar - skar;
}

template<class T> int strncmp(const T* dest, const T* src, int limit) {
	while (limit-- > 0 && *dest == *src && *dest != 0) {
		dest++;
		src++;
	}
	return *dest - *src;
}

template<class T> int strnicmp(const T* dest, const T* src, int limit) {
	T dkar, skar;
	while (limit-- > 0 && (dkar = tolower<T> (*dest)) == (skar = tolower<T> (
			*src)) && *dest != 0) {
		dest++;
		src++;
	}
	return dkar - skar;
}

template<class T> T* strnewdup(const T* src) {
	T* p = 0;
	if (src) {
		int i = 0;
		while (src[i++])
			;
		p = new T[i];
		T* dest = p;
		while ((*dest++ = *src++) != 0)
			;
	}
	return p;
}

template<class T> void strcat(T* dest, const T* src) {
	while (*dest != 0)
		++dest;
	while ((*dest++ = *src++) != 0)
		;
}

template<class T> void copy(T* dest, const T* src, int n) {
	for (int i = 0; i < n; i++)
		*dest++ = *src++;
}

template<class T> int strlen(const T* s) {
	int i = 0;
	while (s[i] != 0)
		i++;
	return i;
}

template<class T> void strrev(T* string) {
	T* ptr = string;
	while (*ptr != 0)
		ptr++;
	while (--ptr > string) {
		T c = *ptr;
		*ptr = *string;
		*string++ = c;
	}
}

template<class T> const T* strchr(const T* string, T symbol) {
	do {
		if (*string == symbol)
			return string;
	} while (*string++);
	return 0;
}

template<class T> const T* strrchr(const T* string, T symbol) {
	const T* ptr = string;
	while (*ptr != 0)
		ptr++;
	do {
		if (*ptr == symbol)
			return ptr;
	} while (ptr-- > string);
	return 0;
}

template<class T> const T* strpbrk(const T* string, const T* set) {
	while (*string) {
		if (strchr(set, *string))
			return string;
		string++;
	}
	return 0;
}

template<class T> const T* strrpbrk(const T* string, const T* set) {
	const T* ptr = string;
	while (*ptr != 0)
		ptr++;
	while (ptr-- > string) {
		if (strchr(set, *ptr))
			return ptr;
	}
	return 0;
}

template<class T> T* strtok(T* string, const T* delim) {
#ifdef HAVE_TLS
	static T* __thread pStrtok = 0; // local token pointer
#else
	static T* pStrtok = 0; // local token pointer
#endif
	if (!string)
		string = pStrtok;
	while (*string && strchr<T> (delim, *string))
		string++;
	T* p = (*string) ? string : 0;
	if (p) {
		while (*string && !strchr<T> (delim, *string))
			string++;
		if (*string)
			*string++ = 0;
		pStrtok = string;
	}
	return p;
}

template<class T> T* strstr(T* string, T* pattern) {
	int plen = strlen<T> (pattern);
	while ((string = strchr<T> (string, *pattern)) != 0) {
		if (strncmp<T> (string, pattern, plen) == 0)
			break;
		string++;
	}
	return string;
}

template<class T> int atoi(T* number) {
	int n = 0;
	bool neg = false;

	while (ISSPACE(*number)) {
		++number;
	}
	if (*number == (T) '-') {
		neg = true;
		++number;
	} else if (*number == (T) '+') {
		++number;
	}
	while (ISDIGIT(*number)) {
		n = (n * 10) + ((*number++) - (T) '0');
	}
	return (neg) ? -n : n;
}

template<class T> const T* GetFileName(const T* path) {
	const T* ptr = strrchr<T> (path, '\\');
	return (ptr) ? ptr + 1 : path;
}

template<class T> bool CompareMask(const T* name, const T* mask) {
	// * is any number of characters
	// ? is any character
	while (*mask && *name) {
		T MaskKar = tolower<T> (*mask++);
		switch (MaskKar) {
		case (T) '*':
			do {
				if (CompareMask(name, mask)) {
					return true;
				}
				name++;
			} while (*name);
			break;
		case (T) '?':
			name++;
			break;
		default:
			if (MaskKar == tolower<T> (*name)) {
				name++;
			} else {
				return false;
			}
		}
		if ((*mask == 0 && *name == 0) || (*mask == (T) '*' && mask[1] == 0)) {
			return true;
		}
	}
	return false;
}

// nCount -1 means upto and including ending zero
template<class T> T* WideToAnsi(const wchar_t* String, int& nCount) {
	int cChars = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, String, nCount,
			0, 0, 0, 0);
	T* Ansi = 0;
	try {
		Ansi = new T[cChars];
		if (WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, String, nCount,
				Ansi, cChars, 0, 0) == 0) {
			delete[] Ansi;
			Ansi = 0;
		}
		nCount = cChars;
	} catch (std::bad_alloc&) {
		nCount = 0;
	}
	return Ansi;
}

// nCount -1 means upto and including ending zero
template<class T> wchar_t* AnsiToWide(const T* String, int& nCount) {
	int cChars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, String, nCount, 0,
			0);
	wchar_t* Wide = 0;
	try {
		Wide = new wchar_t[cChars];
		if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, String, nCount, Wide,
				cChars) == 0) {
			delete[] Wide;
			Wide = 0;
		}
		nCount = cChars;
	} catch (std::bad_alloc&) {
		nCount = 0;
	}
	return Wide;
}

template<class T> T* ExpandEnvironmentStrings(const T* String) {
	T* Expanded;
	try {
		Expanded = new T[MAX_PATH]; // The environment usually contains paths
		DWORD size = ::ExpandEnvironmentStrings(String, Expanded, MAX_PATH);
		if (size == 0 || size > MAX_PATH) {
			delete[] Expanded;
			Expanded = 0;
			if (size > 0) {
				Expanded = new T[size];
				size = ::ExpandEnvironmentStrings(String, Expanded, size);
				if (size == 0) {
					delete[] Expanded;
					Expanded = 0;
				}
			}
		}
	} catch (std::bad_alloc&) {
		Expanded = 0;
	}
	return Expanded;
}

template<class T> T* PathUnExpandEnvStrings(const T* String) {
	T* Unexpanded;
	try {
		Unexpanded = new T[MAX_PATH]; // The environment usually contains paths
		DWORD size = ::PathUnExpandEnvStrings(String, Unexpanded, MAX_PATH);
		if (size == 0 || size > MAX_PATH) {
			delete[] Unexpanded;
			Unexpanded = 0;
			if (size > 0) {
				Unexpanded = new T[size];
				size = ::PathUnExpandEnvStrings(String, Unexpanded, size);
				if (size == 0) {
					delete[] Unexpanded;
					Unexpanded = 0;
				}
			}
		}
	} catch (std::bad_alloc&) {
		Unexpanded = 0;
	}
	return Unexpanded;
}

inline char tohex(int kar) {
	kar &= 0x0F;
	return (char) ((kar < 10) ? '0' + kar : 'A' - 10 + kar);
}

inline char fromhex(char kar) {
	char value = 0;
	if (kar >= '0' && kar <= '9')
		value = (char) (kar - '0');
	else if (kar >= 'A' && kar <= 'F')
		value = (char) (10 + kar - 'A');
	else if (kar >= 'a' && kar <= 'f')
		value = (char) (10 + kar - 'a');
	return value;
}

/**
 * URL Escape codes
 *
 */
#define ESCAPE(a, b)	*(a)++ = (T)'%';\
						*(a)++ = tohex((b) >> 4); \
						*(a)++ = tohex(b);

template<class T> T* escape(const T* src) {
	int len = 1;
	T* ptr = (T*) src;
	short kar;
	while ((kar = *ptr++) != 0) {
		if ((kar >= 'A' && kar <= 'Z') || // 'A'..'Z'
				(kar >= 'a' && kar <= 'z') || // 'a'..'z'
				(kar >= '0' && kar <= '9') || // '0'..'9'
				(kar == '-' || kar == '_' || kar == '.' || kar == '!' || kar
						== '~' || kar == '*' || kar == '\'' || kar == '('
						|| kar == ')')) // unreserved
		{

			// Pass through
			len++;
		} else if (kar <= 0x007f) {

			// other ASCII
			len += 3;
		} else if (kar <= 0x07FF) {

			// non-ASCII <= 0x7FF
			len += 6;
		} else {

			// 0x7FF < c <= 0xFFFF
			len += 9;
		}
	}
	T* dest = new T[len];
	ptr = dest;
	while ((kar = *src++) != 0) {
		if ((kar >= 'A' && kar <= 'Z') || // 'A'..'Z'
				(kar >= 'a' && kar <= 'z') || // 'a'..'z'
				(kar >= '0' && kar <= '9') || // '0'..'9'
				(kar == '-' || kar == '_' || kar == '.' || kar == '!' || kar
						== '~' || kar == '*' || kar == '\'' || kar == '('
						|| kar == ')')) // unreserved
		{

			// Pass through
			*ptr++ = (T) kar;
		} else if (kar <= 0x007f) {

			// other ASCII
			ESCAPE(ptr, kar);
		} else if (kar <= 0x07FF) {

			// non-ASCII <= 0x7FF
			ESCAPE(ptr, 0xC0 | (kar >> 6));
			ESCAPE(ptr, 0x80 | (kar & 0x3F));
		} else {

			// 0x7FF < c <= 0xFFFF
			ESCAPE(ptr, 0xE0 | (kar >> 12));
			ESCAPE(ptr, 0x80 | ((kar >> 6) & 0x3F));
			ESCAPE(ptr, 0x80 | (kar & 0x3F));
		}
	}
	*ptr = 0;
	return dest;
}

template<class T> T* unescape(const T* src) {
	int len = 0;
	T* ptr = (T*) src;
	T kar;
	while ((kar = *ptr++) != 0) {
		if (kar == (T) ('%')) {
			len -= 2;
		} else {
			len++;
		}
	}
	short moreBytes = 0;
	short sumBytes = 0;
	T* dest = new T[(len > 0) ? len : 1];
	ptr = dest;
	while ((kar = *src++) != 0) {
		if (kar == '%' && src[0] && src[1]) {
			T tmp = fromhex(*src++) << 4;
			tmp |= fromhex(*src++);
			kar = tmp;
		}

		// Decode char c as UTF-8, sumBytes collects incomplete chars
		if ((kar & 0xC0) == 0x80) {

			// 10xxxxxx (continuation byte)
			// Add 6 bits to sumBytes
			sumBytes = (sumBytes << 6) | (kar & 0x3F);
			if (--moreBytes == 0) {

				// Write special character
				*ptr++ = (T) sumBytes;
			}
		} else if ((kar & 0x80) == 0x00) {

			// 0xxxxxxx (yields 7 bits)
			// Write regular character
			*ptr++ = kar;
		} else if ((kar & 0xE0) == 0xC0) {

			// 110xxxxx (yields 5 bits)
			sumBytes = kar & 0x1F;

			// Expect 1 more byte
			moreBytes = 1;
		} else if ((kar & 0xF0) == 0xE0) {

			// 1110xxxx (yields 4 bits)
			sumBytes = kar & 0x0F;

			// Expect 2 more bytes
			moreBytes = 2;
		} else if ((kar & 0xF8) == 0xF0) {

			// 11110xxx (yields 3 bits)
			sumBytes = kar & 0x07;

			// Expect 3 more bytes
			moreBytes = 3;
		} else if ((kar & 0xFC) == 0xF8) {

			// 111110xx (yields 2 bits)
			sumBytes = kar & 0x03;

			// Expect 4 more bytes
			moreBytes = 4;
		} else { // if ((kar & 0xFE) == 0xFC)

			// 1111110x (yields 1 bit)
			sumBytes = kar & 0x01;

			// Expect 5 more bytes
			moreBytes = 5;
		}

		// We don't test if the UTF-8 encoding is well-formed
	}
	*ptr = 0;
	return dest;
}

// Class templates
template<class T> class TStack {
	struct link {
		link(const T& a) : Ptr(0), Object(a) {
		}
		link* Ptr;
		T Object;
	};
	typedef struct link TLink;
	TLink* StackPtr;
public:
	TStack() :
		StackPtr(0) {
	}
	~TStack() {
		Flush();
	}
	bool Push(const T& object);
	bool Pop(T& object);
	void Flush();
	int Size();
	bool isEmpty() {
		return StackPtr == 0;
	}
	T& operator[](int index);
};

template<class T> void TStack<T>::Flush() {
	while (StackPtr) {
		TLink* ptr = StackPtr;
		StackPtr = ptr->Ptr;
		delete ptr;
	}
}

template<class T> bool TStack<T>::Push(const T& object) {
	try {
		TLink* ptr = new TLink(object);
		ptr->Ptr = StackPtr;
		StackPtr = ptr;
	} catch (std::bad_alloc&) {
		return false;
	}
	return true;
}

template<class T> bool TStack<T>::Pop(T& object) {
	if (StackPtr) {
		object = StackPtr->Object;
		TLink* ptr = StackPtr;
		StackPtr = ptr->Ptr;
		delete ptr;
		return true;
	}
	return false;
}

template<class T> int TStack<T>::Size() {
	int cCount = 0;
	TLink* ptr = StackPtr;
	while (ptr) {
		cCount++;
		ptr = ptr->Ptr;
	}
	return cCount;
}

template<class T> T& TStack<T>::operator[](int index) {
	class TIndexOutOfRange;
	TLink* ptr = StackPtr;
	while (ptr) {
		if (index <= 0) {
			break;
		}
		index--;
		ptr = ptr->Ptr;
	}
	if (ptr == 0 || index < 0)
		throw TIndexOutOfRange();
	return ptr->Object;
}

template<class T> class TPtrStack {
	struct link {
		link(T* a) :
			ObjectPtr(a), Ptr(0) {
		}
		link* Ptr;
		T* ObjectPtr;
	};
	typedef struct link TLink;
	TLink* StackPtr;
public:
	TPtrStack() :
		StackPtr(0) {
	}
	~TPtrStack() {
		Flush();
	}
	bool Push(T* objectptr);
	bool Pop(T& objectptr);
	void Flush();
	int Size();
	bool isEmpty() {
		return StackPtr == 0;
	}
	T* operator[](int index);
};

template<class T> void TPtrStack<T>::Flush() {
	while (StackPtr) {
		TLink* ptr = StackPtr;
		StackPtr = ptr->Ptr;
		delete ptr->ObjectPtr; // Prevents memory leaks
		delete ptr;
	}
}

template<class T> bool TPtrStack<T>::Push(T* objectptr) {
	try {
		TLink* ptr = new TLink(objectptr);
		ptr->Ptr = StackPtr;
		StackPtr = ptr;
	} catch (std::bad_alloc&) {
		return false;
	}
	return true;
}

template<class T> bool TPtrStack<T>::Pop(T& objectptr) {
	if (StackPtr) {
		objectptr = StackPtr->ObjectPtr;
		TLink* ptr = StackPtr;
		StackPtr = ptr->Ptr;
		delete ptr;
		return true;
	}
	return false;
}

template<class T> int TPtrStack<T>::Size() {
	int cCount = 0;
	TLink* ptr = StackPtr;
	while (ptr) {
		cCount++;
		ptr = ptr->Ptr;
	}
	return cCount;
}

template<class T> T* TPtrStack<T>::operator[](int index) {
	class TIndexOutOfRange;
	TLink* ptr = StackPtr;
	while (ptr) {
		if (index <= 0) {
			break;
		}
		index--;
		ptr = ptr->Ptr;
	}
	if (ptr == 0 || index < 0)
		throw TIndexOutOfRange();
	return ptr->ObjectPtr;
}

template<class T> class TNrToString {
	T buffer[MAXNRSPACE]; // used by itostr && utostr
	bool bSigned;
	bool bPadding;
	int iWidth;
public:
	const T* itostr(long number);
	const T* utostr(unsigned long number);
	const T* ltohex(long number);
	const T* wtohex(short number);
	const T* btohex(char number);
	TNrToString() {
		reset();
	}
	void reset();
	void padding(bool bValue) {
		bPadding = bValue;
	}
	void sign(bool bValue) {
		bSigned = bValue;
	}
	void width(int iValue) {
		iWidth = (iValue >= MAXNRSPACE) ? MAXNRSPACE - 1 : iValue;
	}
};

template<class T> void TNrToString<T>::reset() {
	bSigned = false;
	bPadding = false;
	iWidth = 0;
}

template<class T> const T* TNrToString<T>::itostr(long number) {
	T* p = buffer;
	bool bNegative = false;
	if (number < 0) {
		bNegative = true;
		number = -number;
	}
	do {
		*p++ = (T) ((T) '0' + number % 10); // grab each digit
		number /= 10;
	} while (number > 0);
	if (bSigned && !bNegative) {
		*p++ = (T) '+';
	}
	if (bNegative) {
		*p++ = (T) '-';
	}
	*p = (T) '\0';
	strrev<T> (buffer);
	int len = strlen<T> (buffer);
	int move = iWidth - len;
	if (move > 0) {
		for (int i = len; i >= 0; i--)
			buffer[i + move] = buffer[i];
		T cPad = (bPadding) ? (T) '0' : (T) ' ';
		for (int i = 0; i < move; i++)
			buffer[i] = cPad;
		if (bPadding && (buffer[move] == (T) '-' || buffer[move] == (T) '+')) {
			buffer[0] = buffer[move];
			buffer[move] = cPad;
		}
	}
	return buffer;
}

template<class T> const T* TNrToString<T>::utostr(unsigned long number) {
	T* p = buffer;
	do {
		*p++ = (T) ((T) '0' + number % 10); // grab each digit
		number /= 10;
	} while (number > 0);
	if (bSigned) {
		*p++ = (T) '+';
	}
	*p = (T) '\0';
	strrev<T> (buffer);
	int len = strlen<T> (buffer);
	int move = iWidth - len;
	if (move > 0) {
		for (int i = len; i >= 0; i--)
			buffer[i + move] = buffer[i];
		T cPad = (bPadding) ? (T) '0' : (T) ' ';
		for (int i = 0; i < move; i++)
			buffer[i] = cPad;
		if (bPadding && (buffer[move] == (T) '+')) {
			buffer[0] = buffer[move];
			buffer[move] = cPad;
		}
	}
	return buffer;
}

template<class T> const T* TNrToString<T>::ltohex(long number) {
	T* p = buffer;
	for (int i = 0; i < sizeof(number) * 2; i++) {
		*p++ = tohex(number);
		number >>= 4;
	}
	//*p++ = (T)'x';
	//*p++ = (T)'0';
	*p = (T) '\0';
	strrev<T> (buffer);
	return buffer;
}

template<class T> const T* TNrToString<T>::wtohex(short number) {
	T* p = buffer;
	for (int i = 0; i < sizeof(number) * 2; i++) {
		*p++ = tohex(number);
		number >>= 4;
	}
	//*p++ = (T)'x';
	//*p++ = (T)'0';
	*p = (T) '\0';
	strrev<T> (buffer);
	return buffer;
}

template<class T> const T* TNrToString<T>::btohex(char number) {
	T* p = buffer;
	for (int i = 0; i < sizeof(number) * 2; i++) {
		*p++ = tohex(number);
		number >>= 4;
	}
	//*p++ = (T)'x';
	//*p++ = (T)'0';
	*p = (T) '\0';
	strrev<T> (buffer);
	return buffer;
}

// Manipulator classes and templates
class TValue {
public:
	enum types {
		undefined, pointer, dword, longint, integer, word, byte
	} type;
	union numbers {
		void* p;
		unsigned long dw;
		long l;
		int i;
		short w;
		char b;
	} number;
	TValue() {
		type = undefined;
	}
	TValue(void* value) {
		type = pointer;
		number.p = value;
	}
	TValue(unsigned long value) {
		type = dword;
		number.dw = value;
	}
	TValue(long value) {
		type = longint;
		number.l = value;
	}
	TValue(int value) {
		type = integer;
		number.i = value;
	}
	TValue(short value) {
		type = word;
		number.w = value;
	}
	TValue(char value) {
		type = byte;
		number.b = value;
	}
};

template<class T> class TManip {
public:
	const TValue value;
	T& (*f)(T&, const TValue&);
	TManip(T& (*func)(T&, const TValue&), const TValue& val) :
		value(val), f(func) {
	}
};

template<class T> T& to_hex(T& s, const TValue& value) {
	switch (value.type) {
	case TValue::pointer:
		s << s.ltohex((long) value.number.p);
		break;
	case TValue::dword:
		s << s.ltohex(value.number.dw);
		break;
	case TValue::longint:
		s << s.ltohex(value.number.l);
		break;
	case TValue::integer:
		s << s.ltohex(value.number.i);
		break;
	case TValue::word:
		s << s.wtohex(value.number.w);
		break;
	case TValue::byte:
		s << s.btohex(value.number.b);
		break;
	case TValue::undefined:
		break;
	}
	return s;
}

template<class T> TManip<T> hex(TValue value) {
	return TManip<T> (to_hex, value);
}

template<class T> class TManipInt {
public:
	const int value;
	T& (*f)(T&, int value);
	TManipInt(T& (*func)(T&, int), int val) :
		value(val), f(func) {
	}
};

template<class T> T& set_width(T& s, int value) {
	s.width(value);
	return s;
}

template<class T> const TManipInt<T> setw(int value) {
	return TManipInt<T> (set_width, value);
}

class TRandom {
	long Randx;
public:
	TRandom() {
		Randomize();
	}
	void Randomize() {
		// Produces not random enough initial results so xor it with the process id
		Randx = GetTickCount() ^ GetCurrentProcessId();
		FILETIME FileTime;
		GetSystemTimeAsFileTime(&FileTime);

		// Convert to DOS time for 'long' result
		WORD DosDate;
		WORD DosTime;
		FileTimeToDosDateTime(&FileTime, &DosDate, &DosTime);

		// Seed based on a 'long' time xored with the previous Randx value
		Randx ^= MAKELONG(DosTime, DosDate);
	}
	int GetRandom(unsigned Modulo = -1) {
		return (int) (((Randx = Randx * 505360173L + 907633385L) & 0x7FFFFFFFL)
				% Modulo);
	}
};

}
#endif //BUTILS_H
