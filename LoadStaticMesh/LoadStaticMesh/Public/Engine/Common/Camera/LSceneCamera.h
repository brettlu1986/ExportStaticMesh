#pragma once


#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "LDefine.h"
#include "LCamera.h"


class LSceneCamera : public LCamera
{
public:
	LSceneCamera();
	virtual ~LSceneCamera();

	virtual void Init() override;
	virtual void OnResize() override;
	virtual void Update(float DeltaTime) override;
	virtual void SetActive(bool bActivate) override;

	virtual XMMATRIX GetViewMarix() override;

	void ProcessCameraMouseInput(FInputResult& MouseInput);
	void ProcessCameraKeyInput(FInputResult& KeyInput);
	void CalculateLocation();

	
private:
	void ProcessInput();

	void UpdateInput(float dt);
	bool IsKeyDown(char Key);
	bool IsKeyUp(char Key);

	static const UINT KEY_SIZE = 256;
	bool Keys[KEY_SIZE];

	XMVECTOR RightDirection;
	POINT LastMousePoint;
	XMVECTOR MoveOffset;
	bool bUpdateDirty;

	XMVECTOR MoveDirection;
};