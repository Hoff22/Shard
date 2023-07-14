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

uniform float time;

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
#define displace_amt 0.25
//#define f 100
//#define n 0.1

out vec4 vertexNormal;
out vec4 vertexTangent;
out vec4 vertexPos;
out vec4 worldPos;
out vec4 vertexCol;
out vec2 texcoords;
out mat4 fmode_m;
out float displace_factor;


float luminance(vec4 rgba_color){
    return ((rgba_color.x + rgba_color.y + rgba_color.z) / 3.0) * rgba_color.w;
}

float rand(vec2 c){
    return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
    float unit = 1000/freq;
    vec2 ij = floor(p/unit);
    vec2 xy = mod(p,unit)/unit;
    //xy = 3.*xy*xy-2.*xy*xy*xy;
    xy = 0.5*(1.0-cos(PI*xy));
    float a = rand((ij+vec2(0.0,0.0)));
    float b = rand((ij+vec2(1.0,0.0)));
    float c = rand((ij+vec2(0.0,1.0)));
    float d = rand((ij+vec2(1.0,1.0)));
    float x1 = mix(a, b, xy.x);
    float x2 = mix(c, d, xy.x);
    return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
    float persistance = 0.5;
    float n = 0.0;
    float normK = 0.0;
    float f = 2.0;
    float amp = 1.0;
    int iCount = 0;
    for (int i = 0; i<5; i++){
        n+=amp*noise(p, f);
        f*=2.0;
        normK+=amp;
        amp*=persistance;
        if (iCount == res) break;
        iCount++;
    }
    float nf = n/normK;
    return nf*nf*nf*nf;
}

float pNoise(vec2 p){
    return luminance(texture(TextureImage2, p));
}

void main()
{
    // model matrix

    // view matrix

    // projection matrix

    texcoords = uv;
    vertexPos = aPos;
    displace_factor = displace_amt;

    if(is_instance){
        fmode_m = inst_m * mode_m;
        worldPos = inst_m * mode_m * aPos;
        vertexNormal = normalize(inverse(transpose(mode_m)) * vNormal);
        vertexTangent = normalize(inverse(transpose(mode_m)) * vTan);
    }
    else{
        fmode_m = mode_m;
        worldPos = mode_m * (aPos + vNormal * pNoise(uv) * displace_amt);
        vertexNormal = normalize(inverse(transpose(mode_m)) * vNormal);
        vertexTangent = normalize(inverse(transpose(mode_m)) * vTan);
    }

    gl_Position = proj_m * view_m * worldPos; // see how we directly give a vec3 to vec4's constructor
}
