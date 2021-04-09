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
	FMesh(std::string FileName, std::string TextureName);
	virtual ~FMesh();

	virtual void Destroy();
	virtual void Initialize();

	void InitRenderResource();
	void Render();
	void EndRender();
	
	void SetModelLocation(XMFLOAT3 Location) { ModelLocation = Location; }
	void SetModelRotation(XMFLOAT3 Rotator) { ModelRotation = Rotator; }
	void SetModelScale(XMFLOAT3 Scale) { ModelScale = Scale; }

	XMMATRIX GetModelMatrix();

	FVertexBuffer* GetVertexBuffer()
	{
		return VertexBuffer;
	}

	FIndexBuffer* GetIndexBuffer()
	{
		return IndexBuffer;
	}

private:
	XMFLOAT3 ModelLocation;
	XMFLOAT3 ModelRotation;
	XMFLOAT3 ModelScale;

	std::string MeshFileName;
	std::string MeshTextureName;
	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	FTexture* TextureRes;
};
