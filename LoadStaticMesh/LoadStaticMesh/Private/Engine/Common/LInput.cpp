

#include "LInput.h"

LInput::LInput()
	:MainApplication(nullptr)
{

}

LInput::~LInput()
{

}


void LInput::Initialize(ApplicationMain* Application)
{
	MainApplication = Application;
}

bool LInput::Update()
{

	return true;
}

void LInput::Destroy()
{
	MainApplication = nullptr;
}