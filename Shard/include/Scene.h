#pragma once
#include <vector>
#include "camera.h"

class Scene
{
public:
	static Camera*	main_camera;
	static float*	paramsf;
	static int*		paramsi;
	static bool*	paramsb;
	static bool*	click;
	static bool*	mouse_drag;
	static bool*	is_pressed;
	static float	Zoom;
	static double	lastMouseX;
	static double	lastMouseY;
	static double	mouseDeltaX;
	static double	mouseDeltaY;
	static double	deltaTime;
};

