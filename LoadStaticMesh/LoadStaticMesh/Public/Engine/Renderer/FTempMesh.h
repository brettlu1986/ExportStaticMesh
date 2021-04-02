#pragma once

#include "LDataComponent.h"
#include "FRenderComponent.h"

class FTempMesh
{
public:
	FTempMesh();
	~FTempMesh();

	void Init();
	void Destroy();

	LDataComponent* DataComponent;
	FRenderComponent* RenderComponent;
	
};