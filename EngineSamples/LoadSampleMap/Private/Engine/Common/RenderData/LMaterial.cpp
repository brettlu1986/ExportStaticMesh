

#include "LMaterial.h"

LMaterial::LMaterial()
:DiffuseTex(nullptr)
,NormalTex(nullptr)
{
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


