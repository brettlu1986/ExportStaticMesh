#pragma once

#include "stdafx.h"
#include "LResource.h"
#include "LIndexBuffer.h"
#include "LVertexBuffer.h"

class FMesh;
class LMesh : public LResource
{
public:
	LMesh();
	virtual ~LMesh();

	void SetVertexBufferInfo(const char* DataSource, UINT DataSize, UINT DataCount);
	void SetIndexBufferInfo(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);

	void InitRenderThreadResource();
	void DestroyRenderThreadResource();

	void SetModelLocation(XMFLOAT3 Location);
	void SetModelRotation(XMFLOAT3 Rotator);
	void SetModelScale(XMFLOAT3 Scale);
	XMMATRIX GetModelMatrix() const { return ModelMatrix; }

private:
	void UpdateModelMatrix();

	shared_ptr<LIndexBuffer> IndexBufferData;
	shared_ptr<LVertexBuffer> VertexBufferData;

	XMFLOAT3 ModelLocation;
	XMFLOAT3 ModelRotation;
	XMFLOAT3 ModelScale;
	XMMATRIX ModelMatrix;

	shared_ptr<FMesh> RenderMesh;
};