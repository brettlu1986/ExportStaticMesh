#pragma once

#include "stdafx.h"
#include "FDefine.h"
#include "LDefine.h"
#include "LActor.h"
#include "LCamera.h"


class LThirdPersonCamera : public LCamera
{
public:
	LThirdPersonCamera();
	~LThirdPersonCamera();

	virtual void Init() override;
	virtual void OnResize() override;
	virtual void Update(float DeltaTime) override;

	virtual XMMATRIX GetViewMarix() override;
	virtual void SetViewTarget(LActor* Target) override;
	

private:
	void UpdateViewTarget(float dt);

	LActor* ViewTarget;
	
};