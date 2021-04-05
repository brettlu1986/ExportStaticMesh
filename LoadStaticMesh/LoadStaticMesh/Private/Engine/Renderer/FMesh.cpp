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

}


void FMesh::SetModelLocation(XMFLOAT3 Location)
{
	ModelLocation = Location;
}

XMMATRIX FMesh::GetModelMatrix()
{
	//calculate model matrix : scale * rotation * translation
	//determine the watch target matrix, the M view, make no scale no rotation , so we dont need to multiply scale and rotation
	 //XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f) * XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
	return XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
}

void FMesh::InitData()
{	
	VertexBuffer = CreateVertexBuffer();
	IndexBuffer = CreateIndexBuffer();
	TextureRes = CreateTexture();
	TextureRes->SetTextureName(MeshTextureName);
	LAssetDataLoader::LoadMeshVertexDataFromFile(MeshFileName, *IndexBuffer, *VertexBuffer);
}

void FMesh::InitRenderResource()
{
	InitData();
	InitMeshGPUResource(IndexBuffer, VertexBuffer, TextureRes);
}

void FMesh::Render()
{	
	DrawMesh(IndexBuffer, VertexBuffer);
}

void FMesh::EndRender()
{

}