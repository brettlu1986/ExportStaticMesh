// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomExportBPLibrary.h"
#include "CustomExport.h"
#include "Runtime/RawMesh/Public/RawMesh.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/PrettyJsonPrintPolicy.h"

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
	UINT NumVerts = VertexBuffer.GetNumVertices();//顶点个数 not use

	MeshData meshData;
	TArray<FVector> PositionVerts;//顶点位置数据
	const FPositionVertexBuffer& PositionBuffers = StaticMeshResource.VertexBuffers.PositionVertexBuffer;
	UINT NumPositionVertex = PositionBuffers.GetNumVertices();//可能跟 StaticMeshVertexBuffer获取的顶点数不一样?
	PositionVerts.Reserve(NumPositionVertex);
	meshData.vertices.reserve(NumPositionVertex * 3);
	for (UINT i = 0; i < NumPositionVertex; i++)
	{
		PositionVerts.Add(PositionBuffers.VertexPosition(i));
		meshData.vertices.push_back(PositionBuffers.VertexPosition(i).X);
		meshData.vertices.push_back(PositionBuffers.VertexPosition(i).Y);
		meshData.vertices.push_back(PositionBuffers.VertexPosition(i).Z);
	}

	TArray<FColor> ColorVerts;//顶点颜色数据
	const FColorVertexBuffer& ColorBuffers = StaticMeshResource.VertexBuffers.ColorVertexBuffer;
	UINT NumColorVertex = ColorBuffers.GetNumVertices();
	ColorVerts.Reserve(NumColorVertex);
	ColorBuffers.GetVertexColors(ColorVerts);
	meshData.colors.reserve(NumColorVertex * 4);
	for (FColor color : ColorVerts)
	{
		meshData.colors.push_back(color.R);
		meshData.colors.push_back(color.G);
		meshData.colors.push_back(color.B);
		meshData.colors.push_back(color.A);
	}

	const FRawStaticIndexBuffer& IndexBuffer = StaticMeshResource.IndexBuffer;
	UINT NumIndices = IndexBuffer.GetNumIndices();//索引个数
	TArray<uint32> Indices;//索引数据
	Indices.Reserve(NumIndices);
	IndexBuffer.GetCopy(Indices);
	meshData.indices.reserve(NumIndices);
	for (float index : Indices)
	{
		meshData.indices.push_back(index);
	}
	
	//UINT NumberOfTriangles = IndexBuffer.GetNumIndices() / 3;//顶点索引个数 是 三角形个数的3倍?
	//UINT NumVertices = VertexBuffer.GetNumVertices();//顶点个数
	
	//export json
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> JsonVertexPositions;
	for (auto& ver : PositionVerts)
	{
		JsonVertexPositions.Emplace(MakeShareable(new FJsonValueNumber(ver.X)));
		JsonVertexPositions.Emplace(MakeShareable(new FJsonValueNumber(ver.Y)));
		JsonVertexPositions.Emplace(MakeShareable(new FJsonValueNumber(ver.Z)));
	}
	JsonObject->SetArrayField("vertices", JsonVertexPositions);

	TArray<TSharedPtr<FJsonValue>> JsonVertexColors;
	for (auto& color : ColorVerts)
	{
		JsonVertexColors.Emplace(MakeShareable(new FJsonValueNumber(color.R)));
		JsonVertexColors.Emplace(MakeShareable(new FJsonValueNumber(color.G)));
		JsonVertexColors.Emplace(MakeShareable(new FJsonValueNumber(color.B)));
		JsonVertexColors.Emplace(MakeShareable(new FJsonValueNumber(color.A)));
	}
	JsonObject->SetArrayField("colors", JsonVertexColors);

	TArray<TSharedPtr<FJsonValue>> JsonIndices;
	for (auto& index : Indices)
	{
		JsonIndices.Emplace(MakeShareable(new FJsonValueNumber(index)));
	}
	JsonObject->SetArrayField("indices", JsonIndices);

	FString Json;
	TSharedRef< FPrettyJsonStringWriter > JsonWriter = FPrettyJsonStringWriterFactory::Create(&Json);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter))
	{
		//delete old files
		CheckFileExistAndDelete(MeshFileName);
		CheckFileExistAndDelete(MeshBinaryFileName);

		//save json file
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

		size_t vSize = meshData.vertices.size();
		wf.write((char*)&vSize, sizeof(vSize));
		wf.write((char*)&meshData.vertices[0], vSize * sizeof(float));

		size_t cSize = meshData.colors.size();
		wf.write((char*)&cSize, sizeof(cSize));
		wf.write((char*)&meshData.colors[0], cSize * sizeof(float));

		size_t iSize = meshData.indices.size();
		wf.write((char*)&iSize, sizeof(iSize));
		wf.write((char*)&meshData.indices[0], iSize * sizeof(UINT));

		wf.close();
		if (!wf.good())
		{
			return;
		}
	}
}

void UCustomExportBPLibrary::ExportCamera(const UCameraComponent* Component)
{
	const FVector& CameraLocation = Component->GetComponentLocation();
	const FRotator& CameraRot = Component->GetComponentRotation();
	float FOV = Component->FieldOfView;
	float AspectRatio = Component->AspectRatio;

	//for export binary
	CameraData cameraData = {};
	cameraData.location = { CameraLocation.X, CameraLocation.Y, CameraLocation.Z };
	cameraData.rotator = { CameraRot.Pitch, CameraRot.Yaw, CameraRot.Roll };
	cameraData.fov = FOV;
	cameraData.aspect = AspectRatio;

	//export json
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	TArray<TSharedPtr<FJsonValue>> JsonLocationArray;
	JsonLocationArray.Add(MakeShareable(new FJsonValueNumber(CameraLocation.X)));
	JsonLocationArray.Add(MakeShareable(new FJsonValueNumber(CameraLocation.Y)));
	JsonLocationArray.Add(MakeShareable(new FJsonValueNumber(CameraLocation.Z)));
	JsonObject->SetArrayField("location", JsonLocationArray);

	TArray<TSharedPtr<FJsonValue>> JsonRotatorArray;
	JsonRotatorArray.Add(MakeShareable(new FJsonValueNumber(CameraRot.Pitch)));
	JsonRotatorArray.Add(MakeShareable(new FJsonValueNumber(CameraRot.Yaw)));
	JsonRotatorArray.Add(MakeShareable(new FJsonValueNumber(CameraRot.Roll)));
	JsonObject->SetArrayField("rotator", JsonRotatorArray);

	JsonObject->SetNumberField("fov", FOV);
	JsonObject->SetNumberField("aspect", AspectRatio);

	FString Json;
	TSharedRef< FPrettyJsonStringWriter > JsonWriter = FPrettyJsonStringWriterFactory::Create(&Json);
	
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter))
	{
		//delete old files
		CheckFileExistAndDelete(CameraFileName);
		CheckFileExistAndDelete(CameraBinaryFileName);

		//save json file
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
		UINT sizeCamera = sizeof(CameraData);
		char* cameraBytes = new char[sizeCamera];
		memset(cameraBytes, 0, sizeCamera);
		memcpy(cameraBytes, (char*)&cameraData, sizeCamera);
		wf.write(cameraBytes, sizeCamera);
		wf.close();
		delete[] cameraBytes;
		if (!wf.good())
		{
			return;
		}
	}
}


