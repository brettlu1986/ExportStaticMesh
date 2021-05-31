

#include "LMesh.h"

#include "LEngine.h"
#include "FRenderThread.h"
#include "FMesh.h"

LMesh::LMesh()
:LResource(E_LRESOURCE_TYPE::L_TYPE_MESH)
,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
,ModelRotation(XMFLOAT3(0.f, 0.f, 0.f))
,ModelScale(XMFLOAT3(1.f, 1.f, 1.f))
{

}

LMesh::~LMesh()
{

}

void LMesh::SetVertexBufferInfo(const char* DataSource, UINT DataSize, UINT DataCount)
{
	VertexBufferData = make_shared<LVertexBuffer>(DataSource, DataSize, DataCount);
}

void LMesh::SetIndexBufferInfo(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData)
{
	IndexBufferData = make_shared<LIndexBuffer>(InCount, InByteSize, InType, InData);
}

void LMesh::InitRenderThreadResource()
{
	assert(LEngine::GetEngine()->IsGameThread());
	RenderMesh = make_shared<FMesh>(this);

	auto RenderMeshRes = RenderMesh;
	auto VertexData = VertexBufferData;
	auto IndexData = IndexBufferData;
	RENDER_THREAD_TASK(
		[RenderMeshRes, VertexData, IndexData]()
		{
			RenderMeshRes->InitRenderThreadResource(*VertexData, *IndexData);
			RenderMeshRes->AddMeshInRenderThread();
		}
	);
}

//TODO: add dynamic remove render resource
void LMesh::DestroyRenderThreadResource()
{

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
}



