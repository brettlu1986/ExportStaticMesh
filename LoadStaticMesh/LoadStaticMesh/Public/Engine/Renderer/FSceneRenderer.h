#pragma once
#include "FRenderer.h"

class FScene;
class RHICommandList;
class FSceneRenderer : public FRenderer
{
public: 
	FSceneRenderer();
	virtual ~FSceneRenderer();

	virtual void BeginRenderFrame(FScene* Scene) override;
	virtual void EndRenderFram(FScene* Scene) override;
	virtual void Render(FRHICommandList& CommandList, FScene* Scene) override;

};
