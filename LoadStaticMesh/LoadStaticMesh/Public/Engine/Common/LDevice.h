#pragma once

#include "stdafx.h"

class LDevice
{
public:
	LDevice(UINT InW, UINT InH);
	virtual ~LDevice();

	virtual bool Run();
	virtual void Destroy();

protected:
	UINT Width;
	UINT Height;
};
