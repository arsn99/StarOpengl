#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;  
in vec3 Normal;
 
uniform vec3 viewPos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

struct Light {
    vec3 position;  
    vec3 direction;
	
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float cutOff;
    float outerCutOff;
	
	float constant;
    float linear;
    float quadratic;

};

uniform Light light;
void main()
{    
	vec3 ambient= 0.7f*vec3(texture(texture_diffuse1, TexCoords));
	
	vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords)); 
	
	vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
	
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
	
	float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
	
	 float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (1.0f * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
	
	vec3 result = ambient + diffuse + specular;
	
    FragColor = vec4(result, 1.0);
}