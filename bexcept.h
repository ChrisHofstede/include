#ifndef BEXCEPT_H
#define BEXCEPT_H
#include <exception>
#include <errno.h>
#include <butils.h>

// Defines
#define NEWLINE			":\n"
#define UNKNOWN_ERROR	"Unknown error"

// Prototypes

// Classes
class TException: public std::exception {
	char* message;
	inline char* WideToAnsi(const wchar_t* string) {
		int nCount = -1;
		return b::WideToAnsi<char>(string, nCount);
	}
protected:
	void AddInfo(const char* info) {
		char* new_message = new char[b::strlen(message) + sizeof(NEWLINE)
				+ b::strlen(info)];
		if (new_message) {
			b::strcpy(new_message, message);
			b::strcat(new_message, NEWLINE);
			b::strcat(new_message, info);
			delete[] message;
			message = new_message;
		}
	}
	void AddInfo(const wchar_t* info) {
		char* msg_info = WideToAnsi(info);
		if (msg_info) {
			AddInfo(msg_info);
			delete[] msg_info;
		}
	}
public:
	TException(const char* msg) {
		message = b::strnewdup(msg);
	}
	TException(const wchar_t* msg) {
		message = WideToAnsi(msg);
	}
	TException(const TException& e) {
		message = b::strnewdup(e.message);
	}
	virtual ~TException() throw() {
		delete[] message;
	}
	TException& operator =(TException& e) {
		delete[] message;
		message = b::strnewdup(e.message);
		return *this;
	}
	const char* what() const throw() {
		return message;
	}
};

class TRtlException: public TException {
public:
	TRtlException(const char* msg) :
		TException(msg) {
		AddInfo(strerror(errno));
	}
};

class TIndexOutOfRange : public TException {
public:
	TIndexOutOfRange() : TException((const char*)"Index out of range") {
	}
	TIndexOutOfRange(const TIndexOutOfRange& e) : TException((TException&)e) {
	}
};
#endif //BEXCEPT_H
