// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomExportBPLibrary.h"
#include "CustomExport.h"
#include "Runtime/RawMesh/Public/RawMesh.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "JsonObjectConverter.h"

#include <iostream>
#include <fstream>

using namespace std;

typedef TJsonWriter< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriter;
typedef TJsonWriterFactory< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriterFactory;

static FString SavePath = FPaths::ProjectContentDir() + "Save/";
static FString CameraFileName = TEXT("camera.json");
static FString CameraBinaryFileName = TEXT("camera.bin");

static FString MeshFileName = TEXT("mesh.json");
static FString MeshBinaryFileName = TEXT("mesh.bin");

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

void UCustomExportBPLibrary::ExportStaticMesh(const UStaticMesh* Mesh)
{
	/*const TArray<FStaticMeshSourceModel>& SourceModels = Mesh->GetSourceModels();
	FRawMesh RawMesh;
	SourceModels[0].LoadRawMesh(RawMesh);*/

	auto& LODResources = Mesh->RenderData->LODResources;
	const FStaticMeshLODResources& StaticMeshResource = LODResources[0];

	const FStaticMeshVertexBuffer& VertexBuffer = StaticMeshResource.VertexBuffers.StaticMeshVertexBuffer;

	FMeshData meshData;
	const FPositionVertexBuffer& PositionBuffers = StaticMeshResource.VertexBuffers.PositionVertexBuffer;
	UINT NumPositionVertex = PositionBuffers.GetNumVertices();
	meshData.vertices.Reserve(NumPositionVertex * 3);
	for (UINT i = 0; i < NumPositionVertex; i++)
	{
		meshData.vertices.Add(PositionBuffers.VertexPosition(i).X);
		meshData.vertices.Add(PositionBuffers.VertexPosition(i).Y);
		meshData.vertices.Add(PositionBuffers.VertexPosition(i).Z);
	}

	TArray<FColor> ColorVerts;
	const FColorVertexBuffer& ColorBuffers = StaticMeshResource.VertexBuffers.ColorVertexBuffer;
	UINT NumColorVertex = ColorBuffers.GetNumVertices();
	ColorVerts.Reserve(NumColorVertex);
	ColorBuffers.GetVertexColors(ColorVerts);
	meshData.colors.Reserve(NumColorVertex * 4);
	for (FColor color : ColorVerts)
	{
		meshData.colors.Add(color.R);
		meshData.colors.Add(color.G);
		meshData.colors.Add(color.B);
		meshData.colors.Add(color.A);
	}

	const FRawStaticIndexBuffer& IndexBuffer = StaticMeshResource.IndexBuffer;
	UINT NumIndices = IndexBuffer.GetNumIndices();//索引个数
	TArray<uint32> Indices;//索引数据
	Indices.Reserve(NumIndices);
	IndexBuffer.GetCopy(Indices);
	meshData.indices.Reserve(NumIndices);
	for (uint32 index : Indices)
	{
		meshData.indices.Add(index);
	}

	//delete old files
	CheckFileExistAndDelete(MeshFileName);
	CheckFileExistAndDelete(MeshBinaryFileName);

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(meshData, Json);
	FString MeshSaveFile = SavePath + MeshFileName;
	FFileHelper::SaveStringToFile(Json, *MeshSaveFile);

	//save binary file
	MeshSaveFile = SavePath + MeshBinaryFileName;
	char* resultName = TCHAR_TO_ANSI(*MeshSaveFile);
	ofstream wf(resultName, ios::out | ios::binary);
	if (!wf)
	{
		return;
	}

	uint32 vSize = meshData.vertices.Num();
	wf.write((char*)&vSize, sizeof(uint32));
	wf.write((char*)(meshData.vertices.GetData()), vSize * sizeof(float));

	uint32 cSize = meshData.colors.Num();
	wf.write((char*)&cSize, sizeof(uint32));
	wf.write((char*)meshData.colors.GetData(), cSize * sizeof(float));

	uint32 iSize = meshData.indices.Num();
	wf.write((char*)&iSize, sizeof(uint32));
	wf.write((char*)meshData.indices.GetData(), iSize * sizeof(uint16));

	wf.close();
	if (!wf.good())
	{
		return;
	}
}

void UCustomExportBPLibrary::ExportCamera(const UCameraComponent* Component)
{
	const FVector& CameraLocation = Component->GetComponentLocation();
	const FRotator& CameraRot = Component->GetComponentRotation();
	float FOV = Component->FieldOfView;
	float AspectRatio = Component->AspectRatio;
	
	FVector FaceDir = CameraRot.Vector();
	FVector target = FaceDir * 5.f + CameraLocation;
	//for export binary
	FCameraData cameraData = {};
	cameraData.location = FVector(CameraLocation.X, CameraLocation.Y, CameraLocation.Z);
	cameraData.target = FVector(target.X, target.Y, target.Z);
	cameraData.rotator = FRotator(CameraRot.Pitch, CameraRot.Yaw, CameraRot.Roll);
	cameraData.fov = FOV;
	cameraData.aspect = AspectRatio;

	//delete old files
	CheckFileExistAndDelete(CameraFileName);
	CheckFileExistAndDelete(CameraBinaryFileName);

	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(cameraData, Json);
	FString CameraSaveFile = SavePath + CameraFileName;
	FFileHelper::SaveStringToFile(Json, *CameraSaveFile);

	////save binary file
	CameraSaveFile = SavePath + CameraBinaryFileName;
	char* resultName = TCHAR_TO_ANSI(*CameraSaveFile);
	ofstream wf(resultName, ios::out | ios::binary);
	if (!wf)
	{
		return;
	}
	wf.write((char*)&cameraData, sizeof(FCameraData));
	wf.close();
	if (!wf.good())
	{
		return;
	}
}


