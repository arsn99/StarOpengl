#pragma once


#include "meshanim.h"
#include "Shader.h"
#include "Header.h"

class ModAnim
{
public:
	MeshAnim Model;
	glm::mat4 ModelMatrix; 
	Shader shaderProgram;
	Shader shaderProgramLight;
	int NumAnim;
	GLfloat AnimStart;
	GLfloat *TimeAnim;
	glm::vec3 vectors[8];
	glm::vec3 Position;
	GLfloat MyRot;
	glm::mat4 HitBoxMatrix;
	ModAnim();
    ModAnim(const GLchar* vertexPath, const GLchar* fragmentPath);
	void EditModelMatrix(glm::vec3 Posit, glm::vec3 ScaleVec, GLfloat rot);
	void EditHitBoxMatrix(glm::vec3 ScaleVec, glm::vec3 Posit = glm::vec3(0.0f));
	void LoadShaderUniform(glm::mat4 view, glm::mat4 projection, glm::vec3 Cam, LightUnit light, GLfloat currentFrame);
	bool LoadModel(const GLchar* vertexPath);
	void Draw(glm::mat4 view, glm::mat4 projection, bool flag);
	void AnimNow(int num);
	~ModAnim();
};

