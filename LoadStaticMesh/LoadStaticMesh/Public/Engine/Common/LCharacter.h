#pragma once

#include "stdafx.h"
#include "LAnimator.h"
#include "LActor.h"

class LPlayerController;
class FSkeletalMesh;
class LCharacterMovement;
class LCharacter : public LActor
{
public:
	LCharacter();
	virtual ~LCharacter();

	void Destroy();

	void SetSkeletalMesh(FSkeletalMesh* Mesh)
	{
		SkeletalMesh = Mesh;
	}

	void SetAnimator(LAnimator* Animator)
	{
		AnimatorIns = Animator;
	}

	FSkeletalMesh* GetSkeletalMesh()
	{
		return SkeletalMesh;
	}

	LAnimator* GetAnimator()
	{
		return AnimatorIns;
	}

	void PlayAnimation(std::string AnimName, bool bLoop)
	{
		AnimatorIns->Play(AnimName, bLoop);
	}

	void SetPlayerController(LPlayerController* InController)
	{
		Controller = InController;
		IsLocalControlled = true;
	}

	bool IsLocalControlledPlayer()
	{
		return IsLocalControlled;
	}

	virtual void SetLocation(XMFLOAT3 Location) override;
	virtual void SetRotation(XMFLOAT3 Rotator) override;
	virtual void SetScale3D(XMFLOAT3 Scale) override;

	virtual XMFLOAT3 GetLocation() override;
	virtual XMFLOAT3 GetRotation() override;
	virtual XMFLOAT3 GetScale3D() override;

	void Update(float dt);
private: 

	FSkeletalMesh* SkeletalMesh;
	LAnimator* AnimatorIns;

	LPlayerController* Controller;
	LCharacterMovement* ChaMovement;
	bool IsLocalControlled;
};
