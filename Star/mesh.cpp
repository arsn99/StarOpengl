#include <assert.h>
#include <iostream>

#include "mesh.h"

Mesh::MeshEntry::MeshEntry()
{
    VB = 0;
    IB = 0;
	VAO = 0;
    NumIndices = 0;
    MaterialIndex = INVALID_MATERIAL;
}

Mesh::MeshEntry::~MeshEntry()
{
    if (VB != 0){
        glDeleteBuffers(1, &VB);
    }

    if (IB != 0){
        glDeleteBuffers(1, &IB);
    }


	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
	}
}

bool Mesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
                      const std::vector<unsigned int>& Indices)
{
    NumIndices = Indices.size();

    glGenBuffers(1, &VB);
    glGenBuffers(1, &IB);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*Vertices.size(),&Vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*NumIndices, &Indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_tex));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return true;
}

void Mesh::Clear()
{
	m_TexturesDif.clear();
	m_TexturesSpec.clear();
}

bool Mesh::LoadMesh(const std::string& Filename)
{
    // Удаляем данные предыдущей модели (если она была загружена)
    Clear();

    bool Ret = false;

    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(),
                                    aiProcess_Triangulate | aiProcess_GenSmoothNormals| aiProcess_FlipUVs);

    if (pScene){
        Ret = InitFromScene(pScene, Filename);
    }
    else {
		std::cout << "Error parsing " << Filename.c_str() << " : " << Importer.GetErrorString() << std::endl;
    }

    return Ret;
}

bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
	m_TexturesDif.resize(pScene->mNumMaterials);
	m_TexturesSpec.resize(pScene->mNumMaterials);
    // Инициализируем меши один за другим
    for (unsigned int i = 0; i < m_Entries.size(); i++){
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    return InitMaterials(pScene, Filename);
}

void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;
	
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

	std::cout <<  "stat  "  << std::endl;
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    for (unsigned int i = 0; i< paiMesh->mNumVertices; i++){
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);


        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ?
                            &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		//std::cout << pTexCoord->x << "  " << pTexCoord->y << std::endl;
        Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
				 glm::vec2(pTexCoord->x,pTexCoord->y),
				 glm::vec3(pNormal->x, pNormal->y, pNormal->z));
        Vertices.push_back(v);
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++){
        const aiFace& Face = paiMesh->mFaces[i];
	  // assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}

bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Извлекаем директорию из полного имени файла
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos){
        Dir = ".";
    }
    else if (SlashIndex == 0){
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Инициализируем материал
    for (unsigned int i = 0; i< pScene->mNumMaterials; i++){
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_TexturesDif[i] = NULL;
		m_TexturesSpec[i] = NULL;
        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			std::cout << "stat text" << " type dif "  << std::endl;
            aiString Path;
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
                std::string FullPath = Dir + "/" + Path.data;
				m_TexturesDif[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());
				std::cout << "stat text "<<  FullPath.c_str() << " type dif " << aiTextureType_DIFFUSE << std::endl;

                if (!m_TexturesDif[i]->Load()){
					std::cout << "Error loading texture" << FullPath.c_str() << std::endl;
                    delete m_TexturesDif[i];
					m_TexturesDif[i] = NULL;
                    Ret = false;
                }
            }
        }
		else 
		{
			std::cout << "stat text fail dif"  << std::endl;
		}
        if (!m_TexturesDif[i]){
			m_TexturesDif[i] = new Texture(GL_TEXTURE_2D, "white.png");
            Ret = m_TexturesDif[i]->Load();
        }

		if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			std::cout << "stat text" << " type spec "  << std::endl;
			aiString Path;
			if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string FullPath = Dir + "/" + Path.data;
				m_TexturesSpec[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());
				std::cout << "stat text " << FullPath.c_str() << " type spec " << aiTextureType_SPECULAR << std::endl;

				if (!m_TexturesSpec[i]->Load()) {
					std::cout << "Error loading texture" <<  FullPath.c_str() << std::endl;
					delete m_TexturesSpec[i];
					m_TexturesSpec[i] = NULL;
					Ret = false;
				}
			}
		}
		else
		{
			std::cout << "stat text fail spec" << std::endl;
		}
		if (!m_TexturesSpec[i]) {
			m_TexturesSpec[i] = new Texture(GL_TEXTURE_2D, "black.png");
			Ret = m_TexturesSpec[i]->Load();
		}
    }

    return Ret;
}

void Mesh::Render()
{

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		glBindVertexArray(m_Entries[i].VAO);
		const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;
		if (MaterialIndex < m_TexturesDif.size() && m_TexturesDif[MaterialIndex]) {
			m_TexturesDif[MaterialIndex]->Bind(GL_TEXTURE0);
			//std::cout << "stat dif " << m_TexturesDif[MaterialIndex]->m_fileName << std::endl;
		}
		if (MaterialIndex < m_TexturesSpec.size() && m_TexturesSpec[MaterialIndex]) {
			m_TexturesSpec[MaterialIndex]->Bind(GL_TEXTURE1);
			//std::cout << "stat spec " << m_TexturesSpec[MaterialIndex]->m_fileName << std::endl;
		}
		glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

}
