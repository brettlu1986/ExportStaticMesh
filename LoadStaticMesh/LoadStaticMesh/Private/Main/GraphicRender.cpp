
#include "Logic.h"

using namespace Microsoft::WRL;

Logic::Logic()
	:WndWidth(0)
	,WndHeight(0)
	,AspectRatio(0.f)
	,UseWarpDevice(false)
	,MainApplication(nullptr)
	,bDestroy(false)
{

}

Logic::~Logic()
{

}

void Logic::Initialize(ApplicationMain* Application, UINT Width, UINT Height)
{
	MainApplication = Application;
	WndWidth = Width;
	WndHeight = Height;

	WCHAR AssetsPathChar[512];
	GetAssetsPath(AssetsPathChar, _countof(AssetsPathChar));
	AssetsPath = AssetsPathChar;
	AspectRatio = static_cast<float>(Width) / static_cast<float>(Height);

	OnInit();
}

std::wstring Logic::GetAssetFullPath(LPCWSTR AssetName)
{
	return AssetsPath + AssetName;
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
void Logic::GetHardwareAdapter(
	IDXGIFactory1* pFactory,
	IDXGIAdapter1** ppAdapter,
	bool RequestHighPerformanceAdapter)
{
	*ppAdapter = nullptr;

	ComPtr<IDXGIAdapter1> Adapter;

	ComPtr<IDXGIFactory6> Factory6;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&Factory6))))
	{
		for (
			UINT AdapterIndex = 0;
			DXGI_ERROR_NOT_FOUND != Factory6->EnumAdapterByGpuPreference(
				AdapterIndex,
				RequestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&Adapter));
			++AdapterIndex)
		{
			DXGI_ADAPTER_DESC1 Desc;
			Adapter->GetDesc1(&Desc);

			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}
	else
	{
		for (UINT AdapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(AdapterIndex, &Adapter); ++AdapterIndex)
		{
			DXGI_ADAPTER_DESC1 Desc;
			Adapter->GetDesc1(&Desc);

			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*ppAdapter = Adapter.Detach();
}


