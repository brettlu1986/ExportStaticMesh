#pragma once

#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FRHIResource.h"
#include "FRHIDef.h"

class FTexture;
class FMesh
{
public:
	FMesh();
	~FMesh();
	
	void InitData(LPCWSTR FileName, std::wstring TextureName);
	void InitRenderResource();
	void Render(FRHICommandList& CommandList);
	void EndRender();

	void SetModelLocation(XMFLOAT3 Location);
	XMMATRIX GetModelMatrix();

private:
	XMFLOAT3 ModelLocation;
	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	FTexture* TextureRes;
};
