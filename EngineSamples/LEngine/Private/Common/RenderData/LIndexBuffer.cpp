
#include "pch.h"
#include "LIndexBuffer.h"

LIndexBuffer::LIndexBuffer(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData)
:LResource(E_LRESOURCE_TYPE::L_TYPE_INDEX_BUFFER)
,IndicesCount(InCount)
,IndicesByteSize(InByteSize)
,IndicesData(nullptr)
,IndicesType(InType)
{
	IndicesData = (void*)(new char[IndicesByteSize]);
	memset(IndicesData, 0, IndicesByteSize);
	memcpy(IndicesData, InData, IndicesByteSize);
}

LIndexBuffer::~LIndexBuffer()
{
	delete[] IndicesData;
	IndicesData = nullptr;
}

