#pragma once

#include "pch.h"
#include "FDefine.h"
#include "LDefine.h"
#include "LActor.h"


class LENGINE_API LCamera
{
public:
	LCamera();
	virtual ~LCamera();

	virtual void Init(LCameraData Data);
	virtual void OnResize();
	virtual void Update(float DeltaTime);
	
	virtual XMMATRIX GetViewMarix() = 0;
	
	XMMATRIX GetProjectionMatrix(float NearPlane = 0.1f, float FarPlane = 1000.0f);

	const Vec3& GetCameraLocation()
	{
		return Position;
	}

	void SetCameraLocation(const Vec3& InLoc)
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
	
	void SetSocketOffset(Vec3 Offset);
	virtual void SetViewTarget(LActor* Target) {};

	void UpdateViewProjectionRenderThread();

protected:
	bool bActive;
	LCameraData CameraDatas;
	Vec3 Position;
	float Yaw;
	float Pitch;
	//we can use either look direction or focus position to calculate view matrix
	float ArmLength;

	Vec3 LookDirection;
	Vec3 FocusPosition;
	Vec3 UpDirection;
	float Fov;
	float AspectRatio;

	Vec3 SocketOffset;

	E_CAMERA_TYPE CameraType;
};