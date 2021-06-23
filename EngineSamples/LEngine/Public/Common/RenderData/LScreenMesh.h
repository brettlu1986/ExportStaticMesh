#pragma once

#include "pch.h"

class LIndexBuffer;
class LVertexBuffer;
class FScreenMesh;
class LENGINE_API LScreenMesh
{
public:
	LScreenMesh();
	~LScreenMesh();

	void InitRenderThreadResource();
	void DestroyRenderThreadResource();


private: 

	LIndexBuffer* FullScreenIBData;
	LVertexBuffer* FullScreenVBData;
	shared_ptr<FScreenMesh> RenderMesh;
};