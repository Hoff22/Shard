#pragma once
#include <vector>
#include "Spline.h"
#include "camera.h"

class Scene
{
public:
	static Camera*				main_camera;
	static std::vector<Spline*>	objects;
	static float*				paramsf;
	static int*					paramsi;
	static bool*				paramsb;
	static bool*				click;
	static bool*				mouse_drag;
	static bool*				is_pressed;
	static float				Zoom;
	static float				deltaTime;
	static float				lastTime;
};

