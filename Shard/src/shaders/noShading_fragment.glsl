# version 460 core

layout(location = 0) out vec4 FragColor;

in vec4 vertexNormal; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexTangent; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexPos; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexCol; // the input variable from the vertex shader (same name and same type)  
in vec2 texcoords; // the input variable from the vertex shader (same name and same type)  
in mat4 fmode_m;

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

uniform bool is_instance;

#define solid_color VectorParameter0
#define camera_pos VectorParameter1

void main()
{
    float f = clamp((distance(camera_pos, vertexPos) - 200.0) / 1000.0, 0.0, 1.0);
    vec4 diffuse_color = vec4(normalize(vertexPos.xyz), 1.0);
    // vec4 diffuse_color = vertexCol;
    FragColor = mix(texture(TextureImage0, texcoords), solid_color, f);
    // FragColor = mix(diffuse_color, solid_color, f);
}