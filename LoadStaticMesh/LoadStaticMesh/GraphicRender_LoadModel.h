#pragma once

#include "GraphicRender.h"

class GraphicRender_LoadModel : public GraphicRender
{
public:
	GraphicRender_LoadModel();
	~GraphicRender_LoadModel();

	virtual void OnInit();
	virtual bool Render();
	virtual void Destroy();

};