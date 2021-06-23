#pragma once


#pragma once

#include "pch.h"
#include "FDefine.h"
#include "LDefine.h"
#include "LCamera.h"


class LENGINE_API LSceneCamera : public LCamera
{
public:
	LSceneCamera();
	virtual ~LSceneCamera();

	virtual void Init(LCameraData Data) override;
	virtual void OnResize() override;
	virtual void Update(float DeltaTime) override;
	virtual void SetActive(bool bActivate) override;

	virtual XMMATRIX GetViewMarix() override;

	void ProcessCameraMouseInput(LInputResult& MouseInput);
	void ProcessCameraKeyInput(LInputResult& KeyInput);
	
private:
	void ProcessInput();

	void UpdateInput(float dt);
	bool IsKeyDown(char Key);
	bool IsKeyUp(char Key);

	static const UINT KEY_SIZE = 256;
	bool Keys[KEY_SIZE];

	XMVECTOR RightDirection;
	POINT LastMousePoint;
	POINT CurrentMousePoint;

	XMVECTOR MoveOffset;
	bool bUpdateDirty;
	bool bUpdateRotDirty;

	float Dx;
	float Dy;

	float MoveSpeed;
	float RotSpeed;
	XMVECTOR MoveDirection;
};