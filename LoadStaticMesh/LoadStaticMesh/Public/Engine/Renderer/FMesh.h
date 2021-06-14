#pragma once

#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FMaterial.h"
#include "FRHIDef.h"
#include "FRenderResource.h"
#include "FTexture.h"
#include "LMesh.h"

class FResourceView;
class FMesh : public FRenderResource
{
public:
	FMesh();
	FMesh(LMesh* MeshData);
	virtual ~FMesh();

	virtual void Destroy();
	virtual void Initialize();

	void InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData, LMaterialBase& MaterialData);
	void AddMeshInRenderThread();
	void DeleteInRenderThread();
	void UpdateMeshMatrixInRenderThread(XMMATRIX Mat);

	FVertexBuffer* GetVertexBuffer()
	{
		return VertexBuffer;
	}

	FIndexBuffer* GetIndexBuffer()
	{
		return IndexBuffer;
	}

	const string& GetPsoKey()
	{
		return UsePsoKey;
	}

	void SetPsoKey(string InKey)
	{
		UsePsoKey = InKey;
	}

	void SetMeshIndex(UINT Idx)
	{
		MeshIndex = Idx;
	}

	UINT GetMeshIndex()
	{
		return MeshIndex;
	}

	FMaterial* GetMaterial()
	{
		return Material;
	}

	XMMATRIX GetModelMatrix();
	
	FResourceView* MatrixConstantBufferView;

	bool IsTranparency()
	{
		return bTranparency;
	}

private:

	string UsePsoKey;

	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	FMaterial* Material;

	XMMATRIX ModelMatrix;
	UINT MeshIndex;
	bool bTranparency;
};
