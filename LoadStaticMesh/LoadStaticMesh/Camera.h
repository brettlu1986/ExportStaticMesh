#pragma once

#include "stdafx.h"

using namespace DirectX;

#pragma pack(push)
#pragma pack(4)
struct CameraData {
	XMFLOAT3 Location;
	XMFLOAT3 Target;
	XMFLOAT3 Rotator; // {Pitch, Yaw , Roll}
	float Fov;
	float Aspect;
};
#pragma pack(pop)

class Camera
{
public:
	Camera();
	~Camera();

	void Init();
	void OnResize(float WndWidth, float WndHeight);
	
	XMMATRIX GetViewMarix();
	XMMATRIX GetProjectionMatrix(float NearPlane = 1.0f, float FarPlane = 1000.0f);

	const XMFLOAT3& GetViewTargetLocation() {
		return CameraDatas.Target;
	}

private:

	void ReadCameraDataFromFile(LPCWSTR FileName );

	CameraData CameraDatas;
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
};