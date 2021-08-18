#pragma once

#include "meshanim.h"
#include "mesh.h"
#include "Shader.h"
#include "Header.h"
enum Dir_Movement {
	DFORWARD,
	DBACKWARD,
	DLEFT,
	DRIGHT
};
class Person
{
public:
	glm::vec3 Position;
	glm::vec3 CamPosition;
	glm::vec3 Front;
	glm::vec3 CamFront;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	GLfloat CamUp;
	GLfloat CamZoom;
	GLfloat Yaw;
	GLfloat Pitch;
	MeshAnim Model;
	glm::mat4 ModelMatrix;
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	Shader shaderProgram;
	GLfloat MyRot;
	int NumAnim;
	GLfloat constRot;
	GLfloat AnimStart;
	GLfloat *TimeAnim;
	GLfloat radius;
	GLfloat hight;
	GLfloat deltay;
	Person();
	Person(const GLchar* vertexPath, const GLchar* fragmentPath);
	void EditModelMatrix(glm::vec3 Posit, glm::vec3 ScaleVec, GLfloat xrot, GLfloat zrot, GLfloat deltaTime);
	void LoadShaderUniform(glm::mat4 view, glm::mat4 projection, glm::vec3 Cam, LightUnit light, GLfloat currentFrame);
	bool LoadModel(const GLchar* vertexPath, GLfloat rad, GLfloat hi, GLfloat dely, GLfloat rot, GLfloat uper, GLfloat zoomer);
	void Draw();
	glm::vec3 ProcessKeyboard(Dir_Movement direction, GLfloat deltaTime, bool shift);
	void AnimNow(GLfloat xrot, GLfloat zrot, GLfloat currentFrame);
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
	void updateCameraVectors(GLfloat xoffset, GLfloat yoffset);
	glm::mat4 GetViewMatrix();
	~Person();

};

