
#include "pch.h"
#include "LMaterialBase.h"

LMaterialBase::LMaterialBase()
:LResource(E_LRESOURCE_TYPE::L_TYPE_MATERIAL)
{

}

LMaterialBase::~LMaterialBase()
{

}

void LMaterialBase::AddParamType(E_MATERIAL_PARAM_TYPE Type)
{
	ParamTypes.push_back(Type);
}

void LMaterialBase::AddParamFloat(float Value)
{
	ParamFloats.push_back(Value);
}

void LMaterialBase::AddParamVector(XMFLOAT4 Vec)
{
	ParamVectors.push_back(Vec);
}

void LMaterialBase::AddParamTexture(LTexture* Tex)
{
	ParamTextures.push_back(Tex);
}
