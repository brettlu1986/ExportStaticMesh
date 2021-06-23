

#include "pch.h"
#include "FLight.h"
#include "LEngine.h"

FLight::FLight(LLight* Light)
{
	Strength = Light->Strength;
	FalloffStart = Light->FalloffStart;
	Direction = Light->Direction;
	FalloffEnd = Light->FalloffEnd;
	Position = Light->Position;
	SpotPower = Light->SpotPower;
	LightIndex = Light->LightIndex;
}

FLight::~FLight()
{

}

void FLight::Destroy()
{

}

void FLight::Initialize()
{

}

void FLight::AddLightInRenderThread()
{
	assert(LEngine::GetEngine()->IsRenderThread());

	FRenderThread::Get()->GetRenderScene()->AddLightToScene(this);
}

void FLight::DeleteLightInRenderThread()
{
	assert(LEngine::GetEngine()->IsRenderThread());

	FRenderThread::Get()->GetRenderScene()->DeleteLightToScene(this);
}

void FLight::UpdateLightInRenderThread(XMFLOAT3 Dir, XMFLOAT3 StrengthValues, XMFLOAT3 Pos)
{
	assert(LEngine::GetEngine()->IsRenderThread());
	Direction = Dir;
	Strength = StrengthValues;
	Position = Pos;
	FRenderThread::Get()->GetRenderScene()->UpdateLightToScene(this);
}
