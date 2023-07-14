#version 460 core
layout (location = 0) in vec4 aPos; 
layout (location = 1) in vec4 vNormal; 
layout (location = 2) in vec4 vTan; 
layout (location = 3) in vec2 uv;
layout (location = 4) in vec4 instance_M0; 
layout (location = 5) in vec4 instance_M1; 
layout (location = 6) in vec4 instance_M2; 
layout (location = 7) in vec4 instance_M3;

mat4 inst_m = mat4(instance_M0, instance_M1, instance_M2, instance_M3); 

uniform mat4 mode_m;
uniform mat4 view_m;
uniform mat4 proj_m;

uniform bool is_instance;

uniform float FloatParameter0;
uniform float FloatParameter1;
uniform float FloatParameter2;
uniform float FloatParameter3;
uniform float FloatParameter4;
uniform float FloatParameter5;

uniform vec4 VectorParameter0;
uniform vec4 VectorParameter1;
uniform vec4 VectorParameter2;
uniform vec4 VectorParameter3;
uniform vec4 VectorParameter4;
uniform vec4 VectorParameter5;

uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;
uniform sampler2D TextureImage4;
uniform sampler2D TextureImage5;

#define PI 3.14159265359
#define f 100
#define n 0.1

out vec4 vertexNormal;
out vec4 vertexTangent;
out vec4 vertexPos;
out vec4 vertexCol;
out vec2 texcoords;
out mat4 fmode_m;

void main()
{
    // model matrix

    // view matrix

    // projection matrix

    texcoords = uv;

    if(is_instance){
        fmode_m = inst_m * mode_m;
        vertexPos = inst_m * mode_m * aPos;
        vertexNormal = normalize(inverse(transpose(mode_m)) * vNormal);
        vertexTangent = normalize(inverse(transpose(mode_m)) * vTan);
    }
    else{
        fmode_m = mode_m;
        vertexPos = mode_m * aPos;
        vertexNormal = normalize(inverse(transpose(mode_m)) * vNormal);
        vertexTangent = normalize(inverse(transpose(mode_m)) * vTan);
    }

    vertexCol = vNormal;

    gl_Position = proj_m * view_m * vertexPos; // see how we directly give a vec3 to vec4's constructor
}
