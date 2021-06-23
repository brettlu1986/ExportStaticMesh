#pragma once

#include "pch.h"
#include "LResource.h"

class LIndexBuffer : public LResource
{
public:
	LIndexBuffer(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);
	virtual ~LIndexBuffer();

	UINT GetIndicesCount() { return IndicesCount; }
	UINT GetIndicesDataSize() { return IndicesByteSize; }
	E_INDEX_TYPE GetIndicesType() {
		return IndicesType;
	}

	const void* GetIndicesData()
	{
		return IndicesData;
	}

private:
	UINT IndicesCount;
	UINT IndicesByteSize;
	E_INDEX_TYPE IndicesType;
	void* IndicesData;
};