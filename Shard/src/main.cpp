#include <iostream>
#include <string>
#include "Scene.h"
#include "camera.h"
#include "Spline.h"
#include "Renderer.h"
#include "MainWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

double lastMouseX = 0.0;
double lastMouseY = 0.0;
double mouseDeltaX = 0.0;
double mouseDeltaY = 0.0;

bool cursor;

float	p_f[8] = { 25.0f / 255.0f, 25.0f / 255.0f, 25.0f / 255.0f, 255.0f / 255.0f, 3.0, 45.0, 2.0, 5.5 };
int		p_i[8] = { 10, 0, 0, 0, 0, 0, 0, 0 };
bool	p_b[8] = { true, true, false, false, false, false, false, false };
bool	i_p[6] = { false, false ,false ,false ,false ,false };
bool	click[3] = { false, false ,false };
bool	drag[3] = { false, false ,false };

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void initScene() {
	Scene::paramsf = p_f;
	Scene::paramsi = p_i;
	Scene::paramsb = p_b;
	Scene::is_pressed = i_p;
	Scene::click = click;
	Scene::mouse_drag = drag;
	Scene::main_camera = new LookAtCamera(glm::vec3(0.0), glm::vec3(0.0,1.0,0.0), 90, 0);
	Scene::objects = vector<Spline*>();
	Scene::Zoom = 50.0f;
	Scene::deltaTime = 0.0;
	Scene::lastTime = glfwGetTime();
}

void updateCamera() {
	Scene::main_camera->ScreenRatio = 1.0f * MainWindow::SCR_WIDTH / MainWindow::SCR_HEIGHT;
	((LookAtCamera*)(Scene::main_camera))->target = glm::vec3(0.0, 0.0, 0.0);
	Scene::main_camera->Zoom = p_f[5];
	((LookAtCamera*)(Scene::main_camera))->distanceFromTarget = Scene::Zoom;
	Renderer::FRAME_WIDTH = MainWindow::SCR_WIDTH;
	Renderer::FRAME_HEIGHT = MainWindow::SCR_HEIGHT;
	if (cursor) {
		((LookAtCamera*)(Scene::main_camera))->ProcessMouseMovement(mouseDeltaX, mouseDeltaY);
	}
	((LookAtCamera*)(Scene::main_camera))->updateCameraVectors();
}

int main() {


	glfwSetErrorCallback(MainWindow::glfw_error_callback);
	if (!glfwInit()) return 1;


	glfwWindowHint(GLFW_SAMPLES, 4);

	MainWindow::window = glfwCreateWindow(MainWindow::SCR_WIDTH, MainWindow::SCR_HEIGHT, "Program", NULL, NULL);
	if (MainWindow::window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(MainWindow::window);
	glfwSwapInterval(0);

	glfwSetFramebufferSizeCallback(MainWindow::window, MainWindow::framebuffer_size_callback);
	glfwSetScrollCallback(MainWindow::window, scroll_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// init ui
	MainWindow::initUI();
	// setup
	initScene();
	Renderer::setupPrimitives();

	Mesh ms("meshes/unitCube.obj");
	DrawObject d = Renderer::sprite_primitive;
	cursor = false;

	Renderer::LoadTexture("textures/img1.png");

	vector<Transform> trs;
	int n = 1000;
	for (int i = -n; i <= n; i++) {
		for (int j = -n; j <= n; j++) {
			trs.push_back(
				Transform(
					vec3(0.0 + i * p_f[4] * 2.1f, 0.0 + j * p_f[4] * 2.1f, 0.0),
					quat(1.0, vec3(0.0, 0.0, 0.0)),
					vec3(p_f[4])
				)
			);
		}
	}

	Renderer::BuildInstanceVAO(trs, &d);

	while (MainWindow::is_open()) {

		glfwPollEvents();
		MainWindow::handle_input(MainWindow::window, 1.0f);

		double mouseX, mouseY;
		glfwGetCursorPos(MainWindow::window, &mouseX, &mouseY);
		mouseDeltaX = mouseX - lastMouseX;
		mouseDeltaY = lastMouseY - mouseY;
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		
		if (Scene::is_pressed[4]) {
			cursor = true;
			glfwSetInputMode(MainWindow::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		if (Scene::is_pressed[5]) {
			cursor = false;
			glfwSetInputMode(MainWindow::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		
		Renderer::pq.push(make_tuple(
			0,
			&d,
			new Transform(
				vec3(0.0, 0.0, 0.0),
				quat(1.0, vec3(0.0, 0.0, 0.0)),
				vec3(1.0)
			),
			(2 * n + 1) * (2 * n + 1)
		));

		// update camera
		updateCamera();

		Renderer::drawFrame(glm::vec4(p_f[0], p_f[1], p_f[2], 1.0f), Scene::main_camera, Scene::paramsf[4]);

		MainWindow::drawUI();

		glfwSwapBuffers(MainWindow::window);

		Scene::deltaTime = glfwGetTime() - Scene::lastTime;
		Scene::lastTime = glfwGetTime();
	}
	// clean up
	Renderer::UnloadTextures();
	MainWindow::cleanupUI();

	return 0;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Scene::Zoom -= yoffset;
}