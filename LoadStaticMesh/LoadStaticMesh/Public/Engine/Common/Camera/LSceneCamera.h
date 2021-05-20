#pragma once


#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "LDefine.h"
#include "LCamera.h"

using namespace DirectX;

class LSceneCamera : public LCamera
{
public:
	LSceneCamera();
	virtual ~LSceneCamera();

	virtual void Init() override;
	virtual void OnResize() override;
	virtual void Update(float DeltaTime) override;

	virtual XMMATRIX GetViewMarix() override;

	void ProcessCameraMouseInput(FInputResult& MouseInput);
	void ProcessCameraKeyInput(FInputResult& KeyInput);
	void CalculateLocation();

private:
	void UpdateInput(float dt);
	bool IsKeyDown(char Key);
	bool IsKeyUp(char Key);

	static const UINT KEY_SIZE = 256;
	XMVECTOR RightDirection;

	POINT LastMousePoint;
	bool Keys[KEY_SIZE];
	XMVECTOR MoveOffset;

	bool bUpdateDirty;
};