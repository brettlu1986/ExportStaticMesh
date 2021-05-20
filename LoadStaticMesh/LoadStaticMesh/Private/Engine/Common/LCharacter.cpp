
#include "LCharacter.h"
#include "FSkeletalMesh.h"
#include "LPlayerController.h"
#include "LCharacterMovement.h"

#include "LLog.h"

LCharacter::LCharacter()
:SkeletalMesh(nullptr)
,AnimatorIns(nullptr)
,Controller(nullptr)
,IsLocalControlled(false)
,ChaMovement(nullptr)
,MoveSpeed(3.0f)
,bUpdateMove(false)

{
	ChaMovement = new LCharacterMovement();
	ChaMovement->SetOwner(this);
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

void LCharacter::ProcessMouseInput(FInputResult& MouseInput)
{
	if (MouseInput.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_MOVE)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(MouseInput.X - LastMousePoint.x)); //dx
		float dy = XMConvertToRadians(0.25f * static_cast<float>(MouseInput.Y - LastMousePoint.y)); //dy

		if(IsLocalControlled)
		{
			Controller->AddYawInput(dx);
			Controller->AddPitchInput(0);
		}

		LastMousePoint.x = MouseInput.X;
		LastMousePoint.y = MouseInput.Y;
	}

	LastMousePoint.x = MouseInput.X;
	LastMousePoint.y = MouseInput.Y;

	if(MouseInput.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_UP)
	{
		if (IsLocalControlled)
		{
			Controller->Reset();
		}
	}
}

void LCharacter::ProcessKeyInput(FInputResult& KeyInput)
{
	if (KeyInput.TouchType == E_TOUCH_TYPE::KEY_DOWN)
	{
		Keys[KeyInput.KeyMapType] = true;
		//LLog::Log("key down");
	}
	else if (KeyInput.TouchType == E_TOUCH_TYPE::KEY_UP)
	{
		Keys[KeyInput.KeyMapType] = false;
		//LLog::Log("key up");
	}

	ProcessMoveInput();
}

bool LCharacter::IsKeyDown(char InKey)
{
	UINT8 Key = static_cast<UINT8>(InKey);
	return Keys[Key] == true;
}

bool LCharacter::IsKeyUp(char InKey)
{
	UINT8 Key = static_cast<UINT8>(InKey);
	return Keys[Key] == false;
}

void LCharacter::ProcessMoveInput()
{
	bUpdateMove = false;

	XMFLOAT3 Zero = XMFLOAT3(0.f, 0.f, 0.f);
	XMVECTOR MoveDirection = XMLoadFloat3(&Zero);
	if(IsKeyDown('W'))
	{
		MoveDirection += GetMoveForwardVector();
		bUpdateMove = true;
	}

	if (IsKeyDown('S'))
	{
		MoveDirection -= GetMoveForwardVector();
		bUpdateMove = true;
	}

	if (IsKeyDown('A'))
	{
		MoveDirection += GetMoveRightVector();
		bUpdateMove = true;
	}

	if (IsKeyDown('D'))
	{
		MoveDirection -= GetMoveRightVector();
		bUpdateMove = true;
	}

	ChaMovement->SetActive(bUpdateMove);
	if(bUpdateMove)
	{
		ChaMovement->AddMoveInput(MoveDirection, MoveSpeed);
	}
}

void LCharacter::Update(float dt)
{
	ChaMovement->Update(dt);

	if (IsLocalControlled)
	{
		Controller->Update(dt);
	}
	
	if (AnimatorIns)
	{
		AnimatorIns->Update(dt);
	}
}

XMVECTOR LCharacter::GetMoveForwardVector()
{
	XMFLOAT3 Rot = GetRotation();

	float Pitch = XMConvertToRadians(Rot.x);
	float Yaw = XMConvertToRadians(Rot.y);

	XMFLOAT3 Direction;
	float R = cosf(Pitch);
	Direction.y = R * sinf(Yaw);
	Direction.z = sinf(Pitch);
	Direction.x = R * cosf(Yaw);

	return XMLoadFloat3(&Direction);
}

XMVECTOR LCharacter::GetMoveRightVector()
{
	XMFLOAT3 Up = XMFLOAT3(0,0,1);
	XMVECTOR UpDir = XMLoadFloat3(&Up);
	return XMVector3Cross(GetMoveForwardVector(), UpDir);
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