#pragma once

#include "GraphicRender.h"

class GraphicRender_ClearWindow : public GraphicRender
{
public:
	GraphicRender_ClearWindow();
	~GraphicRender_ClearWindow();

	virtual void OnInit() ;
	virtual bool Render() ;
	virtual void Destroy();

};