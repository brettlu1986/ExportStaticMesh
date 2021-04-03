#pragma once

#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FRHIResource.h"
#include "FRHIDef.h"

class FMesh
{
public:
	FMesh();
	~FMesh();
	
	void InitData(LPCWSTR FileName);
	void InitRenderResource();
	void Render(FRHICommandList& CommandList);
	void EndRender();

	void SetModelLocation(XMFLOAT3 Location);
	XMMATRIX GetModelMatrix();

	void SetTextureName(std::wstring Name) {  TexName = Name; }
private:
	std::wstring TexName;

	FRHIView* RHIShaderResourceView;
	XMFLOAT3 ModelLocation;

	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
};
