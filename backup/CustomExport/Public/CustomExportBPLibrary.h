// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Camera/CameraComponent.h"
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

#pragma pack(push)
#pragma pack(4)
struct Location
{
	float x;
	float y;
	float z;
};

struct Rotator
{
	float Pitch;
	float Yaw;
	float Roll;
};

struct CameraData {
	Location location;
	Rotator rotator;
	float fov;
	float aspect;
};
#pragma pack(pop)

UCLASS()
class UCustomExportBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Static Mesh"), Category = "CustomExportBPLibrary")
	static void ExportStaticMesh(const UStaticMesh* Mesh);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Camera"), Category = "CustomExportBPLibrary")
	static void ExportCamera(const UCameraComponent* Component);

};
