
#include "LPlayerController.h"
#include "LCharacter.h"

LPlayerController::LPlayerController()
:ControlledCharacter(nullptr)
, RotationYawInput(0.f)
, RotationPitchInput(0.f)
{

}

LPlayerController::~LPlayerController()
{

}


void LPlayerController::Possess(LCharacter* Cha)
{
	ControlledCharacter = Cha;
}

void LPlayerController::AddYawInput(float DeltaYaw)
{
	
}

void LPlayerController::AddPitchInput(float DeltaPitch)
{

}

void LPlayerController::UpdateRotation(float dt)
{
	
}

void LPlayerController::Update(float dt)
{
	UpdateRotation(dt);
}