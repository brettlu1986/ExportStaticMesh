
#include "LScreenMesh.h"
#include "LVertexBuffer.h"
#include "LIndexBuffer.h"
#include "LMarcoDef.h"
#include "LEngine.h"
#include "FScreenMesh.h"
#include "FRenderThread.h"

LScreenMesh::LScreenMesh()
:FullScreenIBData(nullptr)
,FullScreenVBData(nullptr)
, RenderMesh(nullptr)
{
	static const float Vertices[] = {
				1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, 3.0f, 0.0f, 1.0f, -1.0f,
				-3.0f, -1.0f, 0.0f, -1.0f, 1.0f };

	static UINT Indices[] = { 0, 1, 2 };

	FullScreenVBData = new LVertexBuffer((char*)Vertices, sizeof(Vertices), 3);
	FullScreenIBData = new LIndexBuffer(3, sizeof(UINT) * 3, E_INDEX_TYPE::TYPE_UINT_32, reinterpret_cast<void*>(Indices));
}

LScreenMesh::~LScreenMesh()
{
	SAFE_DELETE(FullScreenIBData);
	SAFE_DELETE(FullScreenVBData);
	DestroyRenderThreadResource();
}

void LScreenMesh::InitRenderThreadResource()
{	
	assert(LEngine::GetEngine()->IsGameThread());
	RenderMesh = make_shared<FScreenMesh>(this);

	auto RenderMeshRes = RenderMesh;
	auto VertexData = FullScreenVBData;
	auto IndexData = FullScreenIBData;
	RENDER_THREAD_TASK("CreateFullScreenMesh",
		[RenderMeshRes, VertexData, IndexData]()
	{
		RenderMeshRes->InitRenderThreadResource(*VertexData, *IndexData);
		RenderMeshRes->AddInRenderThread();
	}
	);
}

void LScreenMesh::DestroyRenderThreadResource()
{
	if(RenderMesh != nullptr)
	{
		auto RenderMeshRes = RenderMesh;
		RENDER_THREAD_TASK("DeleteScreenMesh",
			[RenderMeshRes]()
		{
			RenderMeshRes->DeleteInRenderThread();
		}
		);
		RenderMesh = nullptr;
	}
}
