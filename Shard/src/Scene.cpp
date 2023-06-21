#include "Scene.h"

using namespace std;

Camera*			Scene::main_camera = 0;
vector<Spline*>	Scene::objects = vector<Spline*>();
float*			Scene::paramsf = 0;
int*			Scene::paramsi = 0;
bool*			Scene::paramsb = 0;
bool*			Scene::click = 0;
bool*			Scene::mouse_drag = 0;
bool*			Scene::is_pressed = 0;
float			Scene::Zoom = 0;
float			Scene::deltaTime = 0;
float			Scene::lastTime = 0;