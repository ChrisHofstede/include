#ifndef BMAP_H
#define BMAP_H
#include <bdeque.h>

namespace b {

template<class T, class O = T> class TPair {
public:
	TPair() {
	}
	TPair(const T& key, const O& value) {
		Key = key;
		Value = value;
	}
	T Key;
	O Value;
};

template<class T, class O = T> class TMap: TDeque<TPair<T, O> > {
public:
	typedef typename TDeque<TPair<T, O> >::TDLink TLink;
protected:
	bool Search(TLink* &ptr, const T& key);
	bool Compare(TLink* &ptr, const T& key);
	virtual int Compare(const T& a, const T& b) = 0;
public:
	virtual ~TMap() {
	}
	bool Add(const T& key, const O& value);
	bool Delete(const T& key);
	O& operator[](const T& key);
	inline TPair<T, O>& operator[](int index);
	bool Contains(const T& key);
	inline void Flush() {
		TDeque<TPair<T, O> >::Flush();
	}
	inline int Size() {
		return TDeque<TPair<T, O> >::Size();
	}
};

template<class T, class O> bool TMap<T, O>::Search(TLink* &ptr, const T& key) {
	bool bRet = false;
	if (ptr) {
		do {
			if (Compare(ptr->Object.Key, key) == 0) {
				bRet = true;
				break;
			}
			ptr = ptr->r;
		} while (ptr != TDeque<TPair<T, O> >::first);
	}
	return bRet;
}

template<class T, class O> bool TMap<T, O>::Compare(TLink* &ptr, const T& key) {
	bool bRet = false;
	if (ptr) {
		do {
			if (Compare(ptr->Object.Key, key) > 0) {
				bRet = true;
				break;
			}
			ptr = ptr->r;
		} while (ptr != TDeque<TPair<T, O> >::first);
	}
	return bRet;
}

template<class T, class O> bool TMap<T, O>::Add(const T& key, const O& value) {
	bool bRet = false;
	TLink* ptr = TDeque<TPair<T, O> >::first;
	if (!Search(ptr, key)) {
		TPair<T, O> tmp(key, value);
		if (Compare(ptr, key)) {
			bRet = TDeque<TPair<T, O> >::Insert(ptr, tmp);
		} else {
			bRet = TDeque<TPair<T, O> >::PushEnd(tmp);
		}
	} else {
		ptr->Object.Value = value;
	}
	return bRet;
}

template<class T, class O> bool TMap<T, O>::Delete(const T& key) {
	bool bRet = false;
	TLink* ptr = TDeque<TPair<T, O> >::first;
	if (Search(ptr, key)) {
		TDeque<TPair<T, O> >::Delete(ptr);
		bRet = true;
	}
	return bRet;
}

template<class T, class O> O& TMap<T, O>::operator[](const T& key) {
	TLink* ptr = TDeque<TPair<T, O> >::first;
	if (!Search(ptr, key))
		throw TIndexOutOfRange();
	return ptr->Object.Value;
}

template<class T, class O> TPair<T, O>& TMap<T, O>::operator[](int index) {
	return TDeque<TPair<T, O> >::Item(index);
}

template<class T, class O> bool TMap<T, O>::Contains(const T& key) {
	TLink* ptr = TDeque<TPair<T, O> >::first;
	return Search(ptr, key);
}

template<class T, class O = T> class TStringMap: public TMap<T, O> {
	int Compare(const T& a, const T& b) {
		return (bIgnoreCase) ? a.compare(b) : a.compare_ignore_case(b);
	}
public:
	TStringMap() {
		bIgnoreCase = false;
	}
	bool bIgnoreCase;
	T AddKeyValuePair(const T& pair);
	T GetKeyValuePair(const T& key);
};

template<class T, class O = T> T TStringMap<T, O>::AddKeyValuePair(
		const T& pair) {
	T key;
	O value;
	int index = pair.first_index('=');
	if (index != b::npos) {
		key = pair.sub_string(0, index).trim();
		value = pair.sub_string(index + 1).trim();
	} else {
		key = pair.trim();
		value.clear();
	}
	Add(key, value);
	return key;
}

template<class T, class O = T> T TStringMap<T, O>::GetKeyValuePair(
		const T& key) {
	T pair = key + '=';
	if (Contains(key)) {
		pair += operator[](key);
	}
	return pair;
}

} // End namespace
#endif // BMAP_H

