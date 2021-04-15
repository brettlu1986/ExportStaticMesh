#pragma once

#include "stdafx.h"
#include "FRenderer.h"
#include "FMesh.h"

class FScene;
class RHICommandList;
class FSceneRenderer : public FRenderer
{
public: 
	FSceneRenderer();
	virtual ~FSceneRenderer();

	virtual void RenderInit(FScene* Scene) override;
	virtual void EndRenderFram(FScene* Scene) override;
	virtual void UpdateResource(FScene* Scene) override;
	virtual void Render(FScene* Scene) override;

private:
	void UpdateMtConstants(FScene* Scene);
	void UpdateMatConstants(FScene* Scene);
	void UpdatePassConstants(FScene* Scene);

	UINT MtBufferSingleSize;
	UINT MtBufferTotalSize;
	UINT MatBufferSingleSize;
	UINT MatBufferTotalSize;
	UINT PassConstantSize;
	FPassConstants PassConstant;
};
