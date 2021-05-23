
#include "LCharacter.h"
#include "FSkeletalMesh.h"
#include "LPlayerController.h"
#include "LCharacterMovement.h"
#include "LEngine.h"
#include "LLog.h"

using namespace std;

LCharacter::LCharacter()
:SkeletalMesh(nullptr)
,AnimatorIns(nullptr)
,Controller(nullptr)
,IsLocalControlled(false)
,ChaMovement(nullptr)
,MoveSpeed(0.0f)
,BaseSpeed(3.f)
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

void LCharacter::SetAnimator(LAnimator* Animator)
{
	AnimatorIns = Animator;
	AnimatorIns->InitStateMachines();
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
		if(!Keys[KeyInput.KeyMapType])
		{
			Keys[KeyInput.KeyMapType] = true;
			ProcessMoveInput();
		}
	}
	else if (KeyInput.TouchType == E_TOUCH_TYPE::KEY_UP)
	{
		if(Keys[KeyInput.KeyMapType])
		{
			Keys[KeyInput.KeyMapType] = false;
			ProcessMoveInput();
		}
	}
	
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
	MoveSpeed = BaseSpeed;

	if(IsKeyDown('W'))
	{
		MoveDirection += GetMoveForwardVector();
		bUpdateMove = true;
		LEngine::GetEngine()->GetEventDispacher().DispatchEvent(E_EVENT_KEY::EVENT_ANIM_MACHINE_STATE, string("Walk"));
	}

	if (IsKeyDown('S'))
	{
		MoveDirection -= GetMoveForwardVector();
		bUpdateMove = true;
		LEngine::GetEngine()->GetEventDispacher().DispatchEvent(E_EVENT_KEY::EVENT_ANIM_MACHINE_STATE, string("Walk"));
	}

	if (IsKeyDown('A'))
	{
		MoveDirection += GetMoveRightVector();
		bUpdateMove = true;
		LEngine::GetEngine()->GetEventDispacher().DispatchEvent(E_EVENT_KEY::EVENT_ANIM_MACHINE_STATE, string("Walk"));
	}

	if (IsKeyDown('D'))
	{
		MoveDirection -= GetMoveRightVector();
		bUpdateMove = true;
		LEngine::GetEngine()->GetEventDispacher().DispatchEvent(E_EVENT_KEY::EVENT_ANIM_MACHINE_STATE, string("Walk"));
	}

	if(IsKeyDown(VK_SHIFT) && IsKeyDown('W'))
	{
		MoveSpeed = BaseSpeed * 2;
		bUpdateMove = true;
		LEngine::GetEngine()->GetEventDispacher().DispatchEvent(E_EVENT_KEY::EVENT_ANIM_MACHINE_STATE, string("Run"));
	}

	if(IsKeyDown(VK_SPACE))
	{
		LEngine::GetEngine()->GetEventDispacher().DispatchEvent(E_EVENT_KEY::EVENT_ANIM_MACHINE_STATE, string("Jump"));
	}

	if (!bUpdateMove && IsKeyUp(VK_SPACE))
	{
		LEngine::GetEngine()->GetEventDispacher().DispatchEvent(E_EVENT_KEY::EVENT_ANIM_MACHINE_STATE, string("Idle"));
	}

	ChaMovement->SetActive(bUpdateMove);
	if(bUpdateMove)
	{
		ChaMovement->AddMoveInput(MoveDirection, MoveSpeed);
	}
}

void LCharacter::RegisterAnimationStateEvent()
{
	if(IsLocalControlled)
	{
		EventDispatcher& EventDisp = LEngine::GetEngine()->GetEventDispacher();
		std::function<void(std::string)> Func = std::bind(&LAnimator::ProcessDefaultStateMachineChange, AnimatorIns, std::placeholders::_1);
		EventDisp.RegisterEvent(new LEvent<std::string>(E_EVENT_KEY::EVENT_ANIM_MACHINE_STATE, Func));
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