#include "LCamera.h"
#include "MathHelper.h"
#include "FD3D12Helper.h"
#include <DirectXMath.h>
#include <iostream>
#include <fstream>

using namespace DirectX;
using namespace std;

static const wstring CameraBinName = L"camera.bin";

LCamera::LCamera()
	:Position(0, 0, 0)
	,InitialPosition(0,0,0)
	,Yaw(0)
	,Pitch(0.f)
	,LookDirection(0, 0, 1)
	,FocusPosition(0, 0, 1)
	,UpDirection(0, 0, 1)
	,Fov(XM_PI/2)
	,AspectRatio(1.777f)
	,CameraDatas(CameraData())
	,Radius(1.f)
	,Alpha(0.f)
	,Theta(0.f)
{

}

LCamera::~LCamera()
{

}

void LCamera::Init()
{
	ReadCameraDataFromFile(CameraBinName.c_str());

	InitialPosition = CameraDatas.Location;
	Position = InitialPosition;

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

	//change Cartesian coordinate to Spherical coordinate, so it will be easy to use mouse rotate camera
	XMVECTOR Target = XMVector3Length(XMVectorSet(Position.x - FocusPosition.x, Position.y - FocusPosition.y, Position.z - FocusPosition.z, 1.f));
	Radius = XMVectorGetX(Target);
	// Spherical coordinate, direction vector up angle to z
	Alpha = acosf( (Position.z - FocusPosition.z) / Radius);
	// Spherical coordinate, direction shadow vector angle in x-y plain to y
	Theta = atanf( (Position.x - FocusPosition.x) / (Position.y - FocusPosition.y));
	//Theta range is [0, 2Pi]
	if(Theta < 0.f)
		Theta += XM_PI * 2;
}

void LCamera::OnResize(float WndWidth, float WndHeight)
{
	Fov = XMConvertToRadians(CameraDatas.Fov);
	AspectRatio = static_cast<float>(WndWidth) / WndHeight;
}

void LCamera::ChangeViewMatrixByMouseEvent(float x, float y)
{
	Alpha += y;
	Theta += x;
	Alpha = MathHelper::Clamp(Alpha, 0.1f, MathHelper::Pi - 0.1f);

	XMVECTOR V = DirectX::XMVectorSet(Radius * sinf(Alpha) * sinf(Theta),
		Radius * sinf(Alpha) * cosf(Theta), 
		Radius * cosf(Alpha),
		1.f);
	V += XMLoadFloat3(&FocusPosition);
	Position.x = XMVectorGetX(V);
	Position.y = XMVectorGetY(V);
	Position.z = XMVectorGetZ(V);
}

XMMATRIX LCamera::GetViewMarix()
{
	return XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&FocusPosition), XMLoadFloat3(&UpDirection));
	 //XMMatrixLookToLH(XMLoadFloat3(&Position), XMLoadFloat3(&LookDirection), XMLoadFloat3(&UpDirection));
}

XMMATRIX LCamera::GetProjectionMatrix(float NearPlane /*= 1.0f*/, float FarPlane /*= 1000.0f*/)
{
	return XMMatrixPerspectiveFovLH(Fov, AspectRatio, NearPlane, FarPlane);
}

void LCamera::ReadCameraDataFromFile(LPCWSTR FileName)
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

