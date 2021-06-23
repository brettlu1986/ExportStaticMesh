#pragma once
#include "pch.h"

class LENGINE_API LActor
{
public: 
	LActor() {};
	virtual ~LActor() {};

	virtual void SetLocation(XMFLOAT3 Location) = 0;
	virtual void SetRotation(XMFLOAT3 Rotator) = 0;
	virtual void SetScale3D(XMFLOAT3 Scale) = 0;

	virtual XMFLOAT3 GetLocation() = 0;
	virtual XMFLOAT3 GetRotation() = 0;
	virtual XMFLOAT3 GetScale3D() = 0;

	virtual XMVECTOR GetMoveForwardVector() = 0;
	virtual XMVECTOR GetMoveRightVector() = 0;
};
