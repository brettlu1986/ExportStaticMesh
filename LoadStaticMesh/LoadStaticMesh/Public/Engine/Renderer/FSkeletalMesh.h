#pragma once

#include "stdafx.h"
#include "FRenderResource.h"
#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FTexture.h"
#include "LSkeleton.h"

class FSkeletalMesh : public FRenderResource
{
public:
	FSkeletalMesh();
	virtual ~FSkeletalMesh();

	virtual void Destroy();
	virtual void Initialize();

	void SetModelLocation(XMFLOAT3 Location);
	void SetModelRotation(XMFLOAT3 Rotator);
	void SetModelScale(XMFLOAT3 Scale);

	void SetVertexAndIndexBuffer(FVertexBuffer* VBuffer, FIndexBuffer* IBuffer);

	XMMATRIX& GetModelMatrix()
	{
		return ModelMatrix;
	}

	void SetSkeleton(LSkeleton* Ske)
	{
		Skeleton = Ske;
	}

private:
	void UpdateModelMatrix();

	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	FTexture* DiffuseTex;
	LSkeleton* Skeleton;

	XMFLOAT3 ModelLocation;
	XMFLOAT3 ModelRotation;
	XMFLOAT3 ModelScale;
	XMMATRIX ModelMatrix;
};