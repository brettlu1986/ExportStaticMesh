#include "FMesh.h"
#include "FRHI.h"
#include "FDefine.h"
#include "LAssetDataLoader.h"

FMesh::FMesh()
:VertexBuffer(nullptr)
,IndexBuffer(nullptr)
,TextureRes(nullptr)
,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
, UsePsoKey("")
, UseConstantBufferIdx(0)
{
	
}

FMesh::FMesh(const std::string& FileName, UINT InBufferIdx, const std::string& TextureName, const std::string& InUsePsoKey)
	:MeshFileName(FileName)
	,MeshTextureName(TextureName)
	,UsePsoKey(InUsePsoKey)
	,VertexBuffer(nullptr)
	,IndexBuffer(nullptr)
	,TextureRes(nullptr)
	,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
	,UseConstantBufferIdx(InBufferIdx)
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

	if (TextureRes)
	{
		TextureRes->Destroy();
		delete TextureRes;
		TextureRes = nullptr;
	}
}

void FMesh::Initialize()
{
	VertexBuffer = CreateVertexBuffer();
	IndexBuffer = CreateIndexBuffer();
	LAssetDataLoader::LoadMeshVertexDataFromFile(MeshFileName, this);
	if(MeshTextureName != "")
	{
		TextureRes = CreateTexture();
		TextureRes->InitializeTexture(MeshTextureName);
	}
}

XMMATRIX FMesh::GetModelMatrix()
{
	//calculate model matrix : scale * rotation * translation
	return ModelMatrix;
}

void FMesh::InitRenderResource()
{
	InitMeshGPUResource(IndexBuffer, VertexBuffer, TextureRes);
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