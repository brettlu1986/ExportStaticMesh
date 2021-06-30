#pragma once
#include "pch.h"

class LENGINE_API LActor
{
public: 
	LActor() {};
	virtual ~LActor() {};

	virtual void SetLocation(Vec3 Location) = 0;
	virtual void SetRotation(Vec3 Rotator) = 0;
	virtual void SetScale3D(Vec3 Scale) = 0;

	virtual Vec3 GetLocation() = 0;
	virtual Vec3 GetRotation() = 0;
	virtual Vec3 GetScale3D() = 0;

	virtual XMVECTOR GetMoveForwardVector() = 0;
	virtual XMVECTOR GetMoveRightVector() = 0;
};
