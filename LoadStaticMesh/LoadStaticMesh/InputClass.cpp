

#include "InputClass.h"

InputClass::InputClass()
	:MainApplication(nullptr)
{

}

InputClass::~InputClass()
{

}


void InputClass::Initialize(ApplicationMain* Application)
{
	MainApplication = Application;
}

bool InputClass::Update()
{

	return true;
}


void InputClass::Destroy()
{
	MainApplication = nullptr;
}