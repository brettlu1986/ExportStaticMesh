#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "FRenderResource.h"

class FVertexBuffer : public FRenderResource
{
public:
	FVertexBuffer();
	FVertexBuffer(E_RESOURCE_TYPE Type);
	FVertexBuffer(const char* DataSource, UINT DataSize, UINT DataCount);
	virtual ~FVertexBuffer();

	virtual void Destroy() override;
	virtual void Initialize() override;
	void Init(const char* DataSource, UINT DataSize, UINT DataCount, bool bInSKM = false);

protected: 
	bool bSKMVertex;
	UINT VertexCount;
	UINT VertexDataSize;
	std::vector<FVertexData> VertexData;
	std::vector<FSkeletalVertexData> SKMVertexData;
};