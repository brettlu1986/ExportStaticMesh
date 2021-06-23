
#include "pch.h"
#include "FSkeletalMesh.h"
#include "FRHI.h"
#include "LEngine.h"

FSkeletalMesh::FSkeletalMesh()
:VertexBuffer(nullptr)
,IndexBuffer(nullptr)
{
}

FSkeletalMesh::FSkeletalMesh(LSkeletalMesh* MeshData)
	:VertexBuffer(nullptr)
	, IndexBuffer(nullptr)
{
	ModelMatrix = MeshData->GetModelMatrix();
	Name = MeshData->GetName();
}

FSkeletalMesh::~FSkeletalMesh()
{
	Destroy();
}

void FSkeletalMesh::Destroy()
{
	if (VertexBuffer)
	{
		VertexBuffer->Destroy();
		delete VertexBuffer;
		VertexBuffer = nullptr;
	}

	if (IndexBuffer)
	{
		IndexBuffer->Destroy();
		delete IndexBuffer;
		IndexBuffer = nullptr;
	}

	if (Material)
	{
		Material->Destroy();
		delete Material;
		Material = nullptr;
	}

	delete MatrixConstantBufferView;
	MatrixConstantBufferView = nullptr;

	delete SkeletalConstantBufferView;
	SkeletalConstantBufferView = nullptr;
}

void FSkeletalMesh::Initialize()
{}

void FSkeletalMesh::InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData, LMaterialBase& MaterialData)
{
	assert(LEngine::GetEngine()->IsRenderThread());

	VertexBuffer = GRHI->RHICreateVertexBuffer();
	IndexBuffer = GRHI->RHICreateIndexBuffer();
	Material = new FMaterial();

	GRHI->UpdateVertexBufferResource(VertexBuffer, VertexBufferData);
	GRHI->UpdateIndexBufferResource(IndexBuffer, IndexBufferData);
	Material->Init(MaterialData);

	MatrixConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FObjectConstants)),E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	SkeletalConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FSkeletalConstants)),E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
}

void FSkeletalMesh::AddMeshInRenderThread()
{
	assert(LEngine::GetEngine()->IsRenderThread());
	FRenderThread::Get()->GetRenderScene()->AddSkeletalMeshToScene(this);
}

void FSkeletalMesh::DeleteMeshInRenderThread()
{
	assert(LEngine::GetEngine()->IsRenderThread());
	FRenderThread::Get()->GetRenderScene()->DeleteSkeletalMeshToScene(this);
}

void FSkeletalMesh::UpdateMeshMatrixInRenderThread(XMMATRIX Mat)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	ModelMatrix = Mat;

	FObjectConstants ObjConstants;
	XMStoreFloat4x4(&ObjConstants.World, XMMatrixTranspose(GetModelMatrix()));
	GRHI->UpdateConstantBufferView(MatrixConstantBufferView, &ObjConstants);
}

void FSkeletalMesh::UpdateBoneMapFinalTransInRenderThread(const FSkeletalConstants& SkelConstant)
{	
	assert(LEngine::GetEngine()->IsRenderThread());
	SkeletalConstants = SkelConstant;
	GRHI->UpdateConstantBufferView(SkeletalConstantBufferView, &SkeletalConstants);
}