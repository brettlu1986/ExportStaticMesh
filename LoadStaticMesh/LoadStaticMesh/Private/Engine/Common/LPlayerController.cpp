
#include "LPlayerController.h"
#include "LCharacter.h"
#include "FSkeletalMesh.h"

LPlayerController::LPlayerController()
:ControlledCharacter(nullptr)
, RotationYawInput(0.f)
, RotationPitchInput(0.f)
, Scale(2.2f)
{

}

LPlayerController::~LPlayerController()
{

}

void LPlayerController::Possess(LCharacter* Cha)
{
	ControlledCharacter = Cha;
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
	XMFLOAT3 Rotator = ControlledCharacter->GetSkeletalMesh()->GetRotation();
	Rotator.x += RotationPitchInput;
	Rotator.y += RotationYawInput;
	ControlledCharacter->SetRotation(Rotator);
}

void LPlayerController::Update(float dt)
{
	UpdateRotation(dt);
}