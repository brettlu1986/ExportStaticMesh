

#include "LAssetManager.h"
#include "LAssetDataLoader.h"

LAssetManager* LAssetManager::Instance = nullptr;
LAssetManager::LAssetManager()
{

}

LAssetManager::~LAssetManager()
{
	for (auto& Ma : ResourceMaterials)
	{
		Ma.second = nullptr;
	}

	for (auto& Ske : ResourceSkeletons)
	{
		Ske.second = nullptr;
	}

	for (auto& Tex : ResourceTexs)
	{
		Tex.second = nullptr;
	}

	for (auto& Sha : ResourceShaders)
	{
		Sha.second = nullptr;
	}
}

LAssetManager* LAssetManager::Get()
{
	if (Instance == nullptr)
		Instance = new LAssetManager();

	return Instance;
}

void LAssetManager::LoadSkeletal(std::string FileName, string ReferenceName)
{
	auto Skeleton = make_unique<LSkeleton>();
	LAssetDataLoader::LoadSkeletonFromFile(FileName, Skeleton.get());
	ResourceSkeletons.insert( { ReferenceName, move(Skeleton) } );
}

//current only dds texture suport
void LAssetManager::LoadTexture(std::string FileName, string ReferenceName)
{
	auto Texture = make_unique<LTexture>();
	LAssetDataLoader::LoadDDSTextureFromFile(FileName, Texture.get());
	ResourceTexs.insert( { ReferenceName, move(Texture) } );
}

void LAssetManager::LoadShader(string FileName, string ReferenceName)
{
	auto Shader = make_unique<LShader>();
	LAssetDataLoader::LoadShaderFromeFile(FileName, Shader.get());
	ResourceShaders.insert( { ReferenceName, move(Shader) } );
}

void LAssetManager::LoadMaterial(unique_ptr<LMaterial>&Mat, string RefereneceName)
{
	ResourceMaterials.insert( { RefereneceName, move(Mat) } );
}