#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

#include <tiny_obj_loader.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Transform.h"
#include "camera.h"
#include "Shader.h"

using namespace std;

struct Mesh {
	vector<GLuint> indices;
	vector<float>  model_coefficients;
	vector<float>  normal_coefficients;
	vector<float>  tangent_coefficients;
	vector<float>  texture_coefficients;

	tinyobj::attrib_t                 attrib;
	std::vector<tinyobj::shape_t>     shapes;
	std::vector<tinyobj::material_t>  materials;

	size_t first_index;
	size_t num_triangles;

	float minval;
	float maxval;

	glm::vec3 bbox_min;
	glm::vec3 bbox_max;

	Mesh(const char* filename, const char* basepath = NULL, bool triangulate = true)
	{
		printf("Carregando modelo \"%s\"... ", filename);

		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, basepath, triangulate);

		if (!err.empty())
			fprintf(stderr, "\n%s\n", err.c_str());

		if (!ret)
			throw std::runtime_error("Erro ao carregar modelo.");

		if (shapes.size() > 1) {
			printf("Model path %s has more than one shape\n", filename);
			return;
		}

		first_index = indices.size();
		num_triangles = shapes[0].mesh.num_face_vertices.size();

		minval = std::numeric_limits<float>::min();
		maxval = std::numeric_limits<float>::max();

		bbox_min = glm::vec3(maxval, maxval, maxval);
		bbox_max = glm::vec3(minval, minval, minval);

		for (size_t triangle = 0; triangle < num_triangles; ++triangle)
		{
			assert(shapes[0].mesh.num_face_vertices[triangle] == 3);


			for (size_t vertex = 0; vertex < 3; ++vertex)
			{
				tinyobj::index_t idx = shapes[0].mesh.indices[3 * triangle + vertex];

				indices.push_back(first_index + 3 * triangle + vertex);

				const float vx = attrib.vertices[3 * idx.vertex_index + 0];
				const float vy = attrib.vertices[3 * idx.vertex_index + 1];
				const float vz = attrib.vertices[3 * idx.vertex_index + 2];
				//printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
				model_coefficients.push_back(vx); // X
				model_coefficients.push_back(vy); // Y
				model_coefficients.push_back(vz); // Z
				model_coefficients.push_back(1.0f); // W

				bbox_min.x = std::min(bbox_min.x, vx);
				bbox_min.y = std::min(bbox_min.y, vy);
				bbox_min.z = std::min(bbox_min.z, vz);
				bbox_max.x = std::max(bbox_max.x, vx);
				bbox_max.y = std::max(bbox_max.y, vy);
				bbox_max.z = std::max(bbox_max.z, vz);

				// Inspecionando o código da tinyobjloader, o aluno Bernardo
				// Sulzbach (2017/1) apontou que a maneira correta de testar se
				// existem normais e coordenadas de textura no ObjModel é
				// comparando se o índice retornado é -1. Fazemos isso abaixo.

				if (idx.normal_index != -1)
				{
					const float nx = attrib.normals[3 * idx.normal_index + 0];
					const float ny = attrib.normals[3 * idx.normal_index + 1];
					const float nz = attrib.normals[3 * idx.normal_index + 2];
					normal_coefficients.push_back(nx); // X
					normal_coefficients.push_back(ny); // Y
					normal_coefficients.push_back(nz); // Z
					normal_coefficients.push_back(0.0f); // W
				}

				if (idx.texcoord_index != -1)
				{
					const float u = attrib.texcoords[2 * idx.texcoord_index + 0];
					const float v = attrib.texcoords[2 * idx.texcoord_index + 1];
					texture_coefficients.push_back(u);
					texture_coefficients.push_back(v);
				}
			}
			
		}

		ComputeTangents();
			
		//ComputeNormals(this);

		printf("OK.\n");
	}

	void ComputeNormals(Mesh* model)
	{
		//if (!model->attrib.normals.empty())
			//return;

		// Primeiro computamos as normais para todos os TRIÂNGULOS.
		// Segundo, computamos as normais dos VÉRTICES através do método proposto
		// por Gouraud, onde a normal de cada vértice vai ser a média das normais de
		// todas as faces que compartilham este vértice.

		size_t num_vertices = model->attrib.vertices.size() / 3;

		std::vector<int> num_triangles_per_vertex(num_vertices, 0);
		std::vector<glm::vec3> vertex_normals(num_vertices, glm::vec3(0.0f, 0.0f, 0.0f));

		for (size_t shape = 0; shape < model->shapes.size(); ++shape)
		{
			size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

			for (size_t triangle = 0; triangle < num_triangles; ++triangle)
			{
				assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

				glm::vec3  vertices[3];
				for (size_t vertex = 0; vertex < 3; ++vertex)
				{
					tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
					const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
					const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
					const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
					vertices[vertex] = glm::vec3(vx, vy, vz);
				}

				const glm::vec3  a = vertices[0];
				const glm::vec3  b = vertices[1];
				const glm::vec3  c = vertices[2];

				const glm::vec3  n = glm::cross(b - a, c - a);

				for (size_t vertex = 0; vertex < 3; ++vertex)
				{
					tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
					num_triangles_per_vertex[idx.vertex_index] += 1;
					vertex_normals[idx.vertex_index] += n;
					model->shapes[shape].mesh.indices[3 * triangle + vertex].normal_index = idx.vertex_index;
				}
			}
		}

		model->attrib.normals.resize(3 * num_vertices);
		printf("%d\n", (int)vertex_normals.size());
		for (size_t i = 0; i < vertex_normals.size(); ++i)
		{
			glm::vec3 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
			n = glm::normalize(n);
			if (i < 3) printf("%f %f %f\n", n.x, n.y, n.z);
			model->attrib.normals[3 * i + 0] = n.x;
			model->attrib.normals[3 * i + 1] = n.y;
			model->attrib.normals[3 * i + 2] = n.z;
		}

		for (size_t triangle = 0; triangle < num_triangles; ++triangle)
		{
			assert(shapes[0].mesh.num_face_vertices[triangle] == 3);

			for (size_t vertex = 0; vertex < 3; ++vertex)
			{
				tinyobj::index_t idx = shapes[0].mesh.indices[3 * triangle + vertex];

				if (idx.normal_index != -1)
				{
					const float nx = attrib.normals[3 * idx.normal_index + 0];
					const float ny = attrib.normals[3 * idx.normal_index + 1];
					const float nz = attrib.normals[3 * idx.normal_index + 2];
					normal_coefficients.push_back(nx); // X
					normal_coefficients.push_back(ny); // Y
					normal_coefficients.push_back(nz); // Z
					normal_coefficients.push_back(0.0f); // W
				}

			}
		}
	}

	void ComputeTangents() {
		assert(!model_coefficients.empty());

		vector<glm::vec3> tangents_acc(num_triangles * 3, glm::vec3(0.0f));

		for (size_t triangle = 0; triangle < num_triangles; ++triangle) {

			glm::vec3 p0 = glm::vec3(
				model_coefficients[12 * triangle + 4 * 0 + 0],
				model_coefficients[12 * triangle + 4 * 0 + 1],
				model_coefficients[12 * triangle + 4 * 0 + 2]
			);
			glm::vec3 p1 = glm::vec3(
				model_coefficients[12 * triangle + 4 * 1 + 0],
				model_coefficients[12 * triangle + 4 * 1 + 1],
				model_coefficients[12 * triangle + 4 * 1 + 2]
			);
			glm::vec3 p2 = glm::vec3(
				model_coefficients[12 * triangle + 4 * 2 + 0],
				model_coefficients[12 * triangle + 4 * 2 + 1],
				model_coefficients[12 * triangle + 4 * 2 + 2]
			);

			glm::vec2 uv0 = glm::vec2(
				texture_coefficients[6 * triangle + 2 * 0 + 0],
				texture_coefficients[6 * triangle + 2 * 0 + 1]
			);

			glm::vec2 uv1 = glm::vec2(
				texture_coefficients[6 * triangle + 2 * 1 + 0],
				texture_coefficients[6 * triangle + 2 * 1 + 1]
			);

			glm::vec2 uv2 = glm::vec2(
				texture_coefficients[6 * triangle + 2 * 2 + 0],
				texture_coefficients[6 * triangle + 2 * 2 + 1]
			);

			glm::vec3 q1 = (p1 - p0);
			glm::vec3 q2 = (p2 - p0);

			float s1, s2, t1, t2;
			s1 = (uv1.x - uv0.x);
			s2 = (uv2.x - uv0.x);
			t1 = (uv1.y - uv0.y);
			t2 = (uv2.y - uv0.y);

			glm::mat2 st(
				t2, -s2,
				-t1, s1
			);

			glm::mat3x2 mQ(
				q1.x, q2.x,
				q1.y, q2.y,
				q1.z, q2.z
			);

			glm::mat3x2 mT = (1.0f / (s1 * t2 - s2 * t1)) * (st * mQ);

			glm::vec3 tan(mT[0][0], mT[1][0], mT[2][0]);

			tinyobj::index_t idx0 = shapes[0].mesh.indices[3 * triangle + 0];
			tinyobj::index_t idx1 = shapes[0].mesh.indices[3 * triangle + 1];
			tinyobj::index_t idx2 = shapes[0].mesh.indices[3 * triangle + 2];

			tangents_acc[idx0.vertex_index] += tan;
			tangents_acc[idx1.vertex_index] += tan;
			tangents_acc[idx2.vertex_index] += tan;
		}

		for (size_t triangle = 0; triangle < num_triangles; ++triangle)
		{
			assert(shapes[0].mesh.num_face_vertices[triangle] == 3);
			for (size_t vertex = 0; vertex < 3; ++vertex)
			{
				tinyobj::index_t idx = shapes[0].mesh.indices[3 * triangle + vertex];

				glm::vec3 tangent = tangents_acc[idx.vertex_index];

				tangent = glm::normalize(tangent);

				tangent_coefficients.push_back(tangent.x); // X
				tangent_coefficients.push_back(tangent.y); // Y
				tangent_coefficients.push_back(tangent.z); // Z
				tangent_coefficients.push_back(0.0f); // W
			}
		}
	}
};

struct Material {
	vector<float> fparams;
	vector<glm::vec4> v4params;
	vector<int> textureIDs;

	Material() :
		fparams(vector<float>()),
		v4params(vector<glm::vec4>()),
		textureIDs(vector<int>())
	{}
};

struct DrawObject {
	GLuint VAO;
	GLuint VBO_positions;
	GLuint VBO_normals;
	GLuint VBO_tangents;
	GLuint VBO_uv;
	GLuint VBO_index;
	GLuint VBO_instancedM0;
	GLuint VBO_instancedM1;
	GLuint VBO_instancedM2;
	GLuint VBO_instancedM3;
	GLuint indexes_size;

	Material material;

	DrawObject() :
		VAO(0),
		VBO_positions(0),
		VBO_index(0),
		VBO_normals(0),
		VBO_tangents(0),
		VBO_uv(0),
		VBO_instancedM0(0),
		VBO_instancedM1(0),
		VBO_instancedM2(0),
		VBO_instancedM3(0),
		indexes_size(0)
	{}

	bool operator < (const DrawObject& b) const {
		return (indexes_size < b.indexes_size);
	}
};

class Renderer
{
public:
	static GLsizei FRAME_WIDTH;
	static GLsizei FRAME_HEIGHT;
	static Shader gizmoShader;
	static Shader noShadingShader;
	static Shader PBRShader;
	static Material default_material;
	static DrawObject circle_primitive;
	static DrawObject sprite_primitive;
	static DrawObject sphere_primitive;
	static DrawObject line_primitive;
	static DrawObject axisGizmo_primitive;
	static GLuint numLoadedTextures;
	static GLuint loadedTextureId[10];

	static priority_queue<tuple<int, DrawObject*, Transform*, GLsizei>> pq;

	// returns a VAO for the specified vertex array and indice array;
	static GLuint BuildTrianglesVAO(const vector<float>& vertex_position, const vector<float>& vertex_normals, const vector<GLuint>& face_indexes);
	static void   BuildTrianglesVAO(const vector<float>& model_coefficients, const vector<float>& nromal_coefficients, const vector<float>& tangent_coefficients, const vector<float>& uv_coefficients, const vector<GLuint>& indices, DrawObject* obj);
	static void   BuildTrianglesVAO(const vector<float>& model_coefficients, const vector<float>& nromal_coefficients, const vector<float>& uv_coefficients, const vector<GLuint>& indices, DrawObject* obj);
	static GLuint BuildTrianglesVAO(const vector<float>& vertex_position, const vector<GLuint>& face_indexes);

	static void BuildInstanceVAO(const vector<Transform>& transforms, DrawObject* obj);

	static void SetMaterialParams(const Material& material, Shader* shader);

	static void LoadTextureRGB(string filepath);
	static void UnloadTextures();

	static void DrawEnv(Camera* camera);

	static void RenderTriangles(DrawObject* obj, Transform* tr, Camera* camera, int prio, Shader* shader, GLsizei instance_qnt);
	static void RenderLines(DrawObject* obj, const Transform& tr, Camera* camera, int prio, Shader* shader);

	static void initFrame(glm::vec4 bg_color);

	static void drawFrame(glm::vec4 bg_color, Camera* camera, float thickness);
	
	static void setupPrimitives();
};

