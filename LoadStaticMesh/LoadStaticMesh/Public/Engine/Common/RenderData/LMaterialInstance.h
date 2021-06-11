#pragma once

#include "stdafx.h"
#include "LResource.h"
#include "LMaterialBase.h"

#include "LMaterial.h"

class LMaterialInstance : public LMaterialBase
{
public:
	LMaterialInstance();
	virtual ~LMaterialInstance();
	void InitMaterialTemplate(LMaterial* InParent);
private:
	void InitByParent();
	
	LMaterial* ParentMaterial;
};