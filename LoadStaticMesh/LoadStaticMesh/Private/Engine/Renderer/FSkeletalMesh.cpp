

#include "FSkeletalMesh.h"
#include "FRHI.h"

FSkeletalMesh::FSkeletalMesh()
:VertexBuffer(nullptr)
,IndexBuffer(nullptr)
,DiffuseTex(nullptr)
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

	if(Skeleton)
	{
		delete Skeleton;
		Skeleton = nullptr;
	}
}

void FSkeletalMesh::Initialize()
{}

void FSkeletalMesh::SetVertexAndIndexBuffer(FVertexBuffer* VBuffer, FIndexBuffer* IBuffer)
{
	VertexBuffer = VBuffer;
	IndexBuffer = IBuffer;
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