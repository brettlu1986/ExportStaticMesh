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

	void Destroy();
	void InitRenderResource();

	void SetPsoKey(std::string Key)
	{
		PsoKey = Key;
	}

	std::string& GetPsoKey()
	{
		return PsoKey;
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
