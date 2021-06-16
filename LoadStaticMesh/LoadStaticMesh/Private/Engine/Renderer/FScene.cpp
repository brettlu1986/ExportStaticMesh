
#include "FScene.h"
#include "FMesh.h"
#include "FRHI.h"
#include "FLight.h"
#include "LEngine.h"
#include "LLog.h"

FScene::FScene()
:PassLightInfo(nullptr)
,PassViewProj(nullptr)
{
}

FScene::~FScene()
{
	Destroy();
}

void FScene::Destroy()
{
	delete PassViewProj;
	PassViewProj = nullptr;

	delete PassLightInfo;
	PassLightInfo = nullptr;
}

void FScene::UpdateViewProjInfo(XMMATRIX ViewProj, XMFLOAT3 EyeLoc)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	XMStoreFloat4x4(&ViewProjInfo.ViewProj, XMMatrixTranspose(ViewProj));
	ViewProjInfo.EyePosW = EyeLoc;
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

	if(Mesh->GetMaterial()->IsBlendModeTransparency())
	{
		UINT Len = (UINT)TranparencyMeshes.size();
		Mesh->SetMeshIndex(Len);
		TranparencyMeshes.push_back(Mesh);
	}
	else 
	{
		UINT Len = (UINT)Meshes.size();
		Mesh->SetMeshIndex(Len);
		Meshes.push_back(Mesh);
	}
}

void FScene::DeleteMeshToScene(FMesh* Mesh)
{
	UINT MeshIndex = Mesh->GetMeshIndex();
	if (Mesh->GetMaterial()->IsBlendModeTransparency())
	{
		std::vector<FMesh*>::iterator it;
		for (it = TranparencyMeshes.begin(); it != TranparencyMeshes.end(); ++it)
		{
			if(*it != nullptr && MeshIndex == (*it)->GetMeshIndex())
			{
				(*it)->Destroy();
				TranparencyMeshes.erase(it);
				break;
			}
		}
	}
	else 
	{
		std::vector<FMesh*>::iterator it;
		for (it = Meshes.begin(); it != Meshes.end(); ++it)
		{
			if (*it != nullptr && MeshIndex == (*it)->GetMeshIndex())
			{
				(*it)->Destroy();
				Meshes.erase(it);
				break;
			}
		}
	}
}

void FScene::AddLightToScene(FLight* Light)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	SceneLights.push_back(Light);
}

void FScene::DeleteLightToScene(FLight* Light)
{
	UINT LightIndex = Light->GetLightIndex();
	SceneLights[LightIndex]->Destroy();
	SceneLights[LightIndex] = nullptr;
}

void FScene::AddSkeletalMeshToScene(FSkeletalMesh* Mesh)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	UINT Len = (UINT)SkmMeshes.size();
	Mesh->SetMeshIndex(Len);
	SkmMeshes.push_back(Mesh);
}

void FScene::DeleteSkeletalMeshToScene(FSkeletalMesh* Mesh)
{
	UINT MeshIndex = Mesh->GetMeshIndex();
	std::vector<FSkeletalMesh*>::iterator it;
	for (it = SkmMeshes.begin(); it != SkmMeshes.end(); ++it)
	{
		if (*it != nullptr && MeshIndex == (*it)->GetMeshIndex())
		{
			(*it)->Destroy();
			SkmMeshes.erase(it);
			break;
		}
	}
}

const vector<FMesh*>& FScene::GetDrawTransparencyMeshes()
{
	XMFLOAT3 CameraLocation = ViewProjInfo.EyePosW;
	sort(TranparencyMeshes.begin(), TranparencyMeshes.end(), [CameraLocation]( FMesh* A,  FMesh* B)
	{
		float DisA = MathHelper::Distance(A->GetModelLocation(), CameraLocation);
		float DisB = MathHelper::Distance(B->GetModelLocation(), CameraLocation);
		return DisA > DisB;
	});

	/*LLog::Log("===========\n");
	for(size_t i = 0; i < TranparencyMeshes.size(); i++)
	{
		LLog::Log("Name:: %s \n", TranparencyMeshes[i]->GetName().c_str());
	}
	LLog::Log("===========\n");*/

	return TranparencyMeshes;
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





