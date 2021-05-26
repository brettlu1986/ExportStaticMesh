

#include "FSkeletalMesh.h"
#include "FRHI.h"

FSkeletalMesh::FSkeletalMesh()
:VertexBuffer(nullptr)
,IndexBuffer(nullptr)
,DiffuseTex(nullptr)
,DiffuseResView(nullptr)
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

	delete DiffuseResView;
	DiffuseResView = nullptr;

	delete MatrixConstantBufferView;
	MatrixConstantBufferView = nullptr;

	delete SkeletalConstantBufferView;
	SkeletalConstantBufferView = nullptr;
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
	DiffuseResView = GRHI->CreateResourceView({E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &DiffuseTex, 0, E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	MatrixConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FObjectConstants)),E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	SkeletalConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FSkeletalConstants)),E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
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