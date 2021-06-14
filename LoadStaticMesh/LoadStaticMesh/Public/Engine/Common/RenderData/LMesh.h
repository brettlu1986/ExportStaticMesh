#pragma once

#include "stdafx.h"
#include "LResource.h"
#include "LIndexBuffer.h"
#include "LVertexBuffer.h"
#include "LMaterialBase.h"
#include "LAssetManager.h"

class FMesh;
class LMesh : public LResource
{
public:
	LMesh();
	virtual ~LMesh();

	void SetMeshBufferInfo(LStaticMeshBuffer* MeshBuffer);
	void SetMaterial(LMaterialBase* MatData);

	void InitRenderThreadResource();
	void DestroyRenderThreadResource();

	void SetModelLocation(XMFLOAT3 Location);
	void SetModelRotation(XMFLOAT3 Rotator);
	void SetModelScale(XMFLOAT3 Scale);
	XMMATRIX GetModelMatrix() const { return ModelMatrix; }

	void SetTranparency(bool Value)
	{
		bTranparency = Value;
	}

	bool IsTransparency()
	{
		return bTranparency;
	}
private:
	void UpdateModelMatrix();

	bool bTranparency;
	LStaticMeshBuffer* StaticMeshBuffer;
	LMaterialBase* MaterialData;

	XMFLOAT3 ModelLocation;
	XMFLOAT3 ModelRotation;
	XMFLOAT3 ModelScale;
	XMMATRIX ModelMatrix;

	shared_ptr<FMesh> RenderMesh;
};