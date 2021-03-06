#pragma once

#include "pch.h"
#include "LResource.h"
#include "LMaterialBase.h"
#include "LAssetManager.h"

class LSkeleton;
class FSkeletalMesh;
class LCamera;
class LENGINE_API LSkeletalMesh : public LResource
{
public:
	LSkeletalMesh();
	virtual ~LSkeletalMesh();

	void SetSkeletalMeshBuffer(LSkeletalMeshBuffer* MeshBuffer);

	void InitRenderThreadResource();
	void DestroyRenderThreadResource();

	void SetModelLocation(Vec3 Location);
	void SetModelRotation(Vec3 Rotator);
	void SetModelScale(Vec3 Scale);
	XMMATRIX GetModelMatrix() const { return ModelMatrix; }

	Vec3 GetRotation()
	{
		return ModelRotation;
	}

	Vec3 GetLocation()
	{
		return ModelLocation;
	}

	Vec3 GetScale3D()
	{
		return ModelLocation;
	}

	void UpdateBoneMapFinalTransform(vector<XMFLOAT4X4>& BoneMapFinal);
	void UpdateModelMatrix();

	const vector<UINT16>& GetBoneMap()
	{
		return SkeletalMeshBuffer->BoneMap;
	}

	void SetSkeleton(LSkeleton* Ske)
	{
		Skeleton = Ske;
	}

	LSkeleton* GetSkeleton()
	{
		return Skeleton;
	}

	void SetMaterial(LMaterialBase* MatData);

	void SetName(string& InName)
	{
		Name = InName;
	}

	const string& GetName()
	{
		return Name;
	}
	
private:

	LSkeletalMeshBuffer* SkeletalMeshBuffer;
	LMaterialBase* MaterialData;

	Vec3 ModelLocation;
	Vec3 ModelRotation;
	Vec3 ModelScale;
	XMMATRIX ModelMatrix;
	LSkeleton* Skeleton;

	shared_ptr<FSkeletalMesh> RenderMesh;
	bool bUpdateWorldTrans;
	string Name;
};