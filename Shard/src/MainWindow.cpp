#include "MainWindow.h"

int MainWindow::SCR_WIDTH = 1000;
int MainWindow::SCR_HEIGHT = 1000;
bool MainWindow::is_open_bool;
GLFWwindow* MainWindow::window;
bool options_hovered;

int MainWindow::initUI() {
	// FAZER ISSO NA MAIN
	
	/*
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) return 1;

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Program", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}*/
	
	is_open_bool = 1;

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;    // Enable mouse Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 20);
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	//	ImGuiDockNodeFlags_PassthruCentralNode
	return 0;
}

void MainWindow::drawUI() {
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// start UI frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// setup dockspace of main glfw window
	// this makes glfw window dockable but still see-thru. remove flag if opaque background is needed
	// also must come befor any window declaration
	ImGui::DockSpaceOverViewport(0, ImGuiDockNodeFlags_PassthruCentralNode);
	
	// drawOptions MUST come before drawViewport
	drawOptions();
	
	// Renders the ImGUI elements
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void MainWindow::cleanupUI() {
	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

int MainWindow::is_open() {
	return (!(glfwWindowShouldClose(window)) and is_open_bool);
}

void MainWindow::handle_input(GLFWwindow* window, float _speed) {

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	if (ImGui::IsKeyDown('W')) {
		Scene::is_pressed[2] = true;
	}
	else Scene::is_pressed[2] = false;

	if (ImGui::IsKeyDown('A')) {
		Scene::is_pressed[3] = true;

	}
	else Scene::is_pressed[3] = false;

	if (ImGui::IsKeyDown('S')) {
		Scene::is_pressed[4] = true;

	}
	else Scene::is_pressed[4] = false;

	if (ImGui::IsKeyDown('D')) {
		Scene::is_pressed[5] = true;

	}
	else Scene::is_pressed[5] = false;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		Scene::click[0] = true;
	}
	else {
		Scene::click[0] = false;
	}
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		Scene::click[1] = true;
	}
	else {
		Scene::click[1] = false;
	}
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
		Scene::click[2] = true;
	}
	else {
		Scene::click[2] = false;
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		Scene::mouse_drag[0] = true;
	}
	else Scene::mouse_drag[0] = false;
	
	if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
		Scene::mouse_drag[2] = true;
	}
	else Scene::mouse_drag[2] = false;	

	if (ImGui::IsKeyDown(ImGuiKey_Space)) {
	}
	if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
	}
}

void MainWindow::drawOptions() {
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImVec4 color = ImVec4(Scene::paramsf[0], Scene::paramsf[1], Scene::paramsf[2], Scene::paramsf[3]);
	ImVec4 backup_color = color;

	ImGui::SetNextWindowSize(ImVec2(360,800));
	ImGui::Begin("Options", 0, ImGuiWindowFlags_NoResize);
		ImGui::Text("BACKGROUND COLOR");
		ImGui::Separator();
		ImGui::ColorPicker4("##picker", (float*)Scene::paramsf, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
		ImGui::SameLine();

		ImGui::BeginGroup(); // Lock X position
		ImGui::Text("Current");
		ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
		double xpos;
		double ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		ImGui::Text("x: %d", std::min(std::max((int)xpos, 0), SCR_WIDTH));
		ImGui::Text("y: %d", std::min(std::max((int)ypos, 0), SCR_HEIGHT));
		ImGui::Separator();
		ImGui::Text("FPS: %d", (int)(1.0f / Scene::deltaTime));
		ImGui::EndGroup();
		ImGui::SliderFloat("FOV", &Scene::paramsf[5], 0.0f, 360.0f, "%.2f");
		ImGui::SliderFloat("Zoom", &Scene::Zoom, 0.0f, 100.0f, "%.2f");
		ImGui::SliderFloat("Model Size", &Scene::paramsf[4], 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Rotate", &Scene::paramsf[10], 0.0, 2048.0, "%.3f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Diffuse", &Scene::paramsf[6], 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Specular", &Scene::paramsf[7], 0.0f, 100.0f, "%.2f");
		ImGui::SliderFloat("Reflection", &Scene::paramsf[8], 0.0f, 100.0f, "%.2f");
		ImGui::SliderFloat("Roughness", &Scene::paramsf[9], 1.0f, 100.0f, "%.2f");
		if (ImGui::Button("Clear")) {
			Scene::paramsb[0] = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Shaders")) {
			Scene::paramsb[1] = 1;
		}

	ImGui::End();
}

void MainWindow::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{    // make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.

	MainWindow::SCR_HEIGHT = height;
	MainWindow::SCR_WIDTH = width;

	return;
}