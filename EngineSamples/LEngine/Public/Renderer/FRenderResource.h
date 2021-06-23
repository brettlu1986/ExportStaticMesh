#pragma once

#include "pch.h"
#include "FDefine.h"

class FRenderResource 
{
public:
	FRenderResource();
	FRenderResource(E_FRESOURCE_TYPE Type);
	virtual ~FRenderResource();

	virtual void Destroy() = 0;
	virtual void Initialize() = 0;

protected:
	E_FRESOURCE_TYPE ResourceType;
	string ResourceName;
};