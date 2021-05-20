
#include "stdafx.h"

#include "LThirdPersonCamera.h"
#include "MathHelper.h"
#include "FD3D12Helper.h"

#include "LEngine.h"
#include "LDeviceWindows.h"
#include "FRenderThread.h"
#include "LLog.h"

using namespace DirectX;
using namespace std;

static const float DirectionMoveOffset = 0.15f;
static const float MAX_PITCH = XMConvertToRadians(89.99f);
static const float MIN_PITCH = XMConvertToRadians(-89.99f);

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

void LThirdPersonCamera::SetSocketOffset(XMFLOAT3 Offset)
{
	SocketOffset = Offset;
}

void LThirdPersonCamera::Update(float DeltaTime)
{
	UpdateViewTarget(DeltaTime);
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
		Position = XMFLOAT3(ViewTarget->GetLocation().x + SocketOffset.x,
			ViewTarget->GetLocation().y + SocketOffset.y,
			ViewTarget->GetLocation().z + SocketOffset.z);

		XMFLOAT3 Rotator = ViewTarget->GetRotation();
		Pitch = XMConvertToRadians(Rotator.x);
		Yaw = XMConvertToRadians(Rotator.y);

		float R = cosf(Pitch);
		LookDirection.y = R * sinf(Yaw);
		LookDirection.z = sinf(Pitch);
		LookDirection.x = R * cosf(Yaw);
	}
}

void LThirdPersonCamera::SetViewTarget(LActor* Target)
{
	ViewTarget = Target;
	UpdateViewTarget(0.f);
}

