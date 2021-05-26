#pragma once

#include "stdafx.h"
#include "FDynamicRHI.h"

class FTexture;
class FResourceView;
class FShadowMap
{
public: 
	FShadowMap(UINT InWidth, UINT InHeight);
	~FShadowMap();

	void InitRenderResource();

	void SetPsoKey(std::string Key)
	{
		PsoKey = Key;
	}

	FRHIViewPort ViewPort;
	FTexture* ShadowResource;
	FResourceView* ShadowResView;
	FResourceView* DsvResView;

private: 
	UINT Width;
	UINT Height;
	std::string PsoKey;
};
