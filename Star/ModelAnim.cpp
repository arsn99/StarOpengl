#include "ModelAnim.h"
#include <iostream>


ModAnim::ModAnim(const GLchar* vertexPath, const GLchar* fragmentPath) :shaderProgram(vertexPath, fragmentPath), Model(), shaderProgramLight("vertexShaderSourceLight.txt", "fragmentShaderSourceLight.txt")
{
	vectors[0] = glm::vec3(0.5f, 0.0f, -0.5f);
	vectors[1] = glm::vec3(0.5f, 0.0f, 0.5f);
	vectors[2] = glm::vec3(-0.5f, 0.0f, 0.5f);
	vectors[3] = glm::vec3(-0.5f, 0.0f, -0.5f);
	vectors[4] = glm::vec3(0.5f, 1.0f, -0.5);
	vectors[5] = glm::vec3(0.5f, 1.0f, 0.5f);
	vectors[6] = glm::vec3(-0.5f, 1.0f, 0.5f);
	vectors[7] = glm::vec3(-0.5f, 1.0f, -0.5f);
	NumAnim = 0;
	AnimStart = 0;
	TimeAnim = nullptr;
}
ModAnim::ModAnim() : shaderProgram(), Model()
{
	vectors[0] = glm::vec3(0.5f, 0.0f, -0.5f);
	vectors[1] = glm::vec3(0.5f, 0.0f, 0.5f);
	vectors[2] = glm::vec3(-0.5f, 0.0f, 0.5f);
	vectors[3] = glm::vec3(-0.5f, 0.0f, -0.5f);
	vectors[4] = glm::vec3(0.5f, 1.0f, -0.5);
	vectors[5] = glm::vec3(0.5f, 1.0f, 0.5f);
	vectors[6] = glm::vec3(-0.5f, 1.0f, 0.5f);
	vectors[7] = glm::vec3(-0.5f, 1.0f, -0.5f);
	NumAnim = 0;
	AnimStart = 0;
	TimeAnim = nullptr;
}



void makeVAOVBO222(GLuint &VBO, GLuint &VAO, GLfloat vertices[], int size, int param[][3], int count, bool flagV)
{
	//cout << "start " << flagV << endl;
	if (flagV) glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//cout << "bind" << size << endl;
	if (flagV)
	{
		//cout << "ok " << VBO << endl;
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		//cout << "ok " << VBO << endl;
	}
	//cout << "go " << count << endl;
	for (int i = 0; i < count; i++)
	{
		//	cout << "step " << i << endl;
		glVertexAttribPointer(i, param[i][0], GL_FLOAT, GL_FALSE, sizeof(GLfloat) * param[i][1], (GLvoid*)(param[i][2] * sizeof(GLfloat)));
		glEnableVertexAttribArray(i);
	}
	//cout << "fin" << endl;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//cout << "end" << endl;
}



bool ModAnim::LoadModel(const GLchar* vertexPath)
{
	bool flag = Model.LoadMesh(vertexPath);
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

void ModAnim::Draw(glm::mat4 view, glm::mat4 projection, bool flag)
{
	Model.Render();
	if (flag)
	{
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
		makeVAOVBO222(VBO, VAO1, vertices, sizeof(vertices), param1, 2, 1);

		shaderProgramLight.Use();
		shaderProgramLight.setMat4("model", HitBoxMatrix);
		shaderProgramLight.setMat4("view", view);
		shaderProgramLight.setMat4("projection", projection);
		shaderProgramLight.setVec1("ourTexture", 10.0f);
		glBindVertexArray(VAO1);
		glDrawArrays(GL_LINE_LOOP, 0, 36);
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &VAO1);
		glDeleteBuffers(1, &VBO);
	}
}


void  ModAnim::EditModelMatrix(glm::vec3 Posit, glm::vec3 ScaleVec,  GLfloat rot)
{
	Position = Posit;
	MyRot = rot;
	ModelMatrix = glm::mat4(1.0f);
	ModelMatrix = glm::translate(ModelMatrix, Position);
	ModelMatrix = glm::rotate(ModelMatrix, MyRot, glm::vec3(0.0, 1.0, 0.0));
	ModelMatrix = glm::scale(ModelMatrix, ScaleVec);
}


void  ModAnim::EditHitBoxMatrix(glm::vec3 ScaleVec, glm::vec3 Posit)
{
	HitBoxMatrix = glm::mat4(1.0f);
	HitBoxMatrix = glm::translate(HitBoxMatrix, Position+Posit);
	HitBoxMatrix = glm::rotate(HitBoxMatrix, MyRot, glm::vec3(0.0, 1.0, 0.0));
	HitBoxMatrix = glm::scale(HitBoxMatrix, ScaleVec);
	for (int i = 0; i < 8; i++)
	{
		vectors[i] = HitBoxMatrix * glm::vec4(vectors[i], 1.0f);
	}

}


void ModAnim::LoadShaderUniform(glm::mat4 view, glm::mat4 projection, glm::vec3 Cam, LightUnit light, GLfloat currentFrame)
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
	Model.BoneTransform((currentFrame - AnimStart), Transforms, NumAnim);
	for (int i = 0; i < Transforms.size(); i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));
		sprintf_s(Name, sizeof(Name), "gBones[%d]", i);
		shaderProgram.setMat4(Name, Transforms[i]);
	}
}


void ModAnim::AnimNow(int num)
{
	//AnimStart = (GLfloat)glfwGetTime();
	NumAnim = num;

}

ModAnim::~ModAnim()
{
	delete[] TimeAnim;
}
