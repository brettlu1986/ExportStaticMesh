

#include "stdafx.h"
#include "ApplicationMain.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

	ApplicationMain* application = new ApplicationMain();

	bool result = application->Initialize(hInstance, 1280, 720, L"Load Mesh Sample");
	if (result)
	{
		application->Run();
	}

	application->Destroy();
	delete application;
	application = nullptr;

	return 0;
}