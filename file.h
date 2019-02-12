#ifndef FILE_H
#define FILE_H
#include <windows.h>
#include "bstring.h"

namespace b {

template<class T> class TFile {
	T* file_name;
public:
	static const T separator[];
	TFile() :
			file_name(0) {
	}
	TFile(const T* file) {
		file_name = strnewdup(file);
	}
	TFile(const T* parent, const T* child);
	TFile(const TFile& e) {
		file_name = strnewdup(e.file_name);
	}
	~TFile() {
		delete[] file_name;
	}
	TFile& operator =(T* ptr) {
		delete[] file_name;
		file_name = strnewdup(ptr);
		return *this;
	}
	operator bool() {
		return file_name;
	}
	operator const T*() {
		return file_name;
	}
	T** operator &() {
		delete[] file_name;
		file_name = 0;
		return &file_name;
	}
	T* GetBuffer(int length = MAX_PATH) {
		delete[] file_name;
		file_name = new T[length];
		return file_name;
	}
	const T* GetPath() {
		return file_name;
	}
	const T* GetName();
	TFile<T> GetParent();
	static bool Exists(const T* file) {
		return GetFileAttributes(file) != INVALID_FILE_ATTRIBUTES;
	}
	inline bool Exists() {
		return Exists(file_name);
	}
	bool IsDirectory() {
		return GetFileAttributes(file_name) & FILE_ATTRIBUTE_DIRECTORY;
	}
	bool Delete() {
		return DeleteFile(file_name);
	}
	bool Copy(const T* dest) {
		return CopyFile(file_name, dest);
	}
	bool Move(const T* dest) {
		return MoveFile(file_name, dest);
	}
	bool MakeDirectory() {
		return CreateDirectory(file_name, 0);
	}
	bool DeleteDirectory() {
		return RemoveDirectory(file_name);
	}
	bool MakeDirectories();
};

template<class T> const T TFile<T>::separator[] = { T('\\'), T('\0') };

template<class T> TFile<T>::TFile(const T* parent, const T* child) {
	int len = strlen<T>(parent);
	bool add_separator = len > 0 && parent[len - 1] != separator[0];
	len += (add_separator) ? 2 : 1;
	file_name = new T[len + strlen<T>(child)];
	strcpy<T>(file_name, parent);
	if (add_separator) {
		strcat<T>(file_name, separator);
	}
	strcat<T>(file_name, child);
}

template<class T> const T* TFile<T>::GetName() {
	const T* ptr = b::strrchr(file_name, separator[0]);
	return (ptr) ? ptr + 1 : file_name;
}

template<class T> TFile<T> TFile<T>::GetParent() {
	TFile<T> Parent;
	const T* ptr = b::strrchr(file_name, separator[0]);
	if (ptr) {
		int len = ptr - file_name;
		Parent.file_name = new T[len + 1];
		b::copy(Parent.file_name, file_name, len);
		Parent.file_name[len] = 0;
	}
	return Parent;
}

template<class T> bool TFile<T>::MakeDirectories() {
	ASSERT(file_name);
	TString<T> path(file_name);
	TString<T> dir_path;
	dir_path.set_buffer(path.length() + 1); // Include ending zero and separator
	dir_path[0] = 0;
	T* ptr = b::strtok(path, separator);
	while (ptr) {
		b::strcat(dir_path, ptr);
		b::strcat(dir_path, separator);
		DWORD dwResult = GetFileAttributes(dir_path);
		if (dwResult == INVALID_FILE_ATTRIBUTES) {
			if (!CreateDirectory(dir_path, 0)) {
				break;
			}
		} else {
			if ((dwResult & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				break;
			}
		}
		ptr = b::strtok((T*) 0, separator);
	}
	return IsDirectory();
}

#ifdef UNICODE
typedef TFile<wchar_t> File;
#else
typedef TFile<char> File;
#endif

typedef TFile<char> FileA;
typedef TFile<wchar_t> FileW;
}
#endif // FILE_H
