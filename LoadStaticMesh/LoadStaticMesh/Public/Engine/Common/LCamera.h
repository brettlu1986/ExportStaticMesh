#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "LDefine.h"
#include "LActor.h"

using namespace DirectX;

class LCamera
{
public:
	LCamera();
	~LCamera();

	void Init();
	void OnResize();
	
	XMMATRIX GetViewMarix();
	XMMATRIX GetProjectionMatrix(float NearPlane = 1.0f, float FarPlane = 1000.0f);

	void ProcessCameraMouseInput(FInputResult& MouseInput);
	void ProcessCameraKeyInput(FInputResult& KeyInput);
	void Update(float DeltaTime);


	const XMFLOAT3& GetViewTargetLocation() {
		return CameraDatas.Target;
	}

	FCameraData& GetCameraData()
	{
		return CameraDatas;
	}

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

	void CalculateLocation();

	void SetViewTarget(LActor* Target);
private:

	void UpdateInput(float dt);
	void UpdateViewTarget(float dt);

	bool IsKeyDown(char Key);
	bool IsKeyUp(char Key);

	static const UINT KEY_SIZE = 256;
	
	FCameraData CameraDatas;
	XMFLOAT3 Position;
	float Yaw;
	float Pitch;
	//we can use either look direction or focus position to calculate view matrix
	float ArmLength;
	XMFLOAT3 LookDirection;
	XMFLOAT3 FocusPosition;
	XMFLOAT3 UpDirection;
	XMVECTOR RightDirection;

	float Fov;
	float AspectRatio;

	POINT LastMousePoint;
	bool Keys[KEY_SIZE];
	XMVECTOR MoveOffset;

	bool bUpdateDirty;

};