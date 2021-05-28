#pragma once
#include "stdafx.h"

#include "LDefine.h"
#include "FRenderResource.h"

class FIndexBuffer : public FRenderResource
{
public:
	FIndexBuffer();
	FIndexBuffer(E_FRESOURCE_TYPE Type);
	FIndexBuffer(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);
	virtual ~FIndexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void Init(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);


	UINT GetIndicesCount() const 
	{
		return IndicesCount;
	}

protected:
	UINT IndicesCount;
	UINT IndicesByteSize;
	E_INDEX_TYPE IndicesType;
	void* IndicesData;
};