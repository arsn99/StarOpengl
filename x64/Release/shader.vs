#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;


out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 gBones[100];

void main()
{
	mat4 BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += gBones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += gBones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += gBones[aBoneIDs[3]] * aWeights[3];
	
	vec4 PosL      = BoneTransform * vec4(aPos, 1.0);
    gl_Position    = projection * view * model * PosL;
    TexCoord = aTexCoords;
    vec4 NormalL   = BoneTransform * vec4(aNormal, 0.0);
    FragPos = vec3(model * PosL);
	Normal =  vec3(model * NormalL);
}