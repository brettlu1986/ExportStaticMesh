#pragma once


class LMesh;
class LCamera;
class LLight;
class LCharacter;
class LScreenMesh;
class LENGINE_API LScene
{
public:
	LScene();
	~LScene();

	void SetScreenMesh(shared_ptr<LScreenMesh>& Mesh);
	
	void AddStaticMeshes(shared_ptr<LMesh>& Mesh);
	void AddLightToScene(shared_ptr<LLight>& Light);
	void AddCharacterToScene(shared_ptr<LCharacter> Character);

	void AddCamera(shared_ptr<LCamera>& Camera);
	void ActiveCamera(UINT CameraIndex);
	LCamera* GetActiveCamera();

	void Update(float dt);

	const vector<shared_ptr<LCharacter>>& GetCharacters()
	{
		return Players;
	}

	LCharacter* GetLocalControlPlayer()
	{
		return Players[2].get();
	}

private:

	shared_ptr<LScreenMesh> ScreenMesh;
	vector<shared_ptr<LMesh>> StaticMeshes;
	vector<shared_ptr<LCamera>> Cameras;
	vector<shared_ptr<LLight>> SceneLights;
	vector<shared_ptr<LCharacter>> Players;
};
