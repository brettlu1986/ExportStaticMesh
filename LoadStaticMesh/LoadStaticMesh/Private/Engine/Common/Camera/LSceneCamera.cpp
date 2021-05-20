
#include "stdafx.h"

#include "LSceneCamera.h"
#include "MathHelper.h"
#include "FD3D12Helper.h"

#include "LEngine.h"
#include "LDeviceWindows.h"
#include "FRenderThread.h"
#include "LLog.h"

using namespace DirectX;
using namespace std;

static const float DirectionMoveOffset = 0.15f;
static const float MAX_PITCH = XMConvertToRadians(89.99f);
static const float MIN_PITCH = XMConvertToRadians(-89.99f);

LSceneCamera::LSceneCamera()
	:LCamera()
	,LastMousePoint({ 0, 0 })
	,bUpdateDirty(false)
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

void LSceneCamera::Init()
{
	LCamera::Init();
	OnResize();
}

void LSceneCamera::OnResize()
{
	LCamera::OnResize();
}

void LSceneCamera::UpdateInput(float dt)
{
	MoveOffset = XMVectorSet(0, 0, 0, 1.f);

	if (IsKeyDown('W') || IsKeyDown('S'))
	{
		XMVECTOR Offset = DirectionMoveOffset * XMLoadFloat3(&LookDirection);
		if (IsKeyDown('S'))
			Offset = -Offset;
		MoveOffset += Offset;
		bUpdateDirty = true;
	}

	if (IsKeyDown('A') || IsKeyDown('D'))
	{
		XMVECTOR Offset = DirectionMoveOffset * RightDirection;
		if (IsKeyDown('D'))
			Offset = -Offset;
		MoveOffset += Offset;
		bUpdateDirty = true;
	}

	if (IsKeyDown('Q') || IsKeyDown('E'))
	{
		XMVECTOR Offset = DirectionMoveOffset * XMLoadFloat3(&UpDirection);
		if (IsKeyDown('Q'))
			Offset = -Offset;
		MoveOffset += Offset;
		bUpdateDirty = true;
	}

	if (bUpdateDirty)
	{
		XMVECTOR FocusVec = XMVectorAdd(XMLoadFloat3(&FocusPosition), MoveOffset);
		FocusPosition.x = XMVectorGetX(FocusVec);
		FocusPosition.y = XMVectorGetY(FocusVec);
		FocusPosition.z = XMVectorGetZ(FocusVec);
		CalculateLocation();
		bUpdateDirty = false;
	}
}


void LSceneCamera::Update(float DeltaTime)
{
	LCamera::Update(DeltaTime);
	UpdateInput(DeltaTime);
}

void LSceneCamera::ProcessCameraMouseInput(FInputResult& MouseInput)
{
	if (MouseInput.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_MOVE)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(MouseInput.X - LastMousePoint.x)); //dx
		float dy = XMConvertToRadians(0.25f * static_cast<float>(MouseInput.Y - LastMousePoint.y)); //dy

		Pitch -= dy;
		Yaw += dx;

		Pitch = MathHelper::Clamp(Pitch, MIN_PITCH, MAX_PITCH);
		bUpdateDirty = true;

		LastMousePoint.x = MouseInput.X;
		LastMousePoint.y = MouseInput.Y;
	}

	LastMousePoint.x = MouseInput.X;
	LastMousePoint.y = MouseInput.Y;
}

void LSceneCamera::ProcessCameraKeyInput(FInputResult& KeyInput)
{
	if (KeyInput.TouchType == E_TOUCH_TYPE::KEY_DOWN)
	{
		Keys[KeyInput.KeyMapType] = true;
	}
	else if (KeyInput.TouchType == E_TOUCH_TYPE::KEY_UP)
	{
		Keys[KeyInput.KeyMapType] = false;
	}
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

void LSceneCamera::CalculateLocation()
{
	float R = cosf(Pitch);
	LookDirection.y = R * sinf(Yaw);
	LookDirection.z = sinf(Pitch);
	LookDirection.x = R * cosf(Yaw);

	RightDirection = XMVector3Cross(XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));

	XMVECTOR Offset = -ArmLength * XMLoadFloat3(&LookDirection);
	XMVECTOR V = XMVectorAdd(XMLoadFloat3(&FocusPosition), Offset);

	Position.x = XMVectorGetX(V);
	Position.y = XMVectorGetY(V);
	Position.z = XMVectorGetZ(V);
}

XMMATRIX LSceneCamera::GetViewMarix()
{
	return XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&FocusPosition), XMLoadFloat3(&UpDirection));
}
