#pragma once

#include "stdafx.h"

#include "LTexture.h"
#include "LSkeleton.h"
#include "LShader.h"
#include "LMaterialBase.h"
#include "LIndexBuffer.h"
#include "LVertexBuffer.h"
#include "LAnimationSequence.h"

struct LStaticMeshBuffer
{
	LVertexBuffer* VertexBufferData;
	LIndexBuffer* IndexBufferData;
};

struct LSkeletalMeshBuffer
{
	LVertexBuffer* VertexBufferData;
	LIndexBuffer* IndexBufferData;
	vector<UINT16> BoneMap;
};

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
	void LoadAnimations(std::string FileName, string ReferenceName);
	void LoadStaticMeshGeometry(std::string FileName, string ReferenceName);
	void LoadSkeletalMeshGeometry(std::string FileName, string ReferenceName);

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

	LAnimationSequence* GetAnimationSeq(const std::string& RefName)
	{
		return ResourceAnims[RefName].get();
	}

	LMaterialBase* GetMaterial(const std::string& RefName)
	{
		return ResourceMaterials[RefName].get();
	}

	LStaticMeshBuffer* GetMeshBuffer(const std::string& RefName)
	{
		return ResourceMeshBuffers[RefName].get();
	}

	LSkeletalMeshBuffer* GetSkeletalMeshBuffer(const std::string& RefName)
	{
		return ResourceSkeletalMeshBuffers[RefName].get();
	}
	
	
private:
	static LAssetManager* Instance;

	unordered_map<std::string, unique_ptr<LSkeleton>> ResourceSkeletons;
	unordered_map<std::string, unique_ptr<LTexture>> ResourceTexs;
	unordered_map<std::string, unique_ptr<LShader>> ResourceShaders;
	unordered_map<std::string, unique_ptr<LMaterialBase>> ResourceMaterials;

	unordered_map<std::string, unique_ptr<LAnimationSequence>> ResourceAnims;
	unordered_map<std::string, unique_ptr<LStaticMeshBuffer>> ResourceMeshBuffers;
	unordered_map<std::string, unique_ptr<LSkeletalMeshBuffer>> ResourceSkeletalMeshBuffers;
};
