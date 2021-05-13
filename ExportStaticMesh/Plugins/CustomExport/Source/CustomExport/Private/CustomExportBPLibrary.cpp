// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomExportBPLibrary.h"
#include "CustomExport.h"
#include "Runtime/RawMesh/Public/RawMesh.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Internationalization/Internationalization.h"
#include "Components/DirectionalLightComponent.h"
#include "JsonObjectConverter.h"
#include "Misc/MessageDialog.h"

#include "Rendering/SkeletalMeshRenderData.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Character.h"

#include <iostream>
#include <fstream>

using namespace std;

static FString SavePath = FPaths::ProjectSavedDir() + "Export/";


const FString Success = "Success";
const FString Failed = "Fail";
const uint32 LOD_LEVEL = 0;
const uint32 MAX_UINT16 = 65535;
const float POSITION_SCALE = 0.01f;
const float CameraTargetOffset = 1300.f;

UCustomExportBPLibrary::UCustomExportBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void CheckFileExistAndDelete(const FString& FileName)
{
	IFileManager& FileManager = IFileManager::Get();
	if (FileManager.DirectoryExists(*SavePath))
	{
		TArray<FString> DataFileNames;
		FileManager.FindFilesRecursive(DataFileNames, *SavePath, *FileName, true, false, false);

		for (FString& DataFileName : DataFileNames)
		{
			FileManager.Delete(*DataFileName);
		}
	}
}

void GetValidVsFormat(TArray<bool>& VsFormat, const FStaticMeshVertexBuffers& VertexBuffer)
{
	const FPositionVertexBuffer& PositionBuffer = VertexBuffer.PositionVertexBuffer;
	VsFormat[(uint32)EVsFormat::POSITION] = PositionBuffer.GetNumVertices() > 0;
	const FStaticMeshVertexBuffer& MeshVertexBuffer = VertexBuffer.StaticMeshVertexBuffer;
	bool bValidNum = MeshVertexBuffer.GetNumVertices() > 0;
	VsFormat[(uint32)EVsFormat::NORMAL] = bValidNum;
	VsFormat[(uint32)EVsFormat::TANGENT] = bValidNum;
	VsFormat[(uint32)EVsFormat::TEX0] = bValidNum;
	VsFormat[(uint32)EVsFormat::TEX1] = bValidNum && MeshVertexBuffer.GetNumTexCoords() >= 2;
	const FColorVertexBuffer& ColorBuffers = VertexBuffer.ColorVertexBuffer;
	VsFormat[(uint32)EVsFormat::COLOR] = ColorBuffers.GetNumVertices() > 0;
}

void GetVertexPosition(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FFullMeshDataJson& MeshDataJson, FFullVertexDatas& MeshDataBin)
{
	if (ValidFormat[(uint32)EVsFormat::POSITION])
	{
		const FPositionVertexBuffer& PositionBuffer = VertexBuffer.PositionVertexBuffer;
		FVector Position = PositionBuffer.VertexPosition(Index);
		Position *= POSITION_SCALE;
		MeshDataJson.Positions.Append({ Position.X, Position.Y, Position.Z });
		MeshDataBin.Position = Position;
	}
}

void GetVertexNormal(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FFullMeshDataJson& MeshDataJson, FFullVertexDatas& MeshDataBin)
{
	if (ValidFormat[(uint32)EVsFormat::NORMAL])
	{
		const FStaticMeshVertexBuffer& VertexBuffers = VertexBuffer.StaticMeshVertexBuffer;
		const FVector& Normal = VertexBuffers.VertexTangentZ(Index);
		MeshDataJson.Normals.Append({ Normal.X, Normal.Y, Normal.Z});
		MeshDataBin.Normal = Normal;
	}
}

void GetVertexTangent(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FFullMeshDataJson& MeshDataJson, FFullVertexDatas& MeshDataBin)
{
	if (ValidFormat[(uint32)EVsFormat::TANGENT])
	{
		const FStaticMeshVertexBuffer& VertexBuffers = VertexBuffer.StaticMeshVertexBuffer;
		const FVector& Tangent = VertexBuffers.VertexTangentX(Index);
		MeshDataJson.Tangents.Append({ Tangent.X, Tangent.Y, Tangent.Z});
		MeshDataBin.Tangent = Tangent;
	}
}

void GetVertexTex(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FFullMeshDataJson& MeshDataJson, FFullVertexDatas& MeshDataBin)
{
	const FStaticMeshVertexBuffer& VertexBuffers = VertexBuffer.StaticMeshVertexBuffer;
	if (ValidFormat[(uint32)EVsFormat::TEX0])
	{
		const FVector2D& Texcoord0 = VertexBuffers.GetVertexUV(Index, 0);
		MeshDataJson.Tex0s.Append({ Texcoord0.X,Texcoord0.Y });
		MeshDataBin.Tex0 = Texcoord0;
	}

	if (ValidFormat[(uint32)EVsFormat::TEX1])
	{
		const FVector2D& Texcoord1 = VertexBuffers.GetVertexUV(Index, 1);
		MeshDataJson.Tex1s.Append({ Texcoord1.X,Texcoord1.Y });
		MeshDataBin.Tex1 = Texcoord1;
	}

}

void GetVertexColor(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FFullMeshDataJson& MeshDataJson, FFullVertexDatas& MeshDataBin)
{
	if (ValidFormat[(uint32)EVsFormat::COLOR])
	{
		const FColorVertexBuffer& ColorBuffers = VertexBuffer.ColorVertexBuffer;
		const FLinearColor& Color = ColorBuffers.VertexColor(Index);
		MeshDataJson.Colors.Append({ Color.R, Color.G, Color.B, Color.A });
		MeshDataBin.Color = Color;
	}
}

bool SaveMeshJsonToFile(const FMeshDataJson& MeshJson, FString& FileName)
{
	//save json
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(MeshJson, Json);
	FString MeshSaveFile = SavePath + FileName;
	return FFileHelper::SaveStringToFile(Json, *MeshSaveFile);
}

bool SaveMeshBinaryToFile(const FMeshDataBinary& MeshBinOut, const TArray<uint32>& Indices, const bool& bUseHalfInt32, FString& FileName)
{
	FString MeshSaveFile = SavePath + FileName;
	char* ResultName = TCHAR_TO_ANSI(*MeshSaveFile);
	ofstream Wf(ResultName, ios::out | ios::binary);
	if (!Wf)
	{
		return false;
	}

	Wf.write((char*)&(MeshBinOut.WorldLocation), sizeof(FVector));
	Wf.write((char*)&(MeshBinOut.WorldRotation), sizeof(FRotator));
	Wf.write((char*)&(MeshBinOut.WorldScale), sizeof(FVector));

	uint32  VertexsSize = MeshBinOut.MeshVertexDatas.Num();
	Wf.write((char*)&VertexsSize, sizeof(uint32));
	Wf.write((char*)MeshBinOut.MeshVertexDatas.GetData(), sizeof(FVertexDatas) * VertexsSize);

	Wf.write((char*)&bUseHalfInt32, sizeof(uint8));

	uint32 IndicesSize = Indices.Num();
	Wf.write((char*)&IndicesSize, sizeof(uint32));

	if (!bUseHalfInt32)
	{
		Wf.write((char*)Indices.GetData(), IndicesSize * sizeof(uint32));
	}
	else
	{
		char* IndicesBuffer = new char[sizeof(uint16) * IndicesSize];
		memset(IndicesBuffer, 0, sizeof(uint16) * IndicesSize);
		for(uint32 i = 0; i < IndicesSize; i++)
		{
			uint16 Value = static_cast<uint16>(Indices[i]);
			memcpy(IndicesBuffer + i * sizeof(uint16),&Value, sizeof(uint16));
		}
		Wf.write(IndicesBuffer, IndicesSize * sizeof(uint16));
		delete[] IndicesBuffer;
	}

	Wf.close();
	return Wf.good();
}

#define LOCTEXT_NAMESPACE "CustomExport"
void UCustomExportBPLibrary::ShowMessageDialog(const FText Target, const FText Result)
{
	const FText WarningMessage = FText::Format(LOCTEXT("CustomExport_Message", "{0} export {1}!"), Target, Result);
	EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::Ok, WarningMessage);
}
#undef LOCTEXT_NAMESPACE

void ExportMeshWorld(const AActor* MeshActor, FFullMeshDataJson& MeshDataJsonOut, FFullMeshDataBinary& MeshDataBinaryOut)
{
	FVector MeshLocation = MeshActor->GetActorLocation();
	const FRotator& MeshRotation = MeshActor->GetActorRotation();
	const FVector& MeshScale = MeshActor->GetActorScale();
	MeshLocation = MeshLocation * POSITION_SCALE;

	MeshDataJsonOut.WorldLocation.Append({ MeshLocation.X , MeshLocation.Y , MeshLocation.Z });
	MeshDataJsonOut.WorldRotation.Append({ MeshRotation.Pitch, MeshRotation.Yaw, MeshRotation.Roll });
	MeshDataJsonOut.WorldScale.Append({ MeshScale.X, MeshScale.Y, MeshScale.Z });

	MeshDataBinaryOut.WorldLocation = MeshLocation;
	MeshDataBinaryOut.WorldRotation = MeshRotation;
	MeshDataBinaryOut.WorldScale = MeshScale;
}

void ExportMeshVertexBuffer(const FStaticMeshVertexBuffers& VertexBuffers, FFullMeshDataJson& MeshDataJsonOut, FFullMeshDataBinary& MeshDataBinaryOut)
{
	TArray<bool> ValidFormat;
	ValidFormat.Init(false, (int32)EVsFormat::MAX);
	GetValidVsFormat(ValidFormat, VertexBuffers);
	ValidFormat[(uint32)EVsFormat::SKINWEIGHT] = false;

	//insert format 
	UEnum* Enum = StaticEnum<EVsFormat>();
	for (uint32 i = 0; i < (int32)EVsFormat::MAX; i++)
	{
		if (ValidFormat[i])
		{
			MeshDataJsonOut.VsFormat.Add(Enum->GetDisplayNameTextByValue(i).ToString());
		}
	}

	//insert json/binary data
	uint32 VertexNum = VertexBuffers.StaticMeshVertexBuffer.GetNumVertices();
	MeshDataBinaryOut.MeshVertexDatas.Reserve(VertexNum);
	for (uint32 i = 0; i < VertexNum; ++i)
	{
		FFullVertexDatas MeshDataBin;
		GetVertexPosition(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		GetVertexNormal(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		GetVertexTangent(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		GetVertexTex(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		GetVertexColor(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		MeshDataBinaryOut.MeshVertexDatas.Add(MeshDataBin);
	}

}

void ExportStaticMeshIndices(const FRawStaticIndexBuffer& IndexBuffer, FFullMeshDataJson& MeshDataJsonOut, bool& bUseHalfInt)
{
	//insert indices
	uint32 IndicesCount = IndexBuffer.GetNumIndices();
	for (uint32 Idx = 0; Idx < IndicesCount; ++Idx)
	{
		uint32 Value = IndexBuffer.GetIndex(Idx);
		if (bUseHalfInt && Value > MAX_UINT16)
		{
			bUseHalfInt = false;
		}
		MeshDataJsonOut.Indices.Add(Value);
	}
}

bool UCustomExportBPLibrary::ExportStaticMeshActor(const AStaticMeshActor* MeshActor, FFullMeshDataJson MeshDataJsonOut, FFullMeshDataBinary MeshDataBinaryOut, FString FileBaseName)
{
	ExportMeshWorld(MeshActor, MeshDataJsonOut, MeshDataBinaryOut);

	UStaticMeshComponent* MeshComponent = MeshActor->GetStaticMeshComponent();
	const auto& LODResources = MeshComponent->GetStaticMesh()->RenderData->LODResources;
	const FStaticMeshLODResources& StaticMeshResource = LODResources[LOD_LEVEL];
	const FStaticMeshVertexBuffers& VertexBuffers = StaticMeshResource.VertexBuffers;
	ExportMeshVertexBuffer(VertexBuffers, MeshDataJsonOut, MeshDataBinaryOut);
	bool bUseHalfInt = true;
	ExportStaticMeshIndices(StaticMeshResource.IndexBuffer, MeshDataJsonOut, bUseHalfInt);

	//delete old files
	FString MeshFile = FileBaseName + ".json";
	FString MeshBinFile = FileBaseName + ".bin";
	CheckFileExistAndDelete(MeshFile);
	CheckFileExistAndDelete(MeshBinFile);

	FMeshDataJson DataJson(MeshDataJsonOut);
	FMeshDataBinary DataBin(MeshDataBinaryOut);
	if (!SaveMeshJsonToFile(DataJson, MeshFile))
	{
		return false;
	}

	if (!SaveMeshBinaryToFile(DataBin, DataJson.Indices, bUseHalfInt, MeshBinFile))
	{
		return false;
	}
	return true;
}

bool UCustomExportBPLibrary::ExportCamera(const UCameraComponent* Component, FCameraData CameraDataOut, FString FileName)
{
	const FVector& CameraLocation = Component->GetComponentLocation();
	const FRotator& CameraRot = Component->GetComponentRotation();
	float FOV = Component->FieldOfView;
	float AspectRatio = Component->AspectRatio;

	FVector FaceDir = CameraRot.Vector();
	FVector Target = FaceDir * CameraTargetOffset + CameraLocation;
	
	//calculate model matrix : scale * rotation * translation
	//in ue4: ue4x = Forward ue4y = Right ue4z = Up
	//in direct x: use left hand coordinate, x = Right, y = Up, z = Forward
	//we have the conversion: x = ue4y, y = ue4z, z = ue4x, Yaw Pitch Roll stay the same
	CameraDataOut.Location = CameraLocation * POSITION_SCALE;//{ CameraLocation.Y, CameraLocation.Z, CameraLocation.X } ;
	CameraDataOut.Target = Target * POSITION_SCALE;//{ Target.Y, Target.Z, Target.X };
	CameraDataOut.Rotator = CameraRot;
	CameraDataOut.Fov = FOV;
	CameraDataOut.Aspect = AspectRatio;

	//delete old files
	FString CameraFileName = FileName + ".json";
	FString CameraBinaryFileName = FileName + ".bin";
	CheckFileExistAndDelete(CameraFileName);
	CheckFileExistAndDelete(CameraBinaryFileName);

	//save json
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(CameraDataOut, Json);
	FString CameraSaveFile = SavePath + CameraFileName;
	bool bSaveResult = FFileHelper::SaveStringToFile(Json, *CameraSaveFile);
	if (!bSaveResult)
	{
		return false;
	}

	//save binary file
	CameraSaveFile = SavePath + CameraBinaryFileName;
	char* ResultName = TCHAR_TO_ANSI(*CameraSaveFile);
	ofstream Wf(ResultName, ios::out | ios::binary);
	if (!Wf)
	{
		return false;
	}
	Wf.write((char*)&CameraDataOut, sizeof(FCameraData));
	Wf.close();
	if (!Wf.good())
	{
		return false;
	}
	return true;
}

bool UCustomExportBPLibrary::ExportDirectionLight(const TArray<ADirectionalLight*> DirectionLights, FLightData DataOut, FString FileName)
{
	for (ADirectionalLight* DirectionLight : DirectionLights)
	{
		const FVector& LightLocation = DirectionLight->GetActorLocation();
		const FRotator& LightRot = DirectionLight->GetActorRotation();
		FVector LightDirection = LightRot.Vector();
		UDirectionalLightComponent* DirectionalLightComponent = DirectionLight->GetComponent();
		float Intensity = DirectionalLightComponent->Intensity * 0.1f;
		FVector Strength = FVector(Intensity, Intensity, Intensity);

		FDirectionLightData Light = { Strength, LightDirection, LightLocation * POSITION_SCALE };
		DataOut.DirectionLights.Add(Light);
	}

	//delete old files
	FString LightFileName = FileName + ".json";
	FString LightBinaryFileName = FileName + ".bin";
	CheckFileExistAndDelete(LightFileName);
	CheckFileExistAndDelete(LightBinaryFileName);

	////save json
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(DataOut, Json);
	FString LightSaveFile = SavePath + LightFileName;
	bool bSaveResult = FFileHelper::SaveStringToFile(Json, *LightSaveFile);
	if (!bSaveResult)
	{
		return false;
	}

	//save binary file
	LightSaveFile = SavePath + LightBinaryFileName;
	char* ResultName = TCHAR_TO_ANSI(*LightSaveFile);
	ofstream Wf(ResultName, ios::out | ios::binary);
	if (!Wf)
	{
		return false;
	}

	uint32  DirectionLightSize = DataOut.DirectionLights.Num();
	Wf.write((char*)&DirectionLightSize, sizeof(uint32));
	Wf.write((char*)DataOut.DirectionLights.GetData(), sizeof(FDirectionLightData) * DirectionLightSize);

	Wf.close();
	if (!Wf.good())
	{
		return false;
	}
	return true;
}

bool UCustomExportBPLibrary::ExportSkeletalMeshAnim(const UAnimSequence* Anim, FSkeletalAnimData DataOut, FString FileName)
{
	DataOut.NumFrames = Anim->GetRawNumberOfFrames();

	for (FRawAnimSequenceTrack RawData : Anim->GetRawAnimationData())
	{
		FAnimSequenceTrack TrackData;
		for(FVector Pos : RawData.PosKeys)
		{
			FVector _Pos = Pos * POSITION_SCALE;
			TrackData.PosKeys.Add(_Pos);
		}

		for(FQuat Qua : RawData.RotKeys)
		{
			TrackData.RotKeys.Add(Qua.Rotator());
		}
		TrackData.ScaleKeys.Append(RawData.ScaleKeys);
		
		DataOut.RawAnimationData.Add(TrackData);
	}
	
	for (FTrackToSkeletonMap TrackData : Anim->GetRawTrackToSkeletonMapTable())
	{
		DataOut.TrackToSkeletonMapTable.Add(TrackData);
	}
	
    FString FileNameBase = FileName + "_" + Anim->GetName();
	FString AnimFile = FileNameBase + ".json";
	FString AnimFileBin = FileNameBase + ".bin";
	CheckFileExistAndDelete(AnimFile);
	CheckFileExistAndDelete(AnimFileBin);

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(DataOut, Json);
	FString AnimSave = SavePath + AnimFile;
	bool bSaveResult = FFileHelper::SaveStringToFile(Json, *AnimSave);
	if (!bSaveResult)
	{
		return false;
	}
	 
	//save binary file
	AnimSave = SavePath + AnimFileBin;
	char* ResultName = TCHAR_TO_ANSI(*AnimSave);
	ofstream Wf(ResultName, ios::out | ios::binary);
	if (!Wf)
	{
		return false;
	}

	uint32 NumFrames = DataOut.NumFrames;
	Wf.write((char*)&NumFrames, sizeof(uint32));

	uint32 NumRawAnimData = DataOut.RawAnimationData.Num();
	Wf.write((char*)&NumRawAnimData, sizeof(uint32));
	Wf.write((char*)DataOut.RawAnimationData.GetData(), sizeof(FAnimSequenceTrack) * NumRawAnimData);

	uint32 NumTrackSkeletalTable = DataOut.TrackToSkeletonMapTable.Num();
	Wf.write((char*)&NumTrackSkeletalTable, sizeof(uint32));
	Wf.write((char*)DataOut.TrackToSkeletonMapTable.GetData(), sizeof(FTrackToSkeletonMap) * NumTrackSkeletalTable);

	Wf.close();
	if (!Wf.good())
	{
		return false;
	}
	return true;
 
}

void GetVertexSkinWeightInfo(const TArray<bool>& ValidFormat, uint32 Index, TArray<FSkinWeightInfo>& SkinWeightInfos, FFullMeshDataJson& MeshDataJson, FFullVertexDatas& MeshDataBin)
{
	if (ValidFormat[(uint32)EVsFormat::SKINWEIGHT])
	{
		const FSkinWeightInfo& WeightInfo = SkinWeightInfos[Index];

		FSkinMeshWeightInfo Info;
		for (int i = 0; i < 4; i++)
		{
			MeshDataBin.InfluenceBones[i] = WeightInfo.InfluenceBones[i];
			MeshDataBin.InfluenceWeights[i] = WeightInfo.InfluenceWeights[i];
			
			Info.InfluenceBones[i] = WeightInfo.InfluenceBones[i];
			Info.InfluenceWeights[i] = WeightInfo.InfluenceWeights[i];
		}
		MeshDataJson.SkinMeshWeight0s.Add(Info);
	}
}

void ExportSkeletalMeshVertexBuffer(const FStaticMeshVertexBuffers& VertexBuffers, const FSkinWeightVertexBuffer& SkinWeightBuffer, FFullMeshDataJson& MeshDataJsonOut, FFullMeshDataBinary& MeshDataBinaryOut)
{
	TArray<bool> ValidFormat;
	ValidFormat.Init(false, (int32)EVsFormat::MAX);
	GetValidVsFormat(ValidFormat, VertexBuffers);
	ValidFormat[(uint32)EVsFormat::SKINWEIGHT] = true;

	//insert format 
	UEnum* Enum = StaticEnum<EVsFormat>();
	for (uint32 i = 0; i < (int32)EVsFormat::MAX; i++)
	{
		if (ValidFormat[i])
		{
			MeshDataJsonOut.VsFormat.Add(Enum->GetDisplayNameTextByValue(i).ToString());
		}
	}

	//insert json/binary data
	uint32 VertexNum = VertexBuffers.StaticMeshVertexBuffer.GetNumVertices();

	TArray<FSkinWeightInfo> OutVerticesWeightInfo;
	SkinWeightBuffer.GetSkinWeights(OutVerticesWeightInfo);

	MeshDataBinaryOut.MeshVertexDatas.Reserve(VertexNum);
	for (uint32 i = 0; i < VertexNum; ++i)
	{
		FFullVertexDatas MeshDataBin;
		GetVertexPosition(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		GetVertexNormal(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		GetVertexTangent(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		GetVertexTex(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		GetVertexColor(ValidFormat, i, VertexBuffers, MeshDataJsonOut, MeshDataBin);
		GetVertexSkinWeightInfo(ValidFormat, i, OutVerticesWeightInfo, MeshDataJsonOut, MeshDataBin);
		MeshDataBinaryOut.MeshVertexDatas.Add(MeshDataBin);
	}

}

void ExportSkeletalMeshIndices(const FMultiSizeIndexContainer& IndexContainer, FFullMeshDataJson& MeshDataJsonOut)
{
	//insert indices
	IndexContainer.GetIndexBuffer(MeshDataJsonOut.Indices);
}

bool SaveSkeletalMeshJsonToFile(const FFullMeshDataJson& MeshJson, FString& FileName)
{
	//save json
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(MeshJson, Json);
	FString MeshSaveFile = SavePath + FileName;
	return FFileHelper::SaveStringToFile(Json, *MeshSaveFile);
}

bool SaveSkeletalMeshBinaryToFile(const FFullMeshDataBinary& MeshBinOut, const TArray<uint32>& Indices, FString& FileName)
{
	FString MeshSaveFile = SavePath + FileName;
	char* ResultName = TCHAR_TO_ANSI(*MeshSaveFile);
	ofstream Wf(ResultName, ios::out | ios::binary);
	if (!Wf)
	{
		return false;
	}

	Wf.write((char*)&(MeshBinOut.WorldLocation), sizeof(FVector));
	Wf.write((char*)&(MeshBinOut.WorldRotation), sizeof(FRotator));
	Wf.write((char*)&(MeshBinOut.WorldScale), sizeof(FVector));

	uint32  VertexsSize = MeshBinOut.MeshVertexDatas.Num();
	Wf.write((char*)&VertexsSize, sizeof(uint32));
	Wf.write((char*)MeshBinOut.MeshVertexDatas.GetData(), sizeof(FFullVertexDatas) * VertexsSize);

	uint32 IndicesSize = Indices.Num();
	Wf.write((char*)&IndicesSize, sizeof(uint32));
	Wf.write((char*)Indices.GetData(), IndicesSize * sizeof(uint32));

	Wf.close();
	return Wf.good();
}

bool UCustomExportBPLibrary::ExportSkeletalMeshActor(const ACharacter* PlayerActor, const USkeletalMesh* Mesh, FFullMeshDataJson MeshDataJsonOut, FFullMeshDataBinary MeshDataBinaryOut, FString FileBaseName)
{
	ExportMeshWorld(PlayerActor, MeshDataJsonOut, MeshDataBinaryOut);
	
	const FSkeletalMeshLODRenderData& MeshResource = Mesh->GetResourceForRendering()->LODRenderData[LOD_LEVEL];
	const FStaticMeshVertexBuffers& VertexBuffers = MeshResource.StaticVertexBuffers;
	const FSkinWeightVertexBuffer& VertexSkinWeightBuffer = MeshResource.SkinWeightVertexBuffer;
	ExportSkeletalMeshVertexBuffer(VertexBuffers, VertexSkinWeightBuffer, MeshDataJsonOut, MeshDataBinaryOut);

	const FMultiSizeIndexContainer& IndexBufferContainer = MeshResource.MultiSizeIndexContainer;
	ExportSkeletalMeshIndices(IndexBufferContainer, MeshDataJsonOut);

	//delete old files
	FString MeshFile = FileBaseName + ".json";
	FString MeshBinFile = FileBaseName + ".bin";
	CheckFileExistAndDelete(MeshFile);
	CheckFileExistAndDelete(MeshBinFile);

	if (!SaveSkeletalMeshJsonToFile(MeshDataJsonOut, MeshFile))
	{
		return false;
	}

	if (!SaveSkeletalMeshBinaryToFile(MeshDataBinaryOut, MeshDataJsonOut.Indices, MeshBinFile))
	{
		return false;
	}
	return true;
}

bool UCustomExportBPLibrary::ExportSkeleton(const USkeleton* Skeleton, FSkeletonData DataOut, FString FileName)
{
	const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();

	const TArray<FMeshBoneInfo>& RefBoneInfos = RefSkeleton.GetRawRefBoneInfo();
	TArray<FSkeletonBoneInfo> BinBoneInfos;
	for(FMeshBoneInfo BoneInfo : RefBoneInfos)
	{
		FSkeletalBoneInfoJson InfoJson;
		InfoJson.Name = BoneInfo.Name;
		InfoJson.ParentIndex = BoneInfo.ParentIndex;
		DataOut.RawRefBoneInfo.Add(InfoJson);

		FSkeletonBoneInfo BinBoneInfo;
		char* CName = TCHAR_TO_ANSI(*BoneInfo.Name.ToString());
		strcpy_s(BinBoneInfo.Name, CName);
		BinBoneInfo.ParentIndex = BoneInfo.ParentIndex;
		BinBoneInfos.Add(BinBoneInfo);
	}

	const TArray<FTransform>& RefBoneTrans = RefSkeleton.GetRawRefBonePose();
	for (FTransform BoneTrans : RefBoneTrans)
	{
		FSkeletonBonePose BonePose;
		BonePose.Scale = BoneTrans.GetScale3D();
		BonePose.Rotate = BoneTrans.Rotator();
		BonePose.Translate = BoneTrans.GetTranslation() * POSITION_SCALE;
		DataOut.RawRefBonePose.Add(BonePose);
	}

	TArray<FSkeletonNameIndex> BinNameIndexs;
	for (FMeshBoneInfo BoneInfo : RefBoneInfos)
	{
		int32 Index = RefSkeleton.FindBoneIndex(BoneInfo.Name);
		DataOut.RawNameToIndexMap.Add(BoneInfo.Name, Index);

		FSkeletonNameIndex NameIndex;
		char* CName = TCHAR_TO_ANSI(*BoneInfo.Name.ToString());
		strcpy_s(NameIndex.Name, CName);
		NameIndex.Index = Index;
		BinNameIndexs.Add(NameIndex);
	}

	//delete old files
	FString SkeletonFile = FileName + ".json";
	FString SkeletonBinFile = FileName + ".bin";
	CheckFileExistAndDelete(SkeletonFile);
	CheckFileExistAndDelete(SkeletonBinFile);

	//save json
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(DataOut, Json);
	FString SaveFile = SavePath + SkeletonFile;
	bool bSaveResult = FFileHelper::SaveStringToFile(Json, *SaveFile);
	if (!bSaveResult)
	{
		return false;
	}

	SaveFile = SavePath + SkeletonBinFile;
	char* ResultName = TCHAR_TO_ANSI(*SaveFile);
	ofstream Wf(ResultName, ios::out | ios::binary);
	if (!Wf)
	{
		return false;
	}

	uint32 BonInfosNum = BinBoneInfos.Num();
	Wf.write((char*)&BonInfosNum, sizeof(uint32));
	Wf.write((char*)BinBoneInfos.GetData(), sizeof(FSkeletonBoneInfo) * BonInfosNum);

	uint32 BonePosesNum = DataOut.RawRefBonePose.Num();
	Wf.write((char*)&BonePosesNum, sizeof(uint32));
	Wf.write((char*)DataOut.RawRefBonePose.GetData(), sizeof(FSkeletonBonePose) * BonePosesNum);

	uint32 NameIndexNum = BinNameIndexs.Num();
	Wf.write((char*)&NameIndexNum, sizeof(uint32));
	Wf.write((char*)BinNameIndexs.GetData(), sizeof(FSkeletonNameIndex) * NameIndexNum);

	Wf.close();
	return Wf.good();
}