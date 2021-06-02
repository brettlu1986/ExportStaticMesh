#pragma once

#include "stdafx.h"
#include "FRenderResource.h"
#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FTexture.h"
#include "FResourceViewCreater.h"

#include "LSkeletalMesh.h"

class FSkeletalMesh : public FRenderResource
{
public:
	FSkeletalMesh();
	FSkeletalMesh(LSkeletalMesh* MeshData);
	virtual ~FSkeletalMesh();

	virtual void Destroy();
	virtual void Initialize();

	void InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData);
	void AddMeshInRenderThread();
	void UpdateMeshMatrixInRenderThread(XMMATRIX Mat);
	void UpdateBoneMapFinalTransInRenderThread(const vector<XMFLOAT4X4>& BoneMapFinal);

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

	FResourceView* MatrixConstantBufferView;
	FResourceView* SkeletalConstantBufferView;
	//TODO: move to material
	FResourceView* DiffuseResView;

private:

	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	
	FTexture* DiffuseTex;

	XMMATRIX ModelMatrix;
	string UsePsoKey;

};