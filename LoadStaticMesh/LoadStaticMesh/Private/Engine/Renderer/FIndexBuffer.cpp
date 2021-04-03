
#include "FIndexBuffer.h"

FIndexBuffer::FIndexBuffer()
:IndicesCount(0)
, IndicesByteSize(0)
, IndicesType(E_INDEX_TYPE::TYPE_UINT_16)
, IndicesData(nullptr)
{

}

void FIndexBuffer::Init(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData)
{
	IndicesCount = InCount;
	IndicesByteSize = InByteSize;
	IndicesType = InType;

	IndicesData = (void*)(new char[IndicesByteSize]);
	memset(IndicesData, 0, IndicesByteSize);
	memcpy(IndicesData, InData, IndicesByteSize);
}

FIndexBuffer::FIndexBuffer(UINT InCount, UINT InByteSize, E_INDEX_TYPE IndexType, void* InData)
{
	Init(InCount, InByteSize, IndexType, InData);
}

FIndexBuffer::~FIndexBuffer()
{
	delete[] IndicesData;
	IndicesData = nullptr;
}