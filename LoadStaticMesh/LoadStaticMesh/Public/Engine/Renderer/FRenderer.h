#pragma once

class FScene;
class FRHICommandList;
class FRenderer
{
public:
	FRenderer();
	virtual ~FRenderer();

	virtual void BeginRenderFrame(FScene* Scene) = 0;
	virtual void EndRenderFram(FScene* Scene) = 0;
	virtual void Render(FRHICommandList& CommandList, FScene* Scene) = 0;
};