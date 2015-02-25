#ifndef BLIST_H
#define BLIST_H
#include <bdeque.h>

namespace b {

template <class T> class TList : TDeque<T>
{
	bool Search(typename TDeque<T>::TDLink* &ptr, const T& key);
	bool Compare(typename TDeque<T>::TDLink* &ptr, const T& key);
	virtual int Compare(const T& a, const T& b) = 0;
public:
	typedef TDequeIterator<T> Iterator;
	virtual ~TList() {}
	bool Add(const T& key);
	bool Delete(const T& key);
	T& operator[](int index)
	{
		return TDeque<T>::Item(index);
	}
	T& Item(int index)
	{
		return TDeque<T>::Item(index);
	}
	bool Contains(const T& key);
	inline void Flush()
	{
		TDeque<T>::Flush();
	}
	inline int Size()
	{
		return TDeque<T>::Size();
	}
	inline bool IsEmpty()
	{
		return TDeque<T>::IsEmpty();
	}
	inline TDequeIterator<T> GetIterator()
	{
		return TDeque<T>::GetIterator();
	}
};

template <class T> bool TList<T>::Search(typename TDeque<T>::TDLink* &ptr, const T& key)
{
	bool bRet = false;
	if(ptr)
	{
		do
		{
			if(Compare(ptr->Object, key) == 0)
			{
				bRet = true;
				break;
			}
			ptr = ptr->r;
		}
		while(ptr != TDeque<T>::first);
	}
	return bRet;
}

template <class T> bool TList<T>::Compare(typename TDeque<T>::TDLink* &ptr, const T& key)
{
	bool bRet = false;
	if(ptr)
	{
		do
		{
			if(Compare(ptr->Object, key) > 0)
			{
				bRet = true;
				break;
			}
			ptr = ptr->r;
		}
		while(ptr != TDeque<T>::first);
	}
	return bRet;
}

template <class T> bool TList<T>::Add(const T& key)
{
	bool bRet = false;
	typename TDeque<T>::TDLink* ptr = TDeque<T>::first;
	if(!Search(ptr, key))
	{
		if(Compare(ptr, key))
		{
			bRet = TDeque<T>::Insert(ptr, key);
		}
		else
		{
			bRet = TDeque<T>::PushEnd(key);
		}
	}
	return bRet;
}

template <class T> bool TList<T>::Delete(const T& key)
{
	bool bRet = false;
	typename TDeque<T>::TDLink* ptr = TDeque<T>::first;
	if(Search(ptr, key))
	{
		TDeque<T>::Delete(ptr);
		bRet = true;
	}
	return bRet;
}

template <class T> bool TList<T>::Contains(const T& key)
{
	typename TDeque<T>::TDLink* ptr = TDeque<T>::first;
	return Search(ptr, key);
}

template <class T> class TStringList : public TList<T>
{
	int Compare(const T& a, const T& b)
	{
//		return (bIgnoreCase) ? a.compare(b)
//							 : a.compare_ignore_case(b);
		return (bIgnoreCase) ? b::strcmp(a.c_str(), b.c_str())
							 : b::stricmp(a.c_str(), b.c_str());
	}
public:
	TStringList()
	{
		bIgnoreCase = false;
	}
	bool bIgnoreCase;
};

template <class T> class TNumberList : public TList<T>
{
	int Compare(const T& a, const T& b)
	{
		return a - b;
	}
public:
};
} // End namespace
#endif //BLIST_H
