#include "FMesh.h"
#include "FRHI.h"
#include "FDefine.h"
#include "LAssetDataLoader.h"

FMesh::FMesh()
:VertexBuffer(nullptr)
,IndexBuffer(nullptr)
,DiffuseTex(nullptr)
,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
, UsePsoKey("")
, MatrixCbIndex(0)
{
	
}

FMesh::FMesh(const std::string& FileName, const std::string& TextureName, const std::string& InUsePsoKey)
	:MeshFileName(FileName)
	,MeshTextureName(TextureName)
	,UsePsoKey(InUsePsoKey)
	,VertexBuffer(nullptr)
	,IndexBuffer(nullptr)
	,DiffuseTex(nullptr)
	,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
	, MatrixCbIndex(0)
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
}

void FMesh::Initialize()
{
	VertexBuffer = CreateVertexBuffer();
	IndexBuffer = CreateIndexBuffer();
	LAssetDataLoader::LoadMeshVertexDataFromFile(MeshFileName, this);
	if(MeshTextureName != "")
	{
		DiffuseTex = CreateTexture();
		DiffuseTex->InitializeTexture(MeshTextureName);
	}
	Material = new FMaterial();
}

XMMATRIX FMesh::GetModelMatrix()
{
	//calculate model matrix : scale * rotation * translation
	return ModelMatrix;
}

void FMesh::InitRenderResource()
{
	InitMeshGPUResource(IndexBuffer, VertexBuffer, DiffuseTex);
}

void FMesh::Render()
{	
	DrawMesh(this);
}

void FMesh::EndRender()
{

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

void FMesh::SetDiffuseTextureHeapIndex(UINT Index)
{
	DiffuseTex->SetTextureHeapIndex(Index);
}

void FMesh::SetMaterialCbvHeapIndex(UINT Index)
{
	Material->SetMaterialCbvHeapIndex(Index);
}

void FMesh::InitMaterial(const std::string& Name, XMFLOAT4 InDiffuseAlbedo, XMFLOAT3 InFresnelR0, float Roughness)
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

