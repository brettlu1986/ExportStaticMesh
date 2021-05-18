
#include "stdafx.h"
#include "LAssetDataLoader.h"
#include "SampleAssets.h"
#include "LCharacter.h"
#include "LAnimator.h"
#include "FRHI.h"

using namespace std;

inline void GetAssetsPath(_Out_writes_(PathSize) WCHAR* Path, UINT PathSize)
{
	if (Path == nullptr)
	{
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, Path, PathSize);
	if (size == 0 || size == PathSize)
	{
		// Method failed or path was truncated.
		throw std::exception();
	}

	WCHAR* LastSlash = wcsrchr(Path, L'\\');
	if (LastSlash)
	{
		*(LastSlash + 1) = L'\0';
	}
}

inline std::string GetSaveDirectory()
{
	char CurPath[512];
	GetCurrentDirectoryA(_countof(CurPath), CurPath);
	std::string Path = CurPath;
	return Path + "\\Save\\";
}

std::wstring LAssetDataLoader::GetAssetFullPath(LPCWSTR AssetName)
{
	std::wstring AssetsPath;
	WCHAR AssetsPathChar[512];
	GetAssetsPath(AssetsPathChar, _countof(AssetsPathChar));
	AssetsPath = AssetsPathChar;
	return AssetsPath + AssetName;
}



void LAssetDataLoader::LoadMeshVertexDataFromFile(std::string FileName, FMesh* Mesh)
{
	FVertexBuffer* VertexBuffer = Mesh->GetVertexBuffer();
	FIndexBuffer* IndexBuffer = Mesh->GetIndexBuffer();

	std::string FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}
	XMFLOAT3 Location;
	Rf.read((char*)&Location, sizeof(XMFLOAT3));
	Mesh->SetModelLocation(Location);

	XMFLOAT3 Rotation;
	Rf.read((char*)&Rotation, sizeof(XMFLOAT3));
	Mesh->SetModelRotation(Rotation);

	XMFLOAT3 Scale;
	Rf.read((char*)&Scale, sizeof(XMFLOAT3));
	Mesh->SetModelScale(Scale);

	UINT VertexCount;
	Rf.read((char*)&VertexCount, sizeof(UINT));

	std::vector<FVertexData> VertexDatas;
	VertexDatas.resize(VertexCount);
	Rf.read((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData));
	VertexBuffer->Init((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData), VertexCount);

	bool bUseHalfInt32;
	Rf.read((char*)&bUseHalfInt32, sizeof(UINT8));

	UINT IndicesCount;
	Rf.read((char*)&IndicesCount, sizeof(UINT));

	vector<UINT> Indices;
	vector<UINT16> IndicesHalf;
	UINT IndicesByteSize = bUseHalfInt32 ? IndicesCount * sizeof(UINT16) :
		IndicesCount * sizeof(UINT32);

	if (bUseHalfInt32)
	{
		IndicesHalf.resize(IndicesCount);
		Rf.read((char*)IndicesHalf.data(), IndicesByteSize);
	}
	else
	{
		Indices.resize(IndicesCount);
		Rf.read((char*)Indices.data(), IndicesByteSize);
	}

	IndexBuffer->Init(IndicesCount, IndicesByteSize,
		bUseHalfInt32 ? E_INDEX_TYPE::TYPE_UINT_16 : E_INDEX_TYPE::TYPE_UINT_32,
		bUseHalfInt32 ? reinterpret_cast<void*>(IndicesHalf.data()) : reinterpret_cast<void*>(Indices.data()));

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadCameraDataFromFile(std::string FileName, LCamera& Camera)
{
	FCameraData& CameraDatas = Camera.GetCameraData();
	std::string FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	char* CameraBuffer = new char[sizeof(FCameraData)];
	memset(CameraBuffer, '\0', sizeof(FCameraData));
	Rf.read(CameraBuffer, sizeof(FCameraData));
	CameraDatas = *(FCameraData*)(CameraBuffer);

	delete[] CameraBuffer;
	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadSceneLights(std::string FileName, FScene* Scene)
{
	std::string FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT DirectionLightSize;
	Rf.read((char*)&DirectionLightSize, sizeof(UINT));
	
	std::vector<DirectionLightData> DLights;
	DLights.resize(DirectionLightSize);
	Rf.read((char*)DLights.data(), DirectionLightSize * sizeof(DirectionLightData));

	for(UINT i = 0; i < DirectionLightSize; i++)
	{
		FLight* Light = new FLight();
		Light->Direction = DLights[i].Direction;
		Light->Position = DLights[i].Position;
		Light->Strength = DLights[i].Strength;
		Light->LightIndex = i;
		Scene->AddLightToScene(Light);
	}
}

void LAssetDataLoader::LoadSkeletalMeshVertexDataFromFile(std::string FileName, FSkeletalMesh* SkeletalMesh, LSkeleton* Skeleton)
{
	std::string FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}
	XMFLOAT3 Location;
	Rf.read((char*)&Location, sizeof(XMFLOAT3));
	SkeletalMesh->SetModelLocation(Location);

	XMFLOAT3 Rotation;
	Rf.read((char*)&Rotation, sizeof(XMFLOAT3));
	SkeletalMesh->SetModelRotation(Rotation);

	XMFLOAT3 Scale;
	Rf.read((char*)&Scale, sizeof(XMFLOAT3));
	SkeletalMesh->SetModelScale(Scale);

	UINT VertexCount;
	Rf.read((char*)&VertexCount, sizeof(UINT));

	std::vector<FSkeletalVertexData> VertexDatas;
	VertexDatas.resize(VertexCount);
	Rf.read((char*)VertexDatas.data(), VertexCount * sizeof(FSkeletalVertexData));

	FVertexBuffer* VBuffer = GRHI->RHICreateVertexBuffer();
	VBuffer->Init((char*)VertexDatas.data(), VertexCount * sizeof(FSkeletalVertexData), VertexCount, true);

	UINT IndicesCount;
	Rf.read((char*)&IndicesCount, sizeof(UINT));

	vector<UINT32> Indices;
	Indices.resize(IndicesCount);
	Rf.read((char*)Indices.data(), IndicesCount * sizeof(UINT32));

	FIndexBuffer* IBuffer = GRHI->RHICreateIndexBuffer();
	IBuffer->Init(IndicesCount, IndicesCount * sizeof(UINT32), E_INDEX_TYPE::TYPE_UINT_32, reinterpret_cast<void*>(Indices.data()));

	SkeletalMesh->SetVertexAndIndexBuffer(VBuffer, IBuffer);

	std::vector<UINT16> BoneMap;

	UINT BoneMapSize;
	Rf.read((char*)&BoneMapSize, sizeof(UINT));

	BoneMap.resize(BoneMapSize);
	Rf.read((char*)BoneMap.data(), BoneMapSize * sizeof(UINT16));

	Skeleton->SetCurrentBoneMap(BoneMap);
}

void LAssetDataLoader::LoadSkeletonFromFile(std::string FileName, LSkeleton* Skeleton)
{
	std::string FName = GetSaveDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT BoneNum;
	Rf.read((char*)&BoneNum, sizeof(UINT));
	Skeleton->SetBoneCount(BoneNum);

	std::vector<BoneInfo> BoneInfos;
	BoneInfos.resize(BoneNum);
	Rf.read((char*)BoneInfos.data(), BoneNum * sizeof(BoneInfo));
	for(size_t i = 0; i < BoneInfos.size(); ++i)
	{
		LBoneInfo Info;
		Info.BoneName = BoneInfos[i].Name;
		Info.ParentIndex = BoneInfos[i].ParentIndex;
		Skeleton->AddBoneInfo(Info);
	}

	UINT BonePoseNum;
	Rf.read((char*)&BonePoseNum, sizeof(UINT));
	std::vector<BonePose> BonePoses;
	BonePoses.resize(BonePoseNum);
	Rf.read((char*)BonePoses.data(), BonePoseNum * sizeof(BonePose));
	for (size_t i = 0; i < BonePoses.size(); ++i)
	{
		LBonePose Pose;
		Pose.Scale = BonePoses[i].Scale;
		XMFLOAT3 Rot = { -BonePoses[i].Rotate.x, BonePoses[i].Rotate.y, -BonePoses[i].Rotate.z };
		Pose.Quat = MathHelper::EulerToQuaternion(Rot);
		Pose.Translate = BonePoses[i].Translate;
		Skeleton->AddBonePose(Pose);
	}
	Skeleton->CalculateRefPoseGlobalToLocal();

	UINT BoneNameIdxNum;
	Rf.read((char*)&BoneNameIdxNum, sizeof(UINT));
	std::vector<BoneNameIndex> BoneNameIdxs;
	BoneNameIdxs.resize(BoneNameIdxNum);
	Rf.read((char*)BoneNameIdxs.data(), BoneNameIdxNum * sizeof(BoneNameIndex));
	for (size_t i = 0; i < BoneNameIdxs.size(); ++i)
	{
		Skeleton->SetNameToIndexValue(BoneNameIdxs[i].Name, BoneNameIdxs[i].Index);
	}
}

void LAssetDataLoader::LoadAnimationSquence(std::string SequenceName, LAnimationSequence& Seq)
{
	std::string FName = GetSaveDirectory() + SequenceName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT FrameNum;
	Rf.read((char*)&FrameNum, sizeof(UINT));
	Seq.SetFrameCount(FrameNum);

	UINT TrackNum;
	Rf.read((char*)&TrackNum, sizeof(UINT));

	for(UINT i = 0; i < TrackNum; i++)
	{
		LAnimationTrack Track;

		UINT Num;
		Rf.read((char*)&Num, sizeof(UINT));
		std::vector<XMFLOAT3> Poss;
		Poss.resize(Num);
		Rf.read((char*)Poss.data(), Num * sizeof(XMFLOAT3));
		for(size_t i = 0; i < Poss.size(); ++i)
		{
			LAnimationTranslateChannel T;
			T.Translate = Poss[i];
			Track.AddTranslateChannelFrame(T);
		}

		Rf.read((char*)&Num, sizeof(UINT));
		std::vector<XMFLOAT3> Rots;
		Rots.resize(Num);
		Rf.read((char*)Rots.data(), Num * sizeof(XMFLOAT3));
		for (size_t i = 0; i < Rots.size(); ++i)
		{
			LAnimationQuatChannel Q;
			Q.Rot = Rots[i];

			XMFLOAT3 Rot = { -Rots[i].x, Rots[i].y, -Rots[i].z };
			Q.Quat = MathHelper::EulerToQuaternion(Rot);
			Track.AddQuatChannelFrame(Q);
		}

		Rf.read((char*)&Num, sizeof(UINT));
		std::vector<XMFLOAT3> Scales;
		Scales.resize(Num);
		Rf.read((char*)Scales.data(), Num * sizeof(XMFLOAT3));
		for (size_t i = 0; i < Scales.size(); ++i)
		{
			LAnimationScaleChannel S;
			S.Scale = Scales[i];
			Track.AddScaleChannelFrame(S);
		}

		Rf.read((char*)&Num, sizeof(UINT));
		Track.TrackToBoneIndex = Num;
		Seq.AddAnimationTrack(Track);
	}
}

void LAssetDataLoader::LoadSampleScene(FScene* Scene)
{
	for (UINT i = 0; i < SampleAssets::SamepleCount; i++)
	{
		FMesh* Mesh = new FMesh(SampleAssets::SampleResources[i], SampleAssets::SampleResourceTexture[i],
			SampleAssets::SampleResourceTexture[i] != "" ? SampleAssets::PsoUseTexture : SampleAssets::PsoNoTexture);

		Mesh->InitMaterial(SampleAssets::SampeMats[i].Name, SampleAssets::SampeMats[i].DiffuseAlbedo, SampleAssets::SampeMats[i].FresnelR0,
			SampleAssets::SampeMats[i].Roughness);
		Scene->AddMeshToScene(Mesh);
	}

	for(UINT i = 0; i < SampleAssets::SampleSkeletalMeshCount; i++)
	{
		LCharacter* Character = new LCharacter();
		FSkeletalMesh* SkeletalMesh = new FSkeletalMesh();
		LSkeleton* Skeleton = new LSkeleton();
		LoadSkeletalMeshVertexDataFromFile(SampleAssets::SkeletalMeshResource[i], SkeletalMesh, Skeleton);
		LoadSkeletonFromFile(SampleAssets::SkeletonResource, Skeleton);
		SkeletalMesh->SetSkeleton(Skeleton);

		FTexture* Tex = GRHI->RHICreateTexture();
		Tex->InitializeTexture(SampleAssets::SkeletonTexture);
		SkeletalMesh->SetDiffuseTexture(Tex);

		LAnimator* Animator = new LAnimator();
		for(UINT i = 0; i< SampleAssets::SkeletalAnimCount; ++i)
		{
			LAnimationSequence AnimSequence;
			LoadAnimationSquence(SampleAssets::SkeletalAnim[i], AnimSequence);
			Animator->AddAnimSequence(SampleAssets::SkeletalAnimName[i], AnimSequence);
		}
		Animator->SetSkeleton(Skeleton);

		Character->SetSkeletalMesh(SkeletalMesh);
		Character->SetAnimator(Animator);
		Scene->AddCharacterToScene(Character);
	}

	LoadSceneLights(SampleAssets::SceneLightsFile, Scene);

	LCamera& Camera = Scene->GetCamera();
	LoadCameraDataFromFile(SampleAssets::CameraBin, Camera);
	Camera.Init();
}