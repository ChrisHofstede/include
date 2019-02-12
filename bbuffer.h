#ifndef BBUFFER_H
#define BBUFFER_H
#include "bdeque.h"
#include "butils.h"

namespace b {

template <class T> class TMemoryBlock
{
   T* MemPtr = 0;
   int Size = 0;
   inline void CopyBlock(const T* ptr, int size)
   {
      try
      {
         Size = size;
         MemPtr = new T[Size];
         copy(MemPtr, ptr, Size);
      }
      catch(std::bad_alloc&)
      {
         Size = 0;
         MemPtr = 0;
      }
   }
public:
   TMemoryBlock(const T* ptr, int size)
   {
      CopyBlock(ptr, size);
   }
   TMemoryBlock(const TMemoryBlock& a)
   {
      CopyBlock(a.MemPtr, a.Size);
   }
   ~TMemoryBlock()
   {
      delete[] MemPtr;
   }
   TMemoryBlock& operator=(const TMemoryBlock& a)
   {
      delete[] MemPtr;
      CopyBlock(a.MemPtr, a.Size);
      return *this;
   }
   int GetSize()
   {
      return Size;
   }
   const T* GetPtr()
   {
      return MemPtr;
   }
   bool SetBlock(const T* ptr, int size)
   {
      delete[] MemPtr;
      CopyBlock(ptr, size);
      return (Size > 0);
   }
   int GetBlock(T* ptr, int size)
   {
      int len = MIN(Size, size);
      copy(ptr, MemPtr, len);
      return len;
   }
   void Clear()
   {
      Size = 0;
      delete[] MemPtr;
      MemPtr = 0;
   }
   operator bool()
   {
      return (Size > 0);
   }
};

template <class T> class TBuffer : TDeque<TMemoryBlock<T> >
{
   int ReadIndex;
   TMemoryBlock<T> Read;
public:
   TBuffer()
   {
      ReadIndex = 0;
   }
   void Flush()
   {
      TDeque<T>::Flush();
   }
   int Size();
   bool write(const T* ptr, int size);
   bool read(T* ptr, int* size);
   int find(T kar);
};

template <class T> int TBuffer<T>::Size()
{
   TDequeIterator<TMemoryBlock<T> > Iter(*((TDeque<TMemoryBlock<T> >*)this));
   int size = 0;
   while(Iter)
   {
      size += Iter++.GetSize();
   }
   size += Read.GetSize() - ReadIndex;
   return size;
}

template <class T> bool TBuffer<T>::write(const T* ptr, int size)
{
   bool bReturn = false;
   if(ptr != 0 && size > 0)
   {
      TMemoryBlock<T> tmp(ptr, size);
      if(tmp)
      {
         bReturn = PushEnd(tmp);
      }
   }
   return bReturn;
}

template <class T> bool TBuffer<T>::read(T* ptr, int* size)
{
   bool bReturn = false;
   if(ptr != 0 && *size > 0)
   {
      int karRead = 0;
      while(karRead < *size)
      {
         if(Read)
         {
            int kars = MIN(Read.GetSize() - ReadIndex, *size - karRead);
            if(kars > 0)
            {
               copy(&ptr[karRead], &Read.GetPtr()[ReadIndex], kars);
               karRead += kars;
               ReadIndex += kars;
               bReturn = true;
            }
            if(ReadIndex >= Read.GetSize())
            {
               Read.Clear();
               ReadIndex = 0;
            }
         }
         else
         {
            if(PopBegin(Read))
            {
               ReadIndex = 0;
            }
            else
            {
               break;
            }
         }
      }
      *size = karRead;
   }
   return bReturn;
}

template <class T> int TBuffer<T>::find(T kar)
{
   int iReturn = -1;
   int pos = 0;
   int cCount;
   const T* ptr;
   if(Read)
   {
      cCount = Read.GetSize();
      ptr = Read.GetPtr();
      for(int i = ReadIndex; i < cCount; i++)
      {
         if(ptr[i] == kar)
         {
            iReturn = i - ReadIndex;
            break;
         }
      }
      pos = cCount - ReadIndex;
   }
   if(iReturn < 0)
   {
      TDequeIterator<TMemoryBlock<T> > Iter(*((TDeque<TMemoryBlock<T> >*)this));
      while(Iter)
      {
         TMemoryBlock<T>& tmp = Iter++;
         cCount = tmp.GetSize();
         ptr = tmp.GetPtr();
         for(int i = 0; i < cCount; i++)
         {
            if(ptr[i] == kar)
            {
               iReturn = pos + i;
               break;
            }
         }
         pos += cCount;
         if(iReturn >= 0)
         {
            break;
         }
      }
   }
   return iReturn;
}

}
#endif //BBUFFER_H
