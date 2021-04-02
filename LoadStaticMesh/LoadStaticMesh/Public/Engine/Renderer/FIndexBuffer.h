#pragma once
#include "stdafx.h"

#include "FDefine.h"

class FIndexBuffer
{
public:
	FIndexBuffer();
	FIndexBuffer(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);
	~FIndexBuffer();

	void Init(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);

	E_INDEX_TYPE GetIndicesType() const 
	{
		return IndicesType;
	}

	UINT GetIndicesCount() const 
	{
		return IndicesCount;
	}

	UINT GetIndicesDataSize() const
	{
		return IndicesByteSize;
	}

	void* GetIndicesData() const 
	{
		return IndicesData;
	}

private:
	UINT IndicesCount;
	UINT IndicesByteSize;
	E_INDEX_TYPE IndicesType;
	void* IndicesData;
};