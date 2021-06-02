#pragma once
#include "stdafx.h"

#include "FRenderResource.h"

class FIndexBuffer : public FRenderResource
{
public:
	FIndexBuffer();
	FIndexBuffer(E_FRESOURCE_TYPE Type);
	virtual ~FIndexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

	UINT GetIndicesCount() const
	{
		return IndicesCount;
	}

protected:
	UINT IndicesCount;
};