#ifndef WEXCEPT_H
#define WEXCEPT_H
#include "bexcept.h"

class TWinException : public TException
{
public:
	TWinException(const char* msg) : TException(msg)
	{
		char* MsgBuf = 0;
		if(FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						  NULL,
						  GetLastError(),
						  0, // MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						  (LPSTR)&MsgBuf,
						  0,
						  NULL))
		{
			AddInfo(MsgBuf);
		}
		else
		{
			AddInfo(UNKNOWN_ERROR);
		}
		LocalFree(MsgBuf);
	}
	TWinException(const wchar_t* msg) : TException(msg)
	{
		wchar_t* MsgBuf = 0;
		if(FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						  NULL,
						  GetLastError(),
						  0, // MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						  (LPWSTR)&MsgBuf,
						  0,
						  NULL))
		{
			AddInfo(MsgBuf);
		}
		else
		{
			AddInfo(UNKNOWN_ERROR);
		}
		LocalFree(MsgBuf);
	}
};
#endif //WEXCEPT_H
