#include "pch.h"
#include "LCharacterMovement.h"

LCharacterMovement::LCharacterMovement()
{

}

LCharacterMovement::~LCharacterMovement()
{

}

void LCharacterMovement::SetOwner(LCharacter* PawnOwner)
{
	Owner = PawnOwner;
}

void LCharacterMovement::Update(float dt)
{
	if(bActive)
	{
		Vec3 CurLocation = Owner->GetLocation();
		XMVECTOR UnitDir = XMVector3Normalize(MoveDirection);
		XMVECTOR FinalLocVec = XMLoadFloat3(&CurLocation) + MoveSpeed * dt * UnitDir;
		Owner->SetLocation(Vec3(XMVectorGetX(FinalLocVec), XMVectorGetY(FinalLocVec), XMVectorGetZ(FinalLocVec)));
	}
}

void LCharacterMovement::AddMoveInput(XMVECTOR Direction, float Speed)
{
	MoveSpeed = Speed;
	MoveDirection = Direction;
}