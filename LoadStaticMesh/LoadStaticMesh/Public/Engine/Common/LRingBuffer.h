#pragma once

#include <thread>
#include <atomic>
#include <vector>
using namespace std;

template <typename T>
class RingBuffer
{
private:
	int MaxCount;
	T* Buffer;
	atomic<int> ReadIndex;
	atomic<int> WriteIndex;
public:
	RingBuffer() : MaxCount(51), ReadIndex(0), WriteIndex(0)
	{
		Buffer = new T[MaxCount];
		memset(Buffer, 0, sizeof(Buffer[0]) * MaxCount);
	}
	RingBuffer(int count) : MaxCount(count + 1), Buffer(NULL), ReadIndex(0), WriteIndex(0)
	{
		Buffer = new T[MaxCount];
		memset(Buffer, 0, sizeof(Buffer[0]) * MaxCount);
	}
	~RingBuffer()
	{
		delete[] Buffer;
		Buffer = 0x0;
	}
	inline void Enqueue(T value)
	{
		// We don't want to overwrite old data if the Buffer is full
		// and the writer thread is trying to add more data. In that case,
		// block the writer thread until data has been read/removed from the ring Buffer.
		while (IsFull()) { this_thread::sleep_for(500ns); }
		Buffer[WriteIndex] = value;
		WriteIndex = (WriteIndex + 1) % MaxCount;
	}
	inline bool Dequeue(T* outValue)
	{
		if (IsEmpty())
			return false;
		*outValue = Buffer[ReadIndex];
		ReadIndex = (ReadIndex + 1) % MaxCount;
		return true;
	}
	inline bool IsEmpty()
	{
		return ReadIndex == WriteIndex;
	}
	inline bool IsFull()
	{
		return ReadIndex == ((WriteIndex + 1) % MaxCount);
	}
	inline void Clear()
	{
		ReadIndex = WriteIndex = 0;
		memset(Buffer, 0, sizeof(Buffer[0]) * MaxCount);
	}
	inline int GetSize()
	{
		return abs(WriteIndex - ReadIndex);
	}
	inline int GetMaxSize()
	{
		return MaxCount;
	}
};