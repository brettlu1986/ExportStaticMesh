

#include "LMaterial.h"

LMaterial::LMaterial()
:DiffuseTex(nullptr)
,NormalTex(nullptr)
{
	DiffuseAlbedo = { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f };
	FresnelR0 = { 0.2f, 0.2f, 0.2f };
	Roughness = 0.02f;
	MaterialTransform = MathHelper::Identity4x4();
}

LMaterial::~LMaterial()
{

}

void LMaterial::SetNormalTex(LTexture* Tex)
{
	NormalTex = Tex;
}

void LMaterial::SetColorTex(LTexture* Tex)
{
	DiffuseTex = Tex;
}


