
#include "stdafx.h"

#include "LCamera.h"
#include "MathHelper.h"
#include "FD3D12Helper.h"

#include "LEngine.h"
#include "LDeviceWindows.h"
#include "FRenderThread.h"

using namespace DirectX;
using namespace std;

static const float DirectionMoveOffset = 0.15f;

LCamera::LCamera()
	:Position(0, 0, 0)
	,InitialPosition(0,0,0)
	,Yaw(0)
	,Pitch(0.f)
	,LookDirection(0, 0, 1)
	,FocusPosition(0, 0, 1)
	,UpDirection(0, 0, 1)
	,Fov(XM_PI/2)
	,AspectRatio(1.777f)
	,CameraDatas(FCameraData())
	,Radius(1.f)
	,Alpha(0.f)
	,Theta(0.f)
	,LastMousePoint({ 0, 0 })
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
	InitialPosition = CameraDatas.Location;
	Position = InitialPosition;

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

	//change Cartesian coordinate to Spherical coordinate, so it will be easy to use mouse rotate camera
	XMVECTOR Target = XMVector3Length(XMVectorSet(Position.x - FocusPosition.x, Position.y - FocusPosition.y, Position.z - FocusPosition.z, 1.f));
	Radius = XMVectorGetX(Target);
	// Spherical coordinate, direction vector up angle to z
	Alpha = acosf( (Position.z - FocusPosition.z) / Radius);
	// Spherical coordinate, direction shadow vector angle in x-y plain to y
	Theta = atanf( (Position.x - FocusPosition.x) / (Position.y - FocusPosition.y));
	//Theta range is [0, 2Pi]
	if(Theta < 0.f)
		Theta += XM_PI * 2;

	OnResize();
}

void LCamera::OnResize()
{
	LDeviceWindows* DeviceWindows = static_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	Fov = XMConvertToRadians(CameraDatas.Fov);
	AspectRatio = static_cast<float>(DeviceWindows->GetWidth()) / DeviceWindows->GetHeight();
}

void LCamera::UpdateForcusPosition(XMVECTOR UpdateVec)
{
	FocusPosition.x = XMVectorGetX(UpdateVec);
	FocusPosition.y = XMVectorGetY(UpdateVec);
	FocusPosition.z = XMVectorGetZ(UpdateVec);
}

void LCamera::Update()
{
	bool bDirty = false;

	XMVECTOR Direction;
	XMVECTOR Offset;
	XMVECTOR CurFocusLoc;

	if(IsKeyDown('W') || IsKeyDown('S'))
	{
		Direction = XMVectorSubtract(XMLoadFloat3(&FocusPosition), XMLoadFloat3(&Position));
		Direction = XMVector3Normalize(Direction);
		Offset = DirectionMoveOffset * Direction;
	}

	if(IsKeyDown('A') || IsKeyDown('D'))
	{
		XMFLOAT3 Up = {0, 1, 0};
		Direction = XMVectorSubtract(XMLoadFloat3(&FocusPosition), XMLoadFloat3(&Position));
		XMVECTOR UpDir = XMLoadFloat3(&Up);
		Direction = XMVectorMultiply(Direction, UpDir);
		Direction = XMVector3Normalize(Direction); //left dir
		Offset = DirectionMoveOffset * Direction;
	}

	if(IsKeyDown('W') || IsKeyDown('A'))
	{
		CurFocusLoc = XMVectorAdd(XMLoadFloat3(&FocusPosition), Offset);
		UpdateForcusPosition(CurFocusLoc);
		bDirty = true;
	}

	if(IsKeyDown('S') || IsKeyDown('D'))
	{
		CurFocusLoc = XMVectorSubtract(XMLoadFloat3(&FocusPosition), Offset);
		UpdateForcusPosition(CurFocusLoc);
		bDirty = true;
	}


	if(bDirty)
	{
		CalculateLocation();
	}
}

void LCamera::ProcessCameraMouseInput(FInputResult& MouseInput)
{
	if (MouseInput.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_DOWN)
	{
		LastMousePoint.x = MouseInput.X;
		LastMousePoint.y = MouseInput.Y;
	}
	else if (MouseInput.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_MOVE)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(MouseInput.X - LastMousePoint.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(MouseInput.Y - LastMousePoint.y));

		Alpha -= dy;
		Theta -= dx;
		Alpha = MathHelper::Clamp(Alpha, 0.1f, MathHelper::Pi - 0.1f);
		CalculateLocation();

		LastMousePoint.x = MouseInput.X;
		LastMousePoint.y = MouseInput.Y;
	}
}

void LCamera::ProcessCameraKeyInput(FInputResult& KeyInput)
{
	if(KeyInput.TouchType == E_TOUCH_TYPE::KEY_DOWN)
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
	return Keys[Key];
}

bool LCamera::IsKeyUp(char InKey)
{
	UINT8 Key = static_cast<UINT8>(InKey);
	return !Keys[Key];
}

void LCamera::CalculateLocation()
{
	XMVECTOR V = DirectX::XMVectorSet(Radius * sinf(Alpha) * sinf(Theta),
		Radius * sinf(Alpha) * cosf(Theta),
		Radius * cosf(Alpha),
		1.f);
	V += XMLoadFloat3(&FocusPosition);
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


