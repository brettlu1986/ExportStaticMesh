#pragma once

#include "stdafx.h"
#include "LScreenMesh.h"

class FIndexBuffer;
class FVertexBuffer;
class LScreenMesh;
class FScreenMesh
{
public:
	FScreenMesh(LScreenMesh* ScreenMeshData);
	~FScreenMesh();

	void InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData);
	void AddInRenderThread();
	void DeleteInRenderThread();

	FIndexBuffer* GetIB()
	{
		return FullScreenIB;
	}

	FVertexBuffer* GetVB()
	{
		return FullScreenVB;
	}
private:
	FIndexBuffer* FullScreenIB;
	FVertexBuffer* FullScreenVB;
};