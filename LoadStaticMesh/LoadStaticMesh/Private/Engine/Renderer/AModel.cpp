#include "AModel.h"
#include "LDataComponent.h"
#include "FRenderComponent.h"

AModel::AModel()
:DataComponent(nullptr)
,RenderComponent(nullptr)
{
	DataComponent = new LDataComponent();
	RenderComponent = new FRenderComponent();
}

AModel::~AModel()
{
}

void AModel::Init()
{
	DataComponent->Init();
	RenderComponent->Init();
}

void AModel::Destroy()
{
	DataComponent->Destroy();
	RenderComponent->Destroy();
}



