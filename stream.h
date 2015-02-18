#ifndef STREAM_H
#define STREAM_H
#include "bprocess.h"
#include "butils.h"

namespace b {

template <class T> class TStream : public TNrToString<T>
{
	THandle<invalid> hFile;
protected:
	bool bNoError;
	bool bNoEOF;
	inline void PushBack();
	void Puts(const T* string);
	void PutChar(const T kar);
	T GetChar();
	int GetInteger();
	unsigned GetUnsigned();
	long Compare(const T* find, bool bIgnoreCase);
	long CompareMask(const T* mask, bool bIgnoreCase);
public:
	enum TResult { Error, EndOfFile, NoCrLf, CrLf };
	enum { NoMatch = -1 };
	static const T crlf[];
	TStream();
	TStream(const char* filename,
			  DWORD access = GENERIC_READ | GENERIC_WRITE,
			  DWORD mode = OPEN_ALWAYS);
	TStream(const wchar_t* filename,
			  DWORD access = GENERIC_READ | GENERIC_WRITE,
			  DWORD mode = OPEN_ALWAYS);
	bool Open(const char* filename,
				 DWORD access = GENERIC_READ | GENERIC_WRITE,
				 DWORD mode = OPEN_ALWAYS);
	bool Open(const wchar_t* filename,
				 DWORD access = GENERIC_READ | GENERIC_WRITE,
				 DWORD mode = OPEN_ALWAYS);
	void Close();
	bool Clear();
	bool IsOpen() const
	{
		return hFile;
	}
	operator bool() const;
	long GetSize();
	long GetPosition();
	bool SetPosition(long lPos);
	bool MovePosition(long lPos);
	bool MoveBOFPosition(); // Moves to BOF
	bool MoveEOFPosition(); // Moves to EOF
	TStream<T>& operator << (const TString<T>& string);
	TStream<T>& operator << (const T* string);
	TStream<T>& operator << (int number);
	TStream<T>& operator << (unsigned number);
	TStream<T>& operator << (T kar);
	bool ReadBuffer(T *buffer, int cCount);
	bool WriteBuffer(T *buffer, int cCount);
	TResult GetLine(T *buffer, int cCount) {
		TResult RetValue = Error;
		cCount--;	// Last character is always a zero
					// Buffer size is including a terminating zero
		if(buffer && cCount > 0)
		{
			DWORD NumberOfBytesRead;
			bNoError = ReadFile(hFile, buffer, cCount * sizeof(T), &NumberOfBytesRead, 0);
			if(bNoError)
			{
				bNoEOF = (NumberOfBytesRead == cCount * sizeof(T));
				int cChars = NumberOfBytesRead / sizeof(T);
				int i = 0;
				while(i < cChars && buffer[i] != crlf[0] && buffer[i] != crlf[1])
				{
					i++;
				}
				buffer[i] = 0;
				if(i == cChars)  // CrLf not found
				{
					if(bNoEOF)
					{
						RetValue = NoCrLf;
						MovePosition(0 - NumberOfBytesRead);
					}
					else
					{
						RetValue = EndOfFile;
					}
				}
				else
				{
					RetValue = CrLf;
					MovePosition((i + 1) * sizeof(T) - NumberOfBytesRead);
					Skip(crlf);
				}
			}
		}
		return RetValue;
	}
	bool Skip(const T* tokens);
	bool Skip();
	TStream<T>& operator >> (T& kar);
	TStream<T>& operator >> (int& integer);
	TStream<T>& operator >> (unsigned& integer);
	TStream<T>& operator >> (short& integer);
	bool operator == (const T* string);
	bool operator == (T kar);
	bool Find(const T* find, bool bIgnoreCase = false);
	int Replace(TStream<T>& out, const T* find, const T* replace, bool bIgnoreCase = false);
 	long FindMask(const T* mask, bool bIgnoreCase);
	int ReplaceMask(TStream<T>& out, const T* mask, const T* replace, bool bIgnoreCase = false);
	TStream<T>& operator << (TStream<T>& (*f)(TStream<T>&))
	{
		return f(*this);
	}
};

template <class T> const T TStream<T>::crlf[] = { T('\r'), T('\n'), T('\0') };

template <class T> TStream<T>::TStream()
{
	bNoError = true;
	bNoEOF = true;
}

template <class T> TStream<T>::TStream(const char* filename, DWORD access, DWORD mode)
{
	Open(filename, access, mode);
}

template <class T> TStream<T>::TStream(const wchar_t* filename, DWORD access, DWORD mode)
{
	Open(filename, access, mode);
}

template <class T> bool TStream<T>::Open(const char* filename, DWORD access, DWORD mode)
{
	bNoError = true;
	bNoEOF = true;
	TStream<T>::reset();
	hFile = CreateFileA(filename, access, FILE_SHARE_READ, 0,
							  mode,
							  FILE_ATTRIBUTE_NORMAL, 0);
	return (hFile) ? GetLastError() != ERROR_ALREADY_EXISTS : false;
}

template <class T> bool TStream<T>::Open(const wchar_t* filename, DWORD access, DWORD mode)
{
	bNoError = true;
	bNoEOF = true;
	TStream<T>::reset();
	hFile = CreateFileW(filename, access, FILE_SHARE_READ, 0,
							  mode,
							  FILE_ATTRIBUTE_NORMAL, 0);
	return (hFile) ? GetLastError() != ERROR_ALREADY_EXISTS : false;
}

template <class T> void TStream<T>::Close()
{
	hFile.Close();
}

template <class T> TStream<T>::operator bool() const
{
	return bNoEOF && bNoError && (bool)hFile;
}

template <class T> bool TStream<T>::Clear()
{
	if(SetPosition(0))
	{
		bNoError = SetEndOfFile(hFile);
	}
	return bNoError;
}

template <class T> void TStream<T>::PushBack()
{
	MovePosition(-(long)(sizeof(T)));
}

template <class T> long TStream<T>::GetSize()
{
	bNoEOF = true;
	DWORD dwRetVal = ::GetFileSize(hFile, 0);
	bNoError = (dwRetVal != INVALID_FILE_SIZE);
	return static_cast<long>(dwRetVal);
}

template <class T> long TStream<T>::GetPosition()
{
	bNoEOF = true;
	DWORD dwRetVal = ::SetFilePointer(hFile, 0, 0, FILE_CURRENT);
	bNoError = (dwRetVal != INVALID_SET_FILE_POINTER);
	return static_cast<long>(dwRetVal);
}

template <class T> bool TStream<T>::SetPosition(long pos)
{
	bNoEOF = true;
	return bNoError = (::SetFilePointer(hFile, pos, 0, FILE_BEGIN) != INVALID_SET_FILE_POINTER);
}

template <class T> bool TStream<T>::MovePosition(long pos)
{
	bNoEOF = true;
	return bNoError = (::SetFilePointer(hFile, pos, 0, FILE_CURRENT) != INVALID_SET_FILE_POINTER);
}

template <class T> bool TStream<T>::MoveBOFPosition()
{
	bNoEOF = true;
	return bNoError = (::SetFilePointer(hFile, 0, 0, FILE_BEGIN) != INVALID_SET_FILE_POINTER);
}

template <class T> bool TStream<T>::MoveEOFPosition()
{
	bNoEOF = true;
	return bNoError = (::SetFilePointer(hFile, 0, 0, FILE_END) != INVALID_SET_FILE_POINTER);
}

template <class T> TStream<T>& TStream<T>::operator << (const TString<T>& string)
{
	Puts(string.c_str());
	return *this;
}

template <class T> TStream<T>& TStream<T>::operator << (const T* string)
{
	Puts(string);
	return *this;
}

template <class T> TStream<T>& TStream<T>::operator << (int number)
{
	Puts(TStream<T>::itostr(number));
	return *this;
}

template <class T> TStream<T>& TStream<T>::operator << (unsigned number)
{
	Puts(TStream<T>::utostr(number));
	return *this;
}

template <class T> TStream<T>& TStream<T>::operator << (T kar)
{
	PutChar(kar);
	return *this;
}

template <class T> void TStream<T>::Puts(const T* string)
{
	bNoError = false;
	if(string)
	{
		DWORD NumberOfBytesWritten;
		DWORD cCount = b::strlen<T>(string) * sizeof(T);
		bNoError = WriteFile(hFile, string, cCount, &NumberOfBytesWritten, 0);
		if(bNoError)
		{
			bNoEOF = (NumberOfBytesWritten == cCount);
		}
	}
}

template <class T> void TStream<T>::PutChar(T kar)
{
	DWORD NumberOfBytesWritten;
	bNoError = WriteFile(hFile, &kar, sizeof(kar), &NumberOfBytesWritten, 0);
	if(bNoError)
	{
		bNoEOF = (NumberOfBytesWritten == sizeof(kar));
	}
}

template <class T> bool TStream<T>::ReadBuffer(T* buffer, int cCount)
{
	bNoError = false;
	if(buffer)
	{
		DWORD NumberOfBytesRead;
		bNoError = ReadFile(hFile, buffer, cCount * sizeof(T), &NumberOfBytesRead, 0);
		if(bNoError)
		{
			bNoEOF = (NumberOfBytesRead == cCount * sizeof(T));
		}
	}
	return bNoError && bNoEOF;
}

template <class T> bool TStream<T>::WriteBuffer(T* buffer, int cCount)
{
	bNoError = false;
	if(buffer)
	{
		DWORD NumberOfBytesWritten;
		bNoError = WriteFile(hFile, buffer, cCount * sizeof(T), &NumberOfBytesWritten, 0);
		if(bNoError)
		{
			bNoEOF = (NumberOfBytesWritten == cCount * sizeof(T));
		}
	}
	return bNoError && bNoEOF;
}

template <class T> bool TStream<T>::Skip(const T* tokens)
{
	T kar;
	do
	{
		kar = GetChar();
	}
	while(b::strchr<T>(tokens, kar) && bNoEOF && bNoError);
	if(bNoEOF && bNoError)
		PushBack();
	return bNoError && bNoEOF;
}

template <class T> bool TStream<T>::Skip()
{
	GetChar();
	return bNoError && bNoEOF;
}

template <class T> TStream<T>& TStream<T>::operator >> (T& kar)
{
	kar = GetChar();
	return *this;
}

template <class T> TStream<T>& TStream<T>::operator >> (int& integer)
{
	integer = GetInteger();
	return *this;
}

template <class T> TStream<T>& TStream<T>::operator >> (unsigned& integer)
{
	integer = GetUnsigned();
	return *this;
}

template <class T> TStream<T>& TStream<T>::operator >> (short& integer)
{
	integer = static_cast<short>(GetInteger());
	return *this;
}

template <class T> T TStream<T>::GetChar()
{
	T kar;
	DWORD NumberOfBytesRead;
	bNoError = ReadFile(hFile, &kar, sizeof(kar), &NumberOfBytesRead, 0);
	if(bNoError)
	{
		bNoEOF = (NumberOfBytesRead == sizeof(kar));
	}
	return kar;
}

template <class T> int TStream<T>::GetInteger()
{
	int number = 0;
	bool bNegative = false;
	T kar;

	do
	{
		kar = GetChar();
	}
	while(ISSPACE(kar) && bNoEOF && bNoError);
	if(bNoEOF && bNoError)
	{
		if(kar == (T)'-')
		{
			bNegative = true;
			kar = GetChar();
		}
		else if(kar == (T)'+')
			kar = GetChar();
		while(ISDIGIT(kar) && bNoEOF && bNoError)
		{
			number = (number * 10) + (kar - (T)'0');
			kar = GetChar();
		}
		if(bNoEOF && bNoError)
		{
			PushBack();
		}
	}
	return (bNegative) ? -number : number;
}

template <class T> unsigned TStream<T>::GetUnsigned()
{
	unsigned number = 0;
	T kar;

	do
	{
		kar = GetChar();
	}
	while(ISSPACE(kar) && bNoEOF && bNoError);
	if(bNoEOF && bNoError)
	{
		if(kar == (T)'+')
			kar = GetChar();
		while(ISDIGIT(kar) && bNoEOF && bNoError)
		{
			number = (number * 10) + (kar - (T)'0');
			kar = GetChar();
		}
		if(bNoEOF && bNoError)
		{
			PushBack();
		}
	}
	return number;
}

template <class T> bool TStream<T>::operator == (const T* string)
{
	bool bRetVal = true;
	long pos = GetPosition();
	const T* ptr = string;
	while(*ptr && bNoEOF && bNoError)
	{
		if(*ptr++ != GetChar())
		{
			bRetVal = false;
			if(bNoEOF && bNoError)
				SetPosition(pos);
			break;
		}
	}
	return (bNoEOF && bNoError) ? bRetVal : false;
}

template <class T> bool TStream<T>::operator == (T kar)
{
	bool bRetVal = true;
	if(kar != GetChar())
	{
		bRetVal = false;
		if(bNoEOF && bNoError)
			PushBack();
	}
	return (bNoEOF && bNoError) ? bRetVal : false;
}

template <class T> bool TStream<T>::Find(const T* find, bool bIgnoreCase)
{
	bool bFound = false;
	int last_char = b::strlen<T>(find) - 1;
	T* buffer = 0;
	if(bIgnoreCase)
	{
		buffer = strnewdup(find);
		find = tolower<T>(buffer);
	}
	int index = 0;
	T kar = GetChar();
	while(bNoEOF && bNoError)
	{
		if(find[index] == ((bIgnoreCase) ? tolower<T>(kar) : kar))
		{
			if(index == last_char)
			{
				MovePosition(-index * sizeof(T));
				bFound = true;
				break;
			}
			else
			{
				index++;
			}
		}
		else
		{
			if(index)
			{
				MovePosition(-index * sizeof(T));
				index = 0;
			}
		}
		kar = GetChar();
	}
	delete[] buffer;
	return bFound;
}

template <class T> int TStream<T>::Replace(TStream<T>& out, const T* find, const T* replace, bool bIgnoreCase)
{
	int last_char = b::strlen<T>(find) - 1;
	T* buffer = 0;
	if(bIgnoreCase)
	{
		buffer = strnewdup(find);
		find = tolower<T>(buffer);
	}
	int changes = 0;
	int index = 0;
	T kar = GetChar();
	T first_equal_kar;
	while(bNoEOF && bNoError)
	{
		if(find[index] == ((bIgnoreCase) ? tolower<T>(kar) : kar))
		{
			if(index == last_char)
			{
				if(!(out << replace))
					break;
				index = 0;
				changes++;
			}
			else
			{
				if(index++ == 0)
					first_equal_kar = kar;
			}
		}
		else
		{
			if(index)
			{
				MovePosition(-index * sizeof(T));
				index = 0;
				if(!(out << first_equal_kar))
					break;
			}
			else
			{
				if(!(out << kar))
					break;
			}
		}
		kar = GetChar();
	}
	delete[] buffer;
	return changes;
}

template <class T> long TStream<T>::Compare(const T* find, bool bIgnoreCase)
{
	long pos = GetPosition();
	T kar = GetChar();
	while(*find && bNoEOF && bNoError)
	{
		T FindKar = *find++;
		if((bIgnoreCase) ? tolower<T>(kar) != tolower<T>(FindKar) : kar != FindKar)
		{
			SetPosition(pos);
			return NoMatch;
		}
		if(*find == 0)
		{
			return pos;
		}
		kar = GetChar();
	}
	SetPosition(pos);
	return NoMatch;
}

template <class T> long TStream<T>::CompareMask(const T* mask, bool bIgnoreCase)
{
	// * is any number of characters
	// ? is any character
	long pos = GetPosition();
	T kar = GetChar();
	while(*mask && bNoEOF && bNoError)
	{
		T MaskKar = *mask++;
		switch(MaskKar)
		{
			case (T)'*':	// zero or more char
				PushBack();
				do
				{
					if(*mask != 0 && CompareMask(mask, bIgnoreCase) != NoMatch)
					{
						return pos;
					}
				}
				while(Skip());
				break;
			case (T)'?':	// one char
				break;
			default:
				if((bIgnoreCase) ? tolower<T>(kar) != tolower<T>(MaskKar) : kar != MaskKar)
				{
					SetPosition(pos);
					return NoMatch;
				}
		}
		if(*mask == 0)
		{
			return pos;
		}
		kar = GetChar();
	}
	SetPosition(pos);
	return NoMatch;
}

template <class T> long TStream<T>::FindMask(const T* mask, bool bIgnoreCase)
{
	long pos;
	do
	{
		if((pos = CompareMask(mask, bIgnoreCase)) != NoMatch)
		{
			break;
		}
	}
	while(Skip());
	return pos;
}

template <class T> int TStream<T>::ReplaceMask(TStream<T>& out, const T* mask, const T* replace, bool bIgnoreCase)
{
	int changes = 0;
	do
	{
		if(CompareMask(mask, bIgnoreCase) != NoMatch)
		{
			if(!(out << replace))
				break;
			changes++;
		}
		else
		{
			T kar = GetChar();
			if(bNoEOF && bNoError)
			{
				if(!(out << kar))
					break;
			}
		}
	}
	while(bNoEOF && bNoError);
	return changes;
}

template <class T> TStream<T>& endl(TStream<T>& s)
{
	return s << TStream<T>::crlf;
}

template <class T> TStream<T>& unicode(TStream<T>& s)
{
	return s << (wchar_t)0xFEFF;
}

template <class T> TStream<T>& tab(TStream<T>& s)
{
	return s << T('\t');
}


template <class T> TStream<T>& operator << (TStream<T>& s, const TManip<TStream<T> >& m) {
	return m.f(s, m.value);
}

template <class T> TStream<T>& operator << (TStream<T>& s, const TManipInt<TStream<T> >& m) {
	return m.f(s, m.value);
}

#ifdef UNICODE
typedef TStream<wchar_t> Stream;
#else
typedef TStream<char> Stream;
#endif

typedef TStream<char> StreamA;
typedef TStream<wchar_t> StreamW;
}
#endif //STREAM_H
