
#include "FBloomDown.h"
#include "FPostResources.h"
#include "FBloomSetUp.h"

#include "FRHI.h"

FBloomDown::FBloomDown()
:PostRes(nullptr)
{
	//bloom down
	for (UINT i = 0; i < DOWN_SAMPLE_COUNT; i++)
	{
		BloomDownTex[i] = nullptr;
		RTVBloomDown[i] = nullptr;
		CBVBloomDown[i] = nullptr;
		SRVBloomDown[i] = nullptr;
	}
}

FBloomDown::~FBloomDown()
{
	//bloom down 
	for (UINT i = 0; i < DOWN_SAMPLE_COUNT; i++)
	{
		SAFE_DESTROY(BloomDownTex[i]);
		SAFE_DELETE(RTVBloomDown[i]);
		SAFE_DELETE(CBVBloomDown[i]);
		SAFE_DELETE(SRVBloomDown[i]);
	}
}

void FBloomDown::Initialize(FPostResources* PostResIns)
{
	PostRes = PostResIns;

	//bloom down
	GRHI->CreatePipelineStateObject({ "BloomDown", FullInputElementDescs, _countof(FullInputElementDescs), "BloomDownVs",
		"BloomDownPs", 1, BloomRTFormat, FRHIRasterizerState() });

	FBloomSetup* BloomSetup = PostRes->GetBloomSetup();

	FClearValue BloomClearValue;
	BloomClearValue.Format = BloomRTFormat;
	BloomClearValue.ClearDepth = nullptr;
	BloomClearValue.ClearColor = &PostClearColor;

	UINT SetUpWidth = (UINT)BloomSetup->GetViewPort().Width;
	UINT SetUpHeight = (UINT)BloomSetup->GetViewPort().Height;
	//create setup render target
	for (UINT i = 0; i < DOWN_SAMPLE_COUNT; i++)
	{
		UINT W = SetUpWidth >> (i + 1);
		UINT H = SetUpHeight >> (i + 1);
		BloomDowmViewPort[i] = FRHIViewPort(0.f, 0.f, (float)W, (float)H);
		FTextureInitializer DownTexInitializer =
		{
			W, H, 1, 1, BloomRTFormat, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &BloomClearValue,
			E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
		};
		BloomDownTex[i] = GRHI->CreateTexture(DownTexInitializer);
		SRVBloomDown[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &BloomDownTex[i],
		0, BloomRTFormat });
		RTVBloomDown[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &BloomDownTex[i], 0, BloomRTFormat });
		CBVBloomDown[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassBloomDown)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });

		FPassBloomDown PassBloomDown;
		PassBloomDown.BufferSizeAndInvSize = XMFLOAT4(static_cast<float>(W), static_cast<float>(H), 1.f / W, 1.f / H);
		PassBloomDown.BloomDownScale = 0.66f * 4.f;
		GRHI->UpdateConstantBufferView(CBVBloomDown[i], &PassBloomDown);
	}
}

void FBloomDown::Render(FIndexBuffer* IndexBuffer)
{
	USER_MARKER("Bloom Down");
	GRHI->SetPiplineStateObject(GRHI->GetPsoObject("BloomDown"));
	FBloomSetup* BloomSetup = PostRes->GetBloomSetup();
	for (UINT i = 0; i < DOWN_SAMPLE_COUNT; i++)
	{
		USER_MARKER_FORMAT("Bloom Down %.0f x %.0f", BloomDowmViewPort[i].Width, BloomDowmViewPort[i].Height);
		GRHI->SetViewPortInfo(BloomDowmViewPort[i]);
		FResourceView* ResView = nullptr;
		GRHI->ResourceTransition(SRVBloomDown[i], E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
		GRHI->SetRenderTargets(RTVBloomDown[i], nullptr);
		GRHI->SetResourceParams(0, CBVBloomDown[i]);
		if (i == 0)
			ResView = BloomSetup->GetSRV();
		else
			ResView = SRVBloomDown[i - 1];
		GRHI->ResourceTransition(ResView, E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		GRHI->SetResourceParams(4, ResView);
		GRHI->DrawTriangleList(IndexBuffer);
	}
}

