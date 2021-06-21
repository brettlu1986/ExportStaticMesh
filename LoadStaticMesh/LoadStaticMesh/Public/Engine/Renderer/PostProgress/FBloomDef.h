#pragma once

#include "FDefine.h"

static const UINT DOWN_SAMPLE_COUNT = 4;
static const UINT UP_SAMPLE_COUNT = 3;

//scene color render target, HDR
static FClearColor PostClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

const FRHIInputElement FullInputElementDescs[] =
{
	{ "POSITION", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 0, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32_FLOAT, 0, 12, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

static const auto BloomRTFormat = E_GRAPHIC_FORMAT::FORMAT_R11G11B10_FLOAT;

static const float BloomIntensity = 1.0f;
static const float BloomThreHold = 1.0f;

static const XMFLOAT4 BloomTint1 = { 0.3465f, 0.3465f, 0.3465f, 0.3465f };
static const XMFLOAT4 BloomTint2 = { 0.138f, 0.138f, 0.138f, 0.138f };
static const XMFLOAT4 BloomTint3 = { 0.1176f,  0.1176f, 0.1176f, 0.1176f };
static const XMFLOAT4 BloomTint4 = { 0.066f,  0.066f, 0.066f, 0.066f };
static const XMFLOAT4 BloomTint5 = { 0.066f,  0.066f, 0.066f, 0.066f };

static const XMFLOAT4 BloomTintAs[] = { BloomTint4,
		XMFLOAT4(BloomTint3.x * BloomIntensity, BloomTint3.y * BloomIntensity, BloomTint3.z * BloomIntensity, BloomTint3.w * BloomIntensity),
		XMFLOAT4(BloomTint2.x * BloomIntensity, BloomTint2.y * BloomIntensity, BloomTint2.z * BloomIntensity, BloomTint2.w * BloomIntensity)
};
static const XMFLOAT4 BloomTintBs[] = { BloomTint5,
	XMFLOAT4(1.f, 1.f, 1.f, 0.f),
	XMFLOAT4(1.f, 1.f, 1.f, 0.f),
};