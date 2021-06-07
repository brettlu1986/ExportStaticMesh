#pragma once

#include "stdafx.h"
#include "LResource.h"
#include "LIndexBuffer.h"
#include "LVertexBuffer.h"
#include "LMaterial.h"

class LSkeleton;
class FSkeletalMesh;
class LCamera;
class LSkeletalMesh : public LResource
{
public:
	LSkeletalMesh();
	virtual ~LSkeletalMesh();

	void SetVertexBufferInfo(const char* DataSource, UINT DataSize, UINT DataCount);
	void SetIndexBufferInfo(UINT InCount, UINT InByteSize, E_INDEX_TYPE InType, void* InData);

	void InitRenderThreadResource();
	void DestroyRenderThreadResource();

	void SetModelLocation(XMFLOAT3 Location);
	void SetModelRotation(XMFLOAT3 Rotator);
	void SetModelScale(XMFLOAT3 Scale);
	XMMATRIX GetModelMatrix() const { return ModelMatrix; }

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

	void SetCurrentBoneMap(vector<UINT16>& BoneMap)
	{
		CurrentBoneMap.resize(BoneMap.size());
		copy(BoneMap.begin(), BoneMap.end(), CurrentBoneMap.begin());
	}

	void UpdateBoneMapFinalTransform(vector<XMFLOAT4X4>& BoneMapFinal);
	void UpdateModelMatrix();

	const vector<UINT16>& GetBoneMap()
	{
		return CurrentBoneMap;
	}

	void SetSkeleton(LSkeleton* Ske)
	{
		Skeleton = Ske;
	}

	LSkeleton* GetSkeleton()
	{
		return Skeleton;
	}

	void SetMaterial(LMaterial* MatData);
	
private:

	shared_ptr<LIndexBuffer> IndexBufferData;
	shared_ptr<LVertexBuffer> VertexBufferData;

	LMaterial* MaterialData;

	XMFLOAT3 ModelLocation;
	XMFLOAT3 ModelRotation;
	XMFLOAT3 ModelScale;
	XMMATRIX ModelMatrix;
	LSkeleton* Skeleton;

	shared_ptr<FSkeletalMesh> RenderMesh;
	bool bUpdateWorldTrans;

	vector<UINT16> CurrentBoneMap;// real bone index used in this skeletal, it is associate with the vertex.InfluenceBones Index
};