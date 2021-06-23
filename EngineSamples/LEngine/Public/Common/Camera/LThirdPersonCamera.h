#pragma once

#include "pch.h"
#include "FDefine.h"
#include "LDefine.h"
#include "LActor.h"
#include "LCamera.h"


class LENGINE_API LThirdPersonCamera : public LCamera
{
public:
	LThirdPersonCamera();
	~LThirdPersonCamera();

	virtual void Init(LCameraData Data) override;
	virtual void OnResize() override;
	virtual void Update(float DeltaTime) override;
	virtual void SetActive(bool bActivate) override;

	virtual XMMATRIX GetViewMarix() override;
	virtual void SetViewTarget(LActor* Target) override;
	

private:
	void UpdateViewTarget(float dt);

	LActor* ViewTarget;
	
};