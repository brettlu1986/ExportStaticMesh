
#include "stdafx.h"

#include "FRHI.h"
#include "FMaterial.h"

FMaterial::FMaterial()
:FRenderResource(E_FRESOURCE_TYPE::F_TYPE_MATERIAL)
, DiffuseTex(nullptr)
, NormalTex(nullptr)
, DiffuseResView(nullptr)
, NormalResView(nullptr)
, MaterialConstantBufferView(nullptr)
{

}

FMaterial::FMaterial(E_FRESOURCE_TYPE Type)
:FRenderResource(Type)
, DiffuseTex(nullptr)
, NormalTex(nullptr)
, DiffuseResView(nullptr)
, NormalResView(nullptr)
, MaterialConstantBufferView(nullptr)
{

}

FMaterial::~FMaterial()
{

}

void FMaterial::Destroy() 
{
	if (DiffuseTex)
	{
		DiffuseTex->Destroy();
		delete DiffuseTex;
		DiffuseTex = nullptr;

		delete DiffuseResView;
		DiffuseResView = nullptr;
	}

	if (NormalTex)
	{
		NormalTex->Destroy();
		delete NormalTex;
		NormalTex = nullptr;

		delete NormalResView;
		NormalResView = nullptr;
	}

	delete MaterialConstantBufferView;
	MaterialConstantBufferView = nullptr;
}

void FMaterial::Initialize()
{

}

void FMaterial::Init(LMaterialBase& MaterialData)
{
	BlendMode.BlendModeName = MaterialData.GetBlendMode().BlendModeName;
	BlendMode.BlendModeValue = MaterialData.GetBlendMode().BlendModeValue;

	if(MaterialData.GetParamTexture(0) != nullptr)
	{
		DiffuseTex = GRHI->CreateTexture(MaterialData.GetParamTexture(0));
		DiffuseResView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &DiffuseTex,
			0, E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	}

	if (MaterialData.GetParamTexture(1) != nullptr)
	{
		NormalTex = GRHI->CreateTexture(MaterialData.GetParamTexture(1));
		NormalResView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &NormalTex,
			0, E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	}

	MaterialConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FMaterialConstants)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });

	UpdateMaterialConstantInRenderThread(MaterialData.GetParamFloat(0), MaterialData.GetParamVector(0));
}

void FMaterial::UpdateMaterialConstantInRenderThread(float Alpha, XMFLOAT4 ColorBlendAdd)
{
	FMaterialConstants MatConstants;
	MatConstants.Alpha = Alpha;
	MatConstants.ColorBlendAdd = ColorBlendAdd;
	GRHI->UpdateConstantBufferView(MaterialConstantBufferView, &MatConstants);
}
