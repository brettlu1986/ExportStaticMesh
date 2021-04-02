#pragma once

#include "stdafx.h"
#include "FD3D12Helper.h"
#include "ApplicationMain.h"


class Logic
{

public:
	Logic();
	~Logic();

	void Initialize(ApplicationMain* Application, UINT Width, UINT Height);

	virtual void OnInit() = 0;
	virtual bool Render(void* Param) = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y){}
	virtual void OnMouseUp(WPARAM btnState, int x, int y){}
	virtual void OnMouseMove(WPARAM btnState, int x, int y){}

protected:

	std::wstring GetAssetFullPath(LPCWSTR AssetName);

	void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool RequestHighPerformanceAdapter = false);

	UINT WndWidth;
	UINT WndHeight;
	float AspectRatio;
	bool UseWarpDevice;
	bool bDestroy;

private: 
	std::wstring AssetsPath;
	
public:
	ApplicationMain* MainApplication;
};