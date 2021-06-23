#pragma once

#include "stdafx.h"

class LIndexBuffer;
class LVertexBuffer;
class FScreenMesh;
class LScreenMesh
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