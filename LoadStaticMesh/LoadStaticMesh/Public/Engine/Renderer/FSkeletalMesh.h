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

private:
	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	FTexture* DiffuseTex;
	LSkeleton* Skeleton;
};