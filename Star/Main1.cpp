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

bool keys[1024];
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool firstMouse = true;
glm::vec3 lightPos(1.5f, 3.5f, 1.5f);
GLfloat lastX = 400, lastY = 300;
Camera Cam(glm::vec3(0.0f, 1.5, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);


bool compare(Person& per, Mod& dom)
{
	glm::vec3  lolpos = per.Position;
	glm::vec3 tempPos[8];
	for (int i = 0; i < 8; i++)
		tempPos[i] = dom.ModelMatrix * glm::vec4(dom.vectors[i], 1.0);
	if (lolpos.y >= tempPos[7].y || lolpos.y + per.hight <= tempPos[0].y)
		return 0;
	glm::vec2 lolPosA;
	lolPosA.x = lolpos.x;
	lolPosA.y = lolpos.z;
	GLfloat min1 = 0, min2 = 0;
	int num1 = 0, num2 = 0;
	for (int i = 0; i < 4; i++)
	{
		GLfloat dis;
		glm::vec2 lolPosB;
		lolPosB.x = tempPos[i].x;
		lolPosB.y = tempPos[i].z;
		dis = glm::distance(lolPosA, lolPosB);
		if (glm::distance(lolPosA, lolPosB) < per.radius)
			return 1;
		if (i == 0)
		{
			min1 = dis;
			num1 = i;
		}
		else if (i == 1)
		{
			if (dis < min1)
			{
				num2 = num1;
				min2 = min1;
				num1 = i;
				min1 = dis;
			}
			else
			{
				num2 = i;
				min2 = dis;
			}
		}
		else if (dis < min1)
		{
			num2 = num1;
			min2 = min1;
			num1 = i;
			min1 = dis;
		}
		else if (dis < min2)
		{
			num2 = i;
			min2 = dis;
		}
	}
	if ((lolPosA.x > tempPos[3].x && lolPosA.x < tempPos[0].x) && (lolPosA.y > tempPos[0].z && lolPosA.y < tempPos[1].z))
		return 1;
	else if ((lolPosA.x > tempPos[3].x && lolPosA.x < tempPos[0].x) || (lolPosA.y > tempPos[0].z && lolPosA.y < tempPos[1].z))
	{
		glm::vec2 loldot1, loldot2;
		loldot1.x = tempPos[num1].x;
		loldot1.y = tempPos[num1].z;
		loldot2.x = tempPos[num2].x;
		loldot2.y = tempPos[num2].z;
		GLfloat dis = glm::distance(loldot1, loldot2);
		GLfloat one = glm::abs((loldot1.x - lolPosA.x)*(loldot2.y - lolPosA.y) - (loldot2.x - lolPosA.x)*(loldot1.y - lolPosA.y));
		if (one / dis >= per.radius)
			return 0;
		else
			return 1;
	}
	else
		return 0;
		return 0;
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
glm::vec3 do_movement()
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
		ret += Cam.ProcessKeyboard(FORWARD, deltaTime, flag);
	}
	if (keys[GLFW_KEY_S])
	{
		zrot -= 1.0f;
		ret += Cam.ProcessKeyboard(BACKWARD, deltaTime, flag);
	}
	if (keys[GLFW_KEY_A])
	{
		xrot += 1.0f;
		ret += Cam.ProcessKeyboard(LEFT, deltaTime, flag);
	}
	if (keys[GLFW_KEY_D])
	{
		xrot -= 1.0f;
		ret += Cam.ProcessKeyboard(RIGHT, deltaTime, flag);
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
	Cam.ProcessMouseMovement(xoffset, yoffset);
	
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
	srand(time(0));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "lab4", nullptr, nullptr);
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

	Shader shaderProgramLight("vertexShaderSourceLight.txt", "fragmentShaderSourceLight.txt");
	Shader shaderProgram("vertexShaderSource.txt", "fragmentShaderSource.txt");
	MeshAnim ourModel;
	Mesh ourModel1;
	ourModel1.LoadMesh("Mos/source/Cantina1.fbx_Scene/textures/untitled.obj");
	Mod dom("vertexShaderSource.txt", "fragmentShaderSource.txt");
	Person per("shader.vs", "fragmentShaderSource.txt");
	per.LoadModel("God/LambdaShuttle.blend.fbx", 0.3, 1.26, 0.1, 3.14f);
	//dom.LoadModel("Yoda/2.fbx");
	//Person per("vertexShaderSource.txt", "fragmentShaderSource.txt");
	//per.LoadModel("Yoda/2.fbx", 0.3, 0.518, -0.385, 0.0f);
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f,  0.0f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f,  0.0f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  1.0f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  1.0f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  1.0f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f,  0.0f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  1.0f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  1.0f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f,  0.0f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.0f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.0f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  1.0f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  1.0f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  1.0f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.0f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  1.0f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  1.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  1.0f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  1.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  1.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  1.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  1.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	GLuint VBO = 0, VAO1 = 0;
	int param1[][3] = {
		{3,8,0},{3,8,6}
	};
	makeVAOVBO(VBO, VAO1, vertices, sizeof(vertices), param1, 2, 1);
	Texture texture2(GL_TEXTURE_2D, "container7.jpg");
	texture2.Load();
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

	while (!glfwWindowShouldClose(window))
	{
		glm::vec3 ret = glm::vec3(0.0f);
		bool fleg = 1;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();
		glm:vec3 tempCamPos = Cam.Position;
		ret = do_movement();

		view = glm::mat4(1.0f);
		view = Cam.GetViewMatrix();
		projection = glm::mat4(1.0f);
		projection = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		cout << "Cam Position " << Cam.Position.x << " " << Cam.Position.y << " " << Cam.Position.z << endl;
		per.AnimNow(xrot, zrot, currentFrame);
		per.EditModelMatrix(glm::vec3(Cam.Position.x, Cam.Position.y-1.5, Cam.Position.z - 2), glm::vec3(0.003, 0.003, 0.003), xrot, zrot, deltaTime);
		if (compare(per, dom))
		{
			glm::vec3 tempret = ret;
			tempret.x = 0;
			Cam.Position = tempCamPos + tempret;
			per.EditModelMatrix(glm::vec3(Cam.Position.x, Cam.Position.y - 1.5, Cam.Position.z - 2), glm::vec3(0.003, 0.003, 0.003), xrot, zrot, deltaTime);
			fleg = compare(per, dom);
			/*if (fleg)
			{
				tempret = ret;
				tempret.y = 0;
				Cam.Position = tempCamPos + tempret;
				per.EditModelMatrix(glm::vec3(Cam.Position.x, Cam.Position.y - 1.5, Cam.Position.z - 2), glm::vec3(0.003, 0.003, 0.003), xrot, zrot, deltaTime);
				fleg = compare(per, dom);
			}*/
			if (fleg)
			{
				tempret = ret;
				tempret.z = 0;
				Cam.Position = tempCamPos + tempret;
				per.EditModelMatrix(glm::vec3(Cam.Position.x, Cam.Position.y - 1.5, Cam.Position.z - 2), glm::vec3(0.003, 0.003, 0.003), xrot, zrot, deltaTime);
				fleg = compare(per, dom);
			}
			/*if (fleg)
			{
				tempret = ret;
				tempret.x = 0;
				tempret.z = 0;
				Cam.Position = tempCamPos + tempret;
				per.EditModelMatrix(glm::vec3(Cam.Position.x, Cam.Position.y - 1.5, Cam.Position.z - 2), glm::vec3(0.003, 0.003, 0.003), xrot, zrot, deltaTime);
				fleg = compare(per, dom);
			}
			if (fleg)
			{
				tempret = ret;
				tempret.y = 0;
				tempret.z = 0;
				Cam.Position = tempCamPos + tempret;
				per.EditModelMatrix(glm::vec3(Cam.Position.x, Cam.Position.y - 1.5, Cam.Position.z - 2), glm::vec3(0.003, 0.003, 0.003), xrot, zrot, deltaTime);
				fleg = compare(per, dom);
			}
			if (fleg)
			{
				tempret = ret;
				tempret.x = 0;
				tempret.y = 0;
				Cam.Position = tempCamPos + tempret;
				per.EditModelMatrix(glm::vec3(Cam.Position.x, Cam.Position.y - 1.5, Cam.Position.z - 2), glm::vec3(0.003, 0.003, 0.003), xrot, zrot, deltaTime);
				fleg = compare(per, dom);
			}*/
			if (fleg)
			{
				Cam.Position = tempCamPos;
				per.EditModelMatrix(glm::vec3(Cam.Position.x, Cam.Position.y - 1.5, Cam.Position.z - 2), glm::vec3(0.003, 0.003, 0.003), xrot, zrot, deltaTime);
			}
		}
		per.LoadShaderUniform(view, projection, Cam.Position, light, currentFrame);
		per.Draw();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-50, Cam.Position.y - 1.4,  - 2));
		shaderProgram.Use();
		shaderProgram.setMat4("model",model);
		shaderProgram.setMat4("view", view);
		shaderProgram.setMat4("projection", projection);
		shaderProgram.setVec1("material.diffuse", 0.0f);
		shaderProgram.setVec1("material.specular", 1.0f);
		shaderProgram.setFloat("material.shininess", 32.0f);
		shaderProgram.setVec3("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
		shaderProgram.setVec3("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
		shaderProgram.setVec3("light.specular", light.specular.x, light.specular.y, light.specular.z);
		shaderProgram.setVec3("light.position", light.position.x, light.position.y, light.position.z);
		shaderProgram.setVec3("viewPos", Cam.Position.x, Cam.Position.y, Cam.Position.z);

		ourModel1.Render();



		model = glm::mat4(1.0f);
		glm::vec3 ligpos = glm::vec3(0.0f);
		model = glm::translate(model, glm::vec3(Cam.Position.x, Cam.Position.y-1.4, Cam.Position.z-2));
		model = glm::scale(model, glm::vec3(0.7, 1.26, 0.7));
		shaderProgramLight.Use();
		shaderProgramLight.setMat4("model", model);
		shaderProgramLight.setMat4("view", view);
		shaderProgramLight.setMat4("projection", projection);
		shaderProgramLight.setVec1("ourTexture", 10.0f);

		

		texture2.Bind(GL_TEXTURE10);
		glBindVertexArray(VAO1);
		glDrawArrays(GL_LINE_LOOP, 0, 36);
		glBindVertexArray(0);


		dom.EditModelMatrix(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.7, 1.26, 0.7), 0.0f, deltaTime);
		dom.Draw(view, projection);
		/*model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, 0.0, 0));
		model = glm::scale(model, glm::vec3(0.7, 1.26, 0.7));
		shaderProgramLight.Use();
		shaderProgramLight.setMat4("model", model);
		shaderProgramLight.setMat4("view", view);
		shaderProgramLight.setMat4("projection", projection);
		shaderProgramLight.setVec1("ourTexture", 10.0f);

		texture2.Bind(GL_TEXTURE10);
		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);*/

		
		glDepthFunc(GL_LEQUAL);

		Sky_Box.Use();

		view = glm::mat4(glm::mat3(Cam.GetViewMatrix()));
		Sky_Box.setMat4("view", view);
		Sky_Box.setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		Sky_Tex.Bind(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO1);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &skyboxVBO);
	glfwTerminate();
	return 0;
}
