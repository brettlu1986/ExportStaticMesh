#include "GraphicCamera.h"
#include "MathHelper.h"

using namespace DirectX;

GraphicCamera::GraphicCamera()
	:Position(0, 0, 0)
	,InitialPosition(0,0,0)
	,Yaw(0)
	,Pitch(0.f)
	,LookDirection(0, 0, 1)
	,FocusPosition(0, 0, 1)
	,UpDirection(0, 1, 0)
{

}

GraphicCamera::~GraphicCamera()
{

}

void GraphicCamera::Init(XMFLOAT3 CameraLocation, XMFLOAT3 CameraTarget, XMFLOAT3 CameraRotator)
{
	InitialPosition = MathHelper::GetUe4ConvertLocation(CameraLocation);
	Position = InitialPosition;

	//{x, y, z} == {pitch , yaw, roll}
	Pitch = XMConvertToRadians(CameraRotator.x);
	Yaw = XMConvertToRadians(CameraRotator.y);
	float R = cosf(Pitch);
	LookDirection.x = R * sinf(Yaw);
	LookDirection.y = sinf(Pitch);
	LookDirection.z = R * cosf(Yaw);

	FocusPosition = MathHelper::GetUe4ConvertLocation(CameraTarget);
}

void GraphicCamera::InitFovAndAspect(float InFov, float Aspect)
{
	Fov = InFov;
	AspectRatio = Aspect;
}

XMMATRIX GraphicCamera::GetViewMarix()
{
	return XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&FocusPosition), XMLoadFloat3(&UpDirection));
	//XMMatrixLookToLH(XMLoadFloat3(&Position), XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));
}

XMMATRIX GraphicCamera::GetProjectionMatrix(float NearPlane /*= 1.0f*/, float FarPlane /*= 1000.0f*/)
{
	return XMMatrixPerspectiveFovLH(Fov, AspectRatio, NearPlane, FarPlane);
}


