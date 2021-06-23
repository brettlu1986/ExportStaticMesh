#pragma once

#include "pch.h"
#include "LDevice.h"
#include "LInput.h"
class LDeviceWindows : public LDevice
{
public:
	LDeviceWindows(UINT InW, UINT InH, const char* Name);
	virtual ~LDeviceWindows();

	virtual void Destroy() override;
	virtual bool Run() override;

	const HWND& GetHwnd() { return hMainWnd; }

	LInput* GetInput()
	{
		return Input;
	}
protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

private:
	void CreateMainWindow(const char* Name);
	void CreateInput();

	HWND hMainWnd;
	LInput* Input;

};