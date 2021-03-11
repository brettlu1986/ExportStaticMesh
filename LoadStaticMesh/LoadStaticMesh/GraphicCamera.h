#pragma once

#include "stdafx.h"

using namespace DirectX;

class GraphicCamera
{
public:
	GraphicCamera();
	~GraphicCamera();

	void Init(XMFLOAT3 InPosition);
	XMMATRIX GetViewMarix();
	XMMATRIX GetProjectionMatrix(float Fov, float AspectRatio, float NearPlane = 1.0f, float FarPlane = 1000.0f);

private:
	void Reset();

	XMFLOAT3 InitialPosition;
	XMFLOAT3 Position;
	float Yaw;
	float Pitch;
	XMFLOAT3 LookDirection;
	XMFLOAT3 UpDirection;
};