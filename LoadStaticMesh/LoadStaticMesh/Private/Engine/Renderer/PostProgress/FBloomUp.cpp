
#include "FBloomUp.h"
#include "FPostResources.h"
#include "FRHI.h"

FBloomUp::FBloomUp()
{
	//bloom up
	for (UINT i = 0; i < UP_SAMPLE_COUNT; i++)
	{
		BloomUpTex[i] = nullptr;
		SRVBloomUp[i] = nullptr;
		RTVBloomUp[i] = nullptr;
		CBVBloomUp[i] = nullptr;
	}
}

FBloomUp::~FBloomUp()
{
	//bloom up
	for (UINT i = 0; i < UP_SAMPLE_COUNT; i++)
	{
		SAFE_DESTROY(BloomUpTex[i]);
		SAFE_DELETE(RTVBloomUp[i]);
		SAFE_DELETE(CBVBloomUp[i]);
		SAFE_DELETE(SRVBloomUp[i]);
	}
}

void FBloomUp::Initialize(FPostResources* PostResIns)
{
	PostRes = PostResIns;
	//bloom up
	GRHI->CreatePipelineStateObject({ "BloomUp", FullInputElementDescs, _countof(FullInputElementDescs), "BloomUpVs",
		"BloomUpPs", 1, BloomRTFormat, FRHIRasterizerState() });

	FBloomSetup* BloomSetup = PostRes->GetBloomSetup();
	UINT SetUpWidth = (UINT)BloomSetup->GetViewPort().Width;
	UINT SetUpHeight = (UINT)BloomSetup->GetViewPort().Height;

	FClearValue BloomClearValue;
	BloomClearValue.Format = BloomRTFormat;
	BloomClearValue.ClearDepth = nullptr;
	BloomClearValue.ClearColor = &PostClearColor;

	for (UINT i = 0; i < UP_SAMPLE_COUNT; i++)
	{
		UINT UpW = SetUpWidth >> (DOWN_SAMPLE_COUNT - 1 - i);
		UINT UpH = SetUpHeight >> (DOWN_SAMPLE_COUNT - 1 - i);
		BloomUpViewPort[i] = FRHIViewPort(0.f, 0.f, (float)UpW, (float)UpH);

		UINT UpCompW = SetUpWidth >> (DOWN_SAMPLE_COUNT - i);
		UINT UpCompH = SetUpHeight >> (DOWN_SAMPLE_COUNT - i);

		FTextureInitializer UpTexInitializer =
		{
			UpW, UpH, 1, 1, BloomRTFormat, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &BloomClearValue,
			E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
		};
		BloomUpTex[i] = GRHI->CreateTexture(UpTexInitializer);
		SRVBloomUp[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &BloomUpTex[i],
		0, BloomRTFormat });
		RTVBloomUp[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &BloomUpTex[i], 0, BloomRTFormat });
		CBVBloomUp[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassBloomUp)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });

		FPassBloomUp BloomUp;
		BloomUp.BufferASizeAndInvSize = XMFLOAT4(static_cast<float>(UpW), static_cast<float>(UpH), 1.f / UpW, 1.f / UpH);
		BloomUp.BufferBSizeAndInvSize = XMFLOAT4(static_cast<float>(UpCompW), static_cast<float>(UpCompH), 1.f / UpCompW, 1.f / UpCompH);
		float Factor = 1.f / 8.f;
		BloomUp.BloomTintA = XMFLOAT4(BloomTintAs[i].x * Factor, BloomTintAs[i].y * Factor, BloomTintAs[i].z * Factor, BloomTintAs[i].w * Factor); ;
		BloomUp.BloomTintB = XMFLOAT4(BloomTintBs[i].x * Factor, BloomTintBs[i].y * Factor, BloomTintBs[i].z * Factor, BloomTintBs[i].w * Factor);
		BloomUp.BloomUpScales.x = 0.66f * 2.0f;
		BloomUp.BloomUpScales.y = 0.66f * 2.0f;
		GRHI->UpdateConstantBufferView(CBVBloomUp[i], &BloomUp);
	}

}

void FBloomUp::Render(FIndexBuffer* IndexBuffer)
{
	USER_MARKER("Bloom Up");
	GRHI->SetPiplineStateObject(GRHI->GetPsoObject("BloomUp"));
	FBloomDown* BloomDown = PostRes->GetBloomDown();
	for (UINT i = 0; i < UP_SAMPLE_COUNT; i++)
	{
		USER_MARKER_FORMAT("Bloom Up %.0f x %.0f", BloomUpViewPort[i].Width, BloomUpViewPort[i].Height);
		GRHI->SetViewPortInfo(BloomUpViewPort[i]);
		GRHI->ResourceTransition(SRVBloomUp[i], E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
		GRHI->SetRenderTargets(RTVBloomUp[i], nullptr);
		GRHI->SetResourceParams(0, CBVBloomUp[i]);
		if (i == 0)
		{
			GRHI->ResourceTransition(BloomDown->GetSRV(UP_SAMPLE_COUNT - 1), E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			GRHI->ResourceTransition(BloomDown->GetSRV(UP_SAMPLE_COUNT), E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			GRHI->SetResourceParams(4, BloomDown->GetSRV(UP_SAMPLE_COUNT - 1));
			GRHI->SetResourceParams(5, BloomDown->GetSRV(UP_SAMPLE_COUNT));
		}
		else
		{
			GRHI->ResourceTransition(BloomDown->GetSRV(UP_SAMPLE_COUNT - i - 1), E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			GRHI->ResourceTransition(SRVBloomUp[i - 1], E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			GRHI->SetResourceParams(4, BloomDown->GetSRV(UP_SAMPLE_COUNT - i - 1));
			GRHI->SetResourceParams(5, SRVBloomUp[i - 1]);
		}
		GRHI->DrawTriangleList(IndexBuffer);
	}
}

