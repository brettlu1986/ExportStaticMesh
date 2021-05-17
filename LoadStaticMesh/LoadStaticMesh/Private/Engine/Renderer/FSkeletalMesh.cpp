

#include "FSkeletalMesh.h"
#include "FRHI.h"

FSkeletalMesh::FSkeletalMesh()
:VertexBuffer(nullptr)
,IndexBuffer(nullptr)
,DiffuseTex(nullptr)
,ShaderResView(nullptr)
,Skeleton(nullptr)
{
	Initialize();
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

	if(Skeleton)
	{
		delete Skeleton;
		Skeleton = nullptr;
	}

	delete ShaderResView;
	ShaderResView = nullptr;

	delete MtConstantBufferView;
	MtConstantBufferView = nullptr;
}

void FSkeletalMesh::Initialize()
{}

void FSkeletalMesh::SetVertexAndIndexBuffer(FVertexBuffer* VBuffer, FIndexBuffer* IBuffer)
{
	VertexBuffer = VBuffer;
	IndexBuffer = IBuffer;
}

void FSkeletalMesh::SetDiffuseTexture(FTexture* Tex)
{
	DiffuseTex = Tex;
}

void FSkeletalMesh::InitRenderResource()
{
	GRHI->CreateVertexAndIndexBufferView(IndexBuffer, VertexBuffer);
	ShaderResView = GRHI->CreateResourceView({E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &DiffuseTex, 0});
	MtConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FObjectConstants)) });
}

void FSkeletalMesh::SetModelLocation(XMFLOAT3 Location)
{
	ModelLocation = Location;
	UpdateModelMatrix();
}
void FSkeletalMesh::SetModelRotation(XMFLOAT3 Rotator) {

	ModelRotation = Rotator;
	UpdateModelMatrix();
}

void FSkeletalMesh::SetModelScale(XMFLOAT3 Scale) {

	ModelScale = Scale;
	UpdateModelMatrix();
}

void FSkeletalMesh::UpdateModelMatrix()
{
	ModelMatrix = XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(-ModelRotation.z), XMConvertToRadians(-ModelRotation.x), XMConvertToRadians(ModelRotation.y)) *
		XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
}