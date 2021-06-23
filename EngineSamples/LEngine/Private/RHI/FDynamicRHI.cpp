
#include "pch.h"

#include "FRHI.h"
#include "FDynamicRHI.h"

FDynamicRHI* GRHI = nullptr;

void RHIInit()
{
	if (!GRHI)
	{
		GRHI = PlatformCreateDynamicRHI();
	}
	GRHI->Init();
}

void RHIExit()
{
	GRHI->ShutDown();
	delete GRHI;
	GRHI = nullptr;
}

