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

	void InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData);
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

	FTexture* GetDiffuseTexture()
	{
		return DiffuseTex;
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

	XMMATRIX GetModelMatrix();
	
	FResourceView* MatrixConstantBufferView;
	FResourceView* MaterialConstantBufferView;
	//TODO: move to material
	FResourceView* DiffuseResView;

private:

	string UsePsoKey;

	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;

	FTexture* DiffuseTex;
	FMaterial* Material;

	XMMATRIX ModelMatrix;
	UINT MeshIndex;
};
