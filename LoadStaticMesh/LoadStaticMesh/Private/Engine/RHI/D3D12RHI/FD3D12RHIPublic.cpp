
#include "stdafx.h"

#include <dxgidebug.h>
#include <d3dcompiler.h>
#include "FD3D12RHIPrivate.h"
#include "FD3D12Helper.h"
#include "FDefine.h"
#include "LAssetDataLoader.h"
#include "LEngine.h"
#include "d3dx12.h"
#include "LDeviceWindows.h"
#include "FD3D12ResourceViewCreater.h"
#include "pix.h"

using namespace Microsoft::WRL;

static const FRHIColor ClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

FDynamicRHI* FD3D12DynamicRHIModule::CreateRHI()
{
	return new FD3D12DynamicRHI();
}

/////////////////////////// dynamic RHI implement
FD3D12DynamicRHI::FD3D12DynamicRHI()
:FenceEvent(HANDLE())
,FenceValues(0)
,FrameIndex(0)
,Window(HWND())
,WndHeight(0)
,WndWidth(0)
,DefaultViewPort(0.f, 0.f, 0.f, 0.f)
{
	
}

void FD3D12DynamicRHI::Init()
{
	FindAdapter();
	InitializeDevices();
	CreateSampler();
	LDeviceWindows* DeviceWindows = dynamic_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	InitWindow(DeviceWindows->GetWidth(), DeviceWindows->GetHeight(), DeviceWindows->GetHwnd());
}

void FD3D12DynamicRHI::ShutDown()
{
	WaitForPreviousFrame();
	CloseHandle(FenceEvent);
	GpuFence.Reset();
	SwapChain.Reset();

	for (auto Pso : PiplelineStateObjCache)
	{
		Pso.second->Destroy();
	}
	PiplelineStateObjCache.clear();

	ResourceViewCreater->OnDestroy();
	delete ResourceViewCreater;
	ResourceViewCreater = nullptr;

	CommandAllocator.Reset();
	CommandList.Reset();
	CommandQueue.Reset();
	D3DDevice.Reset();

#if defined(_DEBUG)
	{
		ComPtr<IDXGIDebug1> DxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DxgiDebug))))
		{
			DxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
}

void FD3D12DynamicRHI::BeginRenderScene()
{
	ThrowIfFailed(CommandAllocator->Reset());
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));
}

//void FD3D12DynamicRHI::UpdateSceneResources(FScene* RenderScene)
//{
//	if (RenderScene != nullptr)
//	{
//		if (RenderScene->IsConstantDirty())
//		{
//			UpdateSceneMtConstants(RenderScene);
//			RenderScene->DecreaseDirtyCount();
//		}
//		UpdateScenePassConstants(RenderScene);
//		UpdateSceneSkeletalConstants(RenderScene);
//	}
//}

void FD3D12DynamicRHI::EndRenderScene()
{
	vector<ID3D12CommandList*> CmdLists;
	CommandList->Close();
	CmdLists.push_back(CommandList.Get());
	CommandQueue->ExecuteCommandLists(static_cast<UINT>(CmdLists.size()), CmdLists.data());
	//TODO:Present first should use 1, for Vsync 
	SwapChain->Present(0, 0);
	WaitForPreviousFrame();
	FrameIndex = (FrameIndex + 1) % FRAME_COUNT;
}

//void FD3D12DynamicRHI::BeginCreateSceneResource()
//{
//	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));
//}

//void FD3D12DynamicRHI::EndCreateSceneResource()
//{
//	vector<ID3D12CommandList*> CmdLists;
//	CommandList->Close();
//	CmdLists.push_back(CommandList.Get());
//	CommandQueue->ExecuteCommandLists(static_cast<UINT>(CmdLists.size()), CmdLists.data());
//	FenceValues = 1;
//	WaitForPreviousFrame();
//}

FIndexBuffer* FD3D12DynamicRHI::RHICreateIndexBuffer()
{
	return new FD3D12IndexBuffer();
}

FVertexBuffer* FD3D12DynamicRHI::RHICreateVertexBuffer()
{
	return new FD3D12VertexBuffer();
}

void FD3D12DynamicRHI::UpdateVertexBufferResource(FVertexBuffer* VertexBuffer, LVertexBuffer& VertexBufferData)
{
	FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(VertexBuffer);
	D3DVertexBuffer->InitGPUVertexBufferView(D3DDevice.Get(), CommandList.Get(), VertexBufferData);
}

void FD3D12DynamicRHI::UpdateIndexBufferResource(FIndexBuffer* IndexBuffer, LIndexBuffer& IndexBufferData)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	D3DIndexBuffer->InitGPUIndexBufferView(D3DDevice.Get(), CommandList.Get(), IndexBufferData);
}

FShader* FD3D12DynamicRHI::RHICreateShader(LPCWSTR  ShaderFile)
{
	UINT8* ShaderData = nullptr;
	UINT ShaderLen = 0;
	ThrowIfFailed(ReadDataFromFile(LAssetDataLoader::GetAssetFullPath(ShaderFile).c_str(), &ShaderData, &ShaderLen));
	return new FShader(ShaderData, ShaderLen);
}

FTexture* FD3D12DynamicRHI::RHICreateTexture()
{
	return new FD3D12Texture(D3DDevice.Get());
}

FTexture* FD3D12DynamicRHI::CreateTexture(FTextureInitializer TexInitializer)
{
	FTexture* Tex = new FD3D12Texture(D3DDevice.Get());
	Tex->InitializeTexture(TexInitializer);
	return Tex;
}

void FD3D12DynamicRHI::CreateResourceViewCreater(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount)
{
	ResourceViewCreater = new FD3D12ResourceViewCreater(D3DDevice.Get());
	ResourceViewCreater->OnCreate(CbvCount, SrvCount, UavCount, DsvCount, RtvCount, SamplerCount);
}

void FD3D12DynamicRHI::CreateVertexAndIndexBufferView(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(VertexBuffer);
	D3DIndexBuffer->InitGPUIndexBufferView(D3DDevice.Get(), CommandList.Get());
	D3DVertexBuffer->InitGPUVertexBufferView(D3DDevice.Get(), CommandList.Get());
}

FResourceView* FD3D12DynamicRHI::CreateResourceView(FResourceViewInfo ViewInfo)
{
	FD3D12ResourceViewCreater* ViewCreater = dynamic_cast<FD3D12ResourceViewCreater*>(ResourceViewCreater);
	if (ViewInfo.ViewType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV)
	{
		FResourceView* ResView = new FD3D12ResourceView();
		ViewCreater->AllocDescriptor(ViewInfo.ViewCount, ViewInfo.ViewType, ResView);
		for (UINT i = 0; i < ViewInfo.ViewCount; i++)
		{
			FD3D12Texture* D3DTex = dynamic_cast<FD3D12Texture*>(*(ViewInfo.TexResource + i));
			FD3D12ResourceView* View = dynamic_cast<FD3D12ResourceView*>(ResView);
			D3DTex->InitGPUTextureView(D3DDevice.Get(), CommandList.Get(), View->GetCpu(i), static_cast<DXGI_FORMAT>(ViewInfo.Format));
			View->SetViewResource(D3DTex->Resource());
		}
		return ResView;
	}
	else if (ViewInfo.ViewType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV)
	{
		FResourceView* CbResView = new FD3D12CbvResourceView();
		ViewCreater->AllocDescriptor(ViewInfo.ViewCount, ViewInfo.ViewType, CbResView);

		FD3D12CbvResourceView* View = dynamic_cast<FD3D12CbvResourceView*>(CbResView);
		View->SetConstantBufferViewInfo(D3DDevice.Get(), ViewInfo.DataSize);
		return CbResView;
	}
	else if (ViewInfo.ViewType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_DSV)
	{
		FResourceView* DsvResView = new FD3D12DsvResourceView();
		ViewCreater->AllocDescriptor(ViewInfo.ViewCount, ViewInfo.ViewType, DsvResView);

		FD3D12DsvResourceView* View = dynamic_cast<FD3D12DsvResourceView*>(DsvResView);
		FD3D12Texture* D3DTex = dynamic_cast<FD3D12Texture*>(*ViewInfo.TexResource);
		View->SetDsvViewInfo(D3DDevice.Get(), D3DTex, static_cast<DXGI_FORMAT>(ViewInfo.Format));
		return DsvResView;
	}
	else if (ViewInfo.ViewType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV)
	{
		FResourceView* RtvResView = new FD3D12RtvResourceView();
		ViewCreater->AllocDescriptor(ViewInfo.ViewCount, ViewInfo.ViewType, RtvResView);
		FD3D12RtvResourceView* View = dynamic_cast<FD3D12RtvResourceView*>(RtvResView);
		if (ViewInfo.TexResource == nullptr)
		{
			View->SetRtvViewInfo(D3DDevice.Get(), SwapChain.Get(), static_cast<DXGI_FORMAT>(ViewInfo.Format), ViewInfo.Index);
		}
		else
		{
			FD3D12Texture* D3DTex = dynamic_cast<FD3D12Texture*>(*ViewInfo.TexResource);
			View->SetRtvViewInfo(D3DDevice.Get(), D3DTex, static_cast<DXGI_FORMAT>(ViewInfo.Format));
		}
		return RtvResView;
	}
	assert(!"not valid resource view type");
	return new FResourceView();
}

void FD3D12DynamicRHI::UpdateConstantBufferView(FResourceView* CbvView, void* pDataUpdate)
{
	FD3D12CbvResourceView* CbvResView = dynamic_cast<FD3D12CbvResourceView*>(CbvView);
	CbvResView->UpdateConstantBufferInfo(pDataUpdate);
}

void FD3D12DynamicRHI::CreatePipelineStateObject(FPiplineStateInitializer Initializer)
{
	if (PiplelineStateObjCache.find(Initializer.KeyName) != PiplelineStateObjCache.end())
		return;

	FD3DGraphicPipline* PsoObj = new FD3DGraphicPipline();

	CD3DX12_ROOT_PARAMETER RootParameters[6];
	ZeroMemory(RootParameters, sizeof(RootParameters));
	CD3DX12_DESCRIPTOR_RANGE Ranges[6];
	ZeroMemory(Ranges, sizeof(Ranges));
	Ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); //matrix constant
	Ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1); //material constant & skeleton matrix palette
	Ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2); //pass ViewProj
	Ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 3); //pass Light
	Ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //object diffuse texture
	Ranges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); //shadow map  

	RootParameters[0].InitAsDescriptorTable(1, &Ranges[0], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[1].InitAsDescriptorTable(1, &Ranges[1], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[2].InitAsDescriptorTable(1, &Ranges[2], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[3].InitAsDescriptorTable(1, &Ranges[3], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[4].InitAsDescriptorTable(1, &Ranges[4], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameters[5].InitAsDescriptorTable(1, &Ranges[5], D3D12_SHADER_VISIBILITY_PIXEL);

	// Allow input layout and deny uneccessary access to certain pipeline stages.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(RootParameters), RootParameters, (UINT)StaticSamplers.size(), StaticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> Signature;
	ComPtr<ID3DBlob> Error;

	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
	ThrowIfFailed(D3DDevice.Get()->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&PsoObj->RootSignature)));
	NAME_D3D12_OBJECT(PsoObj->RootSignature);

	CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
	rasterizerStateDesc.FrontCounterClockwise = Initializer.RasterizerStat.FrontCounterClockwise;
	rasterizerStateDesc.DepthBias = Initializer.RasterizerStat.DepthBias;
	rasterizerStateDesc.DepthBiasClamp = Initializer.RasterizerStat.DepthBiasClamp;
	rasterizerStateDesc.SlopeScaledDepthBias = Initializer.RasterizerStat.SlopeScaledDepthBias;
	auto ConvCullMode = [](FCullMode Mode) -> D3D12_CULL_MODE
	{
		switch (Mode)
		{
		case FCullMode::CULL_MODE_NONE: { return D3D12_CULL_MODE_NONE; }
		case FCullMode::CULL_MODE_FRONT: { return D3D12_CULL_MODE_FRONT; }
		case FCullMode::CULL_MODE_BACK: { return D3D12_CULL_MODE_BACK; }
		default: {return D3D12_CULL_MODE_NONE; }
		}
	};
	rasterizerStateDesc.CullMode = ConvCullMode(Initializer.RasterizerStat.CullMode);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
	ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	vector<D3D12_INPUT_ELEMENT_DESC> InputElementDescs;
	auto ConvertRHIFormatToD3DFormat = [](ERHI_DATA_FORMAT InFormat) -> DXGI_FORMAT
	{
		switch (InFormat)
		{
		case ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT: { return DXGI_FORMAT_R32G32B32_FLOAT; }
		case ERHI_DATA_FORMAT::FORMAT_R32G32_FLOAT: { return DXGI_FORMAT_R32G32_FLOAT; }
		case ERHI_DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT: { return DXGI_FORMAT_R32G32B32A32_FLOAT; }
		case ERHI_DATA_FORMAT::FORMAT_R16G16B16A16_UINT: { return DXGI_FORMAT_R16G16B16A16_UINT; }
		default: {return DXGI_FORMAT_UNKNOWN; }
		}
	};

	auto ConvertRHIClassificationToD3D = [](ERHI_INPUT_CLASSIFICATION InClassify) ->D3D12_INPUT_CLASSIFICATION
	{
		switch (InClassify)
		{
		case ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA: { return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA; }
		case ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_INSTANCE_DATA: { return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA; }
		default: { return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA; }
		}
	};

	for (UINT i = 0; i < Initializer.NumElements; i++)
	{
		FRHIInputElement* RHIDesc = (FRHIInputElement*)(Initializer.pInputElement + i);
		D3D12_INPUT_ELEMENT_DESC D3DInputDesc;
		D3DInputDesc.SemanticName = RHIDesc->SemanticName.c_str();
		D3DInputDesc.SemanticIndex = RHIDesc->SemanticIndex;
		D3DInputDesc.Format = ConvertRHIFormatToD3DFormat(RHIDesc->Format);
		D3DInputDesc.InputSlot = RHIDesc->InputSlot;
		D3DInputDesc.AlignedByteOffset = RHIDesc->AlignedByteOffset;
		D3DInputDesc.InputSlotClass = ConvertRHIClassificationToD3D(RHIDesc->InputSlotClass);
		D3DInputDesc.InstanceDataStepRate = RHIDesc->InstanceDataStepRate;
		InputElementDescs.push_back(D3DInputDesc);
	}

	FShader* Vs = RHICreateShader(Initializer.VsResource);
	FShader* Ps = RHICreateShader(Initializer.PsResource);

	PsoDesc.InputLayout = { InputElementDescs.data(), Initializer.NumElements };
	PsoDesc.pRootSignature = PsoObj->RootSignature.Get();
	PsoDesc.VS = CD3DX12_SHADER_BYTECODE(Vs->GetShaderByteCode(), Vs->GetDataLength());
	PsoDesc.PS = CD3DX12_SHADER_BYTECODE(Ps->GetShaderByteCode(), Ps->GetDataLength());
	PsoDesc.RasterizerState = rasterizerStateDesc;
	PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PsoDesc.SampleMask = UINT_MAX;
	PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PsoDesc.NumRenderTargets = Initializer.NumRenderTargets;
	PsoDesc.RTVFormats[0] = Initializer.RtvFormat == FRtvFormat::FORMAT_UNKNOWN ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R8G8B8A8_UNORM;
	PsoDesc.SampleDesc.Count = 1;
	PsoDesc.SampleDesc.Quality = 0;	////not use 4XMSAA
	PsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;//DXGI_FORMAT_D32_FLOAT;// DXGI_FORMAT_D24_UNORM_S8_UINT;

	ComPtr<ID3D12PipelineState> PipelineState;
	ThrowIfFailed(D3DDevice.Get()->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&PsoObj->PipelineState)));
	NAME_D3D12_OBJECT(PsoObj->PipelineState);
	PiplelineStateObjCache[Initializer.KeyName] = PsoObj;

	delete Vs;
	delete Ps;
}

void FD3D12DynamicRHI::SetResourceHeaps(vector<FResourceHeap*>& Heaps)
{
	vector <ID3D12DescriptorHeap*> _Heaps;
	for (size_t i = 0; i < Heaps.size(); i++)
	{
		FD3D12ResourceViewHeap* Heap = dynamic_cast<FD3D12ResourceViewHeap*>(Heaps[i]);
		_Heaps.push_back(Heap->GetHeap());
	}
	CommandList->SetDescriptorHeaps((UINT)_Heaps.size(), _Heaps.data());
}

void FD3D12DynamicRHI::SetRenderTargets(FResourceView* RtvView, FResourceView* DsvView)
{
	FD3D12DsvResourceView* Dsv = dynamic_cast<FD3D12DsvResourceView*>(DsvView);
	CommandList->ClearDepthStencilView(Dsv->GetCpu(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = Dsv->GetCpu();

	if (RtvView != nullptr)
	{
		FD3D12RtvResourceView* Rtv = dynamic_cast<FD3D12RtvResourceView*>(RtvView);
		float Color[4] = { ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A };

		D3D12_CPU_DESCRIPTOR_HANDLE RtvHandle = Rtv->GetCpu();
		CommandList->ClearRenderTargetView(RtvHandle, Color, 0, nullptr);
		CommandList->OMSetRenderTargets(1, &RtvHandle, true, &DsvHandle);
	}
	else
	{
		CommandList->OMSetRenderTargets(0, nullptr, false, &DsvHandle);
	}
}

void FD3D12DynamicRHI::SetVertexAndIndexBuffers(FVertexBuffer* VertexBuffer, FIndexBuffer* IndexBuffer)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(VertexBuffer);
	CommandList->IASetVertexBuffers(0, 1, &(D3DVertexBuffer->GetVertexBufferView()));
	CommandList->IASetIndexBuffer(&(D3DIndexBuffer->GetIndexBufferView()));
}

void FD3D12DynamicRHI::SetPiplineStateObject(FD3DGraphicPipline* PsoObj)
{
	CommandList->SetGraphicsRootSignature(PsoObj->RootSignature.Get());
	CommandList->SetPipelineState(PsoObj->PipelineState.Get());
}

void FD3D12DynamicRHI::SetResourceParams(UINT Index, FResourceView* ResView)
{
	FD3D12ResourceView* View = dynamic_cast<FD3D12ResourceView*>(ResView);
	CommandList->SetGraphicsRootDescriptorTable(Index, View->GetGpu());
}

void FD3D12DynamicRHI::DrawTriangleList(FIndexBuffer* IndexBuffer)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->DrawIndexedInstanced(D3DIndexBuffer->GetIndicesCount(), 1, 0, 0, 0);
}

void FD3D12DynamicRHI::ResourceTransition(FResourceView* ResourceView, E_RESOURCE_STATE StateFrom, E_RESOURCE_STATE StateTo)
{
	FD3D12ResourceView* ResView = dynamic_cast<FD3D12ResourceView*>(ResourceView);
	ID3D12Resource* Resource = ResView->Resource();
	if (Resource)
	{
		const D3D12_RESOURCE_BARRIER Rb1 = CD3DX12_RESOURCE_BARRIER::Transition(Resource, static_cast<D3D12_RESOURCE_STATES>(StateFrom), static_cast<D3D12_RESOURCE_STATES>(StateTo));
		CommandList->ResourceBarrier(1, &Rb1);
	}
	else
	{
		assert(!"resource should be valid!");
	}
}

void FD3D12DynamicRHI::BeginEvent(const char* Name)
{
	PIXBeginEvent(CommandList.Get(), 0, Name);
}

void FD3D12DynamicRHI::EndEvent()
{
	PIXEndEvent(CommandList.Get());
}

void FD3D12DynamicRHI::SetViewPortInfo(FRHIViewPort ViewPort)
{
	CD3DX12_VIEWPORT Viewport = CD3DX12_VIEWPORT(ViewPort.Left,
		ViewPort.Top,
		ViewPort.Width,
		ViewPort.Height);
	CD3DX12_RECT ScissorRect = CD3DX12_RECT(static_cast<LONG>(ViewPort.Left),
		static_cast<LONG>(ViewPort.Top),
		static_cast<LONG>(ViewPort.Left + ViewPort.Width),
		static_cast<LONG>(ViewPort.Top + ViewPort.Height));

	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);
}

//void FD3D12DynamicRHI::UpdateSceneMtConstants(FScene* RenderScene)
//{
//	const vector<FMesh*>& Meshes = RenderScene->GetDrawMeshes();
//	for (size_t i = 0; i < Meshes.size(); i++)
//	{
//		FObjectConstants ObjConstants;
//		XMStoreFloat4x4(&ObjConstants.World, XMMatrixTranspose(Meshes[i]->GetModelMatrix()));
//		XMFLOAT4X4 TexMat = Meshes[i]->GetTextureTransform();
//		XMStoreFloat4x4(&ObjConstants.TexTransform, XMMatrixTranspose(XMLoadFloat4x4(&TexMat)));
//		FD3D12CbvResourceView* MatrixCbvResView = dynamic_cast<FD3D12CbvResourceView*>(Meshes[i]->MatrixConstantBufferView);
//		MatrixCbvResView->UpdateConstantBufferInfo(&ObjConstants);
//
//		FMaterial* Material = Meshes[i]->GetMaterial();
//		XMFLOAT4X4 Mat = Material->GetMaterialTransform();
//		XMMATRIX MatTransform = XMLoadFloat4x4(&Mat);
//		FMaterialConstants MatConstants;
//		MatConstants.DiffuseAlbedo = Material->GetDiffuseAlbedo();
//		MatConstants.FresnelR0 = Material->GetFresnelR0();
//		MatConstants.Roughness = Material->GetRoughness();
//		XMStoreFloat4x4(&MatConstants.MatTransform, XMMatrixTranspose(MatTransform));
//		FD3D12CbvResourceView* MaterialCbvView = dynamic_cast<FD3D12CbvResourceView*>(Meshes[i]->MaterialConstantBufferView);
//		MaterialCbvView->UpdateConstantBufferInfo(&MatConstants);
//	}
//
//}

//void FD3D12DynamicRHI::UpdateScenePassConstants(FScene* RenderScene)
//{
//	FPassConstants PassConstant;
//	LCamera* Camera = RenderScene->GetActiveCamera();
//	XMFLOAT4X4 MtProj;
//	XMStoreFloat4x4(&MtProj, Camera->GetProjectionMatrix());
//	XMMATRIX ViewProj = Camera->GetViewMarix() * XMLoadFloat4x4(&MtProj);
//	XMStoreFloat4x4(&PassConstant.ViewProj, XMMatrixTranspose(ViewProj));
//	PassConstant.EyePosW = Camera->GetCameraLocation();
//	FLight* Light = RenderScene->GetLight(0);
//	PassConstant.Lights[0].Direction = Light->Direction;
//	PassConstant.Lights[0].Strength = Light->Strength;
//	PassConstant.Lights[0].Position = Light->Position;
//
//	XMFLOAT3 LightUp = { 0, 0, 1 };
//	XMMATRIX LightView = XMMatrixLookToLH(XMLoadFloat3(&Light->Position), XMLoadFloat3(&Light->Direction), XMLoadFloat3(&LightUp));
//	XMMATRIX LightProj = XMMatrixOrthographicLH((float)50, (float)50, 1.f, 100.f);
//	XMMATRIX LightSpaceMatrix = LightView * LightProj;
//
//	XMStoreFloat4x4(&PassConstant.LightSpaceMatrix, XMMatrixTranspose(LightSpaceMatrix));
//
//	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
//	XMMATRIX T(
//		0.5f, 0.0f, 0.0f, 0.0f,
//		0.0f, -0.5f, 0.0f, 0.0f,
//		0.0f, 0.0f, 1.0f, 0.0f,
//		0.5f, 0.5f, 0.0f, 1.0f);
//	XMMATRIX S = LightView * LightProj * T;
//	XMStoreFloat4x4(&PassConstant.ShadowTransform, XMMatrixTranspose(S));
//
//	FD3D12CbvResourceView* CbvResView = dynamic_cast<FD3D12CbvResourceView*>(RenderScene->PassContantView);
//	CbvResView->UpdateConstantBufferInfo(&PassConstant);
//}
//
//void FD3D12DynamicRHI::UpdateSceneSkeletalConstants(FScene* RenderScene)
//{
//	for (size_t i = 0; i < RenderScene->GetCharacters().size(); i++)
//	{
//		LCharacter* Character = RenderScene->GetCharacters()[i];
//		FSkeletalMesh* Mesh = Character->GetSkeletalMesh();
//		//update world matrix
//		FObjectConstants ObjConstants;
//		XMStoreFloat4x4(&ObjConstants.World, XMMatrixTranspose(Mesh->GetModelMatrix()));
//		XMFLOAT4X4 TexMat = MathHelper::Identity4x4();
//		XMStoreFloat4x4(&ObjConstants.TexTransform, XMMatrixTranspose(XMLoadFloat4x4(&TexMat)));
//		FD3D12CbvResourceView* CbvResView1 = dynamic_cast<FD3D12CbvResourceView*>(Mesh->MatrixConstantBufferView);
//		CbvResView1->UpdateConstantBufferInfo(&ObjConstants);
//
//		//bone map matrix pallete
//		LAnimator* Animator = Character->GetAnimator();
//		FSkeletalConstants SkeCon;
//		copy(begin(Animator->GetBoneMapFinalTransforms()),
//			end(Animator->GetBoneMapFinalTransforms()), &SkeCon.BoneMapBoneTransforms[0]);
//		FD3D12CbvResourceView* CbvResView2 = dynamic_cast<FD3D12CbvResourceView*>(Mesh->SkeletalConstantBufferView);
//		CbvResView2->UpdateConstantBufferInfo(&SkeCon);
//
//	}
//}

void FD3D12DynamicRHI::FindAdapter()
{
#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		ComPtr<ID3D12Debug> DebugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
		{
			DebugController->EnableDebugLayer();
		}
	}
#endif

	ComPtr<IDXGIFactory4> DxgiFactory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));

	ComPtr<IDXGIAdapter1> Adapter;
	ComPtr<IDXGIFactory6> Factory6;

	ID3D12Device* pDevice = nullptr;
	const D3D_FEATURE_LEVEL MinRequiredFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
	DXGI_ADAPTER_DESC1 Desc;
	UINT Index;
	if (SUCCEEDED(DxgiFactory->QueryInterface(IID_PPV_ARGS(&Factory6))))
	{
		for (
			UINT AdapterIndex = 0;
			DXGI_ERROR_NOT_FOUND != Factory6->EnumAdapterByGpuPreference(
				AdapterIndex,
				DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&Adapter));
			++AdapterIndex)
		{
			Adapter->GetDesc1(&Desc);
			Index = AdapterIndex;
			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
			{
				break;
			}
		}
	}
	else
	{
		for (UINT AdapterIndex = 0; DXGI_ERROR_NOT_FOUND != DxgiFactory->EnumAdapters1(AdapterIndex, &Adapter); ++AdapterIndex)
		{
			Adapter->GetDesc1(&Desc);
			Index = AdapterIndex;
			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}
			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
			{
				break;
			}
		}
	}
	CurrentAdapterDesc = FD3D12AdapterDesc(Desc, Index, MaxSupportedFeatureLevel, pDevice->GetNodeCount());
	pDevice->Release();
}

void FD3D12DynamicRHI::InitWindow(UINT Width, UINT Height, void* InWindow)
{
	WndWidth = Width;
	WndHeight = Height;
	Window = (HWND)InWindow;
	DefaultViewPort = FRHIViewPort(0.f, 0.f, static_cast<float>(Width), static_cast<float>(Height));

	CreateSwapChain();
}

void FD3D12DynamicRHI::InitializeDevices()
{
	if (!D3DDevice)
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));

		ComPtr<IDXGIFactory6> Factory6;
		if (SUCCEEDED(DxgiFactory->QueryInterface(IID_PPV_ARGS(&Factory6))))
		{
			Factory6->EnumAdapterByGpuPreference(
				CurrentAdapterDesc.AdapterIndex,
				DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&DxgiAdapter));
		}

		HRESULT HardwareResult = D3D12CreateDevice(
			DxgiAdapter.Get(),             // default adapter
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&D3DDevice));

		if (FAILED(HardwareResult))
		{
			ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&DxgiAdapter)));
			ThrowIfFailed(D3D12CreateDevice(
				DxgiAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&D3DDevice)));
		}
		NAME_D3D12_OBJECT(D3DDevice);

		D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
		QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ThrowIfFailed(D3DDevice.Get()->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));
		NAME_D3D12_OBJECT(CommandQueue);

		FrameIndex = 0;
		ThrowIfFailed(D3DDevice.Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
		NAME_D3D12_OBJECT(CommandAllocator);

		ThrowIfFailed(D3DDevice.Get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(),
			nullptr, IID_PPV_ARGS(&CommandList)));
		NAME_D3D12_OBJECT(CommandList);
		ThrowIfFailed(CommandList->Close());

		ThrowIfFailed(D3DDevice.Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GpuFence)));
		NAME_D3D12_OBJECT(GpuFence);
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		memset(&FenceValues, 0, sizeof(FenceValues));

	}
}

void FD3D12DynamicRHI::CreateSampler()
{
	CD3DX12_STATIC_SAMPLER_DESC ShadowSampler(
		0, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	CD3DX12_STATIC_SAMPLER_DESC TexSampler 
	{
		1,  
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  
		0.f,  
		8, 
		D3D12_COMPARISON_FUNC_LESS_EQUAL, 
	};
	StaticSamplers.push_back(ShadowSampler);
	StaticSamplers.push_back(TexSampler);
}

void FD3D12DynamicRHI::CreateSwapChain()
{
	SwapChain.Reset();

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC Sd;
	Sd.BufferDesc.Width = WndWidth;
	Sd.BufferDesc.Height = WndHeight;
	Sd.BufferDesc.RefreshRate.Numerator = 60;
	Sd.BufferDesc.RefreshRate.Denominator = 1;
	Sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	Sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Sd.SampleDesc.Count = 1;
	Sd.SampleDesc.Quality = 0;
	Sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Sd.BufferCount = RENDER_TARGET_COUNT;
	Sd.OutputWindow = (HWND)Window;
	Sd.Windowed = true;
	Sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(DxgiFactory->CreateSwapChain(
		CommandQueue.Get(),
		&Sd,
		&SwapChain));

}

void FD3D12DynamicRHI::WaitForPreviousFrame()
{
	const UINT64 Value = FenceValues;
	ThrowIfFailed(CommandQueue->Signal(GpuFence.Get(), Value));
	FenceValues++;
	if (GpuFence->GetCompletedValue() < Value)
	{
		ThrowIfFailed(GpuFence->SetEventOnCompletion(Value, FenceEvent));
		WaitForSingleObject(FenceEvent, INFINITE);
	}
}
