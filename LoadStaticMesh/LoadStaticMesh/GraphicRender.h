#pragma once

#include "stdafx.h"
#include "GraphicHelper.h"
#include "ApplicationMain.h"


class GraphicRender
{

public:
	GraphicRender();
	~GraphicRender();

	void Initialize(ApplicationMain* Application, UINT Width, UINT Height);

	virtual void OnInit() = 0;
	virtual bool Render() = 0;
	virtual void Update() = 0;
	virtual void Destroy() = 0;

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

private: 
	std::wstring AssetsPath;
	
public:
	ApplicationMain* MainApplication;
};