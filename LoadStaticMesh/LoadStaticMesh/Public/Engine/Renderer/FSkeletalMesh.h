#pragma once

#include "stdafx.h"
#include "FRenderResource.h"
#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FTexture.h"
#include "FResourceViewCreater.h"

#include "LSkeletalMesh.h"
#include "FMaterial.h"
#include "LMaterial.h"

class FSkeletalMesh : public FRenderResource
{
public:
	FSkeletalMesh();
	FSkeletalMesh(LSkeletalMesh* MeshData);
	virtual ~FSkeletalMesh();

	virtual void Destroy();
	virtual void Initialize();

	void InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData, LMaterialBase& MaterialData);
	void AddMeshInRenderThread();
	void DeleteMeshInRenderThread();
	void UpdateMeshMatrixInRenderThread(XMMATRIX Mat);
	void UpdateBoneMapFinalTransInRenderThread(const FSkeletalConstants& SkelConstant);

	XMMATRIX& GetModelMatrix()
	{
		return ModelMatrix;
	}

	void SetPsoKey(string InKey)
	{
		UsePsoKey = InKey;
	}

	const string& GetPsoKey()
	{
		return UsePsoKey;
	}

	FVertexBuffer* GetVertexBuffer()
	{
		return VertexBuffer;
	}

	FIndexBuffer* GetIndexBuffer()
	{
		return IndexBuffer;
	}

	void SetMeshIndex(UINT Index)
	{
		MeshIndex = Index;
	}

	UINT GetMeshIndex()
	{
		return MeshIndex;
	}

	FMaterial* GetMaterial()
	{
		return Material;
	}

	FResourceView* MatrixConstantBufferView;
	FResourceView* SkeletalConstantBufferView;

private:
	
	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	FMaterial* Material;

	UINT MeshIndex;
	XMMATRIX ModelMatrix;
	FSkeletalConstants SkeletalConstants;
	string UsePsoKey;
};