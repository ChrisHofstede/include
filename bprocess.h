#ifndef BPROCESS_H
#define BPROCESS_H
#include <windows.h>

namespace b {

typedef enum InvalidHandleType {
	null = 0, invalid = -1
} TInvalidHandleType;

template<TInvalidHandleType invalid_handle> class THandle {
protected:
	HANDLE hHandle;
public:
	THandle() :
		hHandle((HANDLE) invalid_handle) {
	}
	THandle(HANDLE handle) :
		hHandle(handle) {
	}
	THandle(const THandle& a) :
		hHandle(a.hHandle) {
	}
	~THandle() {
		Close();
	}
	void Duplicate(HANDLE handle) {
		HANDLE hProcess = GetCurrentProcess();
		if (DuplicateHandle(hProcess, handle, hProcess, &hHandle, 0, FALSE,
				DUPLICATE_SAME_ACCESS) == FALSE) {
			hHandle = (HANDLE) invalid_handle;
		}
	}
	void Close() {
		if (hHandle != (HANDLE) invalid_handle) {
			::CloseHandle(hHandle);
			hHandle = (HANDLE) invalid_handle;
		}
	}
	THandle& operator =(const THandle& a) {
		if (hHandle != (HANDLE) invalid_handle)
			::CloseHandle(hHandle);
		hHandle = a.hHandle;
		return *this;
	}
	THandle& operator =(HANDLE handle) {
		if (hHandle != (HANDLE) invalid_handle)
			::CloseHandle(hHandle);
		hHandle = handle;
		return *this;
	}
	bool operator ==(const THandle& a) const {
		return hHandle == a.hHandle;
	}
	inline operator HANDLE() const {
		return hHandle;
	}
	inline operator bool() const {
		return hHandle != (HANDLE) invalid_handle;
	}
};

class TThread: THandle<null> {
	static DWORD WINAPI Execute(void* thread) {
		return reinterpret_cast<TThread*> (thread)->Run();
	}
protected:
	volatile bool bQuit;
public:
	TThread() {
		bQuit = true;
	}
	virtual ~TThread() {
		Stop();
	}
	void Stop() {
		bQuit = true;
		if (hHandle) {
			if (WaitForSingleObject(hHandle, INFINITE) != WAIT_FAILED) {
				::CloseHandle(hHandle);
			}
			hHandle = 0;
		}
	}
	void Start() {
		Stop();
		bQuit = false;

		// CreateThread
		DWORD ThreadId;
		hHandle = ::CreateThread(0, 0, &TThread::Execute,
				reinterpret_cast<void*> (this), 0, &ThreadId);
	}
	virtual int Run() = 0;
	bool IsRunning() {
		return ((hHandle) ? (WaitForSingleObject(hHandle, 0) == WAIT_TIMEOUT)
				: false);
	}
	bool Wait(DWORD dwMilliseconds) {
		return ((hHandle) ? (WaitForSingleObject(hHandle, dwMilliseconds)
				== WAIT_TIMEOUT) : false);
	}
};

template<class T> class TMutex: THandle<null> {
public:
	TMutex(const T* szName = 0) {
		hHandle = CreateMutex(0, FALSE, szName);
	}
	bool Wait(DWORD mSecs = INFINITE) {
		return ((hHandle) ? (WaitForSingleObject(hHandle, mSecs)
				== WAIT_OBJECT_0) : false);
	}
	void Release() {
		if (hHandle)
			ReleaseMutex(hHandle);
	}
};

template<class T> class TEvent: THandle<null> {
public:
	TEvent(const T* szName = 0, BOOL bManual = TRUE) {
		hHandle = CreateEvent(0, bManual, FALSE, szName);
	}
	bool Wait(DWORD mSecs = INFINITE) {
		return ((hHandle) ? (WaitForSingleObject(hHandle, mSecs)
				== WAIT_OBJECT_0) : false);
	}
	void Set() {
		if (hHandle)
			SetEvent(hHandle);
	}
	void Reset() {
		if (hHandle)
			ResetEvent(hHandle);
	}
	void Pulse() {
		if (hHandle)
			PulseEvent(hHandle);
	}
};
}
#endif //BPROCESS_H
