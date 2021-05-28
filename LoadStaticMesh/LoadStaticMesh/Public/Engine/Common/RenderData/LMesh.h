#pragma once

#include "stdafx.h"
#include "LResource.h"
#include "LIndexBuffer.h"
#include "LVertexBuffer.h"

class LMesh : public LResource
{
public:
	LMesh();
	virtual ~LMesh();

	void SetVertexBufferInfo(const char* DataSource, UINT DataSize, UINT DataCount);
	void SetIndexBufferInfo(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);

	virtual void InitRenderResource() override;
	virtual void DestroyRenderResource() override;

private:
	unique_ptr<LIndexBuffer> IndexBufferData;
	unique_ptr<LVertexBuffer> VertexBufferData;
};