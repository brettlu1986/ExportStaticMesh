#pragma once

#include "stdafx.h"
#include "FBloomDef.h"
#include "FDynamicRHI.h"

class FPostResources;
class FTexture;
class FResourceView;
class FBloomSetup
{
public:
	FBloomSetup();
	~FBloomSetup();

	void Initialize(FPostResources* PostResIns);
	void Render(FIndexBuffer* IndexBuffer);

	const FRHIViewPort& GetViewPort()
	{
		return SetUpViewPort;
	}

	FResourceView* GetSRV()
	{
		return SRVBloomSetup;
	}
	
private:
	FPostResources* PostRes;

	FRHIViewPort SetUpViewPort;

	//bloom setup
	FTexture* BloomSetupTex;
	FResourceView* RTVBloomSetup;
	FResourceView* CBVBloomSetup;
	FResourceView* SRVBloomSetup;

};
