
#include "pch.h"
#include "FScreenMesh.h"
#include "LScreenMesh.h"
#include "FRHI.h"

#include "LMarcoDef.h"
#include "FRenderThread.h"
#include "FIndexBuffer.h"
#include "FVertexBuffer.h"

FScreenMesh::FScreenMesh(LScreenMesh* ScreenMeshData)
:FullScreenVB(nullptr)
, FullScreenIB(nullptr)
{

}

FScreenMesh::~FScreenMesh()
{
	Destroy();
}

void FScreenMesh::Destroy()
{
	SAFE_DESTROY(FullScreenVB);
	SAFE_DESTROY(FullScreenIB);
}

void FScreenMesh::InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData)
{
	FullScreenVB = GRHI->RHICreateVertexBuffer();
	FullScreenIB = GRHI->RHICreateIndexBuffer();

	GRHI->UpdateVertexBufferResource(FullScreenVB, VertexBufferData);
	GRHI->UpdateIndexBufferResource(FullScreenIB, IndexBufferData);
}

void FScreenMesh::AddInRenderThread()
{
	FRenderThread::Get()->GetRenderScene()->SetScreenMesh(this);
}

void FScreenMesh::DeleteInRenderThread()
{
	FRenderThread::Get()->GetRenderScene()->DeleteScreenMesh();
}
