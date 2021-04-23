#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "LDefine.h"

using namespace DirectX;

class LCamera
{
public:
	LCamera();
	~LCamera();

	void Init();
	void OnResize();
	
	XMMATRIX GetViewMarix();
	XMMATRIX GetProjectionMatrix(float NearPlane = 1.0f, float FarPlane = 1000.0f);

	void ProcessCameraMouseInput(FInputResult& MouseInput);
	void ProcessCameraKeyInput(FInputResult& KeyInput);
	void Update();

	const XMFLOAT3& GetViewTargetLocation() {
		return CameraDatas.Target;
	}

	FCameraData& GetCameraData()
	{
		return CameraDatas;
	}

	const XMFLOAT3& GetCameraLocation()
	{
		return Position;
	}

	void SetCameraLocation(const XMFLOAT3& InLoc)
	{
		Position.x = InLoc.x;
		Position.y = InLoc.x;
		Position.z = InLoc.z;
	}

	void CalculateLocation();
private:

	bool IsKeyDown(char Key);
	bool IsKeyUp(char Key);
	void UpdateFocusPosition(XMVECTOR UpdateVec);
	void UpdateMoveOffset();

	static const UINT KEY_SIZE = 256;
	
	FCameraData CameraDatas;
	//use for reset
	XMFLOAT3 InitialPosition;
	XMFLOAT3 Position;
	float Yaw;
	float Pitch;
	//we can use either look direction or focus position to calculate view matrix
	XMFLOAT3 LookDirection;
	XMFLOAT3 FocusPosition;

	XMFLOAT3 UpDirection;

	float Fov;
	float AspectRatio;

	//use to rotate camera
	float Alpha;// Spherical coordinate, direction vector up angle to y
	float Theta;// Spherical coordinate, direction shadow vector angle in x-z plain to x
	float Radius;

	POINT LastMousePoint;
	bool Keys[KEY_SIZE];
	XMVECTOR MoveOffset;

};