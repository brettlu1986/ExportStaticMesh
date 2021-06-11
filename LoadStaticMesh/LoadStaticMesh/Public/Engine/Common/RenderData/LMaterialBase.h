#pragma once

#include "stdafx.h"
#include "LResource.h"
#include "LTexture.h"

typedef enum class E_MATERIAL_PARAM_TYPE :UINT8
{
	TYPE_FLOAT = 0,
	TYPE_VECTOR,
	TYPE_TEXTURE,
	MAX,
}E_MATERIAL_PARAM_TYPE;

class LMaterialBase : public LResource
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
		return ParamFloats[Index];
	}
	
	XMFLOAT4& GetParamVectors(UINT Index)
	{
		return ParamVectors[Index];
	}

	LTexture* GetParamTexture(UINT Index)
	{
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

protected:
	vector<E_MATERIAL_PARAM_TYPE> ParamTypes;
	vector<float> ParamFloats;
	vector<XMFLOAT4> ParamVectors;
	vector<LTexture*> ParamTextures;
};