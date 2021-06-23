#pragma once

#include "stdafx.h"
#include "LDefine.h"

class LResource
{
public:
	LResource(E_LRESOURCE_TYPE Type);
	virtual ~LResource();

protected:
	E_LRESOURCE_TYPE ResourceType;
};