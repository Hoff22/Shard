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
	static glm::vec2			camera_pos;
	static float				Zoom;
};

