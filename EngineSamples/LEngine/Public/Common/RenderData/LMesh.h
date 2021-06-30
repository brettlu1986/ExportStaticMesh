#pragma once

#include "pch.h"
#include "LResource.h"
#include "LIndexBuffer.h"
#include "LVertexBuffer.h"
#include "LMaterialBase.h"
#include "LAssetManager.h"

class FMesh;
class LENGINE_API LMesh : public LResource
{
public:
	LMesh();
	virtual ~LMesh();

	void SetMeshBufferInfo(LStaticMeshBuffer* MeshBuffer);
	void SetMaterial(LMaterialBase* MatData);

	void InitRenderThreadResource();
	void DestroyRenderThreadResource();

	void SetModelLocation(Vec3 Location);
	void SetModelRotation(Vec3 Rotator);
	void SetModelScale(Vec3 Scale);
	XMMATRIX GetModelMatrix() const { return ModelMatrix; }

	Vec3 GetModelLocation()
	{
		return ModelLocation;
	}

	void SetName(string& InName)
	{
		Name = InName;
	}

	const string& GetName()
	{
		return Name;
	}
	
private:
	void UpdateModelMatrix();

	LStaticMeshBuffer* StaticMeshBuffer;
	LMaterialBase* MaterialData;

	Vec3 ModelLocation;
	Vec3 ModelRotation;
	Vec3 ModelScale;
	XMMATRIX ModelMatrix;

	shared_ptr<FMesh> RenderMesh;
	string Name;
};