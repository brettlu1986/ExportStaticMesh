#include "pch.h"
#include "LPlayerController.h"
#include "LCharacter.h"
#include "FSkeletalMesh.h"

LPlayerController::LPlayerController()
:ControlledCharacter(nullptr)
, RotationYawInput(0.f)
, RotationPitchInput(0.f)
, Scale(3.f)
{

}

LPlayerController::~LPlayerController()
{

}

void LPlayerController::Possess(LCharacter* Cha)
{
	ControlledCharacter = Cha;
	ControlledCharacter->SetPlayerController(this);
}

void LPlayerController::Reset()
{	
	RotationYawInput = 0;
	RotationPitchInput = 0;
}

void LPlayerController::AddYawInput(float DeltaYaw)
{
	RotationYawInput += DeltaYaw * Scale;
}

void LPlayerController::AddPitchInput(float DeltaPitch)
{
	RotationPitchInput += DeltaPitch * Scale;
}

void LPlayerController::UpdateRotation(float dt)
{
	Vec3 Rotator = ControlledCharacter->GetSkeletalMesh()->GetRotation();
	Rotator.x += RotationPitchInput;
	Rotator.y += RotationYawInput;
	ControlledCharacter->SetRotation(Rotator);
}

void LPlayerController::Update(float dt)
{
	if(RotationPitchInput != 0.f || RotationYawInput != 0)
	{
		UpdateRotation(dt);
	}
}

