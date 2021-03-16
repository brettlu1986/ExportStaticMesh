#include "Camera.h"
#include "MathHelper.h"
#include "GraphicHelper.h"
#include <DirectXMath.h>
#include <iostream>
#include <fstream>

using namespace DirectX;
using namespace std;

static const wstring CameraBinName = L"camera.bin";

Camera::Camera()
	:Position(0, 0, 0)
	,InitialPosition(0,0,0)
	,Yaw(0)
	,Pitch(0.f)
	,LookDirection(0, 0, 1)
	,FocusPosition(0, 0, 1)
	,UpDirection(0, 1, 0)
	,Fov(XM_PI/2)
	,AspectRatio(1.777f)
	,CameraDatas(CameraData())
{

}

Camera::~Camera()
{

}

void Camera::Init()
{
	ReadCameraDataFromFile(CameraBinName.c_str());

	InitialPosition = CameraDatas.Location;
	Position = InitialPosition;

	//{x, y, z} == {pitch , yaw, roll}
	Pitch = XMConvertToRadians(CameraDatas.Rotator.x);
	Yaw = XMConvertToRadians(CameraDatas.Rotator.y);

	//Pitch is the angle between the direction vector and x-z plain
	//Yaw is the angle that rot from z
	//R is the direction vector in x-z plain shadow vector length
	float R = cosf(Pitch);
	LookDirection.x = R * sinf(Yaw);
	LookDirection.y = sinf(Pitch);
	LookDirection.z = R * cosf(Yaw);

	FocusPosition = CameraDatas.Target;

	Alpha = XM_PIDIV2 - Pitch;
	Theta = XM_PIDIV2 - Yaw;
	XMVECTOR Target = XMVector3Length(XMVectorSet(FocusPosition.x - Position.x, FocusPosition.y - Position.y, FocusPosition.z - Position.z, 1.f));
	Radius = XMVectorGetX(Target);
}

void Camera::OnResize(float WndWidth, float WndHeight)
{
	Fov = XMConvertToRadians(CameraDatas.Fov);
	AspectRatio = static_cast<float>(WndWidth) / WndHeight;
}

void Camera::ChangeViewMatrixByMouseEvent(float x, float y)
{
	/*Alpha += x;
	Theta += y;
	Theta = MathHelper::Clamp(Theta, 0.1f, MathHelper::Pi - 0.1f);

	Position.x = Radius*sinf(Alpha)*cosf(Theta);
	Position.y = Radius*cosf(Alpha);
	Position.z = Radius*sinf(Alpha)*sinf(Theta);*/
}

XMMATRIX Camera::GetViewMarix()
{
	return XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&FocusPosition), XMLoadFloat3(&UpDirection));
	 //XMMatrixLookToLH(XMLoadFloat3(&Position), XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));
}

XMMATRIX Camera::GetProjectionMatrix(float NearPlane /*= 1.0f*/, float FarPlane /*= 1000.0f*/)
{
	return XMMatrixPerspectiveFovLH(Fov, AspectRatio, NearPlane, FarPlane);
}

void Camera::ReadCameraDataFromFile(LPCWSTR FileName)
{
	std::wstring FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		cout << "Cannot open file!" << endl;
		return;
	}

	char* CameraBuffer = new char[sizeof(CameraData)];
	memset(CameraBuffer, '\0', sizeof(CameraData));
	Rf.read(CameraBuffer, sizeof(CameraData));
	CameraDatas = *(CameraData*)(CameraBuffer);

	delete[] CameraBuffer;
	Rf.close();
	if (!Rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return;
	}
}

