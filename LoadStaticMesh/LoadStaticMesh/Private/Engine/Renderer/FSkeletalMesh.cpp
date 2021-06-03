

#include "FSkeletalMesh.h"
#include "FRHI.h"
#include "LEngine.h"

FSkeletalMesh::FSkeletalMesh()
:VertexBuffer(nullptr)
,IndexBuffer(nullptr)
,DiffuseTex(nullptr)
,DiffuseResView(nullptr)
{
}

FSkeletalMesh::FSkeletalMesh(LSkeletalMesh* MeshData)
	:VertexBuffer(nullptr)
	, IndexBuffer(nullptr)
	, DiffuseTex(nullptr)
	, DiffuseResView(nullptr)
{
	ModelMatrix = MeshData->GetModelMatrix();
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

	if (DiffuseTex)
	{
		DiffuseTex->Destroy();
		delete DiffuseTex;
		DiffuseTex = nullptr;
	}

	delete DiffuseResView;
	DiffuseResView = nullptr;

	delete MatrixConstantBufferView;
	MatrixConstantBufferView = nullptr;

	delete SkeletalConstantBufferView;
	SkeletalConstantBufferView = nullptr;
}

void FSkeletalMesh::Initialize()
{}

void FSkeletalMesh::InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData)
{
	assert(LEngine::GetEngine()->IsRenderThread());

	VertexBuffer = GRHI->RHICreateVertexBuffer();
	IndexBuffer = GRHI->RHICreateIndexBuffer();

	GRHI->UpdateVertexBufferResource(VertexBuffer, VertexBufferData);
	GRHI->UpdateIndexBufferResource(IndexBuffer, IndexBufferData);

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
	XMFLOAT4X4 TexMat = MathHelper::Identity4x4();
	XMStoreFloat4x4(&ObjConstants.TexTransform, XMMatrixTranspose(XMLoadFloat4x4(&TexMat)));
	GRHI->UpdateConstantBufferView(MatrixConstantBufferView, &ObjConstants);
}

void FSkeletalMesh::UpdateBoneMapFinalTransInRenderThread(const FSkeletalConstants& SkelConstant)
{	
	assert(LEngine::GetEngine()->IsRenderThread());
	SkeletalConstants = SkelConstant;
	GRHI->UpdateConstantBufferView(SkeletalConstantBufferView, &SkeletalConstants);
}