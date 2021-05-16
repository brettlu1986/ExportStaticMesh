#pragma once

#include "stdafx.h"
#include "FRenderResource.h"
#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FTexture.h"
#include "LSkeleton.h"
#include "FResourceViewCreater.h"

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
	void SetDiffuseTexture(FTexture* Tex);

	void InitRenderResource();
	XMMATRIX& GetModelMatrix()
	{
		return ModelMatrix;
	}

	void SetSkeleton(LSkeleton* Ske)
	{
		Skeleton = Ske;
	}

	void SetPsoKey(std::string InKey)
	{
		UsePsoKey = InKey;
	}

	const std::string& GetPsoKey()
	{
		return UsePsoKey;
	}
private:
	void UpdateModelMatrix();

	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;

	//TODO: move to material
	FTexture* DiffuseTex;
	FResourceView* ShaderResView;

	LSkeleton* Skeleton;

	XMFLOAT3 ModelLocation;
	XMFLOAT3 ModelRotation;
	XMFLOAT3 ModelScale;
	XMMATRIX ModelMatrix;

	std::string UsePsoKey;
};