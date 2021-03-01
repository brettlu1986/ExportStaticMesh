#pragma once

#include "ApplicationMain.h"

class GraphicRender
{

public:
	GraphicRender();
	~GraphicRender();

	void Initialize(ApplicationMain* application);
	bool Render();
	void Destroy();
public:

	ApplicationMain* m_application;
};