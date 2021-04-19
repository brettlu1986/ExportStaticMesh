#pragma once

#include "stdafx.h"
#include "FDefine.h"

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

	void ChangeViewMatrixByMouseEvent(float x, float y);
	void UpdateCameraDistance(UINT8 Key);

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

	void SetCameraControlRot(XMFLOAT2 InRotate)
	{
		Theta = InRotate.x;
		Alpha = InRotate.y;
		CalculateLocation();
	}

	void CalculateLocation();
	XMFLOAT2 GetControlRotate()
	{
		return XMFLOAT2(Theta, Alpha);
	}
private:

	//void ReadCameraDataFromFile(LPCWSTR FileName );

	FCameraData CameraDatas;
	//use for reset
	XMFLOAT3 InitialPosition;
	XMFLOAT3 Position;
	float Yaw;
	float Pitch;
	//we can use either look direction or focus position to calculate view matrix
	XMFLOAT3 LookDirection;
	XMFLOAT3 FocusPosition;
	float DirectionMoveOffset;

	XMFLOAT3 UpDirection;

	float Fov;
	float AspectRatio;

	//use to rotate camera
	float Alpha;// Spherical coordinate, direction vector up angle to y
	float Theta;// Spherical coordinate, direction shadow vector angle in x-z plain to x
	float Radius;
};