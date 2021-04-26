
#include "stdafx.h"

#include "LCamera.h"
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

LCamera::LCamera()
	:Position(0, 0, 0)
	,Yaw(0)
	,Pitch(0.f)
	,ArmLength(0.f)
	,LookDirection(0, 0, 1)
	,FocusPosition(0, 0, 1)
	,UpDirection(0, 0, 1)
	,Fov(XM_PI/2)
	,AspectRatio(1.777f)
	,CameraDatas(FCameraData())
	,LastMousePoint({ 0, 0 })
	,bUpdateDirty(false)
{
	for (UINT i = 0; i < KEY_SIZE; i++)
	{
		Keys[i] = false;
	}
}

LCamera::~LCamera()
{

}

void LCamera::Init()
{
	Position = CameraDatas.Location;

	//{x, y, z} == {pitch , yaw, roll}
	Pitch = XMConvertToRadians(CameraDatas.Rotator.x);
	Yaw = XMConvertToRadians(CameraDatas.Rotator.y);

	//Pitch is the angle between the direction vector and x-y plain
	//Yaw is the angle that rot from z
	//R is the direction vector in x-y plain shadow vector length
	float R = cosf(Pitch);
	LookDirection.y = R * sinf(Yaw);
	LookDirection.z = sinf(Pitch);
	LookDirection.x = R * cosf(Yaw);

	FocusPosition = CameraDatas.Target;
	XMVECTOR Target = XMVector3Length(XMVectorSet(Position.x - FocusPosition.x, Position.y - FocusPosition.y, Position.z - FocusPosition.z, 1.f));
	ArmLength = XMVectorGetX(Target);

	OnResize();
}

void LCamera::OnResize()
{
	LDeviceWindows* DeviceWindows = static_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	Fov = XMConvertToRadians(CameraDatas.Fov);
	AspectRatio = static_cast<float>(DeviceWindows->GetWidth()) / DeviceWindows->GetHeight();
}

void LCamera::Update(float DeltaTime)
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

void LCamera::ProcessCameraMouseInput(FInputResult& MouseInput)
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

void LCamera::ProcessCameraKeyInput(FInputResult& KeyInput)
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

bool LCamera::IsKeyDown(char InKey)
{
	UINT8 Key = static_cast<UINT8>(InKey);
	return Keys[Key] == true;
}

bool LCamera::IsKeyUp(char InKey)
{
	UINT8 Key = static_cast<UINT8>(InKey);
	return Keys[Key] == false;
}

void LCamera::CalculateLocation()
{
	float R = cosf(Pitch);
	LookDirection.y = R * sinf(Yaw);
	LookDirection.z = sinf(Pitch);
	LookDirection.x = R * cosf(Yaw);
	
	RightDirection = XMVector3Cross(XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));

	XMVECTOR Offset =  -ArmLength * XMLoadFloat3(&LookDirection);
	XMVECTOR V = XMVectorAdd(XMLoadFloat3(&FocusPosition), Offset);

	Position.x = XMVectorGetX(V);
	Position.y = XMVectorGetY(V);
	Position.z = XMVectorGetZ(V);
}

XMMATRIX LCamera::GetViewMarix()
{
	return XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&FocusPosition), XMLoadFloat3(&UpDirection));
	 //XMMatrixLookToLH(XMLoadFloat3(&Position), XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));
}

XMMATRIX LCamera::GetProjectionMatrix(float NearPlane /*= 1.0f*/, float FarPlane /*= 1000.0f*/)
{
	return XMMatrixPerspectiveFovLH(Fov, AspectRatio, NearPlane, FarPlane);
}


