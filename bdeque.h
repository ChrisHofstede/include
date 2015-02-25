#ifndef BDEQUE_H
#define BDEQUE_H
#include <bexcept.h>

namespace b {

template <class T> class TDequeIterator;

template <class T> class TDeque
{
protected:
	struct dlink
	{
		dlink(const T& a) : r(0), l(0), Object(a)  {}
		dlink* r;
		dlink* l;
		T Object;
	};
public:
	typedef struct dlink TDLink;
protected:
	TDLink* first; // Points to the first object
	void Copy(const TDeque& a);
	bool Insert(TDLink* ptr, const T& object);
	bool Append(TDLink* ptr, const T& object);
	void Delete(TDLink* ptr);
public:
	TDeque()
	{
		first = 0;
	}
	TDeque(const TDeque& a)
	{
		first = 0;
		Copy(a);
	}
	~TDeque()
	{
		Flush();
	}
	bool PushBegin(const T& object);
	bool PushEnd(const T& object);
	bool PopBegin(T& object);
	bool PopEnd(T& object);
	bool Insert(int index, const T& object);
	bool Append(int index, const T& object);
	void Delete(int index);
	void Flush();
	TDeque<T>& operator = (const TDeque& a)
	{
		Flush();
		Copy(a);
		return *this;
	}
	int Size() const;
	bool IsEmpty()
	{
		return first == 0;
	}
	inline T& Item(int index)
	{
		return operator[](index);
	}
	T& operator[](int index);
	T& First();
	T& Last();
	friend class TDequeIterator<T>;
	inline TDequeIterator<T> GetIterator()
	{
		return TDequeIterator<T>(*this);
	}
};

template <class T> bool TDeque<T>::PushEnd(const T& object)
{
	try
	{
		TDLink* ptr = new TDLink(object);
		if(first)
		{
			ptr->r = first;
			ptr->l = first->l;
			(first->l)->r = ptr;
			first->l = ptr;
		}
		else
		{
			ptr->r = ptr;
			ptr->l = ptr;
			first = ptr;
		}
	}
	catch(std::bad_alloc&)
	{
		return false;
	}
	return true;
}

template <class T> bool TDeque<T>::PushBegin(const T& object)
{
	try
	{
		TDLink* ptr = new TDLink(object);
		if(first)
		{
			ptr->r = first;
			ptr->l = first->l;
			(first->l)->r = ptr;
			first->l = ptr;
		}
		else
		{
			ptr->r = ptr;
			ptr->l = ptr;
		}
		first = ptr;
	}
	catch(std::bad_alloc&)
	{
		return false;
	}
	return true;
}

template <class T> bool TDeque<T>::PopEnd(T& object)
{
	if(first)
	{
		TDLink* ptr = first->l;
		object = ptr->Object;
		if(first == first->r || first == first->l)
		{
			first = 0;
		}
		else
		{
			(ptr->r)->l = ptr->l;
			(ptr->l)->r = ptr->r;
		}
		delete ptr;
		return true;
	}
	return false;
}

template <class T> bool TDeque<T>::PopBegin(T& object)
{
	if(first)
	{
		TDLink* ptr = first;
		object = ptr->Object;
		if(first == first->r || first == first->l)
		{
			first = 0;
		}
		else
		{
			(ptr->r)->l = ptr->l;
			(ptr->l)->r = ptr->r;
			first = ptr->r;
		}
		delete ptr;
		return true;
	}
	return false;
}

template <class T> void TDeque<T>::Copy(const TDeque& a)
{
	if(a.first)
	{
		TDLink* ptr = a.first;
		do
		{
			PushEnd(ptr->Object);
			ptr = ptr->r;
		}
		while(ptr != a.first);
	}
}

template <class T> bool TDeque<T>::Insert(TDLink* ptr, const T& object)
{
	try
	{
		TDLink* tmp = new TDLink(object);
		tmp->r = ptr;
		tmp->l = ptr->l;
		(ptr->l)->r = tmp;
		ptr->l = tmp;
		if(ptr == first)
			first = tmp;
	}
	catch(std::bad_alloc&)
	{
		return false;
	}
	return true;
}

template <class T> bool TDeque<T>::Insert(int index, const T& object)
{
	TDLink* IndexPtr = first;
	if(first && index >= 0)
	{
		while(index > 0)
		{
			index--;
			IndexPtr = IndexPtr->r;
			if(IndexPtr == first)
				throw TIndexOutOfRange();
		};
	}
	else
		throw TIndexOutOfRange();
	return Insert(IndexPtr, object);
}

template <class T> bool TDeque<T>::Append(TDLink* ptr, const T& object)
{
	try
	{
		TDLink* tmp = new TDLink(object);
		tmp->r = ptr->r;
		tmp->l = ptr;
		(ptr->r)->l = tmp;
		ptr->r = tmp;
	}
	catch(std::bad_alloc&)
	{
		return false;
	}
	return true;
}

template <class T> bool TDeque<T>::Append(int index, const T& object)
{
	TDLink* IndexPtr = first;
	if(first && index >= 0)
	{
		while(index > 0)
		{
			index--;
			IndexPtr = IndexPtr->r;
			if(IndexPtr == first)
				throw TIndexOutOfRange();
		};
	}
	else
		throw TIndexOutOfRange();
	return Append(IndexPtr, object);
}

template <class T> void TDeque<T>::Delete(TDLink* ptr)
{
	if(first == first->r || first == first->l)
	{
		first = 0;
	}
	else
	{
		(ptr->r)->l = ptr->l;
		(ptr->l)->r = ptr->r;
		if(ptr == first)
			first = ptr->r;
	}
	delete ptr;
}

template <class T> void TDeque<T>::Delete(int index)
{
	TDLink* IndexPtr = first;
	if(first && index >= 0)
	{
		while(index > 0)
		{
			index--;
			IndexPtr = IndexPtr->r;
			if(IndexPtr == first)
				throw TIndexOutOfRange();
		};
	}
	else
		throw TIndexOutOfRange();
	Delete(IndexPtr);
}

template <class T> void TDeque<T>::Flush()
{
	if(first)
	{
		TDLink* ptr = first;
		do
		{
			TDLink* next = ptr->r;
			delete ptr;
			ptr = next;
		}
		while(ptr != first);
	}
	first = 0;
}

template <class T> int TDeque<T>::Size() const
{
	int cCount = 0;
	if(first)
	{
		TDLink* ptr = first;
		do
		{
			cCount++;
			ptr = ptr->r;
		}
		while(ptr != first);
	}
	return cCount;
}

template <class T> T& TDeque<T>::operator[](int index)
{
	TDLink* IndexPtr = first;
	if(first && index >= 0)
	{
		while(index > 0)
		{
			index--;
			IndexPtr = IndexPtr->r;
			if(IndexPtr == first)
				throw TIndexOutOfRange();
		};
	}
	else
		throw TIndexOutOfRange();
	return IndexPtr->Object;
}

template <class T> T& TDeque<T>::First()
{
	if(!first)
		throw TIndexOutOfRange();
	return first->Object;
}

template <class T> T& TDeque<T>::Last()
{
	if(!first)
		throw TIndexOutOfRange();
	return (first->l)->Object;
}

template <class T> class TDequeIterator
{
	TDeque<T>* deque;
	typename TDeque<T>::TDLink* current;
public:
	TDequeIterator(TDeque<T>& a)
	{
		deque = &a;
		current = deque->first;
	}
	TDequeIterator(const TDequeIterator& a)
	{
		deque = a.deque;
		current = a.current;
	}
	bool operator == (const TDequeIterator& a)
	{
		return current == a.current;
	}
	void Begin()
	{
		current = deque->first;
	}
	void Last()
	{
		current = (deque->first) ?  (deque->first)->l : 0;
	}
	TDequeIterator& operator = (const TDequeIterator& a);
	operator bool()
	{
		return (current != 0);
	}
	operator T&();
	T& operator ++(int);
	T& operator --(int);
	void Delete();
};

template <class T> TDequeIterator<T>& TDequeIterator<T>::operator = (const TDequeIterator<T>& a)
{
	deque = a.deque;
	current = a.current;
	return *this;
}

template <class T> TDequeIterator<T>::operator T&()
{
	if(!current)
		throw TIndexOutOfRange();
	return current->Object;
}

template <class T> T& TDequeIterator<T>::operator ++(int)
{
	if(!current)
		throw TIndexOutOfRange();
	typename TDeque<T>::TDLink* tmp = current;
	current = current->r;
	if(current == deque->first)
		current = 0;
	return tmp->Object;
}

template <class T> T& TDequeIterator<T>::operator --(int)
{
	if(!current)
		throw TIndexOutOfRange();
	typename TDeque<T>::TDLink* tmp = current;
	current = (current == deque->first) ? 0 : current->l;
	return tmp->Object;
}

template <class T> void TDequeIterator<T>::Delete()
{
	if(!current)
		throw TIndexOutOfRange();
	typename TDeque<T>::TDLink* tmp = current;
	current = current->r;
	if(current == deque->first)
		current = 0;
	deque->Delete(tmp);
}

template <class T> class TPtrDequeIterator;

template <class T> class TPtrDeque
{
protected:
	struct dlink
	{
		dlink(T* a) : r(0), l(0), ObjectPtr(a){}
		dlink* r;
		dlink* l;
		T* ObjectPtr;
	};
public:
	typedef struct dlink TDLink;
protected:
	TDLink* first; // Points to the first object
	void Copy(const TPtrDeque& a);
	bool Insert(TDLink* ptr, T* object);
	bool Append(TDLink* ptr, T* object);
	void Delete(TDLink* ptr);
public:
	TPtrDeque()
	{
		first = 0;
	}
	TPtrDeque(const TPtrDeque& a)
	{
		first = 0;
		Copy(a);
	}
	~TPtrDeque()
	{
		Flush();
	}
	bool PushBegin(T* object);
	bool PushEnd(T* object);
	bool PopBegin(T** object);
	bool PopEnd(T** object);
	bool Insert(int index, T* object);
	bool Append(int index, T* object);
	void Delete(int index);
	void Flush();
	TPtrDeque<T>& operator = (const TPtrDeque& a)
	{
		Flush();
		Copy(a);
		return *this;
	}
	int Size();
	bool IsEmpty()
	{
		return first == 0;
	}
	inline T* Item(int index)
	{
		return operator[](index);
	}
	T* operator[](int index);
	T* First();
	T* Last();
	friend class TPtrDequeIterator<T>;
	inline TPtrDequeIterator<T> GetIterator()
	{
		return TPtrDequeIterator<T>(*this);
	}
};

template <class T> bool TPtrDeque<T>::PushEnd(T* object)
{
	try
	{
		TDLink* ptr = new TDLink(object);
		if(first)
		{
			ptr->r = first;
			ptr->l = first->l;
			(first->l)->r = ptr;
			first->l = ptr;
		}
		else
		{
			ptr->r = ptr;
			ptr->l = ptr;
			first = ptr;
		}
	}
	catch(std::bad_alloc&)
	{
		return false;
	}
	return true;
}

template <class T> bool TPtrDeque<T>::PushBegin(T* object)
{
	try
	{
		TDLink* ptr = new TDLink(object);
		if(first)
		{
			ptr->r = first;
			ptr->l = first->l;
			(first->l)->r = ptr;
			first->l = ptr;
		}
		else
		{
			ptr->r = ptr;
			ptr->l = ptr;
		}
		first = ptr;
	}
	catch(std::bad_alloc&)
	{
		return false;
	}
	return true;
}

template <class T> bool TPtrDeque<T>::PopEnd(T** object)
{
	if(first)
	{
		TDLink* ptr = first->l;
		*object = ptr->ObjectPtr;
		if(first == first->r || first == first->l)
		{
			first = 0;
		}
		else
		{
			(ptr->r)->l = ptr->l;
			(ptr->l)->r = ptr->r;
		}
		delete ptr;
		return true;
	}
	return false;
}

template <class T> bool TPtrDeque<T>::PopBegin(T** object)
{
	if(first)
	{
		TDLink* ptr = first;
		*object = ptr->ObjectPtr;
		if(first == first->r || first == first->l)
		{
			first = 0;
		}
		else
		{
			(ptr->r)->l = ptr->l;
			(ptr->l)->r = ptr->r;
			first = ptr->r;
		}
		delete ptr;
		return true;
	}
	return false;
}

template <class T> void TPtrDeque<T>::Copy(const TPtrDeque& a)
{
	if(a.first)
	{
		TDLink* ptr = a.first;
		do
		{
			PushEnd(ptr->ObjectPtr);
			ptr = ptr->r;
		}
		while(ptr != a.first);
	}
}

template <class T> bool TPtrDeque<T>::Insert(TDLink* ptr, T* object)
{
	try
	{
		TDLink* tmp = new TDLink(object);
		tmp->r = ptr;
		tmp->l = ptr->l;
		(ptr->l)->r = tmp;
		ptr->l = tmp;
		if(ptr == first)
			first = tmp;
	}
	catch(std::bad_alloc&)
	{
		return false;
	}
	return true;
}

template <class T> bool TPtrDeque<T>::Insert(int index, T* object)
{
	TDLink* IndexPtr = first;
	if(first && index >= 0)
	{
		while(index > 0)
		{
			index--;
			IndexPtr = IndexPtr->r;
			if(IndexPtr == first)
				throw TIndexOutOfRange();
		};
	}
	else
		throw TIndexOutOfRange();
	return Insert(IndexPtr, object);
}

template <class T> bool TPtrDeque<T>::Append(TDLink* ptr, T* object)
{
	try
	{
		TDLink* tmp = new TDLink(object);
		tmp->r = ptr->r;
		tmp->l = ptr;
		(ptr->r)->l = tmp;
		ptr->r = tmp;
	}
	catch(std::bad_alloc&)
	{
		return false;
	}
	return true;
}


template <class T> bool TPtrDeque<T>::Append(int index, T* object)
{
	TDLink* IndexPtr = first;
	if(first && index >= 0)
	{
		while(index > 0)
		{
			index--;
			IndexPtr = IndexPtr->r;
			if(IndexPtr == first)
				throw TIndexOutOfRange();
		};
	}
	else
		throw TIndexOutOfRange();
	return Append(IndexPtr, object);
}

template <class T> void TPtrDeque<T>::Delete(TDLink* ptr)
{
	if(first == first->r || first == first->l)
	{
		first = 0;
	}
	else
	{
		(ptr->r)->l = ptr->l;
		(ptr->l)->r = ptr->r;
		if(ptr == first)
			first = ptr->r;
	}
	delete ptr;
}

template <class T> void TPtrDeque<T>::Delete(int index)
{
	TDLink* IndexPtr = first;
	if(first && index >= 0)
	{
		while(index > 0)
		{
			index--;
			IndexPtr = IndexPtr->r;
			if(IndexPtr == first)
				throw TIndexOutOfRange();
		};
	}
	else
		throw TIndexOutOfRange();
	Delete(IndexPtr);
}

template <class T> void TPtrDeque<T>::Flush()
{
	if(first)
	{
		TDLink* ptr = first;
		do
		{
			TDLink* next = ptr->r;
			//delete ptr->ObjectPtr; // Prevents memory leaks
			delete ptr;
			ptr = next;
		}
		while(ptr != first);
	}
	first = 0;
}

template <class T> int TPtrDeque<T>::Size()
{
	int cCount = 0;
	if(first)
	{
		TDLink* ptr = first;
		do
		{
			cCount++;
			ptr = ptr->r;
		}
		while(ptr != first);
	}
	return cCount;
}

template <class T> T* TPtrDeque<T>::operator[](int index)
{
	TDLink* IndexPtr = first;
	if(first && index >= 0)
	{
		while(index > 0)
		{
			index--;
			IndexPtr = IndexPtr->r;
			if(IndexPtr == first)
				throw TIndexOutOfRange();
		};
	}
	else
		throw TIndexOutOfRange();
	return IndexPtr->ObjectPtr;
}

template <class T> T* TPtrDeque<T>::First()
{
	if(!first)
		throw TIndexOutOfRange();
	return first->ObjectPtr;
}

template <class T> T* TPtrDeque<T>::Last()
{
	if(!first)
		throw TIndexOutOfRange();
	return (first->l)->ObjectPtr;
}

template <class T> class TPtrDequeIterator
{
	TPtrDeque<T>* deque;
	typename TPtrDeque<T>::TDLink* current;
public:
	TPtrDequeIterator(TPtrDeque<T>& a)
	{
		deque = &a;
		current = deque->first;
	}
	TPtrDequeIterator(const TPtrDequeIterator& a)
	{
		deque = a.deque;
		current = a.current;
	}
	bool operator == (const TPtrDequeIterator& a)
	{
		return current == a.current;
	}
	void Begin()
	{
		current = deque->first;
	}
	void Last()
	{
		current = (deque->first) ?  (deque->first)->l : 0;
	}
	TPtrDequeIterator& operator = (const TPtrDequeIterator& a);
	operator bool()
	{
		return (current != 0);
	}
	operator T*();
	T* operator ++(int);
	T* operator --(int);
	void Delete();
};

template <class T> TPtrDequeIterator<T>& TPtrDequeIterator<T>::operator = (const TPtrDequeIterator<T>& a)
{
	deque = a.deque;
	current = a.current;
	return *this;
}

template <class T> TPtrDequeIterator<T>::operator T*()
{
	if(!current)
		throw TIndexOutOfRange();
	return current->ObjectPtr;
}

template <class T> T* TPtrDequeIterator<T>::operator ++(int)
{
	if(!current)
		throw TIndexOutOfRange();
	typename TPtrDeque<T>::TDLink* tmp = current;
	current = current->r;
	if(current == deque->first)
		current = 0;
	return tmp->ObjectPtr;
}

template <class T> T* TPtrDequeIterator<T>::operator --(int)
{
	if(!current)
		throw TIndexOutOfRange();
	typename TPtrDeque<T>::TDLink* tmp = current;
	current = (current == deque->first) ? 0 : current->l;
	return tmp->ObjectPtr;
}

template <class T> void TPtrDequeIterator<T>::Delete()
{
	if(!current)
		throw TIndexOutOfRange();
	typename TPtrDeque<T>::TDLink* tmp = current;
	current = current->r;
	if(current == deque->first)
		current = 0;
	deque->Delete(tmp);
}

} // End namespace
#endif //BDEQUE_H
