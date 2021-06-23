#pragma once

#include "pch.h"

class LCharacter;
class LENGINE_API LPlayerController
{
public:
	LPlayerController();
	~LPlayerController();

	void Possess(LCharacter* Cha);
	void AddYawInput(float DeltaYaw);
	void AddPitchInput(float DeltaPitch);

	void Reset();
	void UpdateRotation(float dt);
	void Update(float dt);

private:
	LCharacter* ControlledCharacter;

	float RotationYawInput;
	float RotationPitchInput;

	float Scale;
};