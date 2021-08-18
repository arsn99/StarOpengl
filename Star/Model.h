#pragma once

#include "mesh.h"
#include "Shader.h"
#include "Header.h"

class Mod
{
public:
	Mesh Model;
	glm::mat4 ModelMatrix;
	Shader shaderProgram;
	Shader shaderProgramLight;
	glm::vec3 Position;
	GLfloat MyRot;
	glm::vec3 vectors[8];
	glm::mat4 HitBoxMatrix;
	Mod();
	Mod(const GLchar* vertexPath, const GLchar* fragmentPath);
	void EditModelMatrix(glm::vec3 Posit, glm::vec3 ScaleVec, GLfloat rot);
	void EditHitBoxMatrix(glm::vec3 ScaleVec, glm::vec3 Posit = glm::vec3(0.0f));
	void LoadShaderUniform(glm::mat4 view, glm::mat4 projection, glm::vec3 Cam, LightUnit light, GLfloat currentFrame);
	bool LoadModel(const GLchar* vertexPath);
	void Draw(glm::mat4 view, glm::mat4 projection, bool falg);
	~Mod();
};

