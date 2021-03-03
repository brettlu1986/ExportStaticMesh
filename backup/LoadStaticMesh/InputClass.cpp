

#include "InputClass.h"

InputClass::InputClass()
	:m_application(nullptr)
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
	m_application = nullptr;
}