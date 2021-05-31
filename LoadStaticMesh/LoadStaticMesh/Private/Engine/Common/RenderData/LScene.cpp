

#include "LScene.h"
#include "LMesh.h"

#include "LEngine.h"


LScene::LScene()
{

}

LScene::~LScene()
{

}

void LScene::AddStaticMeshes(shared_ptr<LMesh>& Mesh)
{
	assert(LEngine::GetEngine()->IsGameThread());
	Mesh->InitRenderThreadResource();
	StaticMeshes.push_back(move(Mesh));
}
