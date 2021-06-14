

#include "LSkeletalMesh.h"

#include "FRenderThread.h"
#include "FSkeletalMesh.h"

#include "LEngine.h"

LSkeletalMesh::LSkeletalMesh()
	:LResource(E_LRESOURCE_TYPE::L_TYPE_SKELETAL_MESH)
	, ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
	, ModelRotation(XMFLOAT3(0.f, 0.f, 0.f))
	, ModelScale(XMFLOAT3(1.f, 1.f, 1.f))
	, RenderMesh(nullptr)
	, bUpdateWorldTrans(false)
{

}

LSkeletalMesh::~LSkeletalMesh()
{
	SkeletalMeshBuffer = nullptr;
	MaterialData = nullptr;
	DestroyRenderThreadResource();
}

void LSkeletalMesh::SetSkeletalMeshBuffer(LSkeletalMeshBuffer* MeshBuffer)
{
	SkeletalMeshBuffer = MeshBuffer;
}

void LSkeletalMesh::SetMaterial(LMaterialBase* MatData)
{
	MaterialData = MatData;
}

void LSkeletalMesh::InitRenderThreadResource()
{
	assert(LEngine::GetEngine()->IsGameThread());
	RenderMesh = make_shared<FSkeletalMesh>(this);

	auto RenderMeshRes = RenderMesh;
	auto VertexData = SkeletalMeshBuffer->VertexBufferData;
	auto IndexData = SkeletalMeshBuffer->IndexBufferData;
	auto RenderMaterialData = MaterialData;
	RENDER_THREAD_TASK("InitFSkeletalMeshInRender",
		[RenderMeshRes, VertexData, IndexData, RenderMaterialData]()
		{
			RenderMeshRes->InitRenderThreadResource(*VertexData, *IndexData, *RenderMaterialData);
			RenderMeshRes->SetPsoKey("SKMPso");
			RenderMeshRes->AddMeshInRenderThread();
		}
	);
	
}

void LSkeletalMesh::UpdateBoneMapFinalTransform(vector<XMFLOAT4X4>& BoneMapFinal)
{
	assert(LEngine::GetEngine()->IsGameThread());

	auto RenderMeshRes = RenderMesh;

	FSkeletalConstants SkeCon;
	copy(begin(BoneMapFinal), end(BoneMapFinal), &SkeCon.BoneMapBoneTransforms[0]);
	RENDER_THREAD_TASK("UpdateBoneMapFinalTransform",
		[RenderMeshRes, SkeCon]()
		{
			RenderMeshRes->UpdateBoneMapFinalTransInRenderThread(SkeCon);
		}
	);

}

void LSkeletalMesh::UpdateModelMatrix()
{
	if(!bUpdateWorldTrans)
	{
		return;
	}

	bUpdateWorldTrans = false;
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


void LSkeletalMesh::DestroyRenderThreadResource()
{
	if(RenderMesh != nullptr)
	{
		auto RenderMeshRes = RenderMesh;
		RENDER_THREAD_TASK("DeleteSkeletalMesh",
			[RenderMeshRes]()
		{
			RenderMeshRes->DeleteMeshInRenderThread();
		}
		);
		RenderMesh = nullptr;
	}
}

void LSkeletalMesh::SetModelLocation(XMFLOAT3 Location)
{
	ModelLocation = Location;
	bUpdateWorldTrans = true;
}

void LSkeletalMesh::SetModelRotation(XMFLOAT3 Rotator)
{
	ModelRotation = Rotator;
	bUpdateWorldTrans = true;
}

void LSkeletalMesh::SetModelScale(XMFLOAT3 Scale)
{
	ModelScale = Scale;
	bUpdateWorldTrans = true;
}





