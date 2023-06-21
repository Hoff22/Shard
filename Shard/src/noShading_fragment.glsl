# version 460 core

layout(location = 0) out vec4 FragColor;

in vec4 vertexNormal; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexTangent; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexPos; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexCol; // the input variable from the vertex shader (same name and same type)  
in vec2 texcoords; // the input variable from the vertex shader (same name and same type)  
in mat4 fmode_m;

uniform vec4 solid_color;
uniform vec4 camera_pos;
uniform sampler2D TextureImage0;

vec4 lerp(vec4 a, vec4 b, float alpha){

    return ((b - a) * alpha) + a;
}

float lerp(float a, float b, float alpha){

    return ((b - a) * alpha) + a;
}


void main()
{
    //FragColor = (dot(vec4(1.0,1.0,0.0,0.0),vertexNormal)+1.0) * solid_color / 2.0;
    //FragColor = (dot(vec4(1.0,1.0,0.0,0.0),vertexNormal)+1.0) * vec4(texcoords, 0.0, 1.0) / 2.0;
    //FragColor = solid_color;
    //FragColor = vec4(vertexCol.xyz, 1.0);
    //FragColor = vec4(vec3(-6.0,-6.0,0.0), 1.0);
    float f = clamp((distance(camera_pos, vertexPos) - 200.0) / 1000.0, 0.0, 1.0);
    FragColor = lerp(texture(TextureImage0, texcoords), solid_color, f);
}