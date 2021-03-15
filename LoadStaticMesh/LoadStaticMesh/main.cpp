

#include "stdafx.h"
#include "ApplicationMain.h"

#include <iostream>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	ApplicationMain* Application = new ApplicationMain();

	bool result = Application->Initialize(hInstance, 1280, 720, L"Load Mesh Sample");
	if (result)
	{
		Application->Run();
	}

	Application->Destroy();
	delete Application;
	Application = nullptr;

	return 0;
}