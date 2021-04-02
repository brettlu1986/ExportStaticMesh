#pragma once

#include "LDataComponent.h"
#include "FRenderComponent.h"

class FMesh
{
public:
	FMesh();
	~FMesh();

	void Init();
	void Destroy();

	LDataComponent* DataComponent;
	FRenderComponent* RenderComponent;
	
};