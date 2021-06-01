#pragma once

#include "stdafx.h"

class LMesh;
class LCamera;
class LLight;
class LScene
{
public:
	LScene();
	~LScene();

	void AddStaticMeshes(shared_ptr<LMesh>& Mesh);
	void AddLightToScene(shared_ptr<LLight>& Light);

	void AddCamera(shared_ptr<LCamera>& Camera);
	void ActiveCamera(UINT CameraIndex);
	LCamera* GetActiveCamera();
	

	void Update(float dt);

private:

	vector<shared_ptr<LMesh>> StaticMeshes;
	vector<shared_ptr<LCamera>> Cameras;
	vector<shared_ptr<LLight>> SceneLights;
};
