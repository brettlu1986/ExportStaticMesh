
#include "pch.h"
#include "LAssetDataLoader.h"
#include "LCharacter.h"
#include "LAnimator.h"
#include "FRHI.h"
#include "LDefine.h"
#include "LSceneCamera.h"
#include "LThirdPersonCamera.h"
#include "LLog.h"

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
	return Path + "\\Resource\\";
}

wstring LAssetDataLoader::GetAssetFullPath(LPCWSTR AssetName)
{
	wstring AssetsPath;
	WCHAR AssetsPathChar[512];
	GetAssetsDataPath(AssetsPathChar, _countof(AssetsPathChar));
	AssetsPath = AssetsPathChar;
	return AssetsPath + AssetName;
}

void LAssetDataLoader::LoadMap(string MapFile, vector<LMapStaticObjInfo>& MapStaticObjInfos, vector<LMapSkeletalObjInfo>& MapSkeletalObjInfos, LCameraData& CameraData, vector<LDirectionLightData>& LightsData)
{
	string FName = GetSaveDataDirectory() + MapFile;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT Num;
	Rf >> Num;
	for(UINT i = 0; i < Num; i++)
	{
		LMapStaticObjInfo StaticInfo;
		Rf >> StaticInfo.ObjectName >> StaticInfo.RefGeometry >> StaticInfo.RefMaterial >> 
		StaticInfo.WorldLocation.x >> StaticInfo.WorldLocation.y >> StaticInfo.WorldLocation.z >>
		StaticInfo.WorldRotator.x >> StaticInfo.WorldRotator.y >> StaticInfo.WorldRotator.z >>
		StaticInfo.WorldScale.x >> StaticInfo.WorldScale.y >> StaticInfo.WorldScale.z;
		MapStaticObjInfos.push_back(StaticInfo);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		LMapSkeletalObjInfo SkeletalInfo;
		Rf >> SkeletalInfo.ObjectName >> SkeletalInfo.RefGeometry >> SkeletalInfo.RefMaterial >> SkeletalInfo.RefSkeleton ;
		UINT AnimNum;
		Rf >> AnimNum;
		SkeletalInfo.RefAnims.resize(AnimNum);
		for(UINT j = 0; j < AnimNum; j++)
		{
			Rf >> SkeletalInfo.RefAnims[j];
		}
		Rf >> SkeletalInfo.WorldLocation.x >> SkeletalInfo.WorldLocation.y >> SkeletalInfo.WorldLocation.z 
		>> SkeletalInfo.WorldRotator.x >> SkeletalInfo.WorldRotator.y >> SkeletalInfo.WorldRotator.z 
		>> SkeletalInfo.WorldScale.x >> SkeletalInfo.WorldScale.y >> SkeletalInfo.WorldScale.z;
		
		SkeletalInfo.WorldRotator.y -= 90.f;

		MapSkeletalObjInfos.push_back(SkeletalInfo);
	}
	string LightsFile;
	Rf >> LightsFile;
	LoadDirectionLights(LightsFile, LightsData);

	string CameraFile;
	Rf >> CameraFile;
	LoadCameraDataFromFile(CameraFile, CameraData);
}

void LAssetDataLoader::LoadAssetsFromFile(string FileName, vector<LAssetDef>& Skeletons, vector<LAssetDef>& ShaderFiles, vector<LAssetDef>& Textures, vector<LAssetDef>& Materials, vector<LAssetDef>& MaterialInstances, vector<LAssetDef>& Animations, vector<LAssetDef>& MeshGeometries, vector<LAssetDef>& SkeletalMeshGeometries)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT Num; 
	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		LAssetDef Def;
		Rf >> Def.RefName >> Def.FileName;
		Skeletons.push_back(Def);
	}
	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		LAssetDef Def;
		Rf >> Def.RefName >> Def.FileName;
		ShaderFiles.push_back(Def);
	}
	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		LAssetDef Def;
		Rf >> Def.RefName >> Def.FileName;
		Textures.push_back(Def);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		LAssetDef Def;
		Rf >> Def.RefName >> Def.FileName;
		Materials.push_back(Def);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		LAssetDef Def;
		Rf >> Def.RefName >> Def.FileName;
		MaterialInstances.push_back(Def);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		LAssetDef Def;
		Rf >> Def.RefName >> Def.FileName;
		Animations.push_back(Def);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		LAssetDef Def;
		Rf >> Def.RefName >> Def.FileName;
		MeshGeometries.push_back(Def);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		LAssetDef Def;
		Rf >> Def.RefName >> Def.FileName;
		SkeletalMeshGeometries.push_back(Def);
	}

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadCameraDataFromFile(string FileName, LCameraData& CameraData)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	char* CameraBuffer = new char[sizeof(LCameraData)];
	memset(CameraBuffer, '\0', sizeof(LCameraData));
	Rf.read(CameraBuffer, sizeof(LCameraData));
	CameraData = *(LCameraData*)(CameraBuffer);

	delete[] CameraBuffer;

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadDirectionLights(string FileName,  vector<LDirectionLightData>& LightsData)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT DirectionLightSize;
	Rf.read((char*)&DirectionLightSize, sizeof(UINT));
	
	LightsData.resize(DirectionLightSize);
	Rf.read((char*)LightsData.data(), DirectionLightSize * sizeof(LDirectionLightData));

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadSkeletalMeshVertexDataFromFile(string FileName, LSkeletalMeshBuffer* SkeletalMeshBuffer)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT VertexCount;
	Rf.read((char*)&VertexCount, sizeof(UINT));

	vector<FSkeletalVertexData> VertexDatas;
	VertexDatas.resize(VertexCount);
	Rf.read((char*)VertexDatas.data(), VertexCount * sizeof(FSkeletalVertexData));

	SkeletalMeshBuffer->VertexBufferData = new LVertexBuffer((char*)VertexDatas.data(), VertexCount * sizeof(FSkeletalVertexData), VertexCount);

	UINT IndicesCount;
	Rf.read((char*)&IndicesCount, sizeof(UINT));

	vector<UINT32> Indices;
	Indices.resize(IndicesCount);
	Rf.read((char*)Indices.data(), IndicesCount * sizeof(UINT32));

	SkeletalMeshBuffer->IndexBufferData = new LIndexBuffer(IndicesCount, IndicesCount * sizeof(UINT32),
		E_INDEX_TYPE::TYPE_UINT_32, reinterpret_cast<void*>(Indices.data()));

	UINT BoneMapSize;
	Rf.read((char*)&BoneMapSize, sizeof(UINT));
	SkeletalMeshBuffer->BoneMap.resize(BoneMapSize);
	Rf.read((char*)SkeletalMeshBuffer->BoneMap.data(), BoneMapSize * sizeof(UINT16));

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
		Vec3 Rot = { -BonePoses[i].Rotate.x, BonePoses[i].Rotate.y, -BonePoses[i].Rotate.z };
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
	string FNameC = GetSaveDataDirectory() + FileName;

	size_t Len = strlen(FNameC.c_str()) + 1;
	size_t Converted = 0;
	wchar_t* WStr = new wchar_t[(UINT)(Len * sizeof(wchar_t))];
	memset(WStr, 0, Len * sizeof(wchar_t));
	mbstowcs_s(&Converted, WStr, Len, FNameC.c_str(), _TRUNCATE);

	DirectX::LoadTextureDataFromFile(WStr, Texture->DdsData, &Texture->Header, &Texture->BitData, &Texture->BitSize);
	delete[] WStr;
}

void LAssetDataLoader::LoadShaderFromeFile(string FileName, LShader* Shader)
{
	size_t Len = strlen(FileName.c_str()) + 1;
	size_t Converted = 0;
	wchar_t* WStr = new wchar_t[(UINT)(Len * sizeof(wchar_t))];
	memset(WStr, 0, Len * sizeof(wchar_t));
	mbstowcs_s(&Converted, WStr, Len, FileName.c_str(), _TRUNCATE);

	UINT8* ShaderData = nullptr;
	UINT ShaderLen = 0;
	ReadDataFromFile(LAssetDataLoader::GetAssetFullPath(WStr).c_str(), &ShaderData, &ShaderLen);
	Shader->Init(ShaderData, ShaderLen);
}

void LAssetDataLoader::LoadMaterial(string FileName, LMaterial* Material)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	string Ignore;
	Rf >> Ignore;

	LBlendMode BlendMode;
	UINT ModeValue;
	Rf >> BlendMode.BlendModeName;
	Rf >> ModeValue;
	BlendMode.BlendModeValue = static_cast<E_BLEND_MODE>(ModeValue);
	Material->SetBlendMode(BlendMode);

	UINT Num;
	Rf >> Num;
	for(UINT i = 0; i < Num; i++)
	{
		UINT8 Type;
		Rf >> Type;
		Material->AddParamType(static_cast<E_MATERIAL_PARAM_TYPE>(Type));
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		float Value;
		Rf >> Value;
		Material->AddParamFloat(Value);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		XMFLOAT4 Value;
		Rf >> Value.x >> Value.y >> Value.z >> Value.w;
		Material->AddParamVector(Value);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		string RefName;
		Rf >> RefName;
		Material->AddParamTexture(LAssetManager::Get()->GetTexture(RefName));
	}

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadMaterialInstance(string FileName, LMaterialInstance* MaterialIns)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	string ParentName;
	Rf >> ParentName;
	LMaterial* MatTemplate = dynamic_cast<LMaterial*>(LAssetManager::Get()->GetMaterial(ParentName));
	MaterialIns->InitMaterialTemplate(MatTemplate);

	LBlendMode BlendMode;
	UINT ModeValue;
	Rf >> BlendMode.BlendModeName;
	Rf >> ModeValue;
	BlendMode.BlendModeValue = static_cast<E_BLEND_MODE>(ModeValue);
	MaterialIns->SetBlendMode(BlendMode);

	UINT Num;
	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		UINT8 Type;
		Rf >> Type;
		//instance not need to add type, just update value
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		float Value;
		Rf >> Value;
		MaterialIns->SetParamFloat(i, Value);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		XMFLOAT4 Value;
		Rf >> Value.x >> Value.y >> Value.z >> Value.w;
		MaterialIns->SetParamVector(i, Value);
	}

	Rf >> Num;
	for (UINT i = 0; i < Num; i++)
	{
		string RefName;
		Rf >> RefName;
		MaterialIns->SetParamTexture(i, LAssetManager::Get()->GetTexture(RefName));
	}

}

void LAssetDataLoader::LoadAnimationSquence(string SequenceName, LAnimationSequence* Seq)
{
	string FName = GetSaveDataDirectory() + SequenceName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT FrameNum;
	Rf.read((char*)&FrameNum, sizeof(UINT));
	Seq->SetFrameCount(FrameNum);

	for(UINT i = 0; i< FrameNum; i++)
	{
		Seq->GetFrameTimes().push_back(i * AnimFrameRate);
	}

	UINT TrackNum;
	Rf.read((char*)&TrackNum, sizeof(UINT));

	for(UINT i = 0; i < TrackNum; i++)
	{
		LAnimationTrack Track;

		UINT Num;
		Rf.read((char*)&Num, sizeof(UINT));
		vector<Vec3> Poss;
		Poss.resize(Num);
		Rf.read((char*)Poss.data(), Num * sizeof(Vec3));
		for(size_t i = 0; i < Poss.size(); ++i)
		{
			Track.AddTranslateChannelFrame(Poss[i]);
		}

		Rf.read((char*)&Num, sizeof(UINT));
		vector<Vec3> Rots;
		Rots.resize(Num);
		Rf.read((char*)Rots.data(), Num * sizeof(Vec3));
		for (size_t i = 0; i < Rots.size(); ++i)
		{
			Vec3 Rot = { -Rots[i].x, Rots[i].y, -Rots[i].z };
			XMFLOAT4 Quat = MathHelper::EulerToQuaternion(Rot);
			Track.AddQuatChannelFrame(Quat);
		}

		Rf.read((char*)&Num, sizeof(UINT));
		vector<Vec3> Scales;
		Scales.resize(Num);
		Rf.read((char*)Scales.data(), Num * sizeof(Vec3));
		for (size_t i = 0; i < Scales.size(); ++i)
		{
			Track.AddScaleChannelFrame(Scales[i]);
		}

		Rf.read((char*)&Num, sizeof(UINT));
		Track.TrackToBoneIndex = Num;
		Seq->AddAnimationTrack(Track);
	}

	Seq->GetCurrentAnimPoseToParentTrans().resize(Seq->GetSequenceTracks().size());
	Rf.close();
	if (!Rf.good()) {
		return;
	}
}

void LAssetDataLoader::LoadMeshFromFile(string FileName, LStaticMeshBuffer* MeshBuffer)
{
	string FName = GetSaveDataDirectory() + FileName;
	ifstream Rf(FName, ios::out | ios::binary);
	if (!Rf) {
		return;
	}

	UINT VertexCount;
	Rf.read((char*)&VertexCount, sizeof(UINT));
	vector<FVertexData> VertexDatas;
	VertexDatas.resize(VertexCount);
	Rf.read((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData));

	MeshBuffer->VertexBufferData = new LVertexBuffer((char*)VertexDatas.data(), VertexCount * sizeof(FVertexData), VertexCount);

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

	MeshBuffer->IndexBufferData = new LIndexBuffer(IndicesCount, IndicesByteSize,
		bUseHalfInt32 ? E_INDEX_TYPE::TYPE_UINT_16 : E_INDEX_TYPE::TYPE_UINT_32,
		bUseHalfInt32 ? reinterpret_cast<void*>(IndicesHalf.data()) : reinterpret_cast<void*>(Indices.data()));

	Rf.close();
	if (!Rf.good()) {
		return;
	}
}