#ifndef BASSERT_H
#define BASSERT_H

// ASSERT & VERIFY MACROS
#ifdef _DEBUG
//#include <bstrstre.h>

#ifdef __CONSOLE__
//#include <wconsole.h>
#define FAILMSG(msg)                          	\
		cerr << msg.c_str();                  	\
	}                                         	\
	exit(EXIT_FAILURE);
#else
#define FAILMSG(msg)                          	\
	MessageBoxA(GetActiveWindow(), msg.c_str(),	\
	"Assertion Failed", MB_OK | MB_ICONERROR);	\
	}                                         	\
	DebugBreak();
#endif

// Put up an assertion failure message box.
#define ASSERTFAIL(file, line, func, expr) {  	\
	{                                         	\
		b::StrStreamA msg;                    	\
		msg << "File: " << file << b::endl;   	\
		msg << "Line: " << line << b::endl;   	\
		msg << "Func: " << func << b::endl;   	\
		msg << "Expr: " << expr;              	\
		FAILMSG(msg);                         	\
}
#endif

// Put up a message box if an assertion fails in a debug build.
#ifdef _DEBUG
#define ASSERT(x) if(!(x)) ASSERTFAIL(__FILE__, __LINE__, __FUNC__, #x)
#else
#define ASSERT(x)
#endif

// Assert in debug builds, but don't remove the code in retail builds.
#ifdef _DEBUG
#define VERIFY(x) ASSERT(x)
#else
#define VERIFY(x) (x)
#endif

// Message during debug builds
#ifdef _DEBUG
#ifdef __CONSOLE__
#define MSGBOX(szMSG) {                          \
	cerr << szMSG << endl;                       \
}
#else
#define MSGBOX(szMSG) {                          \
	MessageBoxA(GetActiveWindow(), szMSG,        \
	"Info Message", MB_OK | MB_ICONINFORMATION); \
}
#endif
#else
#define MSGBOX(szMsg)
#endif

#endif //BASSERT_H

