#pragma once

#include "stdafx.h"
#include "LResource.h"

class LIndexBuffer : public LResource
{
public:
	LIndexBuffer(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);
	virtual ~LIndexBuffer();

	virtual void InitRenderResource() override;
	virtual void DestroyRenderResource() override;

private: 

	UINT IndicesCount;
	UINT IndicesByteSize;
	E_INDEX_TYPE IndicesType;
	void* IndicesData;
};