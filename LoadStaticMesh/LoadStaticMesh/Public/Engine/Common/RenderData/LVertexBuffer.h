#pragma once

#include "stdafx.h"
#include "LResource.h"

class LVertexBuffer : public LResource
{
public:
	LVertexBuffer(const char* DataSource, UINT DataSize, UINT DataCount);
	virtual ~LVertexBuffer();

	virtual void InitRenderResource() override;
	virtual void DestroyRenderResource() override;

private:
	UINT VertexCount;
	UINT VertexDataSize;
	void* VertexByteData;
};