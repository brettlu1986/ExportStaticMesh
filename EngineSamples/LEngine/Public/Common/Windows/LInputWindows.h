#pragma once

#include "pch.h"
#include "LInput.h"

class LInputWindows : public LInput
{
public:
	LInputWindows();
	virtual~ LInputWindows();

	virtual void Initialize() override;
	virtual bool Update() override;

	static LRESULT CALLBACK MessageHandler(HWND Hwnd, UINT Umsg, WPARAM Wparam, LPARAM Lparam);

	virtual void GetCurrentCursorLocation(POINT& p) override;

};