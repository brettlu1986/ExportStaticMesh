
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

void LCharacter::SetLocation(XMFLOAT3 Location)
{
	SkeletalMesh->SetModelLocation(Location);
}	

void LCharacter::SetRotation(XMFLOAT3 Rotator)
{
	SkeletalMesh->SetModelRotation(Rotator);
}

void LCharacter::SetScale3D(XMFLOAT3 Scale)
{
	SkeletalMesh->SetModelScale(Scale);
}

XMFLOAT3 LCharacter::GetLocation()
{
	return SkeletalMesh->GetLocation();
}
 
//in ue4, the character MeshComponent is -90 yaw to parent
XMFLOAT3 LCharacter::GetRotation()
{
	return XMFLOAT3(SkeletalMesh->GetRotation().x, SkeletalMesh->GetRotation().y + 90, SkeletalMesh->GetRotation().z);
}

XMFLOAT3 LCharacter::GetScale3D()
{
	return SkeletalMesh->GetScale3D();
}