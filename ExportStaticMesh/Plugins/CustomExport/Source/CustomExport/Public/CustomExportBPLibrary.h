// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include <vector>
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "CustomExportBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/

UENUM(BlueprintType)
enum class EVsFormat : uint8
{
	POSITION = 0 UMETA(DisplayName = "POSITION"),
	NORMAL		 UMETA(DisplayName = "NORMAL"),
	TANGENT      UMETA(DisplayName = "TANGENT"),
	TEX0		 UMETA(DisplayName = "TEX0"),
	TEX1		 UMETA(DisplayName = "TEX1"),
	COLOR		 UMETA(DisplayName = "COLOR"),
	MAX,
};

#pragma pack(push)
#pragma pack(4)

USTRUCT(BlueprintType)
struct FMeshDataJson
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> WorldLocation;

	UPROPERTY()
	TArray<float> WorldRotation;

	UPROPERTY()
	TArray<float> WorldScale;

	UPROPERTY()
	TArray<FString> VsFormat;

	UPROPERTY()
	TArray<float> Positions;

	UPROPERTY()
	TArray<float> Normals;

	UPROPERTY()
	TArray<float> Tangents;

	UPROPERTY()
	TArray<float> Tex0s;

	UPROPERTY()
	TArray<float> Tex1s;

	UPROPERTY()
	TArray<float> Colors;

	UPROPERTY()
	TArray<uint32> Indices;
};

USTRUCT(BlueprintType)
struct FVertexDatas
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	FVector Normal;

	UPROPERTY()
	FVector Tangent;

	UPROPERTY()
	FVector2D Tex0;

	UPROPERTY()
	FVector2D Tex1;

	UPROPERTY()
	FLinearColor Color;

	FVertexDatas()
		:Position(FVector::ZeroVector)
		, Normal(FVector::ZeroVector)
		, Tangent(FVector::ZeroVector)
		, Tex0(FVector2D::ZeroVector)
		, Tex1(FVector2D::ZeroVector)
		, Color(FLinearColor::White)
	{
	}
};

USTRUCT(BlueprintType)
struct FMeshDataBinary
{
	GENERATED_BODY()

	UPROPERTY()
	FVector WorldLocation;

	UPROPERTY()
	FRotator WorldRotation;

	UPROPERTY()
	FVector WorldScale;

	UPROPERTY()
	TArray<FVertexDatas> MeshVertexDatas;

	FMeshDataBinary()
		:WorldLocation(FVector::ZeroVector)
		,WorldRotation(FRotator::ZeroRotator)
		,WorldScale(FVector::ZeroVector)
	{
	}
};

USTRUCT(BlueprintType)
struct FCameraData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector Target;

	UPROPERTY()
	FRotator Rotator;

	UPROPERTY()
	float Fov;

	UPROPERTY()
	float Aspect;
};

USTRUCT(BlueprintType)
struct FDirectionLightData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Strength;

	UPROPERTY()
	FVector Direction;

	UPROPERTY()
	FVector Position;
};

USTRUCT(BlueprintType)
struct FLightData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FDirectionLightData> DirectionLights;
};

USTRUCT(BlueprintType)
struct FSkeletalAnimData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 NumFrames;

	UPROPERTY()
	TArray<struct FRawAnimSequenceTrack> RawAnimationData;

	UPROPERTY()
	TArray<struct FTrackToSkeletonMap> TrackToSkeletonMapTable;
};

#pragma pack(pop)

UCLASS()
class UCustomExportBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Show Message"), Category = "CustomExportBPLibrary")
	static void ShowMessageDialog(const FText Target, const FText Result);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Camera"), Category = "CustomExportBPLibrary")
	static bool ExportCamera(const UCameraComponent* Component, FCameraData CameraData, FString FileName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Static Mesh Actor"), Category = "CustomExportBPLibrary")
	static bool ExportStaticMeshActor(const AStaticMeshActor* MeshActor, FMeshDataJson MeshDataJson, FMeshDataBinary MeshDataBinary, FString FileName);
	static bool ExportStaticMesh(const UStaticMesh* Mesh, FMeshDataJson& MeshDataJson, FMeshDataBinary& MeshDataBinary, FString& FileName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Direction Light"), Category = "CustomExportBPLibrary")
	static bool ExportDirectionLight(const TArray<ADirectionalLight*> DirectionLights, FLightData DataOut, FString FileName);


	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Skeletal AnimSequence"), Category = "CustomExportBPLibrary")
	static bool ExportSkeletalMeshAnim(const UAnimSequence* Anim, FSkeletalAnimData DataOut, FString FileName);

	/*UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Skeletal Mesh Actor"), Category = "CustomExportBPLibrary")
	static bool ExportSkeletalMeshActor(const ACharacter* PlayerActor, const USkeletalMesh* Mesh, FSkeletalMeshDataJson MeshDataJson, FSkeletalMeshDataBinary MeshDataBinary, FString FileName);
	static bool ExportSkeletalMesh(const USkeletalMesh* Mesh, FSkeletalMeshDataJson& MeshDataJson, FSkeletalMeshDataBinary& MeshDataBinary, FString& FileName);*/
};
