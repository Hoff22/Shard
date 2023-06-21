# version 460 core

layout(location = 0) out vec4 FragColor;

in vec4 vertexNormal; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexTangent; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexPos; // the input variable from the vertex shader (same name and same type)  
in vec2 texcoords; // the input variable from the vertex shader (same name and same type)
in mat4 fmode_m;

uniform vec4 solid_color;

vec4 lerp(vec4 a, vec4 b, float alpha){

    return ((b - a) * alpha) + a;
}

float lerp(float a, float b, float alpha){

    return ((b - a) * alpha) + a;
}


void main()
{
    vec4 object_space_vertex_Pos = inverse(fmode_m) * vertexPos;
    FragColor = vec4(normalize(abs(object_space_vertex_Pos.xyz)), 1.0);
}