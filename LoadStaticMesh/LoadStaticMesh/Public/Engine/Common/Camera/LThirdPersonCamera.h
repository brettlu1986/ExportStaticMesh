#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "LDefine.h"
#include "LActor.h"
#include "LCamera.h"

using namespace DirectX;

class LThirdPersonCamera : public LCamera
{
public:
	LThirdPersonCamera();
	~LThirdPersonCamera();

	virtual void Init() override;
	virtual void OnResize() override;
	virtual void Update(float DeltaTime) override;

	virtual XMMATRIX GetViewMarix() override;

	void SetViewTarget(LActor* Target);
	void SetSocketOffset(XMFLOAT3 Offset);

private:
	void UpdateViewTarget(float dt);

	LActor* ViewTarget;
	XMFLOAT3 SocketOffset;
};