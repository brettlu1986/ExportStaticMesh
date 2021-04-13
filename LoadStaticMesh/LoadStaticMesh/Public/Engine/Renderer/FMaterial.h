#pragma once

#include "stdafx.h"

#include "FDefine.h"
#include "FRenderResource.h"

class FMaterial : public FRenderResource
{
public:
	FMaterial();
	FMaterial(E_RESOURCE_TYPE Type);
	virtual ~FMaterial();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void Init(const std::string& Name, XMFLOAT4 InDiffuseAlbedo, XMFLOAT3 InFresnelR0, float Roughness);
	void SetMaterialCbvHeapIndex(UINT Index) { MatCbHeapIndex = Index; }
	UINT GetMaterialCbvHeapIndex() const { return MatCbHeapIndex; }

	const XMFLOAT4& GetDiffuseAlbedo() const
	{
		return DiffuseAlbedo;
	}

	const XMFLOAT3& GetFresnelR0() const
	{
		return FresnelR0;
	}

	float GetRoughness() const
	{
		return Roughness;
	}

	const XMFLOAT4X4& GetMaterialTransform() 
	{
		return MaterialTransform;
	}

protected:

	//this index is the offset from first mat cbv in heap
	UINT MatCbHeapIndex;

	XMFLOAT4 DiffuseAlbedo;
	XMFLOAT3 FresnelR0;
	float Roughness;
	XMFLOAT4X4 MaterialTransform;

};