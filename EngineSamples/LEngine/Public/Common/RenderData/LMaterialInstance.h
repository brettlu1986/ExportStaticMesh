#pragma once

#include "pch.h"
#include "LResource.h"
#include "LMaterialBase.h"

#include "LMaterial.h"

class LENGINE_API LMaterialInstance : public LMaterialBase
{
public:
	LMaterialInstance();
	virtual ~LMaterialInstance();
	void InitMaterialTemplate(LMaterial* InParent);
private:
	void InitByParent();
	
	LMaterial* ParentMaterial;
};