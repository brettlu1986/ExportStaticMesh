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
	
};