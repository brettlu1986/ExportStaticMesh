#include "FTempMesh.h"
#include "LDataComponent.h"
#include "FRenderComponent.h"

FTempMesh::FTempMesh()
:DataComponent(nullptr)
,RenderComponent(nullptr)
{
	DataComponent = new LDataComponent();
	RenderComponent = new FRenderComponent();
}

FTempMesh::~FTempMesh()
{
}

void FTempMesh::Init()
{
	DataComponent->Init();
	RenderComponent->Init();
}

void FTempMesh::Destroy()
{
	DataComponent->Destroy();
	RenderComponent->Destroy();
}



