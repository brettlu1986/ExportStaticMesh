#pragma once
#include "stdafx.h"

#include "FDefine.h"
#include "FRenderResource.h"

class FIndexBuffer : public FRenderResource
{
public:
	FIndexBuffer();
	FIndexBuffer(E_RESOURCE_TYPE Type);
	FIndexBuffer(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);
	virtual ~FIndexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

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