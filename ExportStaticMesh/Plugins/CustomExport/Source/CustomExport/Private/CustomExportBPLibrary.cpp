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

void UCustomExportBPLibrary::ExportStaticMesh(const UStaticMesh* Mesh)
{
	/*const TArray<FStaticMeshSourceModel>& SourceModels = Mesh->GetSourceModels();
	FRawMesh RawMesh;
	SourceModels[0].LoadRawMesh(RawMesh);*/

	const auto& LODResources = Mesh->RenderData->LODResources;
	const FStaticMeshLODResources& StaticMeshResource = LODResources[0];
	const FStaticMeshVertexBuffer& VertexBuffer = StaticMeshResource.VertexBuffers.StaticMeshVertexBuffer;

	FMeshData MeshData;
	//add vertices
	const FPositionVertexBuffer& PositionBuffers = StaticMeshResource.VertexBuffers.PositionVertexBuffer;
	UINT NumPositionVertex = PositionBuffers.GetNumVertices();
	MeshData.Vertices.Reserve(NumPositionVertex * 3);
	for (UINT i = 0; i < NumPositionVertex; i++)
	{
		MeshData.Vertices.Add(PositionBuffers.VertexPosition(i).X);
		MeshData.Vertices.Add(PositionBuffers.VertexPosition(i).Y);
		MeshData.Vertices.Add(PositionBuffers.VertexPosition(i).Z);
	}

	TArray<FColor> ColorVerts;
	//add colors
	const FColorVertexBuffer& ColorBuffers = StaticMeshResource.VertexBuffers.ColorVertexBuffer;
	UINT NumColorVertex = ColorBuffers.GetNumVertices();
	ColorVerts.Reserve(NumColorVertex);
	ColorBuffers.GetVertexColors(ColorVerts);
	MeshData.Colors.Reserve(NumColorVertex * 4);
	for (FColor Color : ColorVerts)
	{
		MeshData.Colors.Add(Color.R);
		MeshData.Colors.Add(Color.G);
		MeshData.Colors.Add(Color.B);
		MeshData.Colors.Add(Color.A);
	}

	//add indices
	const FRawStaticIndexBuffer& IndexBuffer = StaticMeshResource.IndexBuffer;
	UINT NumIndices = IndexBuffer.GetNumIndices();
	TArray<uint32> Indices;
	Indices.Reserve(NumIndices);
	IndexBuffer.GetCopy(Indices);
	MeshData.Indices.Reserve(NumIndices);
	for (uint32 Index : Indices)
	{
		MeshData.Indices.Add(Index);
	}

	//delete old files
	CheckFileExistAndDelete(MeshFileName);
	CheckFileExistAndDelete(MeshBinaryFileName);

	//save json
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(MeshData, Json);
	FString MeshSaveFile = SavePath + MeshFileName;
	bool bSaveResult = FFileHelper::SaveStringToFile(Json, *MeshSaveFile);
	if (!bSaveResult)
	{
		ShowMessageDialog(FText::FromString(MeshFileName), FText::FromString(Failed));
	}

	//save binary file
	MeshSaveFile = SavePath + MeshBinaryFileName;
	char* ResultName = TCHAR_TO_ANSI(*MeshSaveFile);
	ofstream Wf(ResultName, ios::out | ios::binary);
	if (!Wf)
	{
		ShowMessageDialog(FText::FromString(MeshBinaryFileName), FText::FromString(Failed));
		return;
	}

	uint32 vSize = MeshData.Vertices.Num();
	Wf.write((char*)&vSize, sizeof(uint32));
	Wf.write((char*)(MeshData.Vertices.GetData()), vSize * sizeof(float));

	uint32 cSize = MeshData.Colors.Num();
	Wf.write((char*)&cSize, sizeof(uint32));
	Wf.write((char*)MeshData.Colors.GetData(), cSize * sizeof(float));

	uint32 iSize = MeshData.Indices.Num();
	Wf.write((char*)&iSize, sizeof(uint32));
	Wf.write((char*)MeshData.Indices.GetData(), iSize * sizeof(uint16));

	Wf.close();
	if (!Wf.good())
	{
		ShowMessageDialog(FText::FromString(MeshBinaryFileName), FText::FromString(Failed));
		return;
	}

	ShowMessageDialog(FText::FromString(MeshFileName +" "+ MeshBinaryFileName), FText::FromString(Success));
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
	CameraData.Location = CameraLocation;
	CameraData.Target = Target;
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


