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
	
	for (ControlPoint* i : this->control) {
		if (i->handleL != i->point) { 
			Renderer::pq.push(make_tuple(0, &Renderer::circle_primitive, Transform(glm::vec3(i->handleL, 0.0), glm::quat(), glm::vec3(0.2))));
			Renderer::pq.push(make_tuple(
				1,
				&Renderer::line_primitive,
				Transform(glm::vec3(i->handleL, 0.0), glm::quat(), glm::vec3(distance(i->point, i->handleL))).lookAt(glm::vec3(i->point - i->handleL, 0.0f))
			));
		}
		Renderer::pq.push(make_tuple(2, &Renderer::circle_primitive, Transform(glm::vec3(i->point, 0.0), glm::quat(), glm::vec3(0.5))));
		if (i->handleR != i->point) {
			Renderer::pq.push(make_tuple(0, &Renderer::circle_primitive, Transform(glm::vec3(i->handleR, 0.0), glm::quat(), glm::vec3(0.2))));
			Renderer::pq.push(make_tuple(
				1,
				&Renderer::line_primitive,
				Transform(glm::vec3(i->handleR, 0.0), glm::quat(), glm::vec3(distance(i->point, i->handleR))).lookAt(glm::vec3(i->point - i->handleR, 0.0f))
			));
		}
	}

	if (this->control.size() <= 1) return;

	vector<glm::vec2> points;

	points.push_back(this->control[0]->point);
	double next_length = this->total_length / def;
	for (int i = 0; i < this->LUT_lengths.size()-1; i++) {
		int control_point_idx = i / RESOLUTION;
		double ta = this->LUT_lengths[i].second;
		double tb = this->LUT_lengths[i + 1].second;
		double da = this->LUT_lengths[i].first;
		double db = this->LUT_lengths[i + 1].first;
		if (i % RESOLUTION == 0) {
			ta = 0.0f;
			points.push_back(this->control[i/RESOLUTION]->point);
		}

		double delta_t = tb - ta;
		double delta_d = db - da;
		double t;
		while (next_length <= this->LUT_lengths[i + 1].first + 0.002) {
			t = ta + delta_t * ((next_length - da) / delta_d);
			points.push_back(pointAt(this->control[control_point_idx], this->control[control_point_idx + 1], t));
			next_length += this->total_length / def;
		}
	}
	
	vector<float> uv_coeficients;
	vector<float> model_coeficients;
	vector<float> normal_coeficients;
	vector<float> tangent_coeficients;
	vector<GLuint> indices;

	int cur_idx = 0;
	glm::vec4 last_normal(0.0);
	for (int i = 0; i < points.size(); i++) {

		glm::vec4 tangent;

		if (i == points.size() - 1) {
			tangent = glm::vec4(points[i] - points[i - 1], 0.0, 0.0);
		}
		else {
			tangent = glm::vec4(points[i + 1] - points[i], 0.0, 0.0);
		}

		tangent = glm::normalize(tangent);

		glm::mat4 rotate_90 = glm::rotate(glm::identity<mat4>(), glm::half_pi<float>(), glm::vec3(0.0, 0.0, 1.0));

		glm::vec4 curve_normal = rotate_90 * tangent;

		curve_normal = glm::normalize(curve_normal);

		glm::vec4 cur_offset = (last_normal + curve_normal) / 2.0f;
		if (i <= 1) cur_offset = curve_normal;

		cur_offset = glm::normalize(cur_offset);

		model_coeficients.push_back(points[i].x + cur_offset.x * 0.001);
		model_coeficients.push_back(points[i].y + cur_offset.y * 0.001);
		model_coeficients.push_back(0.0f);
		model_coeficients.push_back(1.0f);

		uv_coeficients.push_back(1.0);
		uv_coeficients.push_back(0.0);

		indices.push_back(cur_idx++);

		model_coeficients.push_back(points[i].x - cur_offset.x * 0.001);
		model_coeficients.push_back(points[i].y - cur_offset.y * 0.001);
		model_coeficients.push_back(0.0f);
		model_coeficients.push_back(1.0f);

		uv_coeficients.push_back(0.0);
		uv_coeficients.push_back(0.0);

		indices.push_back(cur_idx++);

		normal_coeficients.push_back(curve_normal.x);
		normal_coeficients.push_back(curve_normal.y);
		normal_coeficients.push_back(curve_normal.z);
		normal_coeficients.push_back(curve_normal.w);

		tangent_coeficients.push_back(tangent.x);
		tangent_coeficients.push_back(tangent.y);
		tangent_coeficients.push_back(tangent.z);
		tangent_coeficients.push_back(tangent.w);

		normal_coeficients.push_back(curve_normal.x);
		normal_coeficients.push_back(curve_normal.y);
		normal_coeficients.push_back(curve_normal.z);
		normal_coeficients.push_back(curve_normal.w);

		tangent_coeficients.push_back(tangent.x);
		tangent_coeficients.push_back(tangent.y);
		tangent_coeficients.push_back(tangent.z);
		tangent_coeficients.push_back(tangent.w);

		last_normal = curve_normal;
	}
	indices.push_back(cur_idx-1);
	indices.push_back(cur_idx-1);


	Renderer::BuildTrianglesVAO(model_coeficients, normal_coeficients, tangent_coeficients, uv_coeficients, indices, this->draw_object);
	this->draw_object->indexes_size = indices.size();
	Renderer::pq.push(make_tuple(4, this->draw_object, Transform() ) );
}