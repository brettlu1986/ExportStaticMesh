#pragma once

#include "stdafx.h"
#include "LAnimator.h"
#include "LActor.h"
#include "LSkeletalMesh.h"
#include "LCamera.h"

class LPlayerController;
class LCharacterMovement;
class LCharacter : public LActor
{
public:
	LCharacter();
	virtual ~LCharacter();

	void Destroy();
	void SetAnimator(LAnimator* Animator);
	
	LSkeletalMesh* GetSkeletalMesh()
	{
		return SkeletalMeshIns;
	}

	LAnimator* GetAnimator()
	{
		return AnimatorIns;
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

	void SetSkeletalMesh(LSkeletalMesh* Mesh)
	{
		SkeletalMeshIns = Mesh;
	}
private: 
	bool IsKeyDown(char Key);
	bool IsKeyUp(char Key);

	static const UINT KEY_SIZE = 256;
	bool Keys[KEY_SIZE];
	POINT LastMousePoint;

	LSkeletalMesh* SkeletalMeshIns;
	LAnimator* AnimatorIns;

	LPlayerController* Controller;
	LCharacterMovement* ChaMovement;
	bool IsLocalControlled;

	float MoveSpeed;
	float BaseSpeed;
	bool bUpdateMove;
	bool bJump;
};
