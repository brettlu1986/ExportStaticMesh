#pragma once

#include "pch.h"
#include "FRenderResource.h"
#include "LLight.h"

class FLight : public FRenderResource
{
public:
	FLight(LLight* Light);
	virtual ~FLight();

	virtual void Destroy() override;
	virtual void Initialize() override;

	void AddLightInRenderThread();
	void DeleteLightInRenderThread();
	void UpdateLightInRenderThread(XMFLOAT3 Dir, XMFLOAT3 StrengthValues, XMFLOAT3 Pos);

	XMFLOAT3 GetStrength() {
		return Strength;
	}

	XMFLOAT3 GetDir() {
		return Direction;
	}

	XMFLOAT3 GetPosition() {
		return Position;
	}

	UINT GetLightIndex() {
		return LightIndex;
	}
private:

	XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;                          // point/spot light only
	XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
	float FalloffEnd = 10.0f;                           // point/spot light only
	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
	float SpotPower = 64.0f;                            // spot light only
	UINT LightIndex = 0;
};