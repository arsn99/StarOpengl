#ifndef TEXTURE_H
#define	TEXTURE_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2.h>
#include <iostream>
#include <vector>
#include <string>
class Texture
{
public:
	Texture(GLenum TextureTarget, const std::string& FileName)
	{
		m_textureTarget = TextureTarget;
		m_fileName = FileName;
		image = NULL;
		m_textureObj = 0.0f;
		//std::cout << "construct m_textureObj=" << m_textureObj << std::endl;
	}
	Texture(GLenum TextureTarget)
	{
		image = NULL;
		m_textureObj = 0;
		m_textureTarget = TextureTarget;
	}
    bool Load()
	{
		int width, height;
		image = SOIL_load_image(m_fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &m_textureObj);
		glBindTexture(m_textureTarget, m_textureObj);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(m_textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(m_textureTarget);
		SOIL_free_image_data(image);
		glBindTexture(m_textureTarget, 0);
		//std::cout << "Load m_textureObj=" << m_textureObj << std::endl;

		return true;
	}
	bool Load_CubeMap(std::vector <std::string> faces)
	{
		int width, height;


		glGenTextures(1, &m_textureObj);
		glBindTexture(m_textureTarget, m_textureObj);

		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* images = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			if (images)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, images);
				SOIL_free_image_data(images);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				SOIL_free_image_data(images);
			}
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return true;
	}
    void Bind(GLenum TextureUnit)
	{
		glActiveTexture(TextureUnit);
		glBindTexture(m_textureTarget, m_textureObj);
		//std::cout << "BIND m_textureObj=" << m_textureObj << std::endl;
	}

//private:
    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    unsigned char* image;
};



/*Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
    m_textureTarget = TextureTarget;
    m_fileName      = FileName;
    image        = NULL;
}

bool Texture::Load()
{
	int width, height;
	image = SOIL_load_image(m_fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

    glGenTextures(1, &m_textureObj);
    glBindTexture(m_textureTarget, m_textureObj);
    glTexImage2D(m_textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void Texture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}*/

#endif	/* TEXTURE_H */

