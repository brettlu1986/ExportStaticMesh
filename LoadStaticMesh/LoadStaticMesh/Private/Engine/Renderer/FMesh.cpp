
#include "stdafx.h"
#include "FMesh.h"
#include "FRHI.h"
#include "FDefine.h"
#include "LAssetDataLoader.h"
#include "DirectXColors.h"

#include "LEngine.h"

FMesh::FMesh()
	:VertexBuffer(nullptr)
	, IndexBuffer(nullptr)
	, Material(nullptr)
	, UsePsoKey("")
	, MatrixConstantBufferView(nullptr)
{
	
}

FMesh::FMesh(LMesh* MeshData)
	: VertexBuffer(nullptr)
	, IndexBuffer(nullptr)
	, Material(nullptr)
{
	ModelMatrix = MeshData->GetModelMatrix();
	bTranparency = MeshData->IsTransparency();
}

FMesh::~FMesh()
{
	Destroy();
}

void FMesh::Destroy()
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

}

void FMesh::Initialize()
{

}


XMMATRIX FMesh::GetModelMatrix()
{
	//calculate model matrix : scale * rotation * translation
	return ModelMatrix;
}

void FMesh::InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData, LMaterialBase& MaterialData)
{
	assert(LEngine::GetEngine()->IsRenderThread());

	VertexBuffer = GRHI->RHICreateVertexBuffer();
	IndexBuffer = GRHI->RHICreateIndexBuffer();
	Material = new FMaterial();

	GRHI->UpdateVertexBufferResource(VertexBuffer, VertexBufferData);
	GRHI->UpdateIndexBufferResource(IndexBuffer, IndexBufferData);
	Material->Init(MaterialData);

	MatrixConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FObjectConstants)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
}

void FMesh::AddMeshInRenderThread()
{
	assert(LEngine::GetEngine()->IsRenderThread());
	FRenderThread::Get()->GetRenderScene()->AddMeshToScene(this);
}

void FMesh::DeleteInRenderThread()
{
	assert(LEngine::GetEngine()->IsRenderThread());
	FRenderThread::Get()->GetRenderScene()->DeleteMeshToScene(this);
}

void FMesh::UpdateMeshMatrixInRenderThread(XMMATRIX Mat)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	ModelMatrix = Mat;

	FObjectConstants ObjConstants;
	XMStoreFloat4x4(&ObjConstants.World, XMMatrixTranspose(GetModelMatrix()));
	GRHI->UpdateConstantBufferView(MatrixConstantBufferView, &ObjConstants);
}



