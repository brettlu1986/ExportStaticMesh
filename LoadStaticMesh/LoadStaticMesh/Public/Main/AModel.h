#pragma once

#include "LDataComponent.h"
#include "FRenderComponent.h"

class AModel
{
public:
	AModel();
	~AModel();

	void Init();
	void Destroy();

	LDataComponent* DataComponent;
	FRenderComponent* RenderComponent;
	
};