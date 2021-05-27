

#include "FShadowMap.h"

FShadowMap::FShadowMap(UINT InWidth, UINT InHeight)
:Width(InWidth)
,Height(InHeight)
,ShadowResource(nullptr)
,ShadowResView(nullptr)
,DsvResView(nullptr)
,ViewPort(FRHIViewPort(0.f, 0.f, (float)InWidth, (float)InHeight))
{
	
}

FShadowMap::~FShadowMap()
{
	Destroy();
}

void FShadowMap::Destroy()
{
	ShadowResource->Destroy();
	delete ShadowResource;
	ShadowResource = nullptr;

	delete ShadowResView;
	ShadowResView = nullptr;

	delete DsvResView;
	DsvResView = nullptr;
}

void FShadowMap::InitRenderResource()
{
	FClearDepth ClearDepth;
	ClearDepth.Depth = 1.0f;
	ClearDepth.Stencil = 0;

	FClearValue ClearValue;
	ClearValue.Format = E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT;
	ClearValue.ClearDepth = &ClearDepth;
	ClearValue.ClearColor = nullptr;

	FTextureInitializer Initializer = 
	{
		Width, Height, 1, 1, E_GRAPHIC_FORMAT::FORMAT_R24G8_TYPELESS, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &ClearValue,
		E_RESOURCE_STATE::RESOURCE_STATE_GENERIC_READ
	};
	ShadowResource = GRHI->CreateTexture(Initializer);
	ShadowResView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &ShadowResource, 0, E_GRAPHIC_FORMAT::FORMAT_R24_UNORM_X8_TYPELESS });
	DsvResView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_DSV, 1, &ShadowResource, 0, E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT });
}


