#pragma once

#include "stdafx.h"

using namespace DirectX;

struct LAnimationScaleChannel
{
	XMFLOAT3 Scale;
};

struct LAnimationQuatChannel
{
	XMFLOAT4 Quat;
	XMFLOAT3 Rot;
};

struct LAnimationTranslateChannel
{
	XMFLOAT3 Translate;
};

