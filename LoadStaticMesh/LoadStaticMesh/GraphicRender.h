#pragma once

#include "stdafx.h"
#include "GraphicHelper.h"
#include "ApplicationMain.h"


class GraphicRender
{

public:
	GraphicRender();
	~GraphicRender();

	void Initialize(ApplicationMain* application, UINT width, UINT height);

	virtual void OnInit() = 0;
	virtual bool Render() = 0;
	virtual void Destroy() = 0;

protected:

	std::wstring GetAssetFullPath(LPCWSTR assetName);

	void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false);

	UINT m_width;
	UINT m_height;
	float m_aspectRatio;
	bool m_useWarpDevice;

private: 
	std::wstring m_assetsPath;
	
public:
	ApplicationMain* m_application;
};