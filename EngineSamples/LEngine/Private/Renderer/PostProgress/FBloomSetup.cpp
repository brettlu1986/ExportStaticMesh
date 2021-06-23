
#include "pch.h"
#include "FBloomSetup.h"
#include "FPostResources.h"
#include "FRHI.h"

FBloomSetup::FBloomSetup()
:PostRes(nullptr)
,SRVBloomSetup(nullptr)
, BloomSetupTex(nullptr)
, RTVBloomSetup(nullptr)
, CBVBloomSetup(nullptr)
{

}

FBloomSetup::~FBloomSetup()
{
	SAFE_DESTROY(BloomSetupTex);
	SAFE_DELETE(SRVBloomSetup);
	SAFE_DELETE(RTVBloomSetup);
	SAFE_DELETE(CBVBloomSetup);
}

void FBloomSetup::Initialize(FPostResources* PostResIns)
{
	PostRes = PostResIns;

	UINT SetUpWidth = (UINT)(GRHI->GetDefaultViewPort().Width / 4);
	UINT SetUpHeight = (UINT)(GRHI->GetDefaultViewPort().Height / 4);

	SetUpViewPort = FRHIViewPort(0.f, 0.f, (float)SetUpWidth, (float)SetUpHeight);

	GRHI->CreatePipelineStateObject({ "BloomSetup", FullInputElementDescs, _countof(FullInputElementDescs), "BloomSetupVs",
			"BloomSetupPs", 1, BloomRTFormat, FRHIRasterizerState() });


	FClearValue BloomClearValue;
	BloomClearValue.Format = BloomRTFormat;
	BloomClearValue.ClearDepth = nullptr;
	BloomClearValue.ClearColor = &PostClearColor;
	FTextureInitializer SetupTexInitializer =
	{
		SetUpWidth, SetUpHeight, 1, 1, BloomRTFormat, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &BloomClearValue,
		E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
	};
	BloomSetupTex = GRHI->CreateTexture(SetupTexInitializer);
	SRVBloomSetup = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &BloomSetupTex,
		0, BloomRTFormat });
	RTVBloomSetup = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &BloomSetupTex, 0, BloomRTFormat });
	CBVBloomSetup = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassBloomSetup)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });

	FPassBloomSetup PassBloomSetup;
	PassBloomSetup.BufferSizeAndInvSize = XMFLOAT4(static_cast<float>(SetUpWidth), static_cast<float>(SetUpHeight), 1.f / SetUpWidth, 1.f / SetUpHeight);
	PassBloomSetup.BloomThrehold = BloomThreHold;
	GRHI->UpdateConstantBufferView(CBVBloomSetup, &PassBloomSetup);
}

void FBloomSetup::Render(FIndexBuffer* IndexBuffer)
{
	USER_MARKER_FORMAT("Bloom Setup %.0f x %.0f", SetUpViewPort.Width, SetUpViewPort.Height);
	GRHI->SetPiplineStateObject(GRHI->GetPsoObject("BloomSetup"));
	GRHI->SetViewPortInfo(SetUpViewPort);
	GRHI->ResourceTransition(SRVBloomSetup, E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
	GRHI->SetRenderTargets(RTVBloomSetup, nullptr);
	GRHI->SetResourceParams(0, CBVBloomSetup);
	GRHI->SetResourceParams(4, PostRes->SRVSceneColor);
	GRHI->DrawTriangleList(IndexBuffer);
}

