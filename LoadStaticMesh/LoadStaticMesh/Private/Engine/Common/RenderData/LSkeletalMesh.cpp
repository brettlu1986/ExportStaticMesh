

#include "LSkeletalMesh.h"

#include "FRenderThread.h"
#include "FSkeletalMesh.h"

#include "FRenderThread.h"
#include "LEngine.h"

LSkeletalMesh::LSkeletalMesh()
	:LResource(E_LRESOURCE_TYPE::L_TYPE_SKELETAL_MESH)
	, ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
	, ModelRotation(XMFLOAT3(0.f, 0.f, 0.f))
	, ModelScale(XMFLOAT3(1.f, 1.f, 1.f))
	, RenderMesh(nullptr)
{

}

LSkeletalMesh::~LSkeletalMesh()
{
	VertexBufferData = nullptr;
	IndexBufferData = nullptr;
}

void LSkeletalMesh::SetVertexBufferInfo(const char* DataSource, UINT DataSize, UINT DataCount)
{
	VertexBufferData = make_shared<LVertexBuffer>(DataSource, DataSize, DataCount);
}

void LSkeletalMesh::SetIndexBufferInfo(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData)
{
	IndexBufferData = make_shared<LIndexBuffer>(InCount, InByteSize, InType, InData);
}

void LSkeletalMesh::InitRenderThreadResource()
{
	assert(LEngine::GetEngine()->IsGameThread());
	RenderMesh = make_shared<FSkeletalMesh>(this);

	auto RenderMeshRes = RenderMesh;
	auto VertexData = VertexBufferData;
	auto IndexData = IndexBufferData;
	RENDER_THREAD_TASK("InitFSkeletalMeshInRender",
		[RenderMeshRes, VertexData, IndexData]()
		{
			RenderMeshRes->InitRenderThreadResource(*VertexData, *IndexData);
			RenderMeshRes->SetPsoKey("SKMPso");
			RenderMeshRes->AddMeshInRenderThread();
		}
	);
	
}

void LSkeletalMesh::UpdateBoneMapFinalTransform(vector<XMFLOAT4X4>& BoneMapFinal)
{
	assert(LEngine::GetEngine()->IsGameThread());

	auto RenderMeshRes = RenderMesh;
	vector<XMFLOAT4X4> BoneMapFinalTrans;
	BoneMapFinalTrans.resize(BoneMapFinal.size());
	copy(begin(BoneMapFinal), end(BoneMapFinal), BoneMapFinalTrans.begin());
	RENDER_THREAD_TASK("UpdateBoneMapFinalTransform",
		[RenderMeshRes, BoneMapFinalTrans]()
		{
			RenderMeshRes->UpdateBoneMapFinalTransInRenderThread(BoneMapFinalTrans);
		}
	);

}

//TODO: add dynamic remove render resource
void LSkeletalMesh::DestroyRenderThreadResource()
{
	
}

void LSkeletalMesh::SetModelLocation(XMFLOAT3 Location)
{
	ModelLocation = Location;
	UpdateModelMatrix();
}

void LSkeletalMesh::SetModelRotation(XMFLOAT3 Rotator)
{
	ModelRotation = Rotator;
	UpdateModelMatrix();
}

void LSkeletalMesh::SetModelScale(XMFLOAT3 Scale)
{
	ModelScale = Scale;
	UpdateModelMatrix();
}

void LSkeletalMesh::UpdateModelMatrix()
{
	ModelMatrix = XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(-ModelRotation.z), XMConvertToRadians(-ModelRotation.x), XMConvertToRadians(ModelRotation.y)) *
		XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);

	XMMATRIX ModelMat = GetModelMatrix();
	auto RenderMeshRes = RenderMesh;
	assert(RenderMeshRes != nullptr);
	RENDER_THREAD_TASK("UpdateSkeletalMeshMatrix",
		[RenderMeshRes, ModelMat]()
		{
			RenderMeshRes->UpdateMeshMatrixInRenderThread(ModelMat);
		}
	);

}



