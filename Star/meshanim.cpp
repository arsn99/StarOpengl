/*

	Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <iostream>

#include "meshanim.h"

#define POSITION_LOCATION    0
#define NORMAL_LOCATION      1
#define TEX_COORD_LOCATION   2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4


void CopyMatrix(aiMatrix4x4 from, glm::mat4& to)
{
	to[0][0] = from.a1;   to[0][1] = from.b1;    to[0][2] = from.c1;    to[0][3] = from.d1;

	to[1][0] = from.a2;   to[1][1] = from.b2;    to[1][2] = from.c2;    to[1][3] = from.d2;

	to[2][0] = from.a3;   to[2][1] = from.b3;    to[2][2] = from.c3;    to[2][3] = from.d3;

	to[3][0] = from.a4;	  to[3][1] = from.b4;	 to[3][2] = from.c4;    to[3][3] = from.d4; 
	
}


void CopyMatrix3x3(aiMatrix3x3 from, glm::mat4& to)

{
	to[0][0] = from.a1;   to[0][1] = from.b1;    to[0][2] = from.c1;    to[0][3] = 0.0f;

	to[1][0] = from.a2;   to[1][1] = from.b2;    to[1][2] = from.c2;    to[1][3] = 0.0f;

	to[2][0] = from.a3;   to[2][1] = from.b3;    to[2][2] = from.c3;    to[2][3] = 0.0f;

	to[3][0] = 0.0f;	  to[3][1] = 0.0f;		 to[3][2] = 0.0f;		to[3][3] = 1.0f;

}


void MeshAnim::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
    for (unsigned int i = 0 ; i < sizeof(IDs)/sizeof(unsigned int); i++) {
        if (Weights[i] == 0.0) {
            IDs[i]     = BoneID;
            Weights[i] = Weight;
            return;
        }        
    }
}

MeshAnim::MeshAnim()
{
    m_VAO = 0;
	for (int i = 0; i < sizeof(m_Buffers) / sizeof(GLuint); i++)
	{
		m_Buffers[i] = 0;
	}
    m_NumBones = 0;
    m_pScene = NULL;
}


MeshAnim::~MeshAnim()
{
    Clear();
}


void MeshAnim::Clear()
{

	m_TexturesDif.clear();
	m_TexturesSpec.clear();
    if (m_Buffers[0] != 0) {
        glDeleteBuffers(sizeof(m_Buffers) / sizeof(GLuint), m_Buffers);
    }
       
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}


bool MeshAnim::LoadMesh(const string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();
 
    // Create the VAO
    glGenVertexArrays(1, &m_VAO);   
    glBindVertexArray(m_VAO);
    
    // Create the buffers for the vertices attributes
    glGenBuffers(sizeof(m_Buffers) / sizeof(GLuint), m_Buffers);

    bool Ret = false;    
  
    m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if (m_pScene) {  
		CopyMatrix(m_pScene->mRootNode->mTransformation, m_GlobalInverseTransform);
		m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
        Ret = InitFromScene(m_pScene, Filename);
		cout << "Ret meshan" << Ret << endl;
    }
    else {
       
		cout << "Error parsing "<< Filename.c_str() << " : " <<  m_Importer.GetErrorString() << endl;
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	
	cout << "RETM" << Ret << endl;
    return Ret;
}


bool MeshAnim::InitFromScene(const aiScene* pScene, const string& Filename)
{  
    m_Entries.resize(pScene->mNumMeshes);
	m_TexturesDif.resize(pScene->mNumMaterials);
	m_TexturesSpec.resize(pScene->mNumMaterials);

    vector<glm::vec3> Positions;
    vector<glm::vec3> Normals;
    vector<glm::vec2> TexCoords;
    vector<VertexBoneData> Bones;
    vector<unsigned int> Indices;
       
	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;
    // Count the number of vertices and indices
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		
        m_Entries[i].NumIndices    = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex    = NumVertices;
        m_Entries[i].BaseIndex     = NumIndices;
        
        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices  += m_Entries[i].NumIndices;
    }
    
    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Bones.resize(NumVertices);
    Indices.reserve(NumIndices);
        
    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
    }

    if (!InitMaterials(pScene, Filename)) {
		cout << "check" << endl;
		return false;
    }
	cout << "check2" << endl;
    // Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);


   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);    
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
	cout << "glGetError()"<< glGetError() << endl;
    return glGetError();
}


void MeshAnim::InitMesh(unsigned int MeshIndex,
                    const aiMesh* paiMesh,
                    vector<glm::vec3>& Positions,
                    vector<glm::vec3>& Normals,
                    vector<glm::vec2>& TexCoords,
                    vector<VertexBoneData>& Bones,
                    vector<unsigned int>& Indices)
{    
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	std::cout << "anim  " << std::endl;
    // Populate the vertex attribute vectors
    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
	//	std::cout << pTexCoord->x << "  " << pTexCoord->y << std::endl;
        Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
        TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
    }
    
    LoadBones(MeshIndex, paiMesh, Bones);
    
    // Populate the index buffer
    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        //assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}


void MeshAnim::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
    for (unsigned int i = 0 ; i < pMesh->mNumBones ; i++)
	{                
		unsigned int BoneIndex = 0;
        string BoneName(pMesh->mBones[i]->mName.data);
        
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
		{
            // Allocate an index for a new bone
            BoneIndex = m_NumBones;
            m_NumBones++;            
	        BoneInfo bi;			
			m_BoneInfo.push_back(bi);
           // m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;    
			CopyMatrix(pMesh->mBones[i]->mOffsetMatrix,m_BoneInfo[BoneIndex].BoneOffset);
            m_BoneMapping[BoneName] = BoneIndex;
        }
        else 
		{
            BoneIndex = m_BoneMapping[BoneName];
        }                      
        for (unsigned int j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++)
		{
			unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;                   
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }    
}


bool MeshAnim::InitMaterials(const aiScene* pScene, const string& Filename)
{
	// Извлекаем директорию из полного имени файла
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Инициализируем материал
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		m_TexturesDif[i] = NULL;
		m_TexturesSpec[i] = NULL;
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			std::cout << "anim text" << " type dif " << std::endl;
			aiString Path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string FullPath = Dir + "/" + Path.data;
				m_TexturesDif[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());
				std::cout << "anim text " << FullPath.c_str() << " type dif " << aiTextureType_DIFFUSE << std::endl;

				if (!m_TexturesDif[i]->Load()) {
					std::cout << "Error loading texture" << FullPath.c_str() << std::endl;
					delete m_TexturesDif[i];
					m_TexturesDif[i] = NULL;
					Ret = false;
				}
			}
		}
		else
		{
			std::cout << "anim text fail dif" << std::endl;
		}
		if (!m_TexturesDif[i]) {
			m_TexturesDif[i] = new Texture(GL_TEXTURE_2D, "white.png");
			Ret = m_TexturesDif[i]->Load();
		}

		if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			std::cout << "anim text" << " type spec " << std::endl;
			aiString Path;
			if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string FullPath = Dir + "/" + Path.data;
				m_TexturesSpec[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());
				std::cout << "anim text " << FullPath.c_str() << " type spec " << aiTextureType_SPECULAR << std::endl;

				if (!m_TexturesSpec[i]->Load()) {
					std::cout << "Error loading texture" << FullPath.c_str() << std::endl;
					delete m_TexturesSpec[i];
					m_TexturesSpec[i] = NULL;
					Ret = false;
				}
			}
		}
		else
		{
			std::cout << "anim text fail spec" << std::endl;
		}
		if (!m_TexturesSpec[i]) {
			m_TexturesSpec[i] = new Texture(GL_TEXTURE_2D, "black.png");
			Ret = m_TexturesSpec[i]->Load();
		}
	}
	
	return Ret;
}


void MeshAnim::Render()
{
    glBindVertexArray(m_VAO);
    
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        assert(MaterialIndex < m_TexturesSpec.size());
		assert(MaterialIndex < m_TexturesDif.size());
        
        if (m_TexturesDif[MaterialIndex]) {
			m_TexturesDif[MaterialIndex]->Bind(GL_TEXTURE0);
			//cout << "anim dif " << m_TexturesDif[MaterialIndex]->m_fileName << endl;
        }
		if (m_TexturesSpec[MaterialIndex]) {
			m_TexturesSpec[MaterialIndex]->Bind(GL_TEXTURE1);
			//cout << "anim spec " << m_TexturesSpec[MaterialIndex]->m_fileName << endl;
		}
		glDrawElementsBaseVertex(GL_TRIANGLES, 
                                 m_Entries[i].NumIndices, 
                                 GL_UNSIGNED_INT, 
                                 (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
                                 m_Entries[i].BaseVertex);
    }

    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);
}


unsigned int MeshAnim::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{    
    for (unsigned int i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


unsigned int MeshAnim::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


unsigned int MeshAnim::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    
    for (unsigned int i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);

    return 0;
}


void MeshAnim::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }
            
	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void MeshAnim::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    
	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;    
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}


void MeshAnim::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}



void MeshAnim::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform, int numberanimatiao)
{    
    string NodeName(pNode->mName.data);

	if (NodeName == "")
	{
		for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
			ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], ParentTransform, numberanimatiao);
		}
		return;
	}

    const aiAnimation* pAnimation = m_pScene->mAnimations[numberanimatiao];
        
	glm::mat4 NodeTransformation;
	CopyMatrix(pNode->mTransformation, NodeTransformation);
     
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    if (pNodeAnim) {
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        glm::mat4 ScalingM =  glm::mat4(1.0f);
		ScalingM = glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z));
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);  
		glm::mat4 RotationM = glm::mat4(1.0f);
		CopyMatrix3x3(RotationQ.GetMatrix(), RotationM);
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 TranslationM = glm::mat4(1.0f);
		TranslationM = glm::translate(TranslationM, glm::vec3(Translation.x, Translation.y, Translation.z));
        NodeTransformation = TranslationM * RotationM * ScalingM;

		
    }
       
	glm::mat4 GlobalTransformation = ParentTransform* NodeTransformation;

	
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		unsigned int BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }
    for (unsigned int i = 0 ; i < pNode->mNumChildren ; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, numberanimatiao);
    }

}



void MeshAnim::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms, int numberanimatiao)
{
	
		glm::mat4 Identity = glm::mat4(1.0f);
		float TicksPerSecond = (float)(m_pScene->mAnimations[numberanimatiao]->mTicksPerSecond != 0 ? m_pScene->mAnimations[numberanimatiao]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSeconds * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[numberanimatiao]->mDuration);
		ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity, numberanimatiao);
		Transforms.resize(m_NumBones);
		for (unsigned int i = 0; i < m_NumBones; i++) {
			Transforms[i] = m_BoneInfo[i].FinalTransformation;
		}
	
}


const aiNodeAnim* MeshAnim::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
    for (unsigned int i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        
        if (string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }
    
    return NULL;
}