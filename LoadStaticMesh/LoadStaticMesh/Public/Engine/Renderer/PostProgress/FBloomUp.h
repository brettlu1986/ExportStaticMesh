#pragma once

#include "stdafx.h"
#include "FBloomDef.h"
#include "FDynamicRHI.h"

class FPostResources;
class FTexture;
class FResourceView;
class FBloomUp
{
public:
	FBloomUp();
	~FBloomUp();

	void Initialize(FPostResources* PostResIns);
	void Render(FIndexBuffer* IndexBuffer);

	const FRHIViewPort& GetViewPort(UINT Index)
	{
		return BloomUpViewPort[Index];
	}

	FResourceView* GetSRV(UINT Index)
	{
		return SRVBloomUp[Index];
	}

private:
	FPostResources* PostRes;

	FRHIViewPort BloomUpViewPort[UP_SAMPLE_COUNT];
	FTexture* BloomUpTex[UP_SAMPLE_COUNT];
	FResourceView* SRVBloomUp[UP_SAMPLE_COUNT];
	FResourceView* RTVBloomUp[UP_SAMPLE_COUNT];
	FResourceView* CBVBloomUp[UP_SAMPLE_COUNT];

};
