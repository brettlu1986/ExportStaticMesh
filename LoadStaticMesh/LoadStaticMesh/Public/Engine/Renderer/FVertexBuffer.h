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

	UINT GetVertexCount()
	{
		return VertexCount;
	}

	UINT GetVertexDataSize()
	{
		return VertexDataSize;
	}

	std::vector<FVertexData>& GetVertexData()
	{
		return VertexData;
	}

	void GetPositionTex0Input(std::vector<FVertex_PositionTex0>& OutPut)
	{
		if (VertexData.size() == 0)
		{
			return;
		}
		OutPut.reserve(VertexCount);
		for (size_t i = 0; i < VertexCount; i++)
		{
			OutPut.push_back({ VertexData[i].Position, VertexData[i].Tex0 });
		}
	}

	void Init(const char* DataSource, UINT DataSize, UINT DataCount);

private: 

	UINT VertexCount;
	UINT VertexDataSize;
	std::vector<FVertexData> VertexData;
	
};