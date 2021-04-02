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
	
	void InitRenderResource();
	void Render(FRHICommandList& CommandList);
	void EndRender();

	FVertexBuffer& GetVertexBuffer() 
	{
		return VertexBuffer;
	}

	FIndexBuffer& GetIndexBuffer()
	{
		return IndexBuffer;
	}

	void SetModelLocation(XMFLOAT3 Location);
	XMMATRIX GetModelMatrix();

	void SetTextureName(std::wstring Name) {  TexName = Name; }
private:
	std::wstring TexName;
	FVertexBuffer VertexBuffer; 
	FIndexBuffer IndexBuffer;

	FRHIView* RHIVertexBufferView;
	FRHIView* RHIIndexBufferView;
	FRHIView* RHIShaderResourceView;
	XMFLOAT3 ModelLocation;

};
