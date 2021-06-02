
#include "FScene.h"
#include "FMesh.h"
#include "FRHI.h"
#include "FLight.h"
#include "LEngine.h"

FScene::FScene()
{
}

FScene::~FScene()
{
	Destroy();
}


void FScene::Destroy()
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		Meshes[i]->Destroy();
	}
	Meshes.clear();

	delete PassViewProj;
	PassViewProj = nullptr;

	delete PassLightInfo;
	PassLightInfo = nullptr;
}

void FScene::UpdateViewProjInfo(FPassViewProjection UpdateViewProj)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	ViewProjInfo = UpdateViewProj;
	GRHI->UpdateConstantBufferView(PassViewProj, &ViewProjInfo);
}

void FScene::UpdateLightInfo(FPassLightInfo UpdateLight)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	LightInfo = UpdateLight;
	GRHI->UpdateConstantBufferView(PassLightInfo, &LightInfo);
}

void FScene::AddMeshToScene(FMesh* Mesh)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	UINT Len = (UINT)Meshes.size();
	Mesh->SetMeshIndex(Len);
	Meshes.push_back(Mesh);
}

void FScene::AddLightToScene(FLight* Light)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	SceneLights.push_back(Light);
}

void FScene::AddSkeletalMeshToScene(FSkeletalMesh* Mesh)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	SkmMeshes.push_back(Mesh);
}

void FScene::UpdateLightToScene(FLight* Light)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	UINT LightIndex = Light->GetLightIndex();
	if(LightIndex == 0 )
	{
		FPassLightInfo LightInfo;
		XMFLOAT3 Pos = Light->GetPosition();
		XMFLOAT3 Dir = Light->GetDir();
		LightInfo.Lights[LightIndex].Direction = Dir;
		LightInfo.Lights[LightIndex].Strength = Light->GetStrength();
		LightInfo.Lights[LightIndex].Position = Pos;

		XMFLOAT3 LightUp = { 0, 0, 1 };
		
		XMMATRIX LightView = XMMatrixLookToLH(XMLoadFloat3(&Pos), XMLoadFloat3(&Dir), XMLoadFloat3(&LightUp));
		XMMATRIX LightProj = XMMatrixOrthographicLH((float)50, (float)50, 1.f, 100.f);
		XMMATRIX LightSpaceMatrix = LightView * LightProj;

		XMStoreFloat4x4(&LightInfo.LightSpaceMatrix, XMMatrixTranspose(LightSpaceMatrix));

		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
		XMMATRIX T(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);
		XMMATRIX S = LightView * LightProj * T;
		XMStoreFloat4x4(&LightInfo.ShadowTransform, XMMatrixTranspose(S));

		UpdateLightInfo(LightInfo);
	}
}





