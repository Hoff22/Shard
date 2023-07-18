#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Scene.h"

class MainWindow
{
	static bool is_open_bool;

public:
	
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	static void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Scene::Zoom -= yoffset;
	}


	static GLFWwindow*	window;
	static int			SCR_WIDTH;
	static int			SCR_HEIGHT;

	static int	initUI();
	static void	drawUI();
	static void	cleanupUI();
	static int	is_open();
	static void	handle_input(float _speed = 1.0);
	static void	drawOptions();
};

