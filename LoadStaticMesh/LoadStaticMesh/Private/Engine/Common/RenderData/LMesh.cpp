

#include "LMesh.h"

#include "LEngine.h"
#include "FRenderThread.h"
#include "FMesh.h"


LMesh::LMesh()
:LResource(E_LRESOURCE_TYPE::L_TYPE_MESH)
,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
,ModelRotation(XMFLOAT3(0.f, 0.f, 0.f))
,ModelScale(XMFLOAT3(1.f, 1.f, 1.f))
,ModelMatrix(XMMATRIX())
,RenderMesh(nullptr)
,MaterialData(nullptr)
{

}

LMesh::~LMesh()
{
	StaticMeshBuffer = nullptr;
	MaterialData = nullptr;
	DestroyRenderThreadResource();
}

void LMesh::SetMeshBufferInfo(LStaticMeshBuffer* MeshBuffer)
{
	StaticMeshBuffer = MeshBuffer;
}

void LMesh::SetMaterial(LMaterialBase* MatData)
{
	MaterialData = MatData;
}

void LMesh::InitRenderThreadResource()
{
	assert(LEngine::GetEngine()->IsGameThread());
	RenderMesh = make_shared<FMesh>(this);

	auto RenderMeshRes = RenderMesh;
	auto VertexData = StaticMeshBuffer->VertexBufferData;
	auto IndexData = StaticMeshBuffer->IndexBufferData;
	auto RenderMaterialData = MaterialData == nullptr? new LMaterialBase() : MaterialData;
	RENDER_THREAD_TASK("InitFMeshInRender",
		[RenderMeshRes, VertexData, IndexData, RenderMaterialData]()
		{
			RenderMeshRes->InitRenderThreadResource(*VertexData, *IndexData, *RenderMaterialData);
			RenderMeshRes->AddMeshInRenderThread();
		}
	);
}

void LMesh::DestroyRenderThreadResource()
{
	if(RenderMesh != nullptr)
	{
		auto RenderMeshRes = RenderMesh;
		RENDER_THREAD_TASK("DeleteMesh",
			[RenderMeshRes]()
			{
				RenderMeshRes->DeleteInRenderThread();
			}
		);
		RenderMesh = nullptr;
	}
}

void LMesh::SetModelLocation(XMFLOAT3 Location)
{
	ModelLocation = Location;
	UpdateModelMatrix();
}

void LMesh::SetModelRotation(XMFLOAT3 Rotator) 
{
	ModelRotation = Rotator;
	UpdateModelMatrix();
}

void LMesh::SetModelScale(XMFLOAT3 Scale)
{
	ModelScale = Scale;
	UpdateModelMatrix();
}

void LMesh::UpdateModelMatrix()
{
	ModelMatrix = XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(-ModelRotation.z), XMConvertToRadians(-ModelRotation.x), XMConvertToRadians(ModelRotation.y)) *
		XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);

	XMMATRIX ModelMat = GetModelMatrix();
	XMFLOAT3 ModelLoc = GetModelLocation();
	auto RenderMeshRes = RenderMesh;
	assert(RenderMeshRes != nullptr);
	RENDER_THREAD_TASK("UpdateMeshMatrix",
		[RenderMeshRes, ModelMat, ModelLoc]()
		{
			RenderMeshRes->UpdateMeshMatrixInRenderThread(ModelMat, ModelLoc);
		}
	);

}



