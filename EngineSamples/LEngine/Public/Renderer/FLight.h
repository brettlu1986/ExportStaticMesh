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
	void UpdateLightInRenderThread(Vec3 Dir, Vec3 StrengthValues, Vec3 Pos);

	Vec3 GetStrength() {
		return Strength;
	}

	Vec3 GetDir() {
		return Direction;
	}

	Vec3 GetPosition() {
		return Position;
	}

	UINT GetLightIndex() {
		return LightIndex;
	}
private:

	Vec3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;                          // point/spot light only
	Vec3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
	float FalloffEnd = 10.0f;                           // point/spot light only
	Vec3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
	float SpotPower = 64.0f;                            // spot light only
	UINT LightIndex = 0;
};