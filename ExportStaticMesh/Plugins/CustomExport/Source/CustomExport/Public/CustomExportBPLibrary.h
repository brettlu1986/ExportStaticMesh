// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include <vector>
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Runtime/Core/Public/Templates/UnrealTemplate.h"
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
	SKINWEIGHT   UMETA(DisplayName = "SKINWEIGHT"),
	MAX,
};

#pragma pack(push)
#pragma pack(4)

USTRUCT(BlueprintType)
struct FSkinMeshWeightInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FVector InfluenceWeights;

	UPROPERTY()
	uint16 InfluenceBones[4];
};

USTRUCT(BlueprintType)
struct FFullVertexDatas
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

	UPROPERTY()
	FVector InfluenceWeights;

	UPROPERTY()
	uint16 InfluenceBones[4];

	FFullVertexDatas()
		:Position(FVector::ZeroVector)
		, Normal(FVector::ZeroVector)
		, Tangent(FVector::ZeroVector)
		, Tex0(FVector2D::ZeroVector)
		, Tex1(FVector2D::ZeroVector)
		, Color(FLinearColor::White)
		, InfluenceWeights(FVector::ZeroVector)
	{
	}
};

USTRUCT(BlueprintType)
struct FSkeRenderSection
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 BaseVertexIndex; 

	UPROPERTY() 
	uint32 NumVertices;

	UPROPERTY() 
	TArray<uint16> BoneMap;
};

USTRUCT(BlueprintType)
struct FFullMeshDataJson
{
	GENERATED_BODY()

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

	UPROPERTY()
	TArray<FSkinMeshWeightInfo> SkinMeshWeight0s;

	UPROPERTY() 
	TArray<FSkeRenderSection> SkeRenderSections;
	
};

USTRUCT(BlueprintType)
struct FFullMeshDataBinary
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FFullVertexDatas> MeshVertexDatas;

	UPROPERTY()
	TArray<uint16> BoneMap;

	FFullMeshDataBinary()
	{
	}
};

USTRUCT(BlueprintType)
struct FMeshDataJson
{
	GENERATED_BODY()

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

	FMeshDataJson(){}
	FMeshDataJson(const FFullMeshDataJson& FullData)
	{
		VsFormat.Append(FullData.VsFormat);
		Positions.Append(FullData.Positions);
		Normals.Append(FullData.Normals);
		Tangents.Append(FullData.Tangents);
		Tex0s.Append(FullData.Tex0s);
		Tex1s.Append(FullData.Tex1s);
		Colors.Append(FullData.Colors);
		Indices.Append(FullData.Indices);
	}
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

	FVertexDatas(const FFullVertexDatas& FullVexData)
	{
		Position = FullVexData.Position;
		Normal = FullVexData.Normal;
		Tangent = FullVexData.Tangent;
		Tex0 = FullVexData.Tex0;
		Tex1 = FullVexData.Tex1;
		Color = FullVexData.Color;
	}
};

USTRUCT(BlueprintType)
struct FMeshDataBinary
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FVertexDatas> MeshVertexDatas;

	FMeshDataBinary()
	{
	}

	FMeshDataBinary(const FFullMeshDataBinary& FullDataBin)
	{

		for(FFullVertexDatas FullVexData : FullDataBin.MeshVertexDatas)
		{
			FVertexDatas VData(FullVexData);
			MeshVertexDatas.Add(VData);
		}

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
struct FAnimSequenceTrack 
{
	GENERATED_BODY()

	/** Position keys. */
	UPROPERTY()
	TArray<FVector> PosKeys;

	/** Rotation keys. */
	UPROPERTY()
	TArray<FRotator> RotKeys;

	/** Scale keys. */
	UPROPERTY()
	TArray<FVector> ScaleKeys;
};


USTRUCT(BlueprintType)
struct FSkeletalAnimData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 NumFrames;

	UPROPERTY()
	TArray<struct FAnimSequenceTrack> RawAnimationData;

	UPROPERTY()
	TArray<struct FTrackToSkeletonMap> TrackToSkeletonMapTable;
};

//use to export bin
struct FSkeletonBoneInfo
{
	char Name[64];
	int32 ParentIndex;
};

USTRUCT(BlueprintType)
struct FSkeletonBonePose
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Scale;

	UPROPERTY()
	FRotator Rotate;

	UPROPERTY()
	FVector Translate;
};

struct FSkeletonNameIndex
{
	char Name[64];
	int32 Index;
};

USTRUCT(BlueprintType)
struct FSkeletalBoneInfoJson
{
	GENERATED_BODY()

	UPROPERTY()
	FName Name;

	UPROPERTY()
	int32 ParentIndex;
};

USTRUCT(BlueprintType)
struct FSkeletonData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSkeletalBoneInfoJson> RawRefBoneInfo;

	UPROPERTY()
	TArray<FSkeletonBonePose> RawRefBonePose;

	UPROPERTY()
	TMap<FName, int32> RawNameToIndexMap;
};

USTRUCT(BlueprintType)
struct FAssetsDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RefName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FileName;
};

USTRUCT(BlueprintType)
struct FMapStaticMeshes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ObjectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RefGeometry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RefMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator WorldRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldScale;
};

USTRUCT(BlueprintType)
struct FMapSkeletalMeshes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ObjectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RefGeometry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RefMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RefSkeleton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString>	 RefAnims;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator WorldRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldScale;
};

USTRUCT(BlueprintType)
struct FMapExport
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FMapStaticMeshes> StaticMeshes;

	UPROPERTY(BlueprintReadWrite)
	TArray<FMapSkeletalMeshes> SkeletalMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SceneLightsFile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CameraFile;
};


USTRUCT(BlueprintType)
struct FAssetsExport
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FAssetsDef> Skeletons;

	UPROPERTY(BlueprintReadWrite)
	TArray<FAssetsDef> ShaderFiles;

	UPROPERTY(BlueprintReadWrite)
	TArray<FAssetsDef> Textures;

	UPROPERTY(BlueprintReadWrite)
	TArray<FAssetsDef> Materials;

	UPROPERTY(BlueprintReadWrite)
	TArray<FAssetsDef> MaterialInstances;

	UPROPERTY(BlueprintReadWrite)
	TArray<FAssetsDef> Animations;

	UPROPERTY(BlueprintReadWrite)
	TArray<FAssetsDef> MeshGeometries;

	UPROPERTY(BlueprintReadWrite)
	TArray<FAssetsDef> SkeletalMeshGeometries;

};

UENUM(BlueprintType)
enum class EMaterialParamType : uint8
{
	TYPE_FLOAT = 0 UMETA(DisplayName = "FLOAT"),
	TYPE_VECTOR		 UMETA(DisplayName = "VECTOR"),
	TYPE_TEXTURE UMETA(DisplayName = "TEXTURE"),
	MAX,
};

USTRUCT(BlueprintType)
struct FMaterialExport
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString RefParent; 

	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> ParamTypes;

	UPROPERTY(BlueprintReadWrite)
	TArray<float> ParamFloatValues;

	UPROPERTY(BlueprintReadWrite)
	TArray<FLinearColor> ParamVectorValues;

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> ParamRefTextures;
};

#pragma pack(pop)

UCLASS()
class UCustomExportBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Show Message"), Category = "CustomExportBPLibrary")
	static void ShowMessageDialog(const FText Target, const FText Result);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Assets"), Category = "CustomExportBPLibrary")
	static bool ExportsAssets(TArray<FAssetsDef> Skeletons, TArray<FAssetsDef> Shaders, TArray<FAssetsDef> Textures, 
		TArray<FAssetsDef> Materials, TArray<FAssetsDef> MaterialsIns, TArray<FAssetsDef> Animations, TArray<FAssetsDef> MeshGeometries, 
		TArray<FAssetsDef> SkeletalMeshGeometries, FAssetsExport Out, FString FileBaseName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Map"), Category = "CustomExportBPLibrary")
	static bool ExportsMap(TArray<FMapStaticMeshes> StaticMeshes, TArray<FMapSkeletalMeshes> SkeletalMeshes, FString SceneLightsFile, FString CameraFile, FMapExport Out, FString FileBaseName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Material"), Category = "CustomExportBPLibrary")
	static bool ExportMaterial(UMaterialInterface* Material, FString FileBaseName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Camera"), Category = "CustomExportBPLibrary")
	static bool ExportCamera(const UCameraComponent* Component, FCameraData CameraData, FString FileName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Static Mesh Actor"), Category = "CustomExportBPLibrary")
	static bool ExportStaticMeshActor(const AStaticMeshActor* MeshActor, FFullMeshDataJson MeshDataJson, FFullMeshDataBinary MeshDataBinary, FString FileName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Direction Light"), Category = "CustomExportBPLibrary")
	static bool ExportDirectionLight(const TArray<ADirectionalLight*> DirectionLights, FLightData DataOut, FString FileName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Skeletal AnimSequence"), Category = "CustomExportBPLibrary")
	static bool ExportSkeletalMeshAnim(const UAnimSequence* Anim, FSkeletalAnimData DataOut, FString FileName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Skeletal Mesh Actor"), Category = "CustomExportBPLibrary")
	static bool ExportSkeletalMeshActor(const ACharacter* PlayerActor, const USkeletalMesh* Mesh, FFullMeshDataJson MeshDataJson, FFullMeshDataBinary MeshDataBinary, FString FileName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Skeleton"), Category = "CustomExportBPLibrary")
	static bool ExportSkeleton(const USkeleton* Skeleton, FSkeletonData DataOut, FString FileName);
};
