#pragma once

#include "stdafx.h"

#include "LTexture.h"
#include "LSkeleton.h"
#include "LShader.h"
#include "LMaterialBase.h"

class LAssetManager
{
public:
	LAssetManager();
	~LAssetManager();

	static LAssetManager* Get();

	void LoadAssets(std::string AssetsFile);

	void LoadSkeleton(std::string FileName, string ReferenceName);
	void LoadShader(string FileName, string ReferenceName);
	void LoadTexture(std::string FileName, string ReferenceName);
	void LoadMaterial(std::string FileName, string ReferenceName);
	void LoadMaterialInstance(std::string FileName, string ReferenceName);

	//deprecated
	//void LoadMaterial(unique_ptr<LMaterial>&MatTex, string RefereneceName);

	LSkeleton* GetSkeletal(const std::string& RefName) {
		return ResourceSkeletons[RefName].get();
	}

	LShader* GetShader(const std::string& RefName)
	{
		return ResourceShaders[RefName].get();
	}

	LTexture* GetTexture(const std::string& RefName)
	{
		return ResourceTexs[RefName].get();
	}

	LMaterialBase* GetMaterial(const std::string& RefName)
	{
		return ResourceMaterials[RefName].get();
	}
	
private:
	static LAssetManager* Instance;

	unordered_map<std::string, unique_ptr<LSkeleton>> ResourceSkeletons;
	unordered_map<std::string, unique_ptr<LTexture>> ResourceTexs;
	unordered_map<std::string, unique_ptr<LShader>> ResourceShaders;
	unordered_map<std::string, unique_ptr<LMaterialBase>> ResourceMaterials;
};
