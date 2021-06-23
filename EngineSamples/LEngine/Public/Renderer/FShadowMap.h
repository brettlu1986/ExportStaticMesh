#pragma once

#include "pch.h"
#include "FDynamicRHI.h"

class FTexture;
class FResourceView;
class FShadowMap
{
public: 
	FShadowMap(UINT InWidth, UINT InHeight);
	~FShadowMap();

	void Destroy();
	void InitRenderResource();

	FRHIViewPort ViewPort;
	FTexture* ShadowResource;
	FResourceView* ShadowResView;
	FResourceView* DsvResView;

private: 
	UINT Width;
	UINT Height;
};
