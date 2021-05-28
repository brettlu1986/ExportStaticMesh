#pragma once

#include "stdafx.h"
#include "FDefine.h"

class FRenderResource 
{
public:
	FRenderResource();
	FRenderResource(E_RESOURCE_TYPE Type);
	virtual ~FRenderResource();

	virtual void Destroy() = 0;
	virtual void Initialize() = 0;

protected:
	E_RESOURCE_TYPE ResourceType;
	string ResourceName;
};