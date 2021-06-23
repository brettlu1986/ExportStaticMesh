#pragma once

#ifdef LENGINE_EXPORTS
#define LENGINE_API __declspec(dllexport)
#else
#define LENGINE_API __declspec(dllimport)
#endif

extern "C" LENGINE_API int TestPlus(int a, int b);