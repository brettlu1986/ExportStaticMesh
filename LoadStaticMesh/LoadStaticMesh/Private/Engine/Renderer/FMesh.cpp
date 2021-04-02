#include "FMesh.h"
#include "LDataComponent.h"
#include "FRenderComponent.h"

FMesh::FMesh()
:DataComponent(nullptr)
,RenderComponent(nullptr)
{
	DataComponent = new LDataComponent();
	RenderComponent = new FRenderComponent();
}

FMesh::~FMesh()
{
}

void FMesh::Init()
{
	DataComponent->Init();
	RenderComponent->Init();
}

void FMesh::Destroy()
{
	DataComponent->Destroy();
	RenderComponent->Destroy();
}



