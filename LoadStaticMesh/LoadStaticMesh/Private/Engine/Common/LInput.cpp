

#include "LInput.h"

#include "LDeviceWindows.h"
#include "LEngine.h"
#include "LDefine.h"
#include "LInputWindows.h"

LInput* LInput::MainInput = nullptr;

LInput::LInput()
{
}

LInput::~LInput()
{

}

void LInput::CreateInput()
{
#ifdef PLATFORM_WINDOWS 
	MainInput = new LInputWindows();
#else 
#endif
}

void LInput::Initialize()
{
}


bool LInput::Update()
{
	return true;
}

void LInput::Destroy()
{
	if(MainInput)
	{
		delete MainInput;
		MainInput = nullptr;
	}
}