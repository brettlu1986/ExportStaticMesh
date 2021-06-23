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
	void UpdateMaterialConstantInRenderThread(float Alpha, XMFLOAT4 ColorBlendAdd);

	bool IsBlendModeTransparency()
	{
		return BlendMode.BlendModeValue == E_BLEND_MODE::BLEND_Translucent;
	}

	FResourceView* MaterialConstantBufferView;

	FTexture* DiffuseTex;
	FTexture* NormalTex;

	FResourceView* DiffuseResView;
	FResourceView* NormalResView;

	LBlendMode BlendMode;
};