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

	LSkeleton* GetSkeleton()
	{
		return Skeleton;
	}

	void SetPsoKey(std::string InKey)
	{
		UsePsoKey = InKey;
	}

	const std::string& GetPsoKey()
	{
		return UsePsoKey;
	}

	FVertexBuffer* GetVertexBuffer()
	{
		return VertexBuffer;
	}

	FIndexBuffer* GetIndexBuffer()
	{
		return IndexBuffer;
	}

	XMFLOAT3 GetRotation()
	{
		return ModelRotation;
	}

	XMFLOAT3 GetLocation()
	{
		return ModelLocation;
	}

	XMFLOAT3 GetScale3D()
	{
		return ModelLocation;
	}

	void SetCurrentBoneMap(std::vector<UINT16>& BoneMap)
	{
		CurrentBoneMap.resize(BoneMap.size());
		std::copy(BoneMap.begin(), BoneMap.end(), CurrentBoneMap.begin());
	}

	const std::vector<UINT16>& GetBoneMap()
	{
		return CurrentBoneMap;
	}

	FResourceView* MtConstantBufferView;
	FResourceView* SkeletalConstantBufferView;
	//TODO: move to material
	FResourceView* ShaderResView;

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
	std::string UsePsoKey;

	std::vector<UINT16> CurrentBoneMap;// real bone index used in this skeletal, it is associate with the vertex.InfluenceBones Index
};