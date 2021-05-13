#pragma once

#include "stdafx.h"

using namespace DirectX;

class LAnimationTrackFrame
{
public: 
	LAnimationTrackFrame();
	~LAnimationTrackFrame();

	XMFLOAT3 Scale;
	XMFLOAT4 Quat;
	XMFLOAT3 Translate;
};
