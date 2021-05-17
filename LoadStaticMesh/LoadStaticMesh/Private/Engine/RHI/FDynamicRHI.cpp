
#include "stdafx.h"

#include "FRHI.h"
#include "FDynamicRHI.h"

FDynamicRHI* GRHI = nullptr;

bool RHICreate()
{
	if (!GRHI)
	{
		GRHI = PlatformCreateDynamicRHI();
		return true;
	}
	return false;
}

void RHIInit()
{
	if(GRHI)
		GRHI->Init();
}

void RHIExit()
{
	GRHI->ShutDown();
	delete GRHI;
	GRHI = nullptr;
}

