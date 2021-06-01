#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "LDefine.h"
#include "LActor.h"


class LCamera
{
public:
	LCamera();
	virtual ~LCamera();

	virtual void Init();
	virtual void OnResize();
	virtual void Update(float DeltaTime);
	
	virtual XMMATRIX GetViewMarix() = 0;
	
	XMMATRIX GetProjectionMatrix(float NearPlane = 1.0f, float FarPlane = 1000.0f);

	const XMFLOAT3& GetCameraLocation()
	{
		return Position;
	}

	void SetCameraLocation(const XMFLOAT3& InLoc)
	{
		Position.x = InLoc.x;
		Position.y = InLoc.x;
		Position.z = InLoc.z;
	}

	E_CAMERA_TYPE GetCameraType()
	{
		return CameraType;
	}

	bool IsActive()
	{
		return bActive;
	}

	virtual void SetActive(bool bActivate)
	{
		bActive = bActivate;
	}
	
	void SetCameraData(FCameraData Data)
	{
		CameraDatas = Data;
	}

	void SetSocketOffset(XMFLOAT3 Offset);
	virtual void SetViewTarget(LActor* Target) {};

protected:
	bool bActive;
	FCameraData CameraDatas;
	XMFLOAT3 Position;
	float Yaw;
	float Pitch;
	//we can use either look direction or focus position to calculate view matrix
	float ArmLength;

	XMFLOAT3 LookDirection;
	XMFLOAT3 FocusPosition;
	XMFLOAT3 UpDirection;
	float Fov;
	float AspectRatio;

	XMFLOAT3 SocketOffset;

	E_CAMERA_TYPE CameraType;
};