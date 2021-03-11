// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomExportBPLibrary.h"
#include "CustomExport.h"
#include "Runtime/RawMesh/Public/RawMesh.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Internationalization/Internationalization.h"
#include "JsonObjectConverter.h"
#include "Misc/MessageDialog.h"

#include <iostream>
#include <fstream>

using namespace std;

static FString SavePath = FPaths::ProjectSavedDir() + "Export/";

static FString CameraFileName = TEXT("camera.json");
static FString CameraBinaryFileName = TEXT("camera.bin");
static FString MeshFileName = TEXT("mesh.json");
static FString MeshBinaryFileName = TEXT("mesh.bin");

const FString Success = "Success";
const FString Failed = "Fail";
const uint32 LOD_LEVEL = 0;
const uint32 MAX_UINT16 = 65535;
const float POSITION_SCALE = 0.01f;

UCustomExportBPLibrary::UCustomExportBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

#define LOCTEXT_NAMESPACE "CustomExport"
void ShowMessageDialog(const FText& Target, const FText& Result)
{
	const FText WarningMessage = FText::Format(LOCTEXT("CustomExport_Message", "{0} export {1}!"), Target, Result);
	EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::Ok, WarningMessage);
}
#undef LOCTEXT_NAMESPACE

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

void GetVertexPosition(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FMeshDataJson& MeshDataJson, FMeshDataBinary& MeshDataBin)
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

void GetVertexNormal(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FMeshDataJson& MeshDataJson, FMeshDataBinary& MeshDataBin)
{
	if (ValidFormat[(uint32)EVsFormat::NORMAL])
	{
		const FStaticMeshVertexBuffer& VertexBuffers = VertexBuffer.StaticMeshVertexBuffer;
		const FVector& Normal = VertexBuffers.VertexTangentZ(Index);
		MeshDataJson.Normals.Append({ Normal.X, Normal.Y, Normal.Z});
		MeshDataBin.Normal = Normal;
	}
}

void GetVertexTangent(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FMeshDataJson& MeshDataJson, FMeshDataBinary& MeshDataBin)
{
	if (ValidFormat[(uint32)EVsFormat::TANGENT])
	{
		const FStaticMeshVertexBuffer& VertexBuffers = VertexBuffer.StaticMeshVertexBuffer;
		const FVector& Tangent = VertexBuffers.VertexTangentX(Index);
		MeshDataJson.Tangents.Append({ Tangent.X, Tangent.Y, Tangent.Z});
		MeshDataBin.Tangent = Tangent;
	}
}

void GetVertexTex(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FMeshDataJson& MeshDataJson, FMeshDataBinary& MeshDataBin)
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

void GetVertexColor(const TArray<bool>& ValidFormat, uint32 Index, const FStaticMeshVertexBuffers& VertexBuffer, FMeshDataJson& MeshDataJson, FMeshDataBinary& MeshDataBin)
{
	if (ValidFormat[(uint32)EVsFormat::COLOR])
	{
		const FColorVertexBuffer& ColorBuffers = VertexBuffer.ColorVertexBuffer;
		const FLinearColor& Color = ColorBuffers.VertexColor(Index);
		MeshDataJson.Colors.Append({ Color.R, Color.G, Color.B, Color.A });
		MeshDataBin.Color = Color;
	}
}

bool SaveMeshJsonToFile(const FMeshDataJson& MeshJson)
{
	//save json
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(MeshJson, Json);
	FString MeshSaveFile = SavePath + MeshFileName;
	return FFileHelper::SaveStringToFile(Json, *MeshSaveFile);
}

bool SaveMeshBinaryToFile(const TArray<FMeshDataBinary>& MeshBinArr, const TArray<uint32>& Indices, const bool& bUseHalfInt32)
{
	FString MeshSaveFile = SavePath + MeshBinaryFileName;
	char* ResultName = TCHAR_TO_ANSI(*MeshSaveFile);
	ofstream Wf(ResultName, ios::out | ios::binary);
	if (!Wf)
	{
		return false;
	}

	uint32  MeshBinSize = MeshBinArr.Num();
	Wf.write((char*)&MeshBinSize, sizeof(uint32));
	Wf.write((char*)MeshBinArr.GetData(), sizeof(FMeshDataBinary) * MeshBinSize);

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

void UCustomExportBPLibrary::ExportStaticMesh(const UStaticMesh* Mesh)
{
	const auto& LODResources = Mesh->RenderData->LODResources;
	const FStaticMeshLODResources& StaticMeshResource = LODResources[LOD_LEVEL];
	const FStaticMeshVertexBuffers& VertexBuffers = StaticMeshResource.VertexBuffers;

	TArray<bool> ValidFormat;
	ValidFormat.Init(false, (int32)EVsFormat::MAX);
	GetValidVsFormat(ValidFormat, VertexBuffers);

	FMeshDataJson MeshJson;// use for save json
	TArray<FMeshDataBinary> MeshBinArr;// use for save bin

	//insert format 
	UEnum* Enum = StaticEnum<EVsFormat>();
	for (uint32 i = 0; i < (int32)EVsFormat::MAX; i++)
	{
		if (ValidFormat[i])
		{
			MeshJson.VsFormat.Add(Enum->GetDisplayNameTextByValue(i).ToString());
		}
	}

	//insert json/binary data
	uint32 VertexNum = VertexBuffers.StaticMeshVertexBuffer.GetNumVertices();
	MeshBinArr.Reserve(VertexNum);
	for (uint32 i = 0; i < VertexNum; ++i)
	{
		FMeshDataBinary MeshDataBin;
		GetVertexPosition(ValidFormat, i, VertexBuffers, MeshJson, MeshDataBin);
		GetVertexNormal(ValidFormat, i, VertexBuffers, MeshJson, MeshDataBin);
		GetVertexTangent(ValidFormat, i, VertexBuffers, MeshJson, MeshDataBin);
		GetVertexTex(ValidFormat, i, VertexBuffers, MeshJson, MeshDataBin);
		GetVertexColor(ValidFormat, i, VertexBuffers, MeshJson, MeshDataBin);
		MeshBinArr.Add(MeshDataBin);
	}

	//insert indices
	const FRawStaticIndexBuffer& IndexBuffer = StaticMeshResource.IndexBuffer;
	uint32 IndicesCount = IndexBuffer.GetNumIndices();
	bool bUseHalfInt = true;
	for (uint32 Idx = 0; Idx < IndicesCount; ++Idx)
	{
		uint32 Value = IndexBuffer.GetIndex(Idx);
		if(bUseHalfInt && Value > MAX_UINT16)
		{
			bUseHalfInt = false;
		}
		MeshJson.Indices.Add(Value);
	}

	//delete old files
	CheckFileExistAndDelete(MeshFileName);
	CheckFileExistAndDelete(MeshBinaryFileName);

	if(!SaveMeshJsonToFile(MeshJson))
	{
		ShowMessageDialog(FText::FromString(MeshFileName), FText::FromString(Failed));
		return;
	}

	if(!SaveMeshBinaryToFile(MeshBinArr, MeshJson.Indices, bUseHalfInt))
	{
		ShowMessageDialog(FText::FromString(MeshBinaryFileName), FText::FromString(Failed));
		return;
	}
	ShowMessageDialog(FText::FromString(MeshFileName + " " + MeshBinaryFileName), FText::FromString(Success));
}

void UCustomExportBPLibrary::ExportCamera(const UCameraComponent* Component)
{
	const FVector& CameraLocation = Component->GetComponentLocation();
	const FRotator& CameraRot = Component->GetComponentRotation();
	float FOV = Component->FieldOfView;
	float AspectRatio = Component->AspectRatio;

	FVector FaceDir = CameraRot.Vector();
	FVector Target = FaceDir * 5.f + CameraLocation;
	
	FCameraData CameraData = {};
	CameraData.Location = CameraLocation * POSITION_SCALE;
	CameraData.Target = Target * POSITION_SCALE;
	CameraData.Rotator = CameraRot;
	CameraData.Fov = FOV;
	CameraData.Aspect = AspectRatio;

	//delete old files
	CheckFileExistAndDelete(CameraFileName);
	CheckFileExistAndDelete(CameraBinaryFileName);

	//save json
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(CameraData, Json);
	FString CameraSaveFile = SavePath + CameraFileName;
	bool bSaveResult = FFileHelper::SaveStringToFile(Json, *CameraSaveFile);
	if (!bSaveResult)
	{
		ShowMessageDialog(FText::FromString(CameraFileName), FText::FromString(Failed));
	}

	//save binary file
	CameraSaveFile = SavePath + CameraBinaryFileName;
	char* ResultName = TCHAR_TO_ANSI(*CameraSaveFile);
	ofstream Wf(ResultName, ios::out | ios::binary);
	if (!Wf)
	{
		ShowMessageDialog(FText::FromString(CameraBinaryFileName), FText::FromString(Failed));
		return;
	}
	Wf.write((char*)&CameraData, sizeof(FCameraData));
	Wf.close();
	if (!Wf.good())
	{
		ShowMessageDialog(FText::FromString(CameraBinaryFileName), FText::FromString(Failed));
		return;
	}

	ShowMessageDialog(FText::FromString(CameraFileName + " " + CameraBinaryFileName), FText::FromString(Success));
}


