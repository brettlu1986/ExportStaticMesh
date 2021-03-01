

#include "InputClass.h"

InputClass::InputClass()
{

}

InputClass::~InputClass()
{

}


void InputClass::Initialize(ApplicationMain* application)
{
	m_application = application;
}

bool InputClass::Update()
{

	return true;
}


void InputClass::Destroy()
{
}