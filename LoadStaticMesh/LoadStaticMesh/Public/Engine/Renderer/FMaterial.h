#pragma once

#include "stdafx.h"

#include "FDefine.h"
#include "FRenderResource.h"

class FMaterial : public FRenderResource
{
public:
	FMaterial();
	FMaterial(E_FRESOURCE_TYPE Type);
	virtual ~FMaterial();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void Init(const string& Name, XMFLOAT4 InDiffuseAlbedo, XMFLOAT3 InFresnelR0, float Roughness);

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
	XMFLOAT4 DiffuseAlbedo;
	XMFLOAT3 FresnelR0;
	float Roughness;
	XMFLOAT4X4 MaterialTransform;

};