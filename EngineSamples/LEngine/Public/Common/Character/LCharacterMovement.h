#pragma once

#include "pch.h"
#include "LCharacter.h"


class LCharacterMovement
{
public:
	LCharacterMovement();
	~LCharacterMovement();

	void SetOwner(LCharacter* PawnOwner);
	void Update(float dt);
	void AddMoveInput(XMVECTOR Direction, float Speed);

	void SetActive(bool bActivate)
	{
		bActive = bActivate;
	}

private: 
	float MoveSpeed;
	XMVECTOR MoveDirection;
	LCharacter* Owner;
	bool bActive;
};
