#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"

using namespace std;

struct ControlPoint {
	glm::vec2 point;
	glm::vec2 handleL;
	glm::vec2 handleR;
	glm::vec4 color;

	ControlPoint() :
		point(glm::vec2(0.0)),
		handleL(glm::vec2(0.0)),
		handleR(glm::vec2(0.0)),
		color(glm::vec4(1.0))
	{

	}

	ControlPoint(glm::vec2 pos) :
		point(pos),
		handleL(pos + glm::vec2(1.0,1.0)),
		handleR(pos),
		color(glm::vec4(1.0))
	{

	}

	ControlPoint(glm::vec2 p1, glm::vec2 p2) :
		point(p1),
		handleL(p1),
		handleR(p2),
		color(glm::vec4(1.0))
	{

	}

	ControlPoint(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) :
		point(p1),
		handleL(p2),
		handleR(p3),
		color(glm::vec4(1.0))
	{

	}
};

class Spline
{
public:
	static int spl_id_timer;
	vector<ControlPoint*> control;
	vector<pair<double, double>> LUT_lengths;
	DrawObject* draw_object;
	double total_length;
	unsigned int spl_id;

	void updatePoints();
	void computeLUT();
	void addControlPoint(glm::vec2 p1);
	static glm::vec2 pointAt(ControlPoint* p1, ControlPoint* p2, float t);
	void drawSpline(int def, glm::vec2 camera_pos);

	Spline() :
		control(vector<ControlPoint*>()),
		draw_object(new DrawObject()),
		spl_id(++spl_id_timer)
	{
		
	}

	Spline(glm::vec2 p1) : Spline() {
		addControlPoint(p1);
		updatePoints();
	}



};

