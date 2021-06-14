
#include "LCamera.h"
#include "FD3D12Helper.h"

#include "LEngine.h"
#include "LDeviceWindows.h"
#include "FRenderThread.h"
#include "LLog.h"


LCamera::LCamera()
	:Position(0, 0, 0)
	,Yaw(0)
	,Pitch(0.f)
	,ArmLength(0.f)
	,LookDirection(0, 0, 1)
	,FocusPosition(0, 0, 1)
	,UpDirection(0, 0, 1)
	,Fov(XM_PI/2)
	,AspectRatio(1.777f)
	,CameraDatas(LCameraData())
	,bActive(false)
{
}

LCamera::~LCamera()
{

}

void LCamera::Init(LCameraData Data)
{
	CameraDatas = Data;
	Position = CameraDatas.Location;

	//{x, y, z} == {pitch , yaw, roll}
	Pitch = XMConvertToRadians(CameraDatas.Rotator.x);
	Yaw = XMConvertToRadians(CameraDatas.Rotator.y);

	//Pitch is the angle between the direction vector and x-y plain
	//Yaw is the angle that rot from z
	//R is the direction vector in x-y plain shadow vector length
	float R = cosf(Pitch);
	LookDirection.y = R * sinf(Yaw);
	LookDirection.z = sinf(Pitch);
	LookDirection.x = R * cosf(Yaw);

	FocusPosition = CameraDatas.Target;
	XMVECTOR Target = XMVector3Length(XMVectorSet(Position.x - FocusPosition.x, Position.y - FocusPosition.y, Position.z - FocusPosition.z, 1.f));
	ArmLength = XMVectorGetX(Target);

	//OnResize();
}

void LCamera::OnResize()
{
	LDeviceWindows* DeviceWindows = static_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	Fov = XMConvertToRadians(CameraDatas.Fov);
	AspectRatio = static_cast<float>(DeviceWindows->GetWidth()) / DeviceWindows->GetHeight();
}

void LCamera::Update(float DeltaTime)
{
}

XMMATRIX LCamera::GetProjectionMatrix(float NearPlane /*= 1.0f*/, float FarPlane /*= 1000.0f*/)
{
	return XMMatrixPerspectiveFovLH(Fov, AspectRatio, NearPlane, FarPlane);
}


void LCamera::SetSocketOffset(XMFLOAT3 Offset)
{
	SocketOffset = Offset;
}


void LCamera::UpdateViewProjectionRenderThread()
{
	assert(LEngine::GetEngine()->IsGameThread());

	XMFLOAT4X4 MtProj;
	XMStoreFloat4x4(&MtProj, GetProjectionMatrix());
	XMMATRIX ViewProj = GetViewMarix() * XMLoadFloat4x4(&MtProj);

	FPassViewProjection ViewProjInfo;
	XMStoreFloat4x4(&ViewProjInfo.ViewProj, XMMatrixTranspose(ViewProj));
	ViewProjInfo.EyePosW = GetCameraLocation();
	RENDER_THREAD_TASK("FillViewProj",
		[ViewProjInfo]()
	{
		FRenderThread::Get()->GetRenderScene()->UpdateViewProjInfo(ViewProjInfo);
	}
	);

}
