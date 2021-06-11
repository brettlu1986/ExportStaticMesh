
#include "LMaterialInstance.h"

LMaterialInstance::LMaterialInstance()
:ParentMaterial(nullptr)
{
}

LMaterialInstance::~LMaterialInstance()
{
}

void LMaterialInstance::InitMaterialTemplate(LMaterial* InParent)
{
	ParentMaterial = InParent;
	InitByParent();
}

void LMaterialInstance::InitByParent()
{
	const vector<E_MATERIAL_PARAM_TYPE>& Types = ParentMaterial->GetParamTypes();
	for(auto& Type : Types)
	{
		if(Type == E_MATERIAL_PARAM_TYPE::TYPE_FLOAT)
		{
			const vector<float>& Floats = ParentMaterial->GetParamFloats();
			for(auto& Value : Floats)
			{	
				AddParamFloat(Value);
			}
		}
		else if(Type == E_MATERIAL_PARAM_TYPE::TYPE_VECTOR)
		{
			const vector<XMFLOAT4>& Vects = ParentMaterial->GetParamVectors();
			for (auto& Vec : Vects)
			{
				AddParamVector(Vec);
			}
		}
		else if(Type == E_MATERIAL_PARAM_TYPE::TYPE_TEXTURE)
		{
			const vector<LTexture*> Textures = ParentMaterial->GetParamTextures();
			for (auto& Tex : Textures)
			{
				AddParamTexture(Tex);
			}
		}
	}
}


