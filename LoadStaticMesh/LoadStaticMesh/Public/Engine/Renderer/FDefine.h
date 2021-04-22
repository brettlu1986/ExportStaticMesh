#pragma once

#include "stdafx.h"
#include "MathHelper.h"

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
	TYPE_SHADER,
	TYPE_MATERIAL,
	TYPE_PIPLINE,
	TYPE_SHADERMAP,
};

enum class E_CONSTANT_BUFFER_TYPE : UINT8
{
	TYPE_CB_MATRIX,
	TYPE_CB_MATERIAL,
	TYPE_CB_PASSCONSTANT,
	TYPE_CB_UNKNOWN,
};

typedef struct FVertexData
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	XMFLOAT2 Tex0;
	XMFLOAT2 Tex1;
	XMFLOAT4 Color = {0.66f, 0.66f, 0.66f, 1.0};
}FVertexData;

typedef struct FCameraData {
	XMFLOAT3 Location;
	XMFLOAT3 Target;
	XMFLOAT3 Rotator; // {Pitch, Yaw , Roll}
	float Fov;
	float Aspect;
}FCameraData;


typedef enum class ERHI_DATA_FORMAT
{
	FORMAT_UNKNONWN = 0,
	FORMAT_R32G32B32_FLOAT,
	FORMAT_R32G32_FLOAT,
	FORMAT_R32G32B32A32_FLOAT,
}ERHI_DATA_FORMAT;

typedef enum class ERHI_INPUT_CLASSIFICATION
{
	INPUT_CLASSIFICATION_PER_VERTEX_DATA = 0,
	INPUT_CLASSIFICATION_PER_INSTANCE_DATA = 1
} 	ERHI_INPUT_CLASSIFICATION;

struct FRHIInputElement
{
	std::string SemanticName;
	UINT SemanticIndex;
	ERHI_DATA_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	ERHI_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
};

typedef enum class FRtvFormat
{
	FORMAT_UNKNOWN = 0,
	FORMAT_R8G8B8A8_UNORM,
}FRtvFormat;

typedef enum class FCullMode
{
	CULL_MODE_NONE = 1,
	CULL_MODE_FRONT = 2,
	CULL_MODE_BACK = 3
}FCullMode;

struct FRHIRasterizerState
{
	int DepthBias = 0;
	float DepthBiasClamp = 0.f;
	float SlopeScaledDepthBias = 0.f;
	bool FrontCounterClockwise = true;
	FCullMode CullMode = FCullMode::CULL_MODE_NONE;
};
//later will add more param, temp use these for this project
struct FRHIPiplineStateInitializer
{
	std::string KeyName;
	const FRHIInputElement* pInpueElement;
	UINT NumElements;
	BYTE* pVSPointer;
	SIZE_T VsPointerLength;
	BYTE* pPsPointer;
	SIZE_T PsPointerLength;
	UINT NumRenderTargets;
	FRtvFormat RtvFormat;
	FRHIRasterizerState RasterizerStat;
};

const FRHIInputElement StandardInputElementDescs[] =
{
	{ "POSITION", 0, ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 0, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 12, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 24, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, ERHI_DATA_FORMAT::FORMAT_R32G32_FLOAT, 0, 36, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, ERHI_DATA_FORMAT::FORMAT_R32G32_FLOAT, 0, 44, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, ERHI_DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT, 0, 52, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

const UINT StandardInputStride = 68;

struct FObjectConstants
{
public: 
	FObjectConstants()
	:World(MathHelper::Identity4x4())
	,TexTransform(MathHelper::Identity4x4())
	{};

	XMFLOAT4X4 World;
	XMFLOAT4X4 TexTransform;
};

struct FMaterialConstants
{
public: 
	FMaterialConstants()
	:DiffuseAlbedo({ 1.0f, 1.0f, 1.0f, 1.0f })
	, FresnelR0({ 0.01f, 0.01f, 0.01f })
	, Roughness(0.25f)
	, MatTransform(MathHelper::Identity4x4())
	{};

	XMFLOAT4 DiffuseAlbedo;
	XMFLOAT3 FresnelR0;
	float Roughness;
	XMFLOAT4X4 MatTransform;
};

#define MaxLights 16

struct FLight
{
	DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;                          // point/spot light only
	DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
	float FalloffEnd = 10.0f;                           // point/spot light only
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
	float SpotPower = 64.0f;                            // spot light only
	UINT LightIndex = 0;
};

struct FPassConstants
{
public:
	FPassConstants()
	:ViewProj(MathHelper::Identity4x4())
	, EyePosW({ 0.0f, 0.0f, 0.0f })
	, AmbientLight({ 0.0f, 0.0f, 0.0f, 1.0f })
	, CbPerObjectPad1(0.f)
	{};
	DirectX::XMFLOAT4X4 ViewProj;
	DirectX::XMFLOAT3 EyePosW ;
	float CbPerObjectPad1;
	DirectX::XMFLOAT4X4 LightSpaceMatrix;
	DirectX::XMFLOAT4X4 ShadowTransform;
	DirectX::XMFLOAT4 AmbientLight;
	FLight Lights[MaxLights];
};

struct FBufferObject
{
public: 
	FBufferObject()
	:Type(E_CONSTANT_BUFFER_TYPE::TYPE_CB_UNKNOWN)
	, DataSize(0)
	,BufferData(nullptr)
	{}

	~FBufferObject()
	{
		if(BufferData)
		{
			delete BufferData;
			BufferData = nullptr;
		}
	}
	E_CONSTANT_BUFFER_TYPE Type;
	UINT DataSize;
	int8_t* BufferData;
};

typedef struct FConstantBufferDesc
{
	E_CONSTANT_BUFFER_TYPE BufferType;
	UINT HeapOffsetStart;
	UINT BufferSize;
	UINT BufferViewCount;
}FConstantBufferDesc;

typedef struct FShaderResourceDesc
{
	UINT HeapOffsetStart;
	UINT ShaderResourceCount;
}FShaderResourceDesc;

typedef struct FShaderMapDesc
{
	UINT HeapOffsetStart;
}FShaderMapDesc;

typedef struct FCbvSrvDesc
{
	UINT NeedDesciptorCount;
	FConstantBufferDesc CbMatrix;
	FConstantBufferDesc CbMaterial;
	FConstantBufferDesc CbConstant;
	FShaderResourceDesc SrvDesc;
	FShaderMapDesc ShaderMapDesc;
}FCbvSrvDesc;

const UINT RENDER_TARGET_COUNT = 3;
const int FRAME_COUNT = 3;
const UINT SHADOW_WIDTH = 2048;
const UINT SHADOW_HEIGHT = 2048;

