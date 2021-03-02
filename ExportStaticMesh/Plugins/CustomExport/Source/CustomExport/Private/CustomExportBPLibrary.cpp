// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomExportBPLibrary.h"
#include "CustomExport.h"
#include "Runtime/RawMesh/Public/RawMesh.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/PrettyJsonPrintPolicy.h"


typedef TJsonWriter< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriter;
typedef TJsonWriterFactory< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriterFactory;

static FString SavePath = FPaths::ProjectContentDir() + "Save/";
static FString CameraFileName = TEXT("camera.json");
static FString CameraBinaryFileName = TEXT("camera.bin");

static FString MeshFileName = TEXT("mesh.json");

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
	auto& LODResources = Mesh->RenderData->LODResources;
	const TArray<FStaticMeshSourceModel>& SourceModels = Mesh->GetSourceModels();
	FRawMesh RawMesh;
	SourceModels[0].LoadRawMesh(RawMesh);

	/*TArray<TSharedPtr<FJsonValue>> JsonConnectedArray;
	for (auto& j : Waypoints[i].Connected)
	{
		JsonConnectedArray.Emplace(MakeShareable(new FJsonValueNumber(j)));
	}*/
}

void UCustomExportBPLibrary::ExportCamera(const UCameraComponent* Component)
{
	const FVector& CameraLocation = Component->GetComponentLocation();
	const FRotator& CameraRot = Component->GetComponentRotation();
	float FOV = Component->FieldOfView;
	float AspectRatio = Component->AspectRatio;
	//export binary
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

		//change json to byte array
		TArray<uint8> JsonBytes;
		JsonBytes.AddUninitialized(Json.Len());
		StringToBytes(Json, JsonBytes.GetData(), Json.Len());

		//save json file
		FString CameraSaveFile = SavePath + CameraFileName;
		FFileHelper::SaveStringToFile(Json, *CameraSaveFile);

		//sava binary file
		/*CameraSaveFile = SavePath + CameraBinaryFileName;
		FFileHelper::SaveArrayToFile(JsonBytes, *CameraSaveFile);*/
	}

	//test load json file
	/*FString JsonStr;
	FString CameraSaveFile = SavePath + CameraFileName;
	FFileHelper::LoadFileToString(JsonStr, *CameraSaveFile);
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JsonStr);
	TSharedPtr<FJsonObject> JsonObject;*/

	//test load binary
	/*TArray<uint8> TempBytes;
	FString CameraSaveFile = SavePath + CameraBinaryFileName;
	FFileHelper::LoadFileToArray(TempBytes, *CameraSaveFile);
	FString JsonStr = BytesToString(TempBytes.GetData(), TempBytes.Num());
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JsonStr);
	TSharedPtr<FJsonObject> JsonObject;*/

	/*if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		TArray<FString> ArrayJson;
		JsonObject->TryGetStringArrayField("location", ArrayJson);
		FVector Location = FVector{ FCString::Atof(*ArrayJson[0]),
							  FCString::Atof(*ArrayJson[1]),
					  FCString::Atof(*ArrayJson[2]) };

		TArray<FString> ArrayRotJson;
		JsonObject->TryGetStringArrayField("rotator", ArrayRotJson);
		FRotator Rotation = FRotator{ FCString::Atof(*ArrayRotJson[0]),
							  FCString::Atof(*ArrayRotJson[1]),
					  FCString::Atof(*ArrayRotJson[2]) };

		float FOV = JsonObject->GetNumberField(TEXT("fov"));
		float aspect = JsonObject->GetNumberField(TEXT("aspect"));
	}*/
}


