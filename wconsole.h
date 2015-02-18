#ifndef WCONSOLE_H
#define WCONSOLE_H
#include <bprocess.h>
#include <bstring.h>

using b::TValue;
using b::TManip;

template <class T> class TConStream : public b::TNrToString<T>
{
	bool bNoError;
	bool bNoEOF;
	bool Write(const char* lpBuffer, DWORD nNumberOfCharsToWrite, DWORD* lpNumberOfCharsWritten);
	bool Write(const wchar_t* lpBuffer, DWORD nNumberOfCharsToWrite, DWORD* lpNumberOfCharsWritten);
	bool Puts(const char* string);
	bool Puts(const wchar_t* string);
	bool PutChar(T kar);
	void FillScreen(char filler);
	void FillScreen(wchar_t filler);
	void FillEndOfLine(char filler);
	void FillEndOfLine(wchar_t filler);
protected:
	bool bConsole;
	b::THandle<b::invalid> hConOut;
public:
	static const T crlf[];
	TConStream();
	operator bool () const
	{
		return bNoEOF && bNoError && hConOut;
	}
	TConStream<T>& operator << (const b::TString<T>& string);
	TConStream<T>& operator << (const char* string);
	TConStream<T>& operator << (const wchar_t* string);
	TConStream<T>& operator << (long number);
	TConStream<T>& operator << (unsigned long number);
	TConStream<T>& operator << (int number);
	TConStream<T>& operator << (unsigned number);
	TConStream<T>& operator << (T kar);
	TConStream<T>& operator << (TConStream&(*f)(TConStream&))
	{
		return f(*this);
	}
	TConStream& operator << (const TManip<TConStream<T> >& m)
	{
		return m.f(*this, m.value);
	}
	static const TManip<TConStream<T> > hex(TValue value)
	{
		return TManip<TConStream<T> >(b::to_hex, value);
	}
	void ClearScreen()
	{
		FillScreen((T)' ');
	}
	void ClearEndOfLine()
	{
		FillEndOfLine((T)' ');
	}
	bool GotoXY(int x, int y);
	bool TextAttribute(WORD attribute);
};

template <class T> class TConOutStream : public TConStream<T>
{
public:
	TConOutStream()
	{
		TConStream<T>::hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
		TConStream<T>::bConsole = GetFileType(TConStream<T>::hConOut) == FILE_TYPE_CHAR;
	}
};

template <class T> class TConErrStream : public TConStream<T>
{
public:
	TConErrStream()
	{
		TConStream<T>::hConOut = GetStdHandle(STD_ERROR_HANDLE);
		TConStream<T>::bConsole = GetFileType(TConStream<T>::hConOut) == FILE_TYPE_CHAR;
	}
};

template <class T> const T TConStream<T>::crlf[] = { T('\r'), T('\n'), T('\0') };

template <class T> bool TConStream<T>::Write(const char* lpBuffer, DWORD nNumberOfCharsToWrite, DWORD* lpNumberOfCharsWritten)
{
	bool bRet;
	if(bConsole)
	{
		bRet = WriteConsoleA(hConOut, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, 0);
	}
	else
	{
		bRet = WriteFile(hConOut, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, 0);
	}
	return bRet;
}

template <class T> bool TConStream<T>::Write(const wchar_t* lpBuffer, DWORD nNumberOfCharsToWrite, DWORD* lpNumberOfCharsWritten)
{
	bool bRet = false;
	if(bConsole)
	{
		bRet = WriteConsoleW(hConOut, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, 0);
	}
	else
	{
		int cChars = nNumberOfCharsToWrite;
		*lpNumberOfCharsWritten = 0;
		char* Ansi = b::WideToAnsi<char>(lpBuffer, cChars);
		if(cChars)
		{
			bRet = WriteFile(hConOut, Ansi, cChars, lpNumberOfCharsWritten, 0);
			if(*lpNumberOfCharsWritten == (DWORD)cChars)
			{
				*lpNumberOfCharsWritten = nNumberOfCharsToWrite;
			}
		}
		delete[] Ansi;
	}
	return bRet;
}

template <class T> bool TConStream<T>::Puts(const char* string)
{
	bNoError = false;
	if(string)
	{
		DWORD NumberOfCharsWritten;
		DWORD cCount = b::strlen<char>(string);
		bNoError = Write(string, cCount, &NumberOfCharsWritten);
		if(bNoError)
		{
			bNoEOF = (NumberOfCharsWritten == cCount);
		}
	}
	return bNoError;
}

template <class T> bool TConStream<T>::Puts(const wchar_t* string)
{
	bNoError = false;
	if(string)
	{
		DWORD NumberOfCharsWritten;
		DWORD cCount = b::strlen<wchar_t>(string);
		bNoError = Write(string, cCount, &NumberOfCharsWritten);
		if(bNoError)
		{
			bNoEOF = (NumberOfCharsWritten == cCount);
		}
	}
	return bNoError;
}

template <class T> bool TConStream<T>::PutChar(T kar)
{
	DWORD NumberOfCharsWritten;
	bNoError = Write(&kar, 1, &NumberOfCharsWritten);
	if(bNoError)
	{
		bNoEOF = (NumberOfCharsWritten == 1);
	}
	return bNoError;
}

template <class T> TConStream<T>::TConStream()
{
	bNoError = true;
	bNoEOF = true;
	bConsole = true;
	hConOut = INVALID_HANDLE_VALUE;
}

template <class T> TConStream<T>& TConStream<T>::operator << (const b::TString<T>& string)
{
	Puts(string.c_str());
	return *this;
}

template <class T> TConStream<T>& TConStream<T>::operator << (const char* string)
{
	Puts(string);
	return *this;
}

template <class T> TConStream<T>& TConStream<T>::operator << (const wchar_t* string)
{
	Puts(string);
	return *this;
}

template <class T> TConStream<T>& TConStream<T>::operator << (long number)
{
	Puts(TConStream<T>::itostr(number));
	return *this;
}

template <class T> TConStream<T>& TConStream<T>::operator << (unsigned long number)
{
	Puts(TConStream<T>::utostr(number));
	return *this;
}

template <class T> TConStream<T>& TConStream<T>::operator << (int number)
{
	Puts(TConStream<T>::itostr(number));
	return *this;
}

template <class T> TConStream<T>& TConStream<T>::operator << (unsigned number)
{
	Puts(TConStream<T>::utostr(number));
	return *this;
}

template <class T> TConStream<T>& TConStream<T>::operator << (T kar)
{
	PutChar(kar);
	return *this;
}

template <class T> void TConStream<T>::FillScreen(char filler)
{
	bNoError = false;
	const COORD coordScreen = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(GetConsoleScreenBufferInfo(hConOut, &csbi))
	{
		DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
		DWORD cCharsWritten;
		if(FillConsoleOutputCharacterA(hConOut, filler, dwConSize, coordScreen, &cCharsWritten))
		{
			if(FillConsoleOutputAttribute(hConOut, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten))
			{
				if(SetConsoleCursorPosition(hConOut, coordScreen))
					bNoError = true;
			}
		}
	}
}

template <class T> void TConStream<T>::FillScreen(wchar_t filler)
{
	bNoError = false;
	const COORD coordScreen = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(GetConsoleScreenBufferInfo(hConOut, &csbi))
	{
		DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
		DWORD cCharsWritten;
		if(FillConsoleOutputCharacterW(hConOut, filler, dwConSize, coordScreen, &cCharsWritten))
		{
			if(FillConsoleOutputAttribute(hConOut, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten))
			{
				if(SetConsoleCursorPosition(hConOut, coordScreen))
					bNoError = true;
			}
		}
	}
}

template <class T> void TConStream<T>::FillEndOfLine(char filler)
{
	bNoError = false;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(GetConsoleScreenBufferInfo(hConOut, &csbi))
	{
		DWORD dwLength = csbi.dwSize.X - csbi.dwCursorPosition.X;
		DWORD cCharsWritten;
		if(FillConsoleOutputCharacterA(hConOut, filler, dwLength, csbi.dwCursorPosition, &cCharsWritten))
		{
			if(FillConsoleOutputAttribute(hConOut, csbi.wAttributes, dwLength, csbi.dwCursorPosition, &cCharsWritten))
				bNoError = true;
		}
	}
}

template <class T> void TConStream<T>::FillEndOfLine(wchar_t filler)
{
	bNoError = false;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(GetConsoleScreenBufferInfo(hConOut, &csbi))
	{
		DWORD dwLength = csbi.dwSize.X - csbi.dwCursorPosition.X;
		DWORD cCharsWritten;
		if(FillConsoleOutputCharacterW(hConOut, filler, dwLength, csbi.dwCursorPosition, &cCharsWritten))
		{
			if(FillConsoleOutputAttribute(hConOut, csbi.wAttributes, dwLength, csbi.dwCursorPosition, &cCharsWritten))
				bNoError = true;
		}
	}
}

template <class T> bool TConStream<T>::GotoXY(int x, int y)
{
	COORD pos = { (SHORT)x, (SHORT)y };
	return bNoError = SetConsoleCursorPosition(hConOut, pos);
}

template <class T> bool TConStream<T>::TextAttribute(WORD attribute)
{
	return bNoError = SetConsoleTextAttribute(hConOut, attribute);
}

template <class T> TConStream<T>& endl(TConStream<T>& s)
{
	return s << TConStream<T>::crlf;
}

template <class T> TConStream<T>& tab(TConStream<T>& s)
{
	return s << T('\t');
}

template <class T> class TPushBack
{
	T Kar;
	bool bPushed;
public:
	TPushBack()
	{
		bPushed = false;
	}
	inline bool IsPushed()
	{
		return bPushed;
	}
	bool Push(T kar);
	bool Pop(T& kar);
};

template <class T> bool TPushBack<T>::Push(T kar)
{
	if(bPushed)
	{
		return false;
	}
	else
	{
		Kar = kar;
		return bPushed = true;
	}
}

template <class T> bool TPushBack<T>::Pop(T& kar)
{
	if(bPushed)
	{
		kar = Kar;
		bPushed = false;
		return true;
	}
	else
	{
		return false;
	}
}

template <class T> class TConInStream : TPushBack<T>
{
	bool bNoError;
	bool bNoEOF;
	bool Read(char* lpBuffer, DWORD nNumberOfCharsToRead, DWORD* lpNumberOfCharsRead);
	bool Read(wchar_t* lpBuffer, DWORD nNumberOfCharsToRead, DWORD* lpNumberOfCharsRead);
	T GetChar();
	int GetInteger();
	unsigned GetUnsigned();
protected:
	bool bConsole;
	b::THandle<b::invalid> hConIn;
public:
	TConInStream();
	operator bool() const
	{
		return bNoEOF && bNoError && hConIn;
	}
	bool GetBuffer(T *buffer, int cCount);
	bool Skip(const T* tokens);
	bool Skip();
	TConInStream<T>& operator >> (T& kar);
	TConInStream<T>& operator >> (int& integer);
	TConInStream<T>& operator >> (unsigned& integer);
	bool operator == (const T* string);
	bool operator == (T kar);
};

template <class T> TConInStream<T>::TConInStream()
{
	bNoError = true;
	bNoEOF = true;
	hConIn = GetStdHandle(STD_INPUT_HANDLE);
	bConsole = GetFileType(hConIn) == FILE_TYPE_CHAR;
}

template <class T> bool TConInStream<T>::Read(char* lpBuffer,	DWORD nNumberOfCharsToRead, DWORD* lpNumberOfCharsRead)
{
	bool bRet;
	if(bConsole)
	{
		bRet = ReadConsoleA(hConIn, lpBuffer, nNumberOfCharsToRead, lpNumberOfCharsRead, 0);
	}
	else
	{
		bRet = ReadFile(hConIn, lpBuffer, nNumberOfCharsToRead, lpNumberOfCharsRead, 0);
	}
	return bRet;
}

template <class T> bool TConInStream<T>::Read(wchar_t* lpBuffer,	DWORD nNumberOfCharsToRead, DWORD* lpNumberOfCharsRead)
{
	bool bRet = false;
	if(bConsole)
	{
		bRet = ReadConsoleW(hConIn, lpBuffer, nNumberOfCharsToRead, lpNumberOfCharsRead, 0);
	}
	else
	{
		bRet = ReadFile(hConIn, lpBuffer, nNumberOfCharsToRead, lpNumberOfCharsRead, 0);
		int cChars = *lpNumberOfCharsRead;
		if(cChars)
		{  // char to wide
			wchar_t* Wide = b::AnsiToWide<char>((char*)lpBuffer, cChars);
			*lpNumberOfCharsRead = cChars;
			b::copy(lpBuffer, Wide, ((DWORD)cChars < nNumberOfCharsToRead) ? cChars : nNumberOfCharsToRead);
			delete[] Wide;
		}
	}
	return bRet;
}

template <class T> bool TConInStream<T>::GetBuffer(T* buffer, int cCount)
{
	bNoError = false;
	if(buffer)
	{
		bool bPopped = (cCount > 0) ? Pop(buffer[0]) : false;
		if(bPopped)
		{
			cCount--;
		}
		if(cCount > 0)
		{
			DWORD NumberOfCharsRead;
			bNoError = Read(&buffer[(bPopped) ? 1 : 0], cCount, &NumberOfCharsRead);
			if(bNoError)
			{
				bNoEOF = (NumberOfCharsRead == (DWORD)cCount);
			}
		}
	}
	return bNoError && bNoEOF;
}

template <class T> bool TConInStream<T>::Skip(const T* tokens)
{
	T kar;
	do
	{
		kar = GetChar();
	}
	while(b::strchr<T>(tokens, kar) && bNoEOF && bNoError);
	if(bNoEOF && bNoError)
		bNoError = Push(kar);
	return bNoError && bNoEOF;
}

template <class T> bool TConInStream<T>::Skip()
{
	GetChar();
	return bNoError && bNoEOF;
}

template <class T> T TConInStream<T>::GetChar()
{
	T kar;
	if(!Pop(kar))
	{
		DWORD NumberOfCharsRead;
		bNoError = Read(&kar, 1, &NumberOfCharsRead);
		if(bNoError)
		{
			bNoEOF = (NumberOfCharsRead == 1);
		}
	}
	return kar;
}

template <class T> TConInStream<T>& TConInStream<T>::operator >> (T& kar)
{
	kar = GetChar();
	return *this;
}

template <class T> TConInStream<T>& TConInStream<T>::operator >> (int& integer)
{
	integer = GetInteger();
	return *this;
}

template <class T> TConInStream<T>& TConInStream<T>::operator >> (unsigned& integer)
{
	integer = GetUnsigned();
	return *this;
}

template <class T> int TConInStream<T>::GetInteger()
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
		if(kar == '-')
		{
			bNegative = true;
			kar = GetChar();
		}
		else if(kar == '+')
			kar = GetChar();
		while(ISDIGIT(kar) && bNoEOF && bNoError)
		{
			number = (number * 10) + (kar - '0');
			kar = GetChar();
		}
		if(bNoEOF && bNoError)
		{
			bNoError = Push(kar);
		}
	}
	return (bNegative) ? -number : number;
}

template <class T> unsigned TConInStream<T>::GetUnsigned()
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
		if(kar == '+')
			kar = GetChar();
		while(ISDIGIT(kar) && bNoEOF && bNoError)
		{
			number = (number * 10) + (kar - '0');
			kar = GetChar();
		}
		if(bNoEOF && bNoError)
		{
			bNoError = Push(kar);
		}
	}
	return number;
}

template <class T> bool TConInStream<T>::operator == (const T* string)
{
	const T* ptr = string;
	bool bRetVal = true;
	if(*ptr)
	{
		do
		{
			T kar = GetChar();
			if(*ptr++ != kar)
			{
				bRetVal = false;
				if(bNoEOF && bNoError)
					bNoError = Push(kar);
				break;
			}
		}
		while(*ptr && bNoEOF && bNoError);
	}
	return (bNoEOF && bNoError) ? bRetVal : false;
}

template <class T> bool TConInStream<T>::operator == (T kar)
{
	bool bRetVal = true;
	if(kar != GetChar())
	{
		bRetVal = false;
		if(bNoEOF && bNoError)
			bNoError = Push(kar);
	}
	return (bNoEOF && bNoError) ? bRetVal : false;
}

#ifdef UNICODE
typedef TConOutStream<wchar_t> ConsoleOut;
typedef TConErrStream<wchar_t> ConsoleErr;
typedef TConInStream<wchar_t> ConsoleIn;
#else
typedef TConOutStream<char> ConsoleOut;
typedef TConErrStream<char> ConsoleErr;
typedef TConInStream<char> ConsoleIn;
#endif

typedef TConOutStream<char> ConsoleOutA;
typedef TConErrStream<char> ConsoleErrA;
typedef TConInStream<char> ConsoleInA;
#endif //WCONSOLE_H
