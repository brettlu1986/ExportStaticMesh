#pragma once

#include "pch.h"

#include "LResource.h"
#include "LTexture.h"
#include "LMaterialBase.h"

class LENGINE_API LMaterial : public LMaterialBase
{
public:
	LMaterial();
	virtual ~LMaterial();

	void SetNormalTex(LTexture* Tex);
	void SetColorTex(LTexture* Tex);

	bool HasTex()
	{
		return DiffuseTex != nullptr || NormalTex != nullptr;
	}

	LTexture* GetDiffuseTexture()
	{
		return DiffuseTex;
	}

	LTexture* GetNormalTexture()
	{
		return NormalTex;
	}

private:

	LTexture* DiffuseTex;
	LTexture* NormalTex;
};