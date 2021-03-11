#include "GraphicCamera.h"

using namespace DirectX;

GraphicCamera::GraphicCamera()
	:Position(0, 0 , 0)
	,InitialPosition(0,0,0)
	,Yaw(XM_PI)
	,Pitch(0.f)
	,LookDirection(0, 0, -1)
	,UpDirection(0, 1, 0)
{

}

GraphicCamera::~GraphicCamera()
{

}

void GraphicCamera::Init(XMFLOAT3 InPosition)
{
	InitialPosition = InPosition;
	Reset();
}

XMMATRIX GraphicCamera::GetViewMarix()
{
	return XMMatrixLookToRH(XMLoadFloat3(&Position), XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));
}

XMMATRIX GraphicCamera::GetProjectionMatrix(float Fov, float AspectRatio, float NearPlane /*= 1.0f*/, float FarPlane /*= 1000.0f*/)
{
	return XMMatrixPerspectiveFovRH(Fov, AspectRatio, NearPlane, FarPlane);
}

void GraphicCamera::Reset()
{
	Position = InitialPosition;
	Yaw = XM_PI;
	Pitch = 0.0f;
	LookDirection = { 0, 0, -1 };
}
