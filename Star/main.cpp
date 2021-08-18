// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;
using namespace glm;
#include <cmath>
#include <ctime>
#include <cstdlib>
#include<SOIL2.h>
#include"Camera.h"
#include"Shader.h"
#include"Texture.h"
#include "meshanim.h"
#include "mesh.h"
#include "Header.h"
#include "Person.h"
#include "Model.h"
#include "ModelAnim.h"
#include <windows.h>
#include <mmsystem.h>
#if defined(_MSC_VER)
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shlwapi.lib")
#endif 

bool keys[1024];
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool firstMouse = true;
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
GLfloat lastX = 400, lastY = 400;
//Camera per(glm::vec3(0.0f, 1.5, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
Person per;

bool compare(Person& per, Mod& dom)
{
	glm::vec3  lolpos = per.Position;
	glm::vec3 tempPos[8];
	for (int i = 0; i < 8; i++)
		tempPos[i] = dom.vectors[i];
	if (lolpos.y >= tempPos[7].y || lolpos.y + per.hight <= tempPos[0].y)
		return 0;
	glm::vec2 lolPosA;
	lolPosA.x = lolpos.x;
	lolPosA.y = lolpos.z;
	GLboolean D = 0;
	GLint num = 0;
	GLint count = 0;
	GLfloat dis;
	glm::vec2 lolPosB;
	glm::vec2 lolPosC;
	for (int i = 0; i < 4; i++)
	{
		lolPosB.x = tempPos[i].x;
		lolPosB.y = tempPos[i].z;
		lolPosC.x = tempPos[(i + 1) % 4].x;
		lolPosC.y = tempPos[(i + 1) % 4].z;
		dis = glm::distance(lolPosA, lolPosB);
		if (dis < per.radius)
			return 1;
		if ((lolPosA.x - lolPosB.x)*(lolPosC.y - lolPosB.y) - (lolPosA.y - lolPosB.y)*(lolPosC.x - lolPosB.x) > 0.0f)
		{
			D = 0;
			num = i;
			count++;
		}
	}
	if (D) return 1;
	if (count > 1) return 0;
	lolPosB.x = tempPos[num].x;
	lolPosB.y = tempPos[num].z;
	lolPosC.x = tempPos[(num + 1) % 4].x;
	lolPosC.y = tempPos[(num + 1) % 4].z;if (glm::abs((lolPosB.x - lolPosA.x)*(lolPosC.y - lolPosA.y) - (lolPosC.x - lolPosA.x)*(lolPosB.y - lolPosA.y)) / glm::distance(lolPosB, lolPosC) >= per.radius)
		return 0;
	else
		return 1;
	return 0;
}
bool compareround(Person& per, glm::vec3 dot, GLfloat radius, GLfloat hight)
{
	if (per.Position.y >= dot.y + hight || per.Position.y + per.hight <= dot.y)
		return 0;
	glm::vec2 lolPosA;
	lolPosA.x = per.Position.x;
	lolPosA.y = per.Position.z;
	glm::vec2 lolPosB;
	lolPosB.x = dot.x;
	lolPosB.y = dot.z;
	if (glm::distance(lolPosA, lolPosB) < per.radius + radius)
		return 1;
	return 0;
}

bool compare(Person& per, ModAnim& dom)
{
	glm::vec3  lolpos = per.Position;
	glm::vec3 tempPos[8];
	for (int i = 0; i < 8; i++)
		tempPos[i] = dom.vectors[i];
	if (lolpos.y >= tempPos[7].y || lolpos.y + per.hight <= tempPos[0].y)
		return 0;
	glm::vec2 lolPosA;
	lolPosA.x = lolpos.x;
	lolPosA.y = lolpos.z;
	GLboolean D = 0;
	GLint num = 0;
	GLint count = 0;
	GLfloat dis;
	glm::vec2 lolPosB;
	glm::vec2 lolPosC;
	//cout << "##################################################" << endl;
	for (int i = 0; i < 4; i++)
	{
		lolPosB.x = tempPos[i].x;
		lolPosB.y = tempPos[i].z;
		lolPosC.x = tempPos[(i + 1) % 4].x;
		lolPosC.y = tempPos[(i + 1) % 4].z;
		dis = glm::distance(lolPosA, lolPosB);
		if (dis < per.radius)
		{
			//cout << "kek" << endl;
			return 1;
		}
		if ((lolPosA.x - lolPosB.x)*(lolPosC.y - lolPosB.y) - (lolPosA.y - lolPosB.y)*(lolPosC.x - lolPosB.x) > 0.0f)
		{
			D = 0;
			num = i;
			count++;
		}
		//cout << "D " << i << " " << (i + 1) % 4 << " " << (lolPosA.x - lolPosB.x)*(lolPosC.y - lolPosB.y) - (lolPosA.y - lolPosB.y)*(lolPosC.x - lolPosB.x) <<  endl;
		//cout << "lolPosB " << lolPosB.x << " " << lolPosB.y << endl;
		//cout << "lolPosC " << lolPosC.x << " " << lolPosC.y << endl;
	}
	if (D) return 1;
	if (count > 1) return 0;
	lolPosB.x = tempPos[num].x;
	lolPosB.y = tempPos[num].z;
	lolPosC.x = tempPos[(num + 1) % 4].x;
	lolPosC.y = tempPos[(num + 1) % 4].z;
	//GLfloat dis = glm::distance(lolPosB, lolPosC);
	//GLfloat one = glm::abs((lolPosB.x - lolPosA.x)*(lolPosC.y - lolPosA.y) - (lolPosC.x - lolPosA.x)*(lolPosB.y - lolPosA.y));
	if (glm::abs((lolPosB.x - lolPosA.x)*(lolPosC.y - lolPosA.y) - (lolPosC.x - lolPosA.x)*(lolPosB.y - lolPosA.y)) / glm::distance(lolPosB, lolPosC) >= per.radius)
		return 0;
	else
		return 1;
	return 0;
}

bool optimizaition(glm::vec3 vectors[8])
{
	for (int i = 0; i < 8; i++)
		if (per.CamFront.x * (vectors[i].x - per.CamPosition.x) + per.CamFront.y * (vectors[i].y - per.CamPosition.y) + per.CamFront.z * (vectors[i].z - per.CamPosition.z) > 0.0)
			return true;
	return false;
}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_Q:
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_LEFT_CONTROL:
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_W:
			keys[GLFW_KEY_W] = 1;
			break;
		case GLFW_KEY_S:
			keys[GLFW_KEY_S] = 1;
			break;
		case GLFW_KEY_D:
			keys[GLFW_KEY_D] = 1;
			break;
		case GLFW_KEY_A:
			keys[GLFW_KEY_A] = 1;
			break;
		case GLFW_KEY_SPACE:
			keys[GLFW_KEY_SPACE] = 1;
			break;
		default:
			break;
		}
	else if (action == GLFW_RELEASE)
		switch (key)
		{
		case GLFW_KEY_W:
			keys[GLFW_KEY_W] = 0;
			break;
		case GLFW_KEY_S:
			keys[GLFW_KEY_S] = 0;
			break;
		case GLFW_KEY_D:
			keys[GLFW_KEY_D] = 0;
			break;
		case GLFW_KEY_A:
			keys[GLFW_KEY_A] = 0;
			break;
		case GLFW_KEY_SPACE:
			keys[GLFW_KEY_SPACE] = 0;
			break;
		default:
			break;
		}
}
GLfloat xrot, zrot;
glm::vec3 do_movement(Person& per)
{
	glm::vec3 ret = glm::vec3(0.0f);
	bool flag = 0;
	if (keys[GLFW_KEY_SPACE])
	{
		flag = 1;
	}
	xrot = zrot = 0.0f;
	if (keys[GLFW_KEY_W])
	{
		zrot += 1.0f;
		ret += per.ProcessKeyboard(DFORWARD, deltaTime, flag);
	}
	if (keys[GLFW_KEY_S])
	{
		zrot -= 1.0f;
		ret += per.ProcessKeyboard(DBACKWARD, deltaTime, flag);
	}
	if (keys[GLFW_KEY_A])
	{
		xrot += 1.0f;
		ret += per.ProcessKeyboard(DLEFT, deltaTime, flag);
	}
	if (keys[GLFW_KEY_D])
	{
		xrot -= 1.0f;
		ret += per.ProcessKeyboard(DRIGHT, deltaTime, flag);
	}
	return ret;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	
	if (firstMouse) 
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;
	per.ProcessMouseMovement(xoffset, yoffset);
	
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{}

void makeVAOVBO(GLuint &VBO, GLuint &VAO, GLfloat vertices[], int size, int param[][3], int count, bool flagV)
{
	if (flagV) glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (flagV)
	{
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
	for (int i = 0; i < count; i++)
	{
		glVertexAttribPointer(i, param[i][0], GL_FLOAT, GL_FALSE, sizeof(GLfloat) * param[i][1], (GLvoid*)(param[i][2] * sizeof(GLfloat)));
		glEnableVertexAttribArray(i);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(800,600, "lab4", nullptr, nullptr);
	if (window == nullptr)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		glfwTerminate();
		return -1;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	
	per.LoadModel("jango-fett/source/Jango1.fbx", 0.4, 2.1, 0.1, 3.14f, 2.0f, 1.5f);
	per.shaderProgram.ShaderLoad("shader.vs", "fragmentShaderSource.txt");
	per.EditModelMatrix(glm::vec3(8.4,0.0,-0.64), glm::vec3(0.005, 0.005, 0.005), xrot, zrot, deltaTime);
	

	Mod dom("vertexShaderSource.txt", "fragmentShaderSource.txt");
	dom.LoadModel("test/Check (1).obj");
	dom.EditModelMatrix(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0), 0.0f);


	const int WallCount = 24;
	Mod wall[WallCount];
	for (int i = 0; i < WallCount; i++)
	{
		wall[i].shaderProgram.ShaderLoad("vertexShaderSource.txt", "fragmentShadeфыrSource.txt");
		wall[i].LoadModel("testmodelwall.fbx");
	}
    wall[0].EditModelMatrix(glm::vec3(5.6, 0.0, 4.0), glm::vec3(0.012, 0.012, 0.012), glm::radians(18.0f));
	wall[0].EditHitBoxMatrix( glm::vec3(1.0, 1.5, 1.0));
	wall[1].EditModelMatrix(glm::vec3(0.75, 0.0, 7.5), glm::vec3(0.1, 0.1, 0.1), 0.0);
	wall[1].EditHitBoxMatrix( glm::vec3(13.5, 4.0, 6.0));
	wall[2].EditModelMatrix(glm::vec3(-7.2, 0.0, -0.63), glm::vec3(0.1, 0.1, 0.1), 0.0);
	wall[2].EditHitBoxMatrix(glm::vec3(15.5, 4.0, 4.5));
	wall[3].EditModelMatrix(glm::vec3(1.0, 0.0, -7.75), glm::vec3(0.1, 0.1, 0.1), 0.0);
	wall[3].EditHitBoxMatrix(glm::vec3(31.7, 4.0, 4.5));
	wall[4].EditModelMatrix(glm::vec3(15.54, 0.0, -3.14), glm::vec3(0.1, 0.1, 0.1), glm::radians(45.0f));
	wall[4].EditHitBoxMatrix(glm::vec3(2.2,2.27,1.5));
	wall[5].EditModelMatrix(glm::vec3(12.1, 0.0, 2.8), glm::vec3(0.1, 0.1, 0.1), glm::radians(70.0));
	wall[5].EditHitBoxMatrix(glm::vec3(1.70, 4.0,7.0));
	wall[6].EditModelMatrix(glm::vec3(-7.5, 0.0, 7.7), glm::vec3(0.1, 0.1, 0.1),0.0);
	wall[6].EditHitBoxMatrix(glm::vec3(4.0, 4.0, 6.0));
	wall[7].EditModelMatrix(glm::vec3(-13.3, 0.0, 2.5), glm::vec3(0.1, 0.1, 0.1), glm::radians(-2.0));
	wall[7].EditHitBoxMatrix(glm::vec3(3.2, 1.22, 1.15));
	wall[8].EditModelMatrix(glm::vec3(11.4, 0.0, 3.7), glm::vec3(0.1, 0.1, 0.1), glm::radians(-15.0));
	wall[8].EditHitBoxMatrix(glm::vec3(3.2, 1.22, 1.15));
	wall[9].EditModelMatrix(glm::vec3(8.8, 0.0, 7.7), glm::vec3(0.1, 0.1, 0.1), glm::radians(-18.0));
	wall[9].EditHitBoxMatrix(glm::vec3(3.2, 1.22, 1.15));
	wall[10].EditModelMatrix(glm::vec3(-17.0, 0.0, 0.0), glm::vec3(0.1, 0.1, 0.1), glm::radians(90.0));
	wall[10].EditHitBoxMatrix(glm::vec3(20.0, 4.0, 4.5));
	wall[11].EditModelMatrix(glm::vec3(-13.9, 0.0, 5.8), glm::vec3(0.1, 0.1, 0.1), glm::radians(-60.0));
	wall[11].EditHitBoxMatrix(glm::vec3(3.2, 1.22, 1.15));
	wall[12].EditModelMatrix(glm::vec3(-15.4, 0.0, 9.1), glm::vec3(0.1, 0.1, 0.1), glm::radians(38.0));
	wall[12].EditHitBoxMatrix(glm::vec3(5.0, 4.0, 8.5));
	wall[13].EditModelMatrix(glm::vec3(-13.4, 0.0, 8.95), glm::vec3(0.1, 0.1, 0.1), glm::radians(90.0));
	wall[13].EditHitBoxMatrix(glm::vec3(3.5, 4.0, 8.5));
	wall[14].EditModelMatrix(glm::vec3(-12.4, 0.0, 4.7), glm::vec3(0.1, 0.1, 0.1), glm::radians(90.0));
	wall[14].EditHitBoxMatrix(glm::vec3(1.2, 0.70, 1.2));
	wall[15].EditModelMatrix(glm::vec3(9.76, 0.0, 5.46), glm::vec3(0.1, 0.1, 0.1), glm::radians(90.0));
	wall[15].EditHitBoxMatrix(glm::vec3(1.2, 0.70, 1.2));
	wall[16].EditModelMatrix(glm::vec3(13.1, 0.0, 7.1), glm::vec3(0.1, 0.1, 0.1), glm::radians(-41.0));
	wall[16].EditHitBoxMatrix(glm::vec3(5.0, 4.0, 8.5));
	wall[17].EditModelMatrix(glm::vec3(8.3, 0.0, 9.75), glm::vec3(0.1, 0.1, 0.1), 0.0);
	wall[17].EditHitBoxMatrix(glm::vec3(3.2, 1.22, 1.15));
	wall[18].EditModelMatrix(glm::vec3(-13.06, 0.0, -0.5), glm::vec3(0.1, 0.1, 0.1), 0.0);
	wall[18].EditHitBoxMatrix(glm::vec3(9.0, 4.0, 6.0));
	wall[19].EditModelMatrix(glm::vec3(13.0, 0.0, -4.9), glm::vec3(0.1, 0.1, 0.1), 0.0);
	wall[19].EditHitBoxMatrix(glm::vec3(5.5, 4.0, 2.0));
	wall[20].EditModelMatrix(glm::vec3(10.5, 0.0, -4.9), glm::vec3(0.1, 0.1, 0.1), glm::radians(55.0));
	wall[20].EditHitBoxMatrix(glm::vec3(2.0, 4.0, 4.0));
	wall[21].EditModelMatrix(glm::vec3(17.7, 0.0, -2.7), glm::vec3(0.1, 0.1, 0.1), glm::radians(8.0));
	wall[21].EditHitBoxMatrix(glm::vec3(2.0, 4.0, 5.8));
	wall[22].EditModelMatrix(glm::vec3(16.6, 0.0, 4.7), glm::vec3(0.1, 0.1, 0.1), glm::radians(0.0));
	wall[22].EditHitBoxMatrix(glm::vec3(4.0, 4.0, 4.0));
	wall[23].EditModelMatrix(glm::vec3(12.0, 0.0, 1.9), glm::vec3(0.1, 0.1, 0.1), glm::radians(0.0));
	wall[23].EditHitBoxMatrix(glm::vec3(5.0, 4.0, 1.2));


	Mod ClonWall("vertexShaderSource.txt", "fragmentShaderSource.txt");
	ClonWall.LoadModel("Star/star.obj");
	ClonWall.EditModelMatrix(glm::vec3(16.7f, 0.2,-1.8f), glm::vec3(0.012, 0.012, 0.012), glm::radians(-83.0f));
	ClonWall.EditHitBoxMatrix( glm::vec3(2.0, 2.0, 0.5));


	Mod ST("vertexShaderSource.txt", "fragmentShaderSource.txt");
	ST.LoadModel("star-wars-at-st/source/AT-ST.obj");
	ST.EditModelMatrix(glm::vec3(-4.0, 0.7, -7.6), glm::vec3(0.1, 0.1, 0.1), glm::radians(0.0f));
	ST.EditHitBoxMatrix(glm::vec3(0.5, 1.0, 0.5));

	Mod Bol("vertexShaderSource.txt", "fragmentShaderSource.txt");
	Bol.LoadModel("Ben/Ben.obj");
	Bol.EditModelMatrix(glm::vec3(-2.4, -0.5, -7.5), glm::vec3(3.0, 3.0, 3.0), glm::radians(-83.0f));
	Bol.EditHitBoxMatrix(glm::vec3(1.0, 2.0, 0.5), glm::vec3(0.0,0.5,0.0) );

	ModAnim dr2("shader.vs", "fragmentShaderSource.txt");
	dr2.LoadModel("dr/untitled.fbx");
	dr2.EditModelMatrix(glm::vec3(-10.3262, 0.7, 7.6f), glm::vec3(0.01, 0.01, 0.01), 0.0f);
	dr2.EditHitBoxMatrix(glm::vec3(0.7, 1.8, 0.7));
	dr2.AnimNow(0);

	ModAnim dr1("shader.vs", "fragmentShaderSource.txt");
	dr1.LoadModel("dr/rob.fbx");
	dr1.EditModelMatrix(glm::vec3(-10.0, -0.4, 9.8), glm::vec3(0.01, 0.01, 0.01), glm::radians(-180.0f));
	dr1.EditHitBoxMatrix(glm::vec3(0.7, 1.5, 0.7), glm::vec3(0.0, 0.4, 0.0));
	dr1.AnimNow(0);

	
	ModAnim CloneBar("shader.vs", "fragmentShaderSource.txt");
	CloneBar.LoadModel("Clone1/Clone.fbx");
	CloneBar.EditModelMatrix(glm::vec3(-1.04, 0.0, -3.1), glm::vec3(0.0045, 0.0045,0.0045), 0.0f);
	CloneBar.EditHitBoxMatrix(glm::vec3(1.0, 2.5, 1.0), glm::vec3(-0.3,0.0,0.2));
	CloneBar.AnimNow(0);


	ModAnim YodaJump("shader.vs", "fragmentShaderSource.txt");
	YodaJump.LoadModel("YodaRic/source/YodaRigged.fbx");
	YodaJump.EditModelMatrix(glm::vec3(9.65, 0.7,5.34), glm::vec3(0.02, 0.02, 0.02), glm::radians(-135.0f));
	YodaJump.EditHitBoxMatrix(glm::vec3(0.8, 1.2, 1.0));
	YodaJump.AnimNow(0);


	ModAnim RobotOff("shader.vs", "fragmentShaderSource.txt");
	RobotOff.LoadModel("Robot/robot1.fbx");
	RobotOff.EditModelMatrix(glm::vec3(-4.08,0.0,5.13), glm::vec3(0.01, 0.01, 0.01), 0.0f);
	RobotOff.EditHitBoxMatrix(glm::vec3(0.7, 2.0, 0.5));
	RobotOff.AnimNow(3);

	ModAnim DeathStar("shader.vs", "fragmentShaderSource.txt");
	DeathStar.LoadModel("stars/Death.fbx");
	DeathStar.EditModelMatrix(glm::vec3(0.14f, 1.4f, 1.22f), glm::vec3(0.003, 0.003, 0.003), 0.0f);
	DeathStar.EditHitBoxMatrix(glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.0,-0.1,0.0));
	
	
	GLfloat currentFrame;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	GLfloat rot;
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	GLfloat m_startTime = (GLfloat)glfwGetTime();
	LightUnit light;
	light.position = lightPos;
	light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 ret;
	glm::vec3 tempPerPos;
	bool fleg;
	glm::vec3 tempret;
	projection = glm::mat4(1.0f);
	projection = perspective(45.0f, (float)width / (float)height, 0.1f, 50.0f);




	float skyboxVertices[] = {
		// positions          
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	Shader Sky_Box("shaderSky.vs", "shaderSky.frag");
	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces
	{
		"sb_mars/front.tga",
		"sb_mars/back.tga",
		"sb_mars/top.tga",
		"sb_mars/bottom.tga",
		"sb_mars/right.tga",
		"sb_mars/left.tga",
	};



	Texture Sky_Tex(GL_TEXTURE_CUBE_MAP);
	Sky_Tex.Load_CubeMap(faces);

	Sky_Box.Use();
	Sky_Box.setInt("skybox", 0);
	PlaySoundA("f.wav", NULL, SND_ASYNC);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		ret = glm::vec3(0.0f);
		view = glm::mat4(1.0f);
		tempPerPos = per.Position;
		fleg = 0;
		glfwPollEvents();
		ret = do_movement(per);
		//cout << "Position " << per.Position.x << " " << per.Position.y << " " << per.Position.z << endl;

		per.AnimNow(xrot, zrot, currentFrame);
		per.EditModelMatrix(per.Position, glm::vec3(0.005, 0.005, 0.005), xrot, zrot, deltaTime);
		
		for (int i = 0; i < WallCount; i++)
		{
			//wall[i].LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			//wall[i].Draw(view, projection, 1);
			if (compare(per, wall[i]))
			{
				fleg = 1;
				break;
			}
		}
		if (!fleg)
		{
			if (compare(per, dr1))
				fleg = 1;
		}
		if (!fleg)
		{
			if (compare(per, dr2))
				fleg = 1;
		}
		if (!fleg)
		{
			if (compare(per, CloneBar))
			{
				fleg = 1;
				cout << "EZIII4" << endl;

			}
		}
		if (!fleg)
		{
			if (compare(per, YodaJump))
				fleg = 1;
		}
		if (!fleg)
		{
			if (compare(per, RobotOff))
				fleg = 1;
		}
		if (!fleg)
		{
			if (compare(per, DeathStar))
				fleg = 1;
		}
		if (!fleg)
		{
			if (compare(per, ST))
				fleg = 1;
		}
		if (!fleg)
		{
			if (compare(per, Bol))
				fleg = 1;
		}
		if (!fleg)
		{
			if (compare(per, ClonWall))
				fleg = 1;
		}
		if (!fleg)
		{
			if (compareround(per, glm::vec3(0.54, 0.0, -0.63), 2.25, 4.0))
			{
				fleg = 1;
				cout << "EZIII1" << endl;
			}
		}

		if (fleg)
		{
			fleg = 0;
			tempret = ret;
			tempret.x = 0;
			per.Position = tempPerPos + tempret;
			for (int i = 0; i < WallCount; i++)
			{
				//wall[i].LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
				//wall[i].Draw(view, projection, 1);*/
				if (compare(per, wall[i]))
				{
					fleg = 1;
					break;
				}
			}
			if (!fleg)
			{
				if (compare(per, dr1))
					fleg = 1;
			}
			if (!fleg)
			{
				if (compare(per, dr2))
					fleg = 1;
			}
			if (!fleg)
			{
				if (compare(per, CloneBar))
					fleg = 1;
			}
			if (!fleg)
			{
				if (compare(per, YodaJump))
					fleg = 1;
			}
			if (!fleg)
			{
				if (compare(per, RobotOff))
					fleg = 1;
			}
			if (!fleg)
			{
				if (compare(per, DeathStar))
					fleg = 1;
			}
			if (!fleg)
			{
				if (compare(per, ST))
					fleg = 1;
			}
			if (!fleg)
			{
				if (compare(per, Bol))
					fleg = 1;
			}
			if (!fleg)
			{
				if (compare(per, ClonWall))
					fleg = 1;
			}
			if (!fleg)
			{
				if (compareround(per, glm::vec3(0.54, 0.0, -0.63), 2.25, 4.0))
				{
					fleg = 1;
					cout << "EZIII2" << endl;
				}
			}
			if (fleg)
			{
				fleg = 0;
				tempret = ret;
				tempret.z = 0;
				per.Position = tempPerPos + tempret;
				for (int i = 0; i < WallCount; i++)
				{
					//wall[i].LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
					//wall[i].Draw(view, projection, 1);*/
					if (compare(per, wall[i]))
					{
						fleg = 1;
						break;
					}
				}
				if (!fleg)
				{
					if (compare(per, dr1))
						fleg = 1;
				}
				if (!fleg)
				{
					if (compare(per, dr2))
						fleg = 1;
				}
				if (!fleg)
				{
					if (compare(per, CloneBar))
						fleg = 1;
				}
				if (!fleg)
				{
					if (compare(per, YodaJump))
						fleg = 1;
				}
				if (!fleg)
				{
					if (compare(per, RobotOff))
						fleg = 1;
				}
				if (!fleg)
				{
					if (compare(per, DeathStar))
						fleg = 1;
				}
				if (!fleg)
				{
					if (compare(per, ST))
						fleg = 1;
				}
				if (!fleg)
				{
					if (compare(per, Bol))
						fleg = 1;
				}
				if (!fleg)
				{
					if (compare(per, ClonWall))
						fleg = 1;
				}
				if (!fleg)
				{
					if (compareround(per, glm::vec3(0.54, 0.0, -0.63), 2.25, 4.0))
					{
						fleg = 1;
						cout << "EZIII3" << endl;
					}
				}
				if (fleg)
					per.EditModelMatrix(tempPerPos, glm::vec3(0.005, 0.005, 0.005), xrot, zrot, deltaTime);
			}
		}
		view = per.GetViewMatrix();
		per.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
		per.Draw();



		
		/*if (optimizaition(focus.vectors))
		focus.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
		focus.Draw(view, projection, 0);
		}
		*/
		if (optimizaition(dr1.vectors))
		{
			dr1.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			dr1.Draw(view, projection, 0);
		}
		if (optimizaition(dr2.vectors))
		{
			dr2.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			dr2.Draw(view, projection, 0);
		}
		if (optimizaition(CloneBar.vectors))
		{
			CloneBar.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			CloneBar.Draw(view, projection, 0);
		}
		if (optimizaition(YodaJump.vectors))
		{
			YodaJump.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			YodaJump.Draw(view, projection, 0);
		}
		if (optimizaition(RobotOff.vectors))
		{
			RobotOff.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			RobotOff.Draw(view, projection, 0);
		}
		if (optimizaition(DeathStar.vectors))
		{
			DeathStar.AnimNow(0);
			DeathStar.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			DeathStar.Draw(view, projection, 0);
		}
		if (optimizaition(ST.vectors))
		{
			ST.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			ST.Draw(view, projection, 0);
		}
		if (optimizaition(Bol.vectors))
		{
			Bol.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			Bol.Draw(view, projection, 0);
		}
		if (optimizaition(ClonWall.vectors))
		{
			ClonWall.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
			ClonWall.Draw(view, projection, 0);
		}

		dom.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
		dom.Draw(view, projection,1);


		glDepthFunc(GL_LEQUAL);

		Sky_Box.Use();
		
		view = glm::mat4(glm::mat3(per.GetViewMatrix()));
		Sky_Box.setMat4("view", view);
		Sky_Box.setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		Sky_Tex.Bind(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		/*Mol.LoadShaderUniform(view, projection, per.CamPosition, light, currentFrame);
		Mol.Draw(view, projection,0);*/

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}
