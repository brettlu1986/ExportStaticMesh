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

	//change Cartesian coordinate to Spherical coordinate, so it will be easy to use mouse rotate camera
	XMVECTOR Target = XMVector3Length(XMVectorSet(Position.x - FocusPosition.x, Position.y - FocusPosition.y, Position.z - FocusPosition.z, 1.f));
	Radius = XMVectorGetX(Target);
	// Spherical coordinate, direction vector up angle to y
	Alpha = acosf( (Position.y - FocusPosition.y) / Radius);
	// Spherical coordinate, direction shadow vector angle in x-z plain to x
	Theta = atanf( (Position.z - FocusPosition.z) / (Position.x - FocusPosition.x));
	//Theta range is [0, 2Pi]
	if(Theta < 0.f)
		Theta += XM_PI * 2;
}

void Camera::OnResize(float WndWidth, float WndHeight)
{
	Fov = XMConvertToRadians(CameraDatas.Fov);
	AspectRatio = static_cast<float>(WndWidth) / WndHeight;
}

void Camera::ChangeViewMatrixByMouseEvent(float x, float y)
{
	Theta += x;
	Alpha += y;
	Alpha = MathHelper::Clamp(Alpha, 0.1f, MathHelper::Pi - 0.1f);

	XMVECTOR V = MathHelper::SphericalToCartesian(Radius, Theta, Alpha);
	V += XMLoadFloat3(&FocusPosition);
	Position.x = XMVectorGetX(V);
	Position.y = XMVectorGetY(V);
	Position.z = XMVectorGetZ(V);

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

