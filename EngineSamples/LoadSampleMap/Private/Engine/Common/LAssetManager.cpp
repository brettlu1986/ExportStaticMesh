

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

	for (auto& Anim : ResourceAnims)
	{
		Anim.second = nullptr;
	}

	for (auto& MeshBuffer : ResourceMeshBuffers)
	{
		delete MeshBuffer.second->IndexBufferData;
		MeshBuffer.second->IndexBufferData = nullptr;
		delete MeshBuffer.second->VertexBufferData;
		MeshBuffer.second->VertexBufferData = nullptr;
		MeshBuffer.second = nullptr;
	}

	for (auto& SkeMeshBuffer : ResourceSkeletalMeshBuffers)
	{
		delete SkeMeshBuffer.second->IndexBufferData;
		SkeMeshBuffer.second->IndexBufferData = nullptr;
		delete SkeMeshBuffer.second->VertexBufferData;
		SkeMeshBuffer.second->VertexBufferData = nullptr;
		SkeMeshBuffer.second = nullptr;
	}
}

LAssetManager* LAssetManager::Get()
{
	if (Instance == nullptr)
		Instance = new LAssetManager();

	return Instance;
}

void LAssetManager::LoadAssets(std::string AssetsFile)
{
	vector<LAssetDef> Skeletons;
	vector<LAssetDef> ShaderFiles;
	vector<LAssetDef> Textures;
	vector<LAssetDef> Materials;
	vector<LAssetDef> MaterialInstances;
	vector<LAssetDef> Animations;
	vector<LAssetDef> MeshGeometries;
	vector<LAssetDef> SkeletalMeshGeometries;

	LAssetDataLoader::LoadAssetsFromFile(AssetsFile, Skeletons, ShaderFiles, Textures, Materials, MaterialInstances, Animations,
		MeshGeometries, SkeletalMeshGeometries);

	for(auto& Skeleton : Skeletons)
	{
		LoadSkeleton(Skeleton.FileName, Skeleton.RefName);
	}

	for(auto& Shader : ShaderFiles)
	{
		LoadShader(Shader.FileName, Shader.RefName);
	}

	for(auto& Texture : Textures)
	{
		LoadTexture(Texture.FileName, Texture.RefName);
	}

	for(auto& Material : Materials)
	{
		LoadMaterial(Material.FileName, Material.RefName);
	}

	for (auto& MaterialIns : MaterialInstances)
	{
		LoadMaterialInstance(MaterialIns.FileName, MaterialIns.RefName);
	}

	for(auto& AnimIns : Animations)
	{
		LoadAnimations(AnimIns.FileName, AnimIns.RefName);
	}

	for(auto& MeshGeo : MeshGeometries)
	{
		LoadStaticMeshGeometry(MeshGeo.FileName, MeshGeo.RefName);
	}

	for(auto& SkeMeshGeo : SkeletalMeshGeometries)
	{
		LoadSkeletalMeshGeometry(SkeMeshGeo.FileName, SkeMeshGeo.RefName);
	}
}

void LAssetManager::LoadSkeleton(std::string FileName, string ReferenceName)
{
	auto Skeleton = make_unique<LSkeleton>();
	LAssetDataLoader::LoadSkeletonFromFile(FileName, Skeleton.get());
	ResourceSkeletons.insert( { ReferenceName, move(Skeleton) } );
}

void LAssetManager::LoadShader(string FileName, string ReferenceName)
{
	auto Shader = make_unique<LShader>();
	LAssetDataLoader::LoadShaderFromeFile(FileName, Shader.get());
	ResourceShaders.insert({ ReferenceName, move(Shader) });
}

void LAssetManager::LoadTexture(std::string FileName, string ReferenceName)
{
	auto Texture = make_unique<LTexture>();
	LAssetDataLoader::LoadDDSTextureFromFile(FileName, Texture.get());
	ResourceTexs.insert( { ReferenceName, move(Texture) } );
}

void LAssetManager::LoadMaterial(std::string FileName, string ReferenceName)
{
	auto Material = make_unique<LMaterial>();
	LAssetDataLoader::LoadMaterial(FileName, Material.get());
	ResourceMaterials.insert( { ReferenceName, move(Material) } );
}

void LAssetManager::LoadMaterialInstance(std::string FileName, string ReferenceName)
{
	auto MaterialIns = make_unique<LMaterialInstance>();
	LAssetDataLoader::LoadMaterialInstance(FileName, MaterialIns.get());
	ResourceMaterials.insert({ ReferenceName, move(MaterialIns) });
}

void LAssetManager::LoadAnimations(std::string FileName, string ReferenceName)
{
	auto AnimSequence = make_unique<LAnimationSequence>();
	LAssetDataLoader::LoadAnimationSquence(FileName, AnimSequence.get());
	ResourceAnims.insert({ ReferenceName, move(AnimSequence)});
}

void LAssetManager::LoadStaticMeshGeometry(std::string FileName, string ReferenceName)
{
	auto StaticMeshBuffer = make_unique<LStaticMeshBuffer>();
	LAssetDataLoader::LoadMeshFromFile(FileName, StaticMeshBuffer.get());
	ResourceMeshBuffers.insert({ ReferenceName, move(StaticMeshBuffer) });
}

void LAssetManager::LoadSkeletalMeshGeometry(std::string FileName, string ReferenceName)
{
	auto SkeMeshBuffer = make_unique<LSkeletalMeshBuffer>();
	LAssetDataLoader::LoadSkeletalMeshVertexDataFromFile(FileName, SkeMeshBuffer.get());
	ResourceSkeletalMeshBuffers.insert({ ReferenceName, move(SkeMeshBuffer) });
}
