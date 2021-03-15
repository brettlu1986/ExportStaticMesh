#include "Camera.h"
#include "MathHelper.h"
#include "GraphicHelper.h"
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
	float R = cosf(Pitch);
	LookDirection.x = R * sinf(Yaw);
	LookDirection.y = sinf(Pitch);
	LookDirection.z = R * cosf(Yaw);

	FocusPosition = CameraDatas.Target;
}

void Camera::OnResize(float WndWidth, float WndHeight)
{
	Fov = XMConvertToRadians(CameraDatas.Fov);
	AspectRatio = static_cast<float>(WndWidth) / WndHeight;
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

	//calculate model matrix : scale * rotation * translation
	//in ue4: ue4x = Forward ue4y = Right ue4z = Up
	//in direct x: use left hand coordinate, x = Right, y = Up, z = Forward
	//we have the conversion: x = ue4y, y = ue4z, z = ue4x, Yaw Pitch Roll stay the same
	CameraDatas.Location = MathHelper::GetUe4ConvertLocation(CameraDatas.Location);
	CameraDatas.Target = MathHelper::GetUe4ConvertLocation(CameraDatas.Target);

	delete[] CameraBuffer;
	Rf.close();
	if (!Rf.good()) {
		cout << "Error occurred at reading time!" << endl;
		return;
	}
}

