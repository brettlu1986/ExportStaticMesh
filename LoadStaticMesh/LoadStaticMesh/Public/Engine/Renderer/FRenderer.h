#pragma once

class FScene;
class FRHICommandList;
class FRenderer
{
public:
	FRenderer();
	virtual ~FRenderer();

	virtual void RenderInit(FScene* Scene) = 0;
	virtual void EndRenderFram(FScene* Scene) = 0;
	virtual void UpdateResource(FScene* Scene) = 0;
	virtual void Render(FScene* Scene) = 0;
};