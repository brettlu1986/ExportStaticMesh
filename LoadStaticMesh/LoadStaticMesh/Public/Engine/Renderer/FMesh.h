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
	
	FMesh(const string& FileName, const string& TextureName = "", const string& PsoKey = "");
	virtual ~FMesh();

	virtual void Destroy();
	virtual void Initialize();

	void InitRenderResource();
	
	void SetModelLocation(XMFLOAT3 Location);
	void SetModelRotation(XMFLOAT3 Rotator) ;
	void SetModelScale(XMFLOAT3 Scale);


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
	XMFLOAT4X4 GetTextureTransform();

	FMaterial* GetMaterial()
	{
		return Material;
	}

	const string& GetPsoKey()
	{
		return UsePsoKey;
	}

	void SetPsoKey(string InKey)
	{
		UsePsoKey = InKey;
	}


	void InitMaterial(const string& Name, XMFLOAT4 InDiffuseAlbedo, XMFLOAT3 InFresnelR0, float Roughness);


	//
	FResourceView* MatrixConstantBufferView;
	FResourceView* MaterialConstantBufferView;
	//TODO: move to material
	FResourceView* DiffuseResView;
private:
	void UpdateModelMatrix();

	XMFLOAT3 ModelLocation;
	XMFLOAT3 ModelRotation;
	XMFLOAT3 ModelScale;

	string MeshFileName;
	string MeshTextureName;
	string UsePsoKey;

	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;

	FTexture* DiffuseTex;
	FMaterial* Material;

public: 

	//new use
	FMesh(LMesh* MeshData);

	void InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData);
	void AddMeshInRenderThread();
	void UpdateMeshMatrixInRenderThread(XMMATRIX Mat);

	void SetMeshIndex(UINT Idx)
	{
		MeshIndex = Idx;
	}

	UINT GetMeshIndex()
	{
		return MeshIndex;
	}

	XMMATRIX GetModelMatrix();
private:

	XMMATRIX ModelMatrix;

	UINT MeshIndex;
};
