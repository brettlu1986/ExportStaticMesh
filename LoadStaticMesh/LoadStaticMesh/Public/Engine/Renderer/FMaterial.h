#pragma once

#include "stdafx.h"

#include "FDefine.h"
#include "FRenderResource.h"

#include "LMaterial.h"

class FResourceView;
class FTexture;
class FMaterial : public FRenderResource
{
public:
	FMaterial();
	FMaterial(E_FRESOURCE_TYPE Type);
	virtual ~FMaterial();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void Init(LMaterialBase& MaterialData);
	void UpdateMaterialConstantInRenderThread(XMFLOAT4 InDiffuseAlbedo, XMFLOAT3 InFresnelR0, float InRoughness, XMFLOAT4X4 InMatTrans);

	FResourceView* MaterialConstantBufferView;

	FTexture* DiffuseTex;
	FTexture* NormalTex;

	FResourceView* DiffuseResView;
	FResourceView* NormalResView;

protected:
	XMFLOAT4 DiffuseAlbedo;
	XMFLOAT3 FresnelR0;
	float Roughness;
	XMFLOAT4X4 MaterialTransform;

};