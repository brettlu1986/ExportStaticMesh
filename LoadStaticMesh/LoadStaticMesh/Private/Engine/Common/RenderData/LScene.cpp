

#include "LScene.h"
#include "LMesh.h"
#include "LCamera.h"
#include "LLight.h"
#include "LEngine.h"
#include "LCharacter.h"


LScene::LScene()
{
}

LScene::~LScene()
{
	for(auto Mesh : StaticMeshes)
	{
		Mesh = nullptr;
	}

	for (auto Light : SceneLights)
	{
		Light = nullptr;
	}

	for (auto Player : Players)
	{
		Player = nullptr;
	}

	for (auto Camera : Cameras)
	{
		Camera = nullptr;
	}
}

void LScene::AddStaticMeshes(shared_ptr<LMesh>& Mesh)
{
	assert(LEngine::GetEngine()->IsGameThread());
	StaticMeshes.push_back(std::move(Mesh));
}

void LScene::AddLightToScene(shared_ptr<LLight>& Light)
{
	assert(LEngine::GetEngine()->IsGameThread());
	SceneLights.push_back(std::move(Light));
}

void LScene::AddCharacterToScene(shared_ptr<LCharacter> Character)
{
	assert(LEngine::GetEngine()->IsGameThread());
	Players.push_back(std::move(Character));
}

void LScene::AddCamera(shared_ptr<LCamera>& Camera)
{
	assert(LEngine::GetEngine()->IsGameThread());
	Cameras.push_back(std::move(Camera));
}

void LScene::ActiveCamera(UINT CameraIndex)
{
	assert(CameraIndex < Cameras.size());
	for (UINT i = 0; i < Cameras.size(); i++)
	{
		Cameras[i]->SetActive(CameraIndex == i);
	}
}

LCamera* LScene::GetActiveCamera()
{
	for (shared_ptr<LCamera>& Camera : Cameras)
	{
		if (Camera->IsActive())
			return Camera.get();
	}
	assert(!"must have one active camera");
	return Cameras[0].get();
}


void LScene::Update(float dt)
{
	SceneLights[0]->Update(dt);
	for(auto Ch : Players)
	{
		Ch->Update(dt);
	}
	GetActiveCamera()->Update(dt);
	
}