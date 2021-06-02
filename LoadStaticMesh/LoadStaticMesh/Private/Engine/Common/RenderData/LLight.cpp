

#include "LLight.h"
#include "FLight.h"
#include "MathHelper.h"
#include "FRenderThread.h"
#include "LEngine.h"

LLight::LLight()
:LResource(E_LRESOURCE_TYPE::L_TYPE_LIGHT)
,RenderLight(nullptr)
{

}

LLight::~LLight()
{

}

void LLight::Init()
{
	// assume that focus to {0, 0, 0}
	XMVECTOR Target = XMVector3Length(XMVectorSet(Position.x, Position.y, Position.z, 1.f));
	Radius = XMVectorGetX(Target);
	Alpha = acosf(Position.z / Radius);
	Theta = atanf(Position.x / Position.y);
	if (Theta < 0.f)
		Theta += XM_PI * 2;
	Alpha = MathHelper::Clamp(Alpha, 0.1f, MathHelper::Pi - 0.1f);
}

void LLight::Update(float dt)
{
	Theta -= 0.0001f;
	XMVECTOR V = XMVectorSet(Radius * sinf(Alpha) * sinf(Theta),
		Radius * sinf(Alpha) * cosf(Theta),
		Radius * cosf(Alpha),
		1.f);
	Position.x = XMVectorGetX(V);
	Position.y = XMVectorGetY(V);
	Position.z = XMVectorGetZ(V);

	XMVECTOR D = XMVectorSet(-Position.x, -Position.y, -Position.z, 1.f);
	D = XMVector3Normalize(D);
	Direction.x = XMVectorGetX(D);
	Direction.y = XMVectorGetY(D);
	Direction.z = XMVectorGetZ(D);
	
	UpdateLightInRenderThread();
}

void LLight::UpdateLightInRenderThread()
{
	assert(LEngine::GetEngine()->IsGameThread());

	auto RenderLightRes = RenderLight;
	XMFLOAT3 Dir = Direction;
	XMFLOAT3 StrengthValue = Strength;
	XMFLOAT3 Pos = Position;
	RENDER_THREAD_TASK("UpdateLightDirPos",
		[RenderLightRes, Dir, StrengthValue, Pos]()
		{
			RenderLightRes->UpdateLightInRenderThread(Dir, StrengthValue, Pos);
		}
	);
}

void LLight::InitRenderThreadResource()
{
	assert(LEngine::GetEngine()->IsGameThread());
	RenderLight = make_shared<FLight>(this);

	auto RenderLightRes = RenderLight;
	RENDER_THREAD_TASK("AddFlightInRender",
		[RenderLightRes]()
		{
			RenderLightRes->AddLightInRenderThread();
		}
	);
}

void LLight::DestroyRenderThreadResource()
{

}
