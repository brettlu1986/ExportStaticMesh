#include "GraphicCamera.h"

using namespace DirectX;

GraphicCamera::GraphicCamera()
	:m_position(0, 0 , 0)
	,m_initialPosition(0,0,0)
	,m_yaw(XM_PI)
	,m_pitch(0.f)
	,m_lookDirection(0, 0, -1)
	,m_upDirection(0, 1, 0)
{

}

GraphicCamera::~GraphicCamera()
{

}

void GraphicCamera::Init(XMFLOAT3 position)
{
	m_initialPosition = position;
	Reset();
}

XMMATRIX GraphicCamera::GetViewMarix()
{
	return XMMatrixLookToRH(XMLoadFloat3(&m_position), XMLoadFloat3(&m_lookDirection), XMLoadFloat3(&m_upDirection));
}

XMMATRIX GraphicCamera::GetProjectionMatrix(float fov, float aspectRatio, float nearPlane /*= 1.0f*/, float farPlane /*= 1000.0f*/)
{
	return XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane);
}

void GraphicCamera::Reset()
{
	m_position = m_initialPosition;
	m_yaw = XM_PI;
	m_pitch = 0.0f;
	m_lookDirection = { 0, 0, -1 };
}
