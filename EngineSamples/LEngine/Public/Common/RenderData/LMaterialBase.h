#pragma once

#include "pch.h"
#include "LResource.h"
#include "LTexture.h"

typedef enum class E_MATERIAL_PARAM_TYPE :UINT8
{
	TYPE_FLOAT = 0,
	TYPE_VECTOR,
	TYPE_TEXTURE,
	MAX,
}E_MATERIAL_PARAM_TYPE;

class LENGINE_API LMaterialBase : public LResource
{
public:
	LMaterialBase();
	virtual ~LMaterialBase();

	void AddParamType(E_MATERIAL_PARAM_TYPE Type);
	void AddParamFloat(float Value);
	void AddParamVector(XMFLOAT4 Vec);
	void AddParamTexture(LTexture* Tex);

	const vector<E_MATERIAL_PARAM_TYPE>& GetParamTypes()
	{
		return ParamTypes;
	}

	const vector<float>& GetParamFloats()
	{
		return ParamFloats;
	}

	const vector<XMFLOAT4>& GetParamVectors()
	{
		return ParamVectors;
	}

	const vector<LTexture*>& GetParamTextures()
	{
		return ParamTextures;
	}

	float GetParamFloat(UINT Index)
	{
		if (ParamFloats.empty() || Index >= ParamFloats.size())
			return 1.f;
		return ParamFloats[Index];
	}
	
	XMFLOAT4 GetParamVector(UINT Index)
	{
		if (ParamVectors.empty() || Index >= ParamVectors.size())
			return XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		return ParamVectors[Index];
	}

	LTexture* GetParamTexture(UINT Index)
	{
		if(ParamTextures.empty() || Index >= ParamTextures.size())
			return nullptr;
		return ParamTextures[Index];
	}

	void SetParamFloat(UINT Index, float Value)
	{
		ParamFloats[Index] = Value;
	}

	void SetParamVector(UINT Index, XMFLOAT4 Value)
	{
		ParamVectors[Index] = Value;
	}

	void SetParamTexture(UINT Index, LTexture* NewTex)
	{
		ParamTextures[Index] = NewTex;
	}

	void SetBlendMode(LBlendMode Mode)
	{
		BlendMode = Mode;
	}

	const LBlendMode& GetBlendMode()
	{
		return BlendMode;
	}

	bool IsBlendModeTransparency()
	{
		return BlendMode.BlendModeValue == E_BLEND_MODE::BLEND_Translucent;
	}

protected:
	vector<E_MATERIAL_PARAM_TYPE> ParamTypes;
	vector<float> ParamFloats;
	vector<XMFLOAT4> ParamVectors;
	vector<LTexture*> ParamTextures;

	LBlendMode BlendMode;
};