
#include "stdafx.h"

#include "LThirdPersonCamera.h"
#include "MathHelper.h"
#include "FD3D12Helper.h"

#include "LEngine.h"
#include "LDeviceWindows.h"
#include "FRenderThread.h"
#include "LLog.h"


LThirdPersonCamera::LThirdPersonCamera()
	:LCamera()
	,ViewTarget(nullptr)
{
	CameraType = E_CAMERA_TYPE::CAMERA_THIRD_PERSON;
}

LThirdPersonCamera::~LThirdPersonCamera()
{

}

void LThirdPersonCamera::Init()
{
	LCamera::Init();
	OnResize();
}

void LThirdPersonCamera::OnResize()
{
	LCamera::OnResize();
}


void LThirdPersonCamera::Update(float DeltaTime)
{
	if(!bActive)
		return;
	LCamera::Update(DeltaTime);
	UpdateViewTarget(DeltaTime);
}

void LThirdPersonCamera::SetActive(bool bActivate) 
{
	LCamera::SetActive(bActivate);

}

XMMATRIX LThirdPersonCamera::GetViewMarix()
{
	return XMMatrixLookToLH(XMLoadFloat3(&Position), XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));
}

// camera target 
void LThirdPersonCamera::UpdateViewTarget(float dt)
{
	if (ViewTarget)
	{
		XMFLOAT3 Rotator = ViewTarget->GetRotation();
		Pitch = XMConvertToRadians(Rotator.x);
		Yaw = XMConvertToRadians(Rotator.y);

		float R = cosf(Pitch);
		LookDirection.y = R * sinf(Yaw);
		LookDirection.z = sinf(Pitch);
		LookDirection.x = R * cosf(Yaw);

		XMFLOAT3 Loc = ViewTarget->GetLocation();

		//socket offset x is -2, so it will move to backward dir
		XMVECTOR ForwardDir = ViewTarget->GetMoveForwardVector();
		ForwardDir = XMVector3Normalize(ForwardDir);

		XMVECTOR RightDir = XMVector3Cross(XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));
		RightDir = -1.f * XMVector3Normalize(RightDir);

		XMVECTOR FinalVec = XMLoadFloat3(&Loc) + ForwardDir * SocketOffset.x + RightDir * SocketOffset.y +
			XMLoadFloat3(&UpDirection) * SocketOffset.z;
		Position = XMFLOAT3(XMVectorGetX(FinalVec),
			XMVectorGetY(FinalVec),
			XMVectorGetZ(FinalVec));
	}
}

void LThirdPersonCamera::SetViewTarget(LActor* Target)
{
	ViewTarget = Target;
	UpdateViewTarget(0.f);
}


