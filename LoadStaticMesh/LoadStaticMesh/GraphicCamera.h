#pragma once

#include "stdafx.h"

using namespace DirectX;

class GraphicCamera
{
public:
	GraphicCamera();
	~GraphicCamera();

	void Init(XMFLOAT3 CameraLocation, XMFLOAT3 CameraTarget, XMFLOAT3 CameraRotator);
	void InitFovAndAspect(float InFov, float Aspect);
	XMMATRIX GetViewMarix();
	XMMATRIX GetProjectionMatrix(float NearPlane = 1.0f, float FarPlane = 1000.0f);

private:
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