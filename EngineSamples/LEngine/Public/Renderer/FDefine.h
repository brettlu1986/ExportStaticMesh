#pragma once

#include "pch.h"

enum class E_FRESOURCE_TYPE : UINT8
{
	F_TYPE_UNKNOWN = 0,
	F_TYPE_INDEX_BUFFER,
	F_TYPE_VERTEX_BUFFER,
	F_TYPE_MESH,
	F_TYPE_LIGHT,
	F_TYPE_TEXTURE,
	F_TYPE_SHADER,
	F_TYPE_MATERIAL,
	F_TYPE_PIPLINE,
};

enum class E_RESOURCE_VIEW_TYPE : UINT8
{
	RESOURCE_VIEW_NONE = 0,
	RESOURCE_VIEW_RTV = 1,
	RESOURCE_VIEW_DSV = 1 << 1,
	RESOURCE_VIEW_CBV = 1 << 2,
	RESOURCE_VIEW_SRV = 1 << 3,
	RESOURCE_VIEW_UAV = 1 << 4,
	RESOURCE_VIEW_SAMPLER = 1 << 5,
};

enum class E_CONSTANT_BUFFER_TYPE : UINT8
{
	TYPE_CB_MATRIX = 0,
	TYPE_CB_MATERIAL,
	TYPE_CB_PASSCONSTANT,
	TYPE_CB_UNKNOWN,
};

enum class E_GRAPHIC_FORMAT : UINT32
{
	FORMAT_UNKNOWN = 0,
	FORMAT_R32G32B32A32_TYPELESS = 1,
	FORMAT_R32G32B32A32_FLOAT = 2,
	FORMAT_R32G32B32A32_UINT = 3,
	FORMAT_R32G32B32A32_SINT = 4,
	FORMAT_R32G32B32_TYPELESS = 5,
	FORMAT_R32G32B32_FLOAT = 6,
	FORMAT_R32G32B32_UINT = 7,
	FORMAT_R32G32B32_SINT = 8,
	FORMAT_R16G16B16A16_TYPELESS = 9,
	FORMAT_R16G16B16A16_FLOAT = 10,
	FORMAT_R16G16B16A16_UNORM = 11,
	FORMAT_R16G16B16A16_UINT = 12,
	FORMAT_R16G16B16A16_SNORM = 13,
	FORMAT_R16G16B16A16_SINT = 14,
	FORMAT_R32G32_TYPELESS = 15,
	FORMAT_R32G32_FLOAT = 16,
	FORMAT_R32G32_UINT = 17,
	FORMAT_R32G32_SINT = 18,
	FORMAT_R32G8X24_TYPELESS = 19,
	FORMAT_D32_FLOAT_S8X24_UINT = 20,
	FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	FORMAT_R10G10B10A2_TYPELESS = 23,
	FORMAT_R10G10B10A2_UNORM = 24,
	FORMAT_R10G10B10A2_UINT = 25,
	FORMAT_R11G11B10_FLOAT = 26,
	FORMAT_R8G8B8A8_TYPELESS = 27,
	FORMAT_R8G8B8A8_UNORM = 28,
	FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	FORMAT_R8G8B8A8_UINT = 30,
	FORMAT_R8G8B8A8_SNORM = 31,
	FORMAT_R8G8B8A8_SINT = 32,
	FORMAT_R16G16_TYPELESS = 33,
	FORMAT_R16G16_FLOAT = 34,
	FORMAT_R16G16_UNORM = 35,
	FORMAT_R16G16_UINT = 36,
	FORMAT_R16G16_SNORM = 37,
	FORMAT_R16G16_SINT = 38,
	FORMAT_R32_TYPELESS = 39,
	FORMAT_D32_FLOAT = 40,
	FORMAT_R32_FLOAT = 41,
	FORMAT_R32_UINT = 42,
	FORMAT_R32_SINT = 43,
	FORMAT_R24G8_TYPELESS = 44,
	FORMAT_D24_UNORM_S8_UINT = 45,
	FORMAT_R24_UNORM_X8_TYPELESS = 46,
	FORMAT_X24_TYPELESS_G8_UINT = 47,
	FORMAT_R8G8_TYPELESS = 48,
	FORMAT_R8G8_UNORM = 49,
	FORMAT_R8G8_UINT = 50,
	FORMAT_R8G8_SNORM = 51,
	FORMAT_R8G8_SINT = 52,
	FORMAT_R16_TYPELESS = 53,
	FORMAT_R16_FLOAT = 54,
	FORMAT_D16_UNORM = 55,
	FORMAT_R16_UNORM = 56,
	FORMAT_R16_UINT = 57,
	FORMAT_R16_SNORM = 58,
	FORMAT_R16_SINT = 59,
	FORMAT_R8_TYPELESS = 60,
	FORMAT_R8_UNORM = 61,
	FORMAT_R8_UINT = 62,
	FORMAT_R8_SNORM = 63,
	FORMAT_R8_SINT = 64,
	FORMAT_A8_UNORM = 65,
	FORMAT_R1_UNORM = 66,
	FORMAT_R9G9B9E5_SHAREDEXP = 67,
	FORMAT_R8G8_B8G8_UNORM = 68,
	FORMAT_G8R8_G8B8_UNORM = 69,
	FORMAT_BC1_TYPELESS = 70,
	FORMAT_BC1_UNORM = 71,
	FORMAT_BC1_UNORM_SRGB = 72,
	FORMAT_BC2_TYPELESS = 73,
	FORMAT_BC2_UNORM = 74,
	FORMAT_BC2_UNORM_SRGB = 75,
	FORMAT_BC3_TYPELESS = 76,
	FORMAT_BC3_UNORM = 77,
	FORMAT_BC3_UNORM_SRGB = 78,
	FORMAT_BC4_TYPELESS = 79,
	FORMAT_BC4_UNORM = 80,
	FORMAT_BC4_SNORM = 81,
	FORMAT_BC5_TYPELESS = 82,
	FORMAT_BC5_UNORM = 83,
	FORMAT_BC5_SNORM = 84,
	FORMAT_B5G6R5_UNORM = 85,
	FORMAT_B5G5R5A1_UNORM = 86,
	FORMAT_B8G8R8A8_UNORM = 87,
	FORMAT_B8G8R8X8_UNORM = 88,
	FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	FORMAT_B8G8R8A8_TYPELESS = 90,
	FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	FORMAT_B8G8R8X8_TYPELESS = 92,
	FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	FORMAT_BC6H_TYPELESS = 94,
	FORMAT_BC6H_UF16 = 95,
	FORMAT_BC6H_SF16 = 96,
	FORMAT_BC7_TYPELESS = 97,
	FORMAT_BC7_UNORM = 98,
	FORMAT_BC7_UNORM_SRGB = 99,
	FORMAT_AYUV = 100,
	FORMAT_Y410 = 101,
	FORMAT_Y416 = 102,
	FORMAT_NV12 = 103,
	FORMAT_P010 = 104,
	FORMAT_P016 = 105,
	FORMAT_420_OPAQUE = 106,
	FORMAT_YUY2 = 107,
	FORMAT_Y210 = 108,
	FORMAT_Y216 = 109,
	FORMAT_NV11 = 110,
	FORMAT_AI44 = 111,
	FORMAT_IA44 = 112,
	FORMAT_P8 = 113,
	FORMAT_A8P8 = 114,
	FORMAT_B4G4R4A4_UNORM = 115,

	FORMAT_P208 = 130,
	FORMAT_V208 = 131,
	FORMAT_V408 = 132,

	FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
	FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,

	FORMAT_FORCE_UINT = 0xffffffff
};

enum class E_RESOURCE_FLAGS : UINT32
{
	RESOURCE_FLAG_NONE = 0,
	RESOURCE_FLAG_ALLOW_RENDER_TARGET = 0x1,
	RESOURCE_FLAG_ALLOW_DEPTH_STENCIL = 0x2,
	RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS = 0x4,
	RESOURCE_FLAG_DENY_SHADER_RESOURCE = 0x8,
	RESOURCE_FLAG_ALLOW_CROSS_ADAPTER = 0x10,
	RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS = 0x20,
	RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY = 0x40
};

enum class E_RESOURCE_STATE : UINT32
{
	RESOURCE_STATE_COMMON = 0,
	RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
	RESOURCE_STATE_INDEX_BUFFER = 0x2,
	RESOURCE_STATE_RENDER_TARGET = 0x4,
	RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
	RESOURCE_STATE_DEPTH_WRITE = 0x10,
	RESOURCE_STATE_DEPTH_READ = 0x20,
	RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
	RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
	RESOURCE_STATE_STREAM_OUT = 0x100,
	RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
	RESOURCE_STATE_COPY_DEST = 0x400,
	RESOURCE_STATE_COPY_SOURCE = 0x800,
	RESOURCE_STATE_RESOLVE_DEST = 0x1000,
	RESOURCE_STATE_RESOLVE_SOURCE = 0x2000,
	RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x400000,
	RESOURCE_STATE_SHADING_RATE_SOURCE = 0x1000000,
	RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
	RESOURCE_STATE_PRESENT = 0,
	RESOURCE_STATE_PREDICATION = 0x200,
	RESOURCE_STATE_VIDEO_DECODE_READ = 0x10000,
	RESOURCE_STATE_VIDEO_DECODE_WRITE = 0x20000,
	RESOURCE_STATE_VIDEO_PROCESS_READ = 0x40000,
	RESOURCE_STATE_VIDEO_PROCESS_WRITE = 0x80000,
	RESOURCE_STATE_VIDEO_ENCODE_READ = 0x200000,
	RESOURCE_STATE_VIDEO_ENCODE_WRITE = 0x800000
};

typedef struct FVertexData
{
	Vec3 Position;
	Vec3 Normal;
	Vec3 Tangent;
	Vec2 Tex0;
	Vec2 Tex1;
	XMFLOAT4 Color = {0.66f, 0.66f, 0.66f, 1.0};
}FVertexData;

typedef struct FSkeletalVertexData
{
	Vec3 Position;
	Vec3 Normal;
	Vec3 Tangent;
	Vec2 Tex0;
	Vec2 Tex1;
	XMFLOAT4 Color = { 0.66f, 0.66f, 0.66f, 1.0 };
	Vec3 InfluenceWeights;
	UINT16  InfluenceBones[4];
}FSkeletalVertexData;

typedef enum class ERHI_INPUT_CLASSIFICATION
{
	INPUT_CLASSIFICATION_PER_VERTEX_DATA = 0,
	INPUT_CLASSIFICATION_PER_INSTANCE_DATA = 1
} 	ERHI_INPUT_CLASSIFICATION;

struct FRHIInputElement
{
	string SemanticName;
	UINT SemanticIndex;
	E_GRAPHIC_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	ERHI_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
};

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


struct FPiplineStateInitializer
{
	string KeyName;
	const FRHIInputElement* pInputElement;
	UINT NumElements;
	string RefVsShader;
	string RefPsShader;
	UINT NumRenderTargets;
	E_GRAPHIC_FORMAT RtvFormat;
	FRHIRasterizerState RasterizerStat;
	bool bTransparency = false;
	E_GRAPHIC_FORMAT DsvFormat = E_GRAPHIC_FORMAT::FORMAT_UNKNOWN;
};

struct FClearColor
{
	float Color[4];
};

struct FClearDepth
{
	float Depth;
	UINT8 Stencil;
};

struct FClearValue
{
	E_GRAPHIC_FORMAT Format;
	FClearColor* ClearColor;
	FClearDepth* ClearDepth;
};

struct FTextureInitializer
{
	UINT Width;
	UINT Height;
	UINT16 DepthOrArraySize;
	UINT16 MipLevels;
	E_GRAPHIC_FORMAT Format;
	E_RESOURCE_FLAGS Flags;
	FClearValue* ClearValue;
	E_RESOURCE_STATE ResourceState;
};

const FRHIInputElement StandardInputElementDescs[] =
{
	{ "POSITION", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 0, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 12, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 24, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32_FLOAT, 0, 36, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, E_GRAPHIC_FORMAT::FORMAT_R32G32_FLOAT, 0, 44, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32A32_FLOAT, 0, 52, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

const FRHIInputElement SkeletalInputElementDescs[] =
{
	{ "POSITION", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 0, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 12, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 24, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32_FLOAT, 0, 36, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, E_GRAPHIC_FORMAT::FORMAT_R32G32_FLOAT, 0, 44, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32A32_FLOAT, 0, 52, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "WEIGHTS", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 68, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "BONEINDICES", 0, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_UINT, 0, 80, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

struct FObjectConstants
{
public: 
	FObjectConstants()
	:World(MathHelper::Identity4x4())
	{};

	XMFLOAT4X4 World;
};

struct FMaterialConstants
{
public: 
	FMaterialConstants()
	:Alpha(1.0)
	, ColorBlendAdd({ 0.f, 0.f, 0.f, 0.f})
	, Padding({0.f, 0.f, 0.f})
	{};

	float Alpha;
	Vec3 Padding;
	XMFLOAT4 ColorBlendAdd;
};

#define MaxLights 16

struct FPassLight
{
	Vec3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;                          // point/spot light only
	Vec3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
	float FalloffEnd = 10.0f;                           // point/spot light only
	Vec3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
	float SpotPower = 64.0f;                            // spot light only
	UINT LightIndex = 0;
};

struct FPassViewProjection
{
public:
	FPassViewProjection()
	:ViewProj(MathHelper::Identity4x4())
	, EyePosW({ 0.0f, 0.0f, 0.0f })
	, CbPerObjectPad1(0.f)
	{};
	XMFLOAT4X4 ViewProj;
	Vec3 EyePosW ;
	float CbPerObjectPad1;
};

struct FPassLightInfo
{
public:
	FPassLightInfo()
		:ShadowTransform(XMFLOAT4X4())
		,LightSpaceMatrix(XMFLOAT4X4())
	{};
	XMFLOAT4X4 LightSpaceMatrix;
	XMFLOAT4X4 ShadowTransform;
	FPassLight Lights[MaxLights];
};

static const UINT MAX_BONE_TRANS = 80;
struct FSkeletalConstants
{
	XMFLOAT4X4 BoneMapBoneTransforms[MAX_BONE_TRANS];
};

const UINT RENDER_TARGET_COUNT = 3;
const int FRAME_COUNT = 3;
const UINT SHADOW_WIDTH = 4096;
const UINT SHADOW_HEIGHT = 4096;

struct FPassBloomSetup
{
	XMFLOAT4 BufferSizeAndInvSize;
	float BloomThrehold;
};

struct FPassBloomDown
{
	XMFLOAT4 BufferSizeAndInvSize;
	float BloomDownScale;
};

struct FPassBloomUp
{
	XMFLOAT4 BufferASizeAndInvSize;
	XMFLOAT4 BufferBSizeAndInvSize;
	XMFLOAT4 BloomTintA;
	XMFLOAT4 BloomTintB;
	Vec2 BloomUpScales;
};

struct FPassSunMerge
{
	XMFLOAT4 BloomUpSizeAndInvSize;
	Vec3 BloomColor;
};

