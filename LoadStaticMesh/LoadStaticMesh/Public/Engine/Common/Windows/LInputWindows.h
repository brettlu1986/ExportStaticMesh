#pragma once

#include "stdafx.h"
#include "LInput.h"

class LInputWindows : public LInput
{
public:
	LInputWindows();
	virtual~ LInputWindows();

	virtual void Initialize() override;
	static LRESULT CALLBACK MessageHandler(HWND Hwnd, UINT Umsg, WPARAM Wparam, LPARAM Lparam);

};