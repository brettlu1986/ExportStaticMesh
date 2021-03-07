#pragma once

#include "stdafx.h"

using namespace DirectX;

class GraphicCamera
{
public:
	GraphicCamera();
	~GraphicCamera();

	void Init(XMFLOAT3 postion);
	XMMATRIX GetViewMarix();
	XMMATRIX GetProjectionMatrix(float fov, float aspectRatio, float nearPlane = 1.0f, float farPlane = 1000.0f);

private:
	void Reset();

	XMFLOAT3 m_initialPosition;
	XMFLOAT3 m_position;
	float m_yaw;
	float m_pitch;
	XMFLOAT3 m_lookDirection;
	XMFLOAT3 m_upDirection;
};