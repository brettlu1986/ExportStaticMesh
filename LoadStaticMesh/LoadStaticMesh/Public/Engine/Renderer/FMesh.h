#pragma once

#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FRHIDef.h"
#include "FRenderResource.h"

class FTexture;
class FMesh : public FRenderResource
{
public:
	FMesh();
	FMesh(const std::string& FileName, UINT Index, const std::string& TextureName = "", const std::string& PsoKey = "");
	virtual ~FMesh();

	virtual void Destroy();
	virtual void Initialize();

	void InitRenderResource();
	void Render();
	void EndRender();
	
	void SetModelLocation(XMFLOAT3 Location);
	void SetModelRotation(XMFLOAT3 Rotator) ;
	void SetModelScale(XMFLOAT3 Scale);

	XMMATRIX GetModelMatrix();

	FVertexBuffer* GetVertexBuffer()
	{
		return VertexBuffer;
	}

	FIndexBuffer* GetIndexBuffer()
	{
		return IndexBuffer;
	}

	FTexture* GetTexture()
	{
		return TextureRes;
	}

	const std::string& GetPsoKey()
	{
		return UsePsoKey;
	}

	const UINT GetBufferIndex()
	{
		return UseConstantBufferIdx;
	}
private:
	void UpdateModelMatrix();

	XMFLOAT3 ModelLocation;
	XMFLOAT3 ModelRotation;
	XMFLOAT3 ModelScale;

	XMMATRIX ModelMatrix;

	std::string MeshFileName;
	std::string MeshTextureName;
	std::string UsePsoKey;

	UINT UseConstantBufferIdx;
	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	FTexture* TextureRes;
};
