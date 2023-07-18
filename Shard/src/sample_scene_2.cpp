#include <iostream>
#include <string>
#include "Scene.h"
#include "Renderer.h"
#include "MainWindow.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;


float	p_f[11] = {
	25.0f / 255.0f,
	25.0f / 255.0f,
	25.0f / 255.0f,
	255.0f / 255.0f,
	1.0f,
	45.0,
	100.0,
	100.0,
	0.3,
	20.0,
	0.0f
};
int		p_i[8] = { 10, 0, 0, 0, 0, 0, 0, 0 };
bool	p_b[8] = { true, true, false, false, false, false, false, false };
bool	i_p[6] = { false, false ,false ,false ,false ,false };
bool	click[3] = { false, false ,false };
bool	drag[3] = { false, false ,false };
bool cursor;
double curTime;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void initScene() {
	Scene::paramsf = p_f;
	Scene::paramsi = p_i;
	Scene::paramsb = p_b;
	Scene::is_pressed = i_p;
	Scene::click = click;
	Scene::mouse_drag = drag;
	Scene::main_camera = new LookAtCamera(glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0), 90, -15);
	Scene::Zoom = 50.0f;
	Scene::deltaTime = 0.0;
	Scene::lastMouseX = 0.0f;
	Scene::lastMouseY = 0.0f;
	Scene::mouseDeltaX = 0.0f;
	Scene::mouseDeltaY = 0.0f;
}

void updateCamera() {
	Scene::main_camera->ScreenRatio = 1.0f * MainWindow::SCR_WIDTH / MainWindow::SCR_HEIGHT;
	((LookAtCamera*)(Scene::main_camera))->target = glm::vec3(0.0, 0.0, 0.0);
	Scene::main_camera->Zoom = p_f[5];
	((LookAtCamera*)(Scene::main_camera))->distanceFromTarget = Scene::Zoom;
	if (cursor) {
		((LookAtCamera*)(Scene::main_camera))->ProcessMouseMovement(Scene::mouseDeltaX, Scene::mouseDeltaY);
	}
	((LookAtCamera*)(Scene::main_camera))->updateCameraVectors();
}

int main() {

	Renderer::Setup_GLAD_GLFW_OpenGL_Shard("Shard Sample 2");
	initScene();

	curTime = glfwGetTime();

	Mesh ms("meshes/plane.obj");
	DrawObject d_instanced;
	d_instanced.indexes_size = (int)ms.indices.size();
	Renderer::BuildTrianglesVAO(ms.model_coefficients, ms.normal_coefficients, ms.tangent_coefficients, ms.texture_coefficients, ms.indices, &d_instanced);
	d_instanced.material = Renderer::default_material;
	cursor = false;

	Renderer::LoadTextureRGB("textures/img1.png");
	Renderer::LoadTextureRGB("textures/img3.png");

	while (MainWindow::is_open()) {
		Scene::deltaTime = glfwGetTime() - curTime;
		curTime = glfwGetTime();

		MainWindow::handle_input();

		MainWindow::drawUI();

		if (p_b[1]) {
			p_b[1] = 0;
			Renderer::noShadingShader = Shader("src/shaders/default_vertex.glsl", "src/shaders/noShading_fragment.glsl");
			Renderer::PBRShader = Shader("src/shaders/pbr_vertex.glsl", "src/shaders/pbr_fragment.glsl");
			Renderer::gizmoShader = Shader("src/shaders/default_vertex.glsl", "src/shaders/gizmo_fragment.glsl");
		}

		if (Scene::is_pressed[4]) {
			cursor = true;
			glfwSetInputMode(MainWindow::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		if (Scene::is_pressed[5]) {
			cursor = false;
			glfwSetInputMode(MainWindow::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		Transform tr;
		Transform tr_instanced(
			vec3(0.0f, 0.0f, 0.0f),
			quat(1.0, vec3(0.0, 0.0, 0.0)),
			vec3(p_f[4])
		);

		Renderer::pq.push(make_tuple(
			0,
			&d_instanced,
			&tr_instanced,
			0
		));

		d_instanced.material.v4params[4] = vec4(p_f[6], p_f[7], p_f[8], p_f[9]);
		d_instanced.material.fparams[0] = p_f[10];

		glm::vec4 bg_color = glm::vec4(p_f[0], p_f[1], p_f[2], p_f[3]);

		updateCamera();
		Renderer::DrawFrame(bg_color, Scene::main_camera);
	}
	// clean up
	Renderer::UnloadTextures();
	MainWindow::cleanupUI();

	return 0;
}