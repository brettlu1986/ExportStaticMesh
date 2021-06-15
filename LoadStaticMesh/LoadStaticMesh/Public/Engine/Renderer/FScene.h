#pragma once

#include "stdafx.h"
#include "FResourceViewCreater.h"
#include "FSkeletalMesh.h"

class FMesh;
class FLight;
class FScene
{
public: 
	FScene();
	~FScene();

	void AddMeshToScene(FMesh* Mesh);
	void DeleteMeshToScene(FMesh* Mesh);
	void AddSkeletalMeshToScene(FSkeletalMesh* Mesh);
	void DeleteSkeletalMeshToScene(FSkeletalMesh* Mesh);
	void AddLightToScene(FLight* Light);
	void DeleteLightToScene(FLight* Light);
	void UpdateLightToScene(FLight* Light);

	void Destroy();
	const vector<FMesh*>& GetDrawMeshes()
	{
		return Meshes;
	}

	const vector<FMesh*>& GetDrawTransparencyMeshes();

	
	const vector<FSkeletalMesh*>& GetDrawSkeletalMeshes() 
	{
		return SkmMeshes;
	}

	void UpdateViewProjInfo(FPassViewProjection UpdateViewProj);
	void UpdateLightInfo(FPassLightInfo UpdateLight);
	
	FResourceView* PassViewProj;
	FResourceView* PassLightInfo;
	
private: 
	
	vector<FSkeletalMesh*> SkmMeshes;

	vector<FMesh*> Meshes;
	vector<FMesh*> TranparencyMeshes;

	vector<FLight*> SceneLights;

	FPassViewProjection ViewProjInfo;
	FPassLightInfo LightInfo;
};
