#pragma once

#include "stdafx.h"

using namespace DirectX;

enum class E_INDEX_TYPE : UINT8
{
	TYPE_UINT_16 = 0,
	TYPE_UINT_32,
};

enum class E_RESOURCE_TYPE : UINT8
{
	TYPE_UNKNOWN = 0,
	TYPE_INDEX_BUFFER,
	TYPE_VERTEX_BUFFER,
	TYPE_CONSTANT_BUFFER,
	TYPE_TEXTURE,
};


typedef struct FVertexData
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	XMFLOAT2 Tex0;
	XMFLOAT2 Tex1;
	XMFLOAT4 Color;
}FVertexData;

typedef struct FCameraData {
	XMFLOAT3 Location;
	XMFLOAT3 Target;
	XMFLOAT3 Rotator; // {Pitch, Yaw , Roll}
	float Fov;
	float Aspect;
}FCameraData;


struct FVertex_PositionTex0
{
	XMFLOAT3 Position;
	XMFLOAT2 Tex0;
};

const UINT RENDER_TARGET_COUNT = 3;