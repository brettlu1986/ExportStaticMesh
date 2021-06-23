
#include "FSunMerge.h"
#include "FPostResources.h"
#include "FRHI.h"

FSunMerge::FSunMerge()
:SunMergeTex(nullptr)
, RTVSunMerge(nullptr)
, CBVSunMerge(nullptr)
, SRVSunMerge(nullptr)
{

}

FSunMerge::~FSunMerge()
{
	SAFE_DESTROY(SunMergeTex);
	SAFE_DELETE(SRVSunMerge);
	SAFE_DELETE(RTVSunMerge);
	SAFE_DELETE(CBVSunMerge);
}

void FSunMerge::Initialize(FPostResources* PostResIns)
{	
	PostRes = PostResIns;
	//sun merge
	GRHI->CreatePipelineStateObject({ "SunMerge", FullInputElementDescs, _countof(FullInputElementDescs), "SunMergeVs",
		"SunMergePs", 1, BloomRTFormat, FRHIRasterizerState() });

	FBloomSetup* BloomSetup = PostRes->GetBloomSetup();
	UINT W = (UINT)BloomSetup->GetViewPort().Width;
	UINT H = (UINT)BloomSetup->GetViewPort().Height;
	SunMergeViewPort = FRHIViewPort(0.f, 0.f, (float)W, (float)H);

	FClearValue BloomClearValue;
	BloomClearValue.Format = BloomRTFormat;
	BloomClearValue.ClearDepth = nullptr;
	BloomClearValue.ClearColor = &PostClearColor;

	FTextureInitializer SunMergeTexInitializer =
	{
		W, H, 1, 1, BloomRTFormat, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &BloomClearValue,
		E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
	};
	SunMergeTex = GRHI->CreateTexture(SunMergeTexInitializer);
	SRVSunMerge = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &SunMergeTex,
		0, BloomRTFormat });
	RTVSunMerge = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &SunMergeTex, 0, BloomRTFormat });
	CBVSunMerge = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassSunMerge)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });

	const FRHIViewPort UpViewPort = PostRes->GetBloomUp()->GetViewPort(UP_SAMPLE_COUNT - 1);
	FPassSunMerge PassSunMerge;
	//last bloom up size
	UINT UpW = (UINT)UpViewPort.Width;
	UINT UpH = (UINT)UpViewPort.Height;
	PassSunMerge.BloomUpSizeAndInvSize = XMFLOAT4(static_cast<float>(UpW), static_cast<float>(UpH), 1.f / UpW, 1.f / UpH);
	float Factor = BloomIntensity * 0.5f;
	PassSunMerge.BloomColor = XMFLOAT3(BloomTint1.x * Factor, BloomTint1.y * Factor, BloomTint1.z * Factor);
	GRHI->UpdateConstantBufferView(CBVSunMerge, &PassSunMerge);
}

void FSunMerge::Render(FIndexBuffer* IndexBuffer)
{
	USER_MARKER("Sun Merge");
	FBloomSetup* BloomSetup = PostRes->GetBloomSetup();
	FBloomUp* BloopUp = PostRes->GetBloomUp();
	GRHI->SetViewPortInfo(SunMergeViewPort);
	GRHI->SetPiplineStateObject(GRHI->GetPsoObject("SunMerge"));
	GRHI->ResourceTransition(SRVSunMerge, E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
	GRHI->SetRenderTargets(RTVSunMerge, nullptr);
	GRHI->SetResourceParams(0, CBVSunMerge);

	GRHI->ResourceTransition(BloomSetup->GetSRV(), E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	GRHI->ResourceTransition(BloopUp->GetSRV(UP_SAMPLE_COUNT - 1), E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	GRHI->SetResourceParams(4, BloomSetup->GetSRV());
	GRHI->SetResourceParams(5, BloopUp->GetSRV(UP_SAMPLE_COUNT - 1));
	GRHI->DrawTriangleList(IndexBuffer);
}

