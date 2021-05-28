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

	void SetPsoKey(string Key)
	{
		PsoKey = Key;
	}

	string& GetPsoKey()
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
	string PsoKey;
};
