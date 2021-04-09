#include "FMesh.h"
#include "FRHI.h"
#include "FDefine.h"
#include "LAssetDataLoader.h"

FMesh::FMesh()
:VertexBuffer(nullptr)
,IndexBuffer(nullptr)
,TextureRes(nullptr)
,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
{
	
}

FMesh::FMesh(std::string FileName, std::string TextureName)
	:MeshFileName(FileName)
	,MeshTextureName(TextureName)
	,VertexBuffer(nullptr)
	,IndexBuffer(nullptr)
	,TextureRes(nullptr)
	,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
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
	TextureRes = CreateTexture();
	TextureRes->InitializeTexture(MeshTextureName);
}

XMMATRIX FMesh::GetModelMatrix()
{
	//calculate model matrix : scale * rotation * translation
	//determine the watch target matrix, the M view, make no scale no rotation , so we dont need to multiply scale and rotation
	 //XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f) * XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
	return  XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z) * 
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(-ModelRotation.z), XMConvertToRadians(-ModelRotation.x), XMConvertToRadians(ModelRotation.y)) *
		XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
}

void FMesh::InitRenderResource()
{
	InitMeshGPUResource(IndexBuffer, VertexBuffer, TextureRes);
}

void FMesh::Render()
{	
	DrawMesh(IndexBuffer, VertexBuffer);
}

void FMesh::EndRender()
{

}