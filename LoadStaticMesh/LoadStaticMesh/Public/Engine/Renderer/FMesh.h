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
	
	void SetModelLocation(XMFLOAT3 Location);
	XMMATRIX GetModelMatrix();

private:
	std::string MeshFileName;
	std::string MeshTextureName;
	XMFLOAT3 ModelLocation;
	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	FTexture* TextureRes;
};
