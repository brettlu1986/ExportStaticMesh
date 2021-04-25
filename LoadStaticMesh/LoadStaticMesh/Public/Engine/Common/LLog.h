#pragma once

#include "stdafx.h"

class LLog
{

public:

	static void LogA(const char* strOutputString, ...)
	{
#if defined(_DEBUG)
		va_list vlArgs = NULL;
		va_start(vlArgs, strOutputString);
		size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;
		char* strBuffer = new char[nLen];
		_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
		va_end(vlArgs);
		OutputDebugStringA(strBuffer);
		delete[] strBuffer;
#endif
	}

	static void LogW(const wchar_t* strOutputString, ...)
	{
#if defined(_DEBUG)

		va_list vlArgs = NULL;
		va_start(vlArgs, strOutputString);
		size_t nLen = _vscwprintf(strOutputString, vlArgs) + 1;
		wchar_t* strBuffer = new wchar_t[nLen];
		_vsnwprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
		va_end(vlArgs);
		OutputDebugStringW(strBuffer);
		delete[] strBuffer;
#endif
	}

};
