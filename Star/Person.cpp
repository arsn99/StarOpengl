#include "Person.h"
#include <iostream>


Person::Person(const GLchar* vertexPath, const GLchar* fragmentPath) :shaderProgram(vertexPath, fragmentPath), Model(), WorldUp(0.0f, 1.0f, 0.0f), Up(0.0f, 1.0f, 0.0f), Front(0.0f, 0.0f, -1.0f), Right(1.0f, 0.0f, 0.0f)
{
	CamUp = 0.0f;
	CamZoom = 0.0f;
	Position = glm::vec3(0.0f);
	MyRot = glm::radians(180.0f);
	NumAnim = 0;
	AnimStart = 0;
	MouseSensitivity = 0.25f;
	Yaw = 0.0f;
	Pitch = 0.0f;
	TimeAnim = nullptr;
	radius = 0;
	hight = 0;
	deltay = 0;
	constRot = 0.0f;
	MovementSpeed = 4.0f;
	this->updateCameraVectors(0.0f, 0.0f);
}
Person::Person() : shaderProgram(), Model(), WorldUp(0.0f, 1.0f, 0.0f), Up(0.0f, 1.0f, 0.0f), Front(0.0f, 0.0f, -1.0f), Right(1.0f, 0.0f, 0.0f)
{
	CamUp = 0.0f;
	CamZoom = 0.0f;
	Position = glm::vec3(0.0f);
	MyRot = glm::radians(180.0f);
	NumAnim = 0;
	AnimStart = 0;
	MouseSensitivity = 0.25f;
	MovementSpeed = 4.0f;
	Yaw = 0.0f;
	Pitch = 0.0f;
	TimeAnim = nullptr;
	radius = 0;
	hight = 0;
	deltay = 0;
	constRot = 0.0f;
	this->updateCameraVectors(0.0f, 0.0f);
}

glm::vec3 Person::ProcessKeyboard(Dir_Movement direction, GLfloat deltaTime, bool shift)
{
	glm::vec3 ret = glm::vec3(0.0f);
	GLfloat velocity = this->MovementSpeed * deltaTime;
	if (direction == DFORWARD)
	{
		this->Position += this->Front * velocity;
		ret += this->Front * velocity;

	}
	if (direction == DBACKWARD)
	{
		this->Position -= this->Front * velocity;
		ret -= this->Front * velocity;
	}
	if (direction == DLEFT)
	{
		this->Position -= this->Right * velocity;
		ret -= this->Right * velocity;
	}
	if (direction == DRIGHT)
	{
		this->Position += this->Right * velocity;
		ret += this->Right * velocity;
	}
	this->CamPosition = glm::vec3(this->Position.x, this->Position.y + CamUp, this->Position.z) - (this->CamFront) * CamZoom;
	return ret;
}


void Person::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;

	this->Yaw -= xoffset;
	this->Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if (this->Pitch < -89.0f)
			this->Pitch = -89.0f;
	}
	if (this->Yaw < -180.0)
		this->Yaw += 360.0f;
	if (this->Yaw> 180.0)
		this->Yaw -= 360.0f;
	// Update Front, Right and Up Vectors using the updated Eular angles
	this->updateCameraVectors(xoffset, yoffset);
}
void Person::updateCameraVectors(GLfloat xoffset, GLfloat yoffset)
{
	// Calculate the new Front vector

	glm::vec3 front;
	front.x = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	this->CamFront = glm::normalize(front);
	front.y = 0;
	this->Front = glm::normalize(front);
	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->Up = glm::normalize(glm::cross(this->Right, this->CamFront));
	this->CamPosition = glm::vec3(this->Position.x, this->Position.y + CamUp, this->Position.z) - (this->CamFront) * CamZoom;
}

glm::mat4 Person::GetViewMatrix()
{
	return glm::lookAt(this->CamPosition, this->CamPosition + this->CamFront, this->Up);
}

bool Person::LoadModel(const GLchar* vertexPath, GLfloat rad, GLfloat hi, GLfloat dely, GLfloat rot, GLfloat uper, GLfloat zoomer)
{
	CamUp = uper;
	CamZoom = zoomer;
	this->CamPosition = glm::vec3(this->Position.x, this->Position.y + CamUp, this->Position.z) - (this->CamFront) * CamZoom;
	radius = rad;
	hight = hi;
	deltay = dely;
	constRot = rot;
	bool flag = Model.LoadMesh(vertexPath);
	cout << Model.m_pScene->mNumAnimations << "" << flag << endl;
	if (!flag)
	{
		TimeAnim = new GLfloat[Model.m_pScene->mNumAnimations];
		for (int i = 0; i < Model.m_pScene->mNumAnimations; i++)
		{
			float TicksPerSecond = (float)(Model.m_pScene->mAnimations[i]->mTicksPerSecond != 0 ? Model.m_pScene->mAnimations[i]->mTicksPerSecond : 25.0f);
			TimeAnim[i] = (float)Model.m_pScene->mAnimations[i]->mDuration / TicksPerSecond;
		}
	}
	return flag;
}

void Person::Draw()
{

	Model.Render();
}


void  Person::EditModelMatrix(glm::vec3 Posit, glm::vec3 ScaleVec, GLfloat xrot, GLfloat zrot, GLfloat deltaTime)
{
	bool fleg = 1;
	GLfloat rot;
	GLfloat deltarot;
	if (zrot == 0 && xrot == 0)
	{
		rot = MyRot;
		fleg = 0;
	}
	else  if (zrot == -1 && xrot == -1)
		rot = atan(xrot / zrot) - 3.14;
	else  if (zrot == -1)
		rot = atan(xrot / zrot) + 3.14;
	else
		rot = atan(xrot / zrot);
	if (fleg)
	{
		if (Yaw  > 0.0f && rot > 0.0f)
		{
			rot += glm::radians(Yaw)  - glm::radians(180.0f);
		}
		else if (Yaw < 0.0f && rot < 0.0f)
			rot += glm::radians(Yaw) + glm::radians(180.0f);
		else 
		{
			if (glm::radians(Yaw) + rot > 0.0f)
				rot += glm::radians(Yaw) - glm::radians(180.0f);
			else
				rot += glm::radians(Yaw) + glm::radians(180.0f);
		}
	}
	deltarot = MyRot - rot;
	if (glm::abs(deltarot) > glm::radians(180.0f))
	{
		if (MyRot < 0)
			MyRot = glm::radians(360.0f) + MyRot;
		else if(MyRot >= 0)
			MyRot = MyRot - glm::radians(360.0f);
		deltarot = MyRot - rot;
	}
	MyRot -= deltarot * deltaTime*5;
	Position.x = Posit.x;
	Position.y = Posit.y ;
	Position.z = Posit.z ;
	ModelMatrix = glm::mat4(1.0f);
	ModelMatrix = glm::translate(ModelMatrix, Position);
	ModelMatrix = glm::rotate(ModelMatrix, MyRot+constRot, glm::vec3(0.0, 1.0, 0.0));
	ModelMatrix = glm::scale(ModelMatrix, ScaleVec);
	this->CamPosition = glm::vec3(this->Position.x, this->Position.y + CamUp, this->Position.z) - (this->CamFront) * CamZoom;
}


void Person::LoadShaderUniform(glm::mat4 view, glm::mat4 projection, glm::vec3 Cam, LightUnit light, GLfloat currentFrame)
{
	shaderProgram.Use();
	shaderProgram.setMat4("model", ModelMatrix);
	shaderProgram.setMat4("view", view);
	shaderProgram.setMat4("projection", projection);
	shaderProgram.setVec1("material.diffuse", 0.0f);
	shaderProgram.setVec1("material.specular", 1.0f);
	shaderProgram.setFloat("material.shininess", 32.0f);
	shaderProgram.setVec3("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
	shaderProgram.setVec3("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
	shaderProgram.setVec3("light.specular", light.specular.x, light.specular.y, light.specular.z);
	shaderProgram.setVec3("light.position", light.position.x, light.position.y, light.position.z);
	shaderProgram.setVec3("viewPos", Cam.x, Cam.y, Cam.z);

	vector<glm::mat4> Transforms;
	Model.BoneTransform((currentFrame - AnimStart), Transforms,NumAnim);
	for (int i = 0; i < Transforms.size(); i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));
		sprintf_s(Name, sizeof(Name), "gBones[%d]", i);
		shaderProgram.setMat4(Name, Transforms[i]);
	}
}


void Person::AnimNow(GLfloat xrot, GLfloat zrot, GLfloat currentFrame)
{
	GLfloat deltaTime = currentFrame - AnimStart;
	if (xrot != 0 || zrot != 0)
	{
		if (NumAnim == 0 )
		{
			AnimStart = (GLfloat)glfwGetTime();
			NumAnim = 1;
			return;
		}
		if (NumAnim == 1 && deltaTime> TimeAnim[1])
		{
			AnimStart = (GLfloat)glfwGetTime();
			NumAnim = 2;
			return;
		}
		if (NumAnim == 2 && deltaTime > TimeAnim[2])
		{
			AnimStart = (GLfloat)glfwGetTime();
			NumAnim = 2;
			return;
		}
		if (NumAnim == 3 && deltaTime > TimeAnim[3])
		{
			AnimStart = (GLfloat)glfwGetTime();
			NumAnim = 1;
			return;
		}
	}
	if (xrot == 0 && zrot == 0)
	{
		if (NumAnim == 0)
		{
			//AnimStart = (GLfloat)glfwGetTime();
			//NumAnim = 0;
			return;
		}
		if (NumAnim == 1 && deltaTime > TimeAnim[1])
		{
			AnimStart = (GLfloat)glfwGetTime();
			NumAnim = 3;
			return;
		}
		if (NumAnim == 2 && deltaTime > TimeAnim[2])
		{
			AnimStart = (GLfloat)glfwGetTime();
			NumAnim = 3;
			return;
		}
		if (NumAnim == 3 && deltaTime > TimeAnim[3])
		{
			AnimStart = (GLfloat)glfwGetTime();
			NumAnim = 0;
			return;
		}
	}
}

Person::~Person()
{
	delete[] TimeAnim;
}
