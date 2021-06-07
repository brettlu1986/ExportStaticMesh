
#include "stdafx.h"
#include "LAssetDataLoader.h"
#include "SampleAssets.h"
#include "LCharacter.h"
#include "LAnimator.h"
#include "FRHI.h"
#include "LDefine.h"
#include "LSceneCamera.h"
#include "LThirdPersonCamera.h"

#include "FD3D12Helper.h"

inline void GetAssetsDataPath(_Out_writes_(PathSize) WCHAR* Path, UINT PathSize)
{
	if (Path == nullptr)
	{
		throw exception();
	}

	DWORD size = GetModuleFileName(nullptr, Path, PathSize);
	if (size == 0 || size == PathSize)
	{
		// Method failed or path was truncated.
		throw exception();
	}

	WCHAR* LastSlash = wcsrchr(Path, L'\\');
	if (LastSlash)
	{
		*(LastSlash + 1) = L'\0';
	}
}

inline string GetSaveDataDirectory()
{
	char CurPath[512];
	GetCurrentDirectoryA(_countof(CurPath), CurPath);
	string Path = CurPath;
	return Path + "\\Save\\";
}

wstring LAssetDataLoader::GetAssetFullPath(LPCWSTR AssetName)
{
	wstring AssetsPath;
	WCHAR AssetsPathChar[512];
	GetAssetsDataPath(AssetsPathChar, _countof(AssetsPathChar));
	AssetsPath = AssetsPathChar;
	return AssetsPath + AssetName;
}

void LAssetDataLoader::LoadCameraDataFromFile(string FileName, LCamera& Camera)
{
	FCameraData CameraData;
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	char* CameraBuffer = new char[sizeof(FCameraData)];
	memset(CameraBuffer, '\0', sizeof(FCameraData));
	Rf.read(CameraBuffer, sizeof(FCameraData));
	CameraData = *(FCameraData*)(CameraBuffer);

	Camera.SetCameraData(CameraData);
	Camera.Init();
	delete[] CameraBuffer;

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadDirectionLights(string FileName,  vector<DirectionLightData>& LightsData)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT DirectionLightSize;
	Rf.read((char*)&DirectionLightSize, sizeof(UINT));
	
	LightsData.resize(DirectionLightSize);
	Rf.read((char*)LightsData.data(), DirectionLightSize * sizeof(DirectionLightData));

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadSkeletalMeshVertexDataFromFile(string FileName, LSkeletalMesh& SkeletalMesh, XMFLOAT3& Location, XMFLOAT3& Rotation, XMFLOAT3& Scale)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}
	Rf.read((char*)&Location, sizeof(XMFLOAT3));
	Rf.read((char*)&Rotation, sizeof(XMFLOAT3));
	Rotation.y -= 90.f;
	Rf.read((char*)&Scale, sizeof(XMFLOAT3));

	UINT VertexCount;
	Rf.read((char*)&VertexCount, sizeof(UINT));

	vector<FSkeletalVertexData> VertexDatas;
	VertexDatas.resize(VertexCount);
	Rf.read((char*)VertexDatas.data(), VertexCount * sizeof(FSkeletalVertexData));
	SkeletalMesh.SetVertexBufferInfo((char*)VertexDatas.data(), VertexCount * sizeof(FSkeletalVertexData), VertexCount);

	UINT IndicesCount;
	Rf.read((char*)&IndicesCount, sizeof(UINT));

	vector<UINT32> Indices;
	Indices.resize(IndicesCount);
	Rf.read((char*)Indices.data(), IndicesCount * sizeof(UINT32));

	SkeletalMesh.SetIndexBufferInfo(IndicesCount, IndicesCount * sizeof(UINT32),
		E_INDEX_TYPE::TYPE_UINT_32, reinterpret_cast<void*>(Indices.data()));

	vector<UINT16> BoneMap;
	UINT BoneMapSize;
	Rf.read((char*)&BoneMapSize, sizeof(UINT));
	BoneMap.resize(BoneMapSize);
	Rf.read((char*)BoneMap.data(), BoneMapSize * sizeof(UINT16));
	SkeletalMesh.SetCurrentBoneMap(BoneMap);

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadSkeletonFromFile(string FileName, LSkeleton* Skeleton)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT BoneNum;
	Rf.read((char*)&BoneNum, sizeof(UINT));
	Skeleton->SetBoneCount(BoneNum);

	vector<BoneInfo> BoneInfos;
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
	vector<BonePose> BonePoses;
	BonePoses.resize(BonePoseNum);
	Rf.read((char*)BonePoses.data(), BonePoseNum * sizeof(BonePose));
	for (size_t i = 0; i < BonePoses.size(); ++i)
	{
		LBonePose Pose;
		Pose.Scale = BonePoses[i].Scale;
						//Pitch                          Yaw                Roll
		XMFLOAT3 Rot = { -BonePoses[i].Rotate.x, BonePoses[i].Rotate.y, -BonePoses[i].Rotate.z };
		Pose.Quat = MathHelper::EulerToQuaternion(Rot);
		Pose.Translate = BonePoses[i].Translate;
		Skeleton->AddBonePose(Pose);
	}
	Skeleton->CalculateRefPoseGlobalToLocal();

	UINT BoneNameIdxNum;
	Rf.read((char*)&BoneNameIdxNum, sizeof(UINT));
	vector<BoneNameIndex> BoneNameIdxs;
	BoneNameIdxs.resize(BoneNameIdxNum);
	Rf.read((char*)BoneNameIdxs.data(), BoneNameIdxNum * sizeof(BoneNameIndex));
	for (size_t i = 0; i < BoneNameIdxs.size(); ++i)
	{
		Skeleton->SetNameToIndexValue(BoneNameIdxs[i].Name, BoneNameIdxs[i].Index);
	}
	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadDDSTextureFromFile(string FileName, LTexture* Texture)
{
	size_t Len = strlen(FileName.c_str()) + 1;
	size_t Converted = 0;
	wchar_t* WStr = new wchar_t[(UINT)(Len * sizeof(wchar_t))];
	memset(WStr, 0, Len * sizeof(wchar_t));
	mbstowcs_s(&Converted, WStr, Len, FileName.c_str(), _TRUNCATE);

	DirectX::LoadTextureDataFromFile(WStr, Texture->DdsData, &Texture->Header, &Texture->BitData, &Texture->BitSize);
	delete[] WStr;
}

void LAssetDataLoader::LoadShaderFromeFile(LPCWSTR FileName, LShader* Shader)
{
	UINT8* ShaderData = nullptr;
	UINT ShaderLen = 0;
	ReadDataFromFile(LAssetDataLoader::GetAssetFullPath(FileName).c_str(), &ShaderData, &ShaderLen);
	Shader->Init(ShaderData, ShaderLen);
}

void LAssetDataLoader::LoadAnimationSquence(string SequenceName, LAnimationSequence& Seq)
{
	string FName = GetSaveDataDirectory() + SequenceName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT FrameNum;
	Rf.read((char*)&FrameNum, sizeof(UINT));
	Seq.SetFrameCount(FrameNum);

	for(UINT i = 0; i< FrameNum; i++)
	{
		Seq.GetFrameTimes().push_back(i * AnimFrameRate);
	}

	UINT TrackNum;
	Rf.read((char*)&TrackNum, sizeof(UINT));

	for(UINT i = 0; i < TrackNum; i++)
	{
		LAnimationTrack Track;

		UINT Num;
		Rf.read((char*)&Num, sizeof(UINT));
		vector<XMFLOAT3> Poss;
		Poss.resize(Num);
		Rf.read((char*)Poss.data(), Num * sizeof(XMFLOAT3));
		for(size_t i = 0; i < Poss.size(); ++i)
		{
			Track.AddTranslateChannelFrame(Poss[i]);
		}

		Rf.read((char*)&Num, sizeof(UINT));
		vector<XMFLOAT3> Rots;
		Rots.resize(Num);
		Rf.read((char*)Rots.data(), Num * sizeof(XMFLOAT3));
		for (size_t i = 0; i < Rots.size(); ++i)
		{
			XMFLOAT3 Rot = { -Rots[i].x, Rots[i].y, -Rots[i].z };
			XMFLOAT4 Quat = MathHelper::EulerToQuaternion(Rot);
			Track.AddQuatChannelFrame(Quat);
		}

		Rf.read((char*)&Num, sizeof(UINT));
		vector<XMFLOAT3> Scales;
		Scales.resize(Num);
		Rf.read((char*)Scales.data(), Num * sizeof(XMFLOAT3));
		for (size_t i = 0; i < Scales.size(); ++i)
		{
			Track.AddScaleChannelFrame(Scales[i]);
		}

		Rf.read((char*)&Num, sizeof(UINT));
		Track.TrackToBoneIndex = Num;
		Seq.AddAnimationTrack(Track);
	}

	Seq.GetCurrentAnimPoseToParentTrans().resize(Seq.GetSequenceTracks().size());
	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadMeshFromFile(string FileName, LMesh& Mesh, XMFLOAT3& Location, XMFLOAT3& Rotation, XMFLOAT3& Scale)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	Rf.read((char*)&Location, sizeof(XMFLOAT3));
	Rf.read((char*)&Rotation, sizeof(XMFLOAT3));
	Rf.read((char*)&Scale, sizeof(XMFLOAT3));

	UINT VertexCount;
	Rf.read((char*)&VertexCount, sizeof(UINT));
	vector<FVertexData> VertexDatas;
	VertexDatas.resize(VertexCount);
	Rf.read((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData));
	Mesh.SetVertexBufferInfo((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData), VertexCount);

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
	Mesh.SetIndexBufferInfo(IndicesCount, IndicesByteSize,
		bUseHalfInt32 ? E_INDEX_TYPE::TYPE_UINT_16 : E_INDEX_TYPE::TYPE_UINT_32,
		bUseHalfInt32 ? reinterpret_cast<void*>(IndicesHalf.data()) : reinterpret_cast<void*>(Indices.data()));

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}