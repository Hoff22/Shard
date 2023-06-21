#include "Spline.h"

const int RESOLUTION = 30;
int Spline::spl_id_timer = 0;

void Spline::updatePoints() {
	this->control[0]->handleL = this->control[0]->point;
	this->control.back()->handleR = this->control.back()->point;

	this->computeLUT();
}

void Spline::addControlPoint(glm::vec2 p1) {
	if (this->control.size() > 0) {
		glm::vec2 last_point = this->control.back()->point;
		this->control.back()->handleR = this->control.back()->point + glm::normalize(p1 - last_point);
		this->control.push_back(new ControlPoint(p1, p1 + glm::normalize(last_point - p1), p1));
	}
	else {
		this->control.push_back(new ControlPoint(p1, p1 + glm::vec2(3.0, 3.0), p1));
	}
	this->updatePoints();
}

glm::vec2 lerp(glm::vec2 p1, glm::vec2 p2, float t) {
	return p1 + (p2 - p1) * t;
}

glm::vec2 Spline::pointAt(ControlPoint* p1, ControlPoint* p2,  float t) {
	glm::vec2 p3 = lerp(lerp(p1->point, p1->handleR, t), lerp(p1->handleR, p2->handleL, t), t);
	glm::vec2 p4 = lerp(lerp(p1->handleR, p2->handleL, t), lerp(p2->handleL, p2->point, t), t);
	return lerp(p3, p4, t);
}

double segmentLen(ControlPoint* p1, ControlPoint* p2) {
	double len = 0;
	glm::vec2 last = p1->point;
	for (int i = 1; i <= 10; i++) {
		glm::vec2 cur = Spline::pointAt(p1, p2, 1.0 * 10 / i);
		len += glm::distance(last, cur);
		last = cur;
	}
	return len;
}

void Spline::computeLUT() {
	//this->LUT_lengths.resize(RESOLUTION * this->control.size() + 1);
	this->LUT_lengths.clear();
	this->LUT_lengths.push_back({ 0.0,0.0 });
	ControlPoint* last = this->control[0];
	for (int i = 1; i < this->control.size(); i++) {

		ControlPoint* cur = this->control[i];

		glm::vec2 last_p = last->point;
		for (int j = 1; j <= RESOLUTION; j++) {
			double t = 1.0 * j / RESOLUTION;
			glm::vec2 p_j = pointAt(last, cur, t);
			this->LUT_lengths.push_back({
				this->LUT_lengths.back().first + glm::distance(last_p, p_j),
				t
			});
			last_p = p_j;
		}

		last = this->control[i];
	}

	this->total_length = this->LUT_lengths.back().first;
}

void Spline::drawSpline(int def = 30, glm::vec2 camera_pos = glm::vec2(0.0)) {
	
	return;// kek
}