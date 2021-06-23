#pragma once

#include "pch.h"

#include "LResource.h"

class FLight;
class LENGINE_API LLight : public LResource
{
public:
	LLight();
	virtual ~LLight();

	void Init();
	void Update(float dt);

    void InitRenderThreadResource();
	void DestroyRenderThreadResource();
	void UpdateLightInRenderThread();

	XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;                          // point/spot light only
	XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
	float FalloffEnd = 10.0f;                           // point/spot light only
	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
	float SpotPower = 64.0f;                            // spot light only
	UINT LightIndex = 0;

private:
	float Alpha;
	float Theta;
	float Radius;

	shared_ptr<FLight> RenderLight;
};