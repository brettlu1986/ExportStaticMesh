#pragma once

#include "stdafx.h"

class LMesh;
class LScene
{
public:
	LScene();
	~LScene();

	void AddStaticMeshes(shared_ptr<LMesh>& Mesh);

public:

	std::vector<shared_ptr<LMesh>> StaticMeshes;

};
