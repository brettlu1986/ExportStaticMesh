#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <process.h>
#include <MathHelper.h>
#include "d3dx12.h"

#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <assert.h>
#include <algorithm>
#include <memory>

#include <float.h>
#include <cmath>

#include <iostream>
#include <fstream>

#include <map>
#include <unordered_map>
#include <functional>
#include <stdio.h>
#include <thread>
#include <memory>

#include "LType.h"

using namespace DirectX;
using namespace std;
