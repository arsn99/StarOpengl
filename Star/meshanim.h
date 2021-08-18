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

#ifndef MESHANIM_H
#define	MESHANIM_H

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Texture.h"

using namespace std;

class MeshAnim
{
public:
	const aiScene* m_pScene;
    MeshAnim();

    ~MeshAnim();

    bool LoadMesh(const string& Filename);

    void Render();

    unsigned int NumBones() const
    {
        return m_NumBones;
    }
    
    void BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms, int numberanimatiao);
    
private:
    #define NUM_BONES_PER_VEREX 4

    struct BoneInfo
    {
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;

        BoneInfo()
        {
			BoneOffset = glm::mat4(0.0f);
			FinalTransformation = glm::mat4(0.0f);          
        }
    };
    
    struct VertexBoneData
    {        
		unsigned int IDs[NUM_BONES_PER_VEREX];
        float Weights[NUM_BONES_PER_VEREX];

        VertexBoneData()
        {
            Reset();
        };
        
		void Reset()
		{
			for (int i = 0; i < 4; i++)
			{
				IDs[i] = 0;
				Weights[i] = 0;
			}
		}
        
        void AddBoneData(unsigned int BoneID, float Weight);
    };

    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);    
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform, int numberanimatiao);
    bool InitFromScene(const aiScene* pScene, const string& Filename);
    void InitMesh(unsigned int MeshIndex,
                  const aiMesh* paiMesh,
                  vector<glm::vec3>& Positions,
                  vector<glm::vec3>& Normals,
                  vector<glm::vec2>& TexCoords,
                  vector<VertexBoneData>& Bones,
                  vector<unsigned int>& Indices);
    void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
    bool InitMaterials(const aiScene* pScene, const string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF
  
enum VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    NORMAL_VB,
    TEXCOORD_VB,
    BONE_VB,
    NUM_VBs            
};

    GLuint m_VAO;
    GLuint m_Buffers[NUM_VBs];

    struct MeshEntry {
        MeshEntry()
        {
            NumIndices    = 0;
            BaseVertex    = 0;
            BaseIndex     = 0;
            MaterialIndex = INVALID_MATERIAL;
        }
        
        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };
    
    vector<MeshEntry> m_Entries;
	std::vector<Texture*> m_TexturesDif;
	std::vector<Texture*> m_TexturesSpec;
     
    map<string, unsigned int> m_BoneMapping; // maps a bone name to its index
	unsigned int m_NumBones;
    vector<BoneInfo> m_BoneInfo;
	glm::mat4 m_GlobalInverseTransform;
    
    Assimp::Importer m_Importer;
};


#endif	/* MESHANIM_H */

