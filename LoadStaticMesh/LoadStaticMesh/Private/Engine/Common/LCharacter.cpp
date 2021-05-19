
#include "LCharacter.h"
#include "FSkeletalMesh.h"
#include "LPlayerController.h"
#include "LCharacterMovement.h"

LCharacter::LCharacter()
:SkeletalMesh(nullptr)
,AnimatorIns(nullptr)
,Controller(nullptr)
,IsLocalControlled(false)
,ChaMovement(nullptr)
{
	ChaMovement = new LCharacterMovement();
}

LCharacter::~LCharacter()
{
	Destroy();
}

void LCharacter::Destroy()
{
	if (SkeletalMesh)
	{
		SkeletalMesh->Destroy();
		delete SkeletalMesh;
		SkeletalMesh = nullptr;
	}

	if (AnimatorIns)
	{
		delete AnimatorIns;
		AnimatorIns = nullptr;
	}

	if(Controller)
	{
		delete Controller;
		Controller = nullptr;
	}

	delete ChaMovement;
	ChaMovement = nullptr;
}

void LCharacter::Update(float dt)
{
	if (AnimatorIns)
	{
		AnimatorIns->Update(dt);
	}
}