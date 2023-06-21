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

#define PI 3.14159265359
#define f 100
#define n 0.1

out vec4 vertexNormal;
out vec4 vertexTangent;
out vec4 vertexPos;
out vec4 vertexCol;
out vec2 texcoords;
out mat4 fmode_m;

uniform float thickness;

void main()
{
    // model matrix

    // view matrix

    // projection matrix

    fmode_m = mode_m;

    vertexCol = instance_M1;

    texcoords = uv;
    vertexPos = inst_m * mode_m * aPos;

    vertexNormal = normalize(inverse(transpose(mode_m)) * vNormal);

    vertexTangent = inverse(transpose(mode_m)) * vTan;
    
    gl_Position = proj_m * view_m * vertexPos; // see how we directly give a vec3 to vec4's constructor


}
