
#include "stdafx.h"
#include "FMaterial.h"


FMaterial::FMaterial()
:FRenderResource(E_RESOURCE_TYPE::TYPE_MATERIAL)
, MaterialTransform(MathHelper::Identity4x4())
{

}

FMaterial::FMaterial(E_RESOURCE_TYPE Type)
:FRenderResource(Type)
, MaterialTransform(MathHelper::Identity4x4())
{

}

FMaterial::~FMaterial()
{

}

void FMaterial::Destroy() 
{

}

void FMaterial::Initialize()
{

}

void FMaterial::Init(const string& Name, XMFLOAT4 InDiffuseAlbedo, XMFLOAT3 InFresnelR0, float InRoughness)
{
	ResourceName = Name;
	DiffuseAlbedo = InDiffuseAlbedo;
	FresnelR0 = InFresnelR0;
	Roughness = InRoughness;
}