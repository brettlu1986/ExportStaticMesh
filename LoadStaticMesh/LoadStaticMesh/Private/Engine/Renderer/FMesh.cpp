#include "FMesh.h"
#include "FRHI.h"
#include "FDefine.h"
#include "LAssetDataLoader.h"

FMesh::FMesh()
:VertexBuffer(nullptr)
,IndexBuffer(nullptr)
,ModelLocation(XMFLOAT3(0.f, 0.f, 0.f))
{
	
}

FMesh::~FMesh()
{
	if(VertexBuffer)
	{
		delete VertexBuffer;
		VertexBuffer = nullptr;
	}

	if (IndexBuffer)
	{
		delete IndexBuffer;
		IndexBuffer = nullptr;
	}

	if (TextureRes)
	{
		delete TextureRes;
		TextureRes = nullptr;
	}
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

void FMesh::InitData(LPCWSTR FileName, std::wstring TextureName)
{	
	VertexBuffer = CreateVertexBuffer();
	IndexBuffer = CreateIndexBuffer();
	TextureRes = CreateTexture();
	TextureRes->SetTextureName(TextureName);
	LAssetDataLoader::LoadMeshVertexDataFromFile(FileName, *IndexBuffer, *VertexBuffer);
}

void FMesh::InitRenderResource()
{
	InitMeshGPUResource(IndexBuffer, VertexBuffer, TextureRes);
}

void FMesh::Render(FRHICommandList& CommandList)
{	
	DrawMesh(IndexBuffer, VertexBuffer, TextureRes);
}

void FMesh::EndRender()
{

}