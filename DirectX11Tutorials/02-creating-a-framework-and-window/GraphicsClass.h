#pragma once

#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include <Windows.h>

const bool IS_FULL_SCREEN = false;
const bool IS_VSYNC = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();
};

#endif