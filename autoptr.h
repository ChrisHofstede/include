#ifndef AUTOPTR_H
#define AUTOPTR_H

class exception: public std::exception {
	const char* message;
public:
	exception(const char* msg) :
		message(msg) {
	}
};

template<typename T> class autoptr {
protected:
	T* pointer;
public:
	autoptr() :
		pointer(0) {
	}
	explicit autoptr(T* ptr) {
		pointer = ptr;
	}
	~autoptr() {
		delete pointer;
	}
	autoptr& operator =(T* ptr) {
		delete pointer;
		pointer = ptr;
		return *this;
	}
	autoptr& operator =(const autoptr& ptr) {
		delete pointer;
		pointer = ptr;
		return *this;
	}
	T** operator &() {
		delete pointer;
		pointer = 0;
		return &pointer;
	}
	operator bool() {
		return pointer != 0;
	}
	operator T*() const {
		if (!pointer)
			throw exception("Null auto pointer");
		return pointer;
	}
	bool operator ==(autoptr& a) const {
		return pointer == a.pointer;
	}
	T* operator ->() const {
		if (!pointer)
			throw exception("Null auto pointer");
		return pointer;
	}
};
#endif //AUTOPTR_H
