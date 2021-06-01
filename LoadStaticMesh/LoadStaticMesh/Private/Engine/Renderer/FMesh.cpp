
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
	, DiffuseTex(nullptr)
	, ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
	, UsePsoKey("")
	, MatrixConstantBufferView(nullptr)
	, MaterialConstantBufferView(nullptr)
	, DiffuseResView(nullptr)
{
	
}



FMesh::FMesh(const string& FileName, const string& TextureName, const string& InUsePsoKey)
	:MeshFileName(FileName)
	,MeshTextureName(TextureName)
	,UsePsoKey(InUsePsoKey)
	,VertexBuffer(nullptr)
	,IndexBuffer(nullptr)
	,DiffuseTex(nullptr)
	,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
	,MatrixConstantBufferView(nullptr)
	,MaterialConstantBufferView(nullptr)
	,DiffuseResView(nullptr)
{
	Initialize();
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

	if (DiffuseTex)
	{
		DiffuseTex->Destroy();
		delete DiffuseTex;
		DiffuseTex = nullptr;
	}

	if (Material)
	{
		Material->Destroy();
		delete Material;
		Material = nullptr;
	}

	delete DiffuseResView;
	DiffuseResView = nullptr;

	delete MatrixConstantBufferView;
	MatrixConstantBufferView = nullptr;

	delete MaterialConstantBufferView;
	MaterialConstantBufferView = nullptr;
}

void FMesh::Initialize()
{
	//game thread read data from file
	VertexBuffer = GRHI->RHICreateVertexBuffer();
	IndexBuffer = GRHI->RHICreateIndexBuffer();
	LAssetDataLoader::LoadMeshVertexDataFromFile(MeshFileName, this);
	if(MeshTextureName != "")
	{
		DiffuseTex = GRHI->RHICreateTexture();
		DiffuseTex->InitializeTexture(MeshTextureName);
	}
	Material = new FMaterial();
}



void FMesh::InitRenderResource()
{
	//render thread init gpu resource
	//GRHI->RHIInitMeshGPUResource(IndexBuffer, VertexBuffer, DiffuseTex);

	GRHI->CreateVertexAndIndexBufferView(IndexBuffer, VertexBuffer);
	if(DiffuseTex)
	{
		DiffuseResView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &DiffuseTex, 0, E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	}
	MatrixConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FObjectConstants)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	MaterialConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FMaterialConstants)),E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
}

void FMesh::SetModelLocation(XMFLOAT3 Location) 
{ 
	ModelLocation = Location; 
	UpdateModelMatrix();
}
void FMesh::SetModelRotation(XMFLOAT3 Rotator) { 
	
	ModelRotation = Rotator; 
	UpdateModelMatrix();
}

void FMesh::SetModelScale(XMFLOAT3 Scale) { 
	
	ModelScale = Scale; 
	UpdateModelMatrix();
}

void FMesh::UpdateModelMatrix()
{
	ModelMatrix = XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(-ModelRotation.z), XMConvertToRadians(-ModelRotation.x), XMConvertToRadians(ModelRotation.y)) *
		XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
}

void FMesh::InitMaterial(const string& Name, XMFLOAT4 InDiffuseAlbedo, XMFLOAT3 InFresnelR0, float Roughness)
{
	Material->Init(Name, InDiffuseAlbedo, InFresnelR0, Roughness);
}

XMFLOAT4X4 FMesh::GetTextureTransform()
{
	if(DiffuseTex != nullptr)
		return DiffuseTex->GetTextureTransform();

	XMFLOAT4X4 TexMat = MathHelper::Identity4x4();
	XMStoreFloat4x4(&TexMat, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	return TexMat;
}

//new use
FMesh::FMesh(LMesh* MeshData)
:DiffuseTex(nullptr)
, VertexBuffer(nullptr)
, IndexBuffer(nullptr)
{
	ModelMatrix = MeshData->GetModelMatrix();
}

XMMATRIX FMesh::GetModelMatrix()
{
	//calculate model matrix : scale * rotation * translation
	return ModelMatrix;
}

void FMesh::InitRenderThreadResource(LVertexBuffer& VertexBufferData, LIndexBuffer& IndexBufferData)
{
	assert(LEngine::GetEngine()->IsRenderThread());

	VertexBuffer = GRHI->RHICreateVertexBuffer();
	IndexBuffer = GRHI->RHICreateIndexBuffer();

	GRHI->UpdateVertexBufferResource(VertexBuffer, VertexBufferData);
	GRHI->UpdateIndexBufferResource(IndexBuffer, IndexBufferData);

	MatrixConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FObjectConstants)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	MaterialConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FMaterialConstants)),E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
}

void FMesh::AddMeshInRenderThread()
{
	assert(LEngine::GetEngine()->IsRenderThread());
	FRenderThread::Get()->GetRenderScene()->AddMeshToScene(this);
}

void FMesh::UpdateMeshMatrixInRenderThread(XMMATRIX Mat)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	ModelMatrix = Mat;

	FObjectConstants ObjConstants;
	XMStoreFloat4x4(&ObjConstants.World, XMMatrixTranspose(GetModelMatrix()));
	XMFLOAT4X4 TexMat = MathHelper::Identity4x4();
	XMStoreFloat4x4(&ObjConstants.TexTransform, XMMatrixTranspose(XMLoadFloat4x4(&TexMat)));
	GRHI->UpdateConstantBufferView(MatrixConstantBufferView, &ObjConstants);

	//TODO: exchange to export material param
	FMaterialConstants MatConstants;
	MatConstants.DiffuseAlbedo = XMFLOAT4(Colors::DarkGray);
	MatConstants.FresnelR0 = XMFLOAT3(0.3f, 0.3f, 0.3f);
	MatConstants.Roughness = 0.4f;

	XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
	XMStoreFloat4x4(&MatConstants.MatTransform, XMMatrixTranspose(XMLoadFloat4x4(&MatTransform)));
	GRHI->UpdateConstantBufferView(MaterialConstantBufferView, &MatConstants);
}



