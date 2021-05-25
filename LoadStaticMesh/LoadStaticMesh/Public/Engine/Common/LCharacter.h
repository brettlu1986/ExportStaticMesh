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

	void SetAnimator(LAnimator* Animator);
	
	FSkeletalMesh* GetSkeletalMesh()
	{
		return SkeletalMesh;
	}

	LAnimator* GetAnimator()
	{
		return AnimatorIns;
	}

	//void PlayAnimation(std::string AnimName, bool bLoop)
	//{
	//	AnimatorIns->Play(AnimName, bLoop);
	//}

	void SetPlayerController(LPlayerController* InController)
	{
		Controller = InController;
		IsLocalControlled = true;
	}

	//void RegisterAnimationStateEvent();

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

	virtual XMVECTOR GetMoveForwardVector() override;
	virtual XMVECTOR GetMoveRightVector() override;

	void ProcessMouseInput(FInputResult& MouseInput);
	void ProcessKeyInput(FInputResult& KeyInput);

	void Update(float dt);
	void ProcessMoveInput();

	LCharacterMovement* GetCharacterMovement()
	{
		return ChaMovement;
	}
	
	float GetBaseSpeed()
	{
		return BaseSpeed;
	}

	float GetMoveSpeed()
	{
		return MoveSpeed;
	}
private: 
	bool IsKeyDown(char Key);
	bool IsKeyUp(char Key);

	static const UINT KEY_SIZE = 256;
	bool Keys[KEY_SIZE];
	POINT LastMousePoint;

	FSkeletalMesh* SkeletalMesh;
	LAnimator* AnimatorIns;

	LPlayerController* Controller;
	LCharacterMovement* ChaMovement;
	bool IsLocalControlled;

	float MoveSpeed;
	float BaseSpeed;
	bool bUpdateMove;
	bool bJump;
};
