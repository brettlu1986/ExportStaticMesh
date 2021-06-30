
#include "pch.h"
#include "LSceneCamera.h"
#include "FD3D12Helper.h"

#include "LEngine.h"
#include "LDeviceWindows.h"
#include "FRenderThread.h"
#include "LLog.h"
#include "LGameTimer.h"


static const float MAX_PITCH = XMConvertToRadians(89.99f);
static const float MIN_PITCH = XMConvertToRadians(-89.99f);

LSceneCamera::LSceneCamera()
	:LCamera()
	,LastMousePoint({ 0, 0 })
	,CurrentMousePoint({0,0})
	,MoveDirection(XMVECTOR())
	,MoveOffset(XMVECTOR())
	,RightDirection(XMVECTOR())
	,bUpdateDirty(false)
	,bUpdateRotDirty(false)
	,Dx(0.f)
	,Dy(0.f)
	,MoveSpeed(2.f)
	,RotSpeed(10.f)
{
	CameraType = E_CAMERA_TYPE::CAMERA_SCENE;
	for (UINT i = 0; i < KEY_SIZE; i++)
	{
		Keys[i] = false;
	}
}

LSceneCamera::~LSceneCamera()
{

}

void LSceneCamera::Init(LCameraData Data)
{
	LCamera::Init(Data);
	OnResize();
}

void LSceneCamera::OnResize()
{
	LCamera::OnResize();
}

void LSceneCamera::ProcessInput()
{
	bUpdateDirty = false; 
	
	Vec3 Zero = Vec3(0.f, 0.f, 0.f);
	MoveDirection = XMLoadFloat3(&Zero);

	if(IsKeyDown('W'))
	{
		MoveDirection += XMLoadFloat3(&LookDirection);
		bUpdateDirty = true;
	}

	if(IsKeyDown('S'))
	{
		MoveDirection += -1.f * XMLoadFloat3(&LookDirection);
		bUpdateDirty = true;
	}

	RightDirection = XMVector3Cross(XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));

	if(IsKeyDown('A'))
	{
		MoveDirection += RightDirection;
		bUpdateDirty = true;
	}

	if (IsKeyDown('D'))
	{
		MoveDirection = MoveDirection + (-1 * RightDirection);
		bUpdateDirty = true;
	}

	if (IsKeyDown('E'))
	{
		MoveDirection += XMLoadFloat3(&UpDirection);
		bUpdateDirty = true;
	}

	if (IsKeyDown('Q'))
	{
		MoveDirection = MoveDirection + (-1 * XMLoadFloat3(&UpDirection));
		bUpdateDirty = true;
	}
}

void LSceneCamera::UpdateInput(float dt)
{
	bool bDirty = false;
	if (bUpdateDirty )
	{
		XMVECTOR UnitDir = XMVector3Normalize(MoveDirection);
		XMVECTOR FocusVec = XMLoadFloat3(&FocusPosition) +  MoveSpeed * dt * UnitDir;
		FocusPosition.x = XMVectorGetX(FocusVec);
		FocusPosition.y = XMVectorGetY(FocusVec);
		FocusPosition.z = XMVectorGetZ(FocusVec);
		bDirty = true;
	}

	if(bUpdateRotDirty)
	{
		LInput::GetInput()->GetCurrentCursorLocation(CurrentMousePoint);

		Dx = XMConvertToRadians(0.25f * static_cast<float>(CurrentMousePoint.x - LastMousePoint.x));
		Dy = XMConvertToRadians(0.25f * static_cast<float>(CurrentMousePoint.y - LastMousePoint.y));;

		Pitch -= RotSpeed * Dy * dt;
		Yaw += RotSpeed * Dx * dt;
		Pitch = MathHelper::Clamp(Pitch, MIN_PITCH, MAX_PITCH);

		float R = cosf(Pitch);
		LookDirection.y = R * sinf(Yaw);
		LookDirection.z = sinf(Pitch);
		LookDirection.x = R * cosf(Yaw);

		LastMousePoint = CurrentMousePoint;
		bDirty = true;
	}
		
	if(bDirty)
	{
		RightDirection = XMVector3Cross(XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));

		XMVECTOR Offset = -ArmLength * XMLoadFloat3(&LookDirection);
		XMVECTOR V = XMVectorAdd(XMLoadFloat3(&FocusPosition), Offset);

		Position.x = XMVectorGetX(V);
		Position.y = XMVectorGetY(V);
		Position.z = XMVectorGetZ(V);

		UpdateViewProjectionRenderThread();
	}
}

void LSceneCamera::SetActive(bool bActivate)
{
	LCamera::SetActive(bActivate);

	if(bActive)
	{
		UpdateViewProjectionRenderThread();
	}
}

void LSceneCamera::Update(float DeltaTime)
{
	if(!bActive)
		return;
	LCamera::Update(DeltaTime);
	UpdateInput(DeltaTime);
}

void LSceneCamera::ProcessCameraMouseInput(LInputResult& MouseInput)
{
	if (MouseInput.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_DOWN)
	{
		LastMousePoint.x = MouseInput.X;
		LastMousePoint.y = MouseInput.Y;

		bUpdateRotDirty = false;
	}
	else if (MouseInput.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_MOVE)
	{
		bUpdateRotDirty = true;
	}
	else if(MouseInput.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_UP)
	{
		bUpdateRotDirty = false;
		CurrentMousePoint.x = MouseInput.X;
		CurrentMousePoint.y = MouseInput.Y;
	}
}

void LSceneCamera::ProcessCameraKeyInput(LInputResult& KeyInput)
{
	if (KeyInput.TouchType == E_TOUCH_TYPE::KEY_DOWN)
	{
		Keys[KeyInput.KeyMapType] = true;
	}
	else if (KeyInput.TouchType == E_TOUCH_TYPE::KEY_UP)
	{
		Keys[KeyInput.KeyMapType] = false;
	}
	ProcessInput();
}

bool LSceneCamera::IsKeyDown(char InKey)
{
	UINT8 Key = static_cast<UINT8>(InKey);
	return Keys[Key] == true;
}

bool LSceneCamera::IsKeyUp(char InKey)
{
	UINT8 Key = static_cast<UINT8>(InKey);
	return Keys[Key] == false;
}

XMMATRIX LSceneCamera::GetViewMarix()
{
	return XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&FocusPosition), XMLoadFloat3(&UpDirection));
}


