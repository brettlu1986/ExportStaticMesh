#pragma once

#include "pch.h"
#include "FDefine.h"
#include "FRenderResource.h"

class FVertexBuffer : public FRenderResource
{
public:
	FVertexBuffer();
	FVertexBuffer(E_FRESOURCE_TYPE Type);
	virtual ~FVertexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;

};