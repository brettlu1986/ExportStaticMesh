#include "LDevice.h"

LDevice::LDevice(UINT InW, UINT InH)
:Width(InW)
,Height(InH)
{
}

LDevice::~LDevice()
{
}

bool LDevice::Run()
{
	return true;
}

void LDevice::Destroy()
{

}