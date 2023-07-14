# version 460 core

layout(location = 0) out vec4 FragColor;

in vec4 vertexNormal; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexTangent; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexPos; // the input variable from the vertex shader (same name and same type)  
in vec4 worldPos; // the input variable from the vertex shader (same name and same type)  
in vec4 vertexCol; // the input variable from the vertex shader (same name and same type)  
in vec2 texcoords; // the input variable from the vertex shader (same name and same type)  
in mat4 fmode_m;
in float displace_factor;

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

#define PI 3.1415926535897932384626433832795028841971
#define solid_color VectorParameter0
#define camera_pos VectorParameter1
#define camera_dir VectorParameter2
#define object_pos VectorParameter3
#define d_factor (VectorParameter4.x / 100.0)
#define s_factor (VectorParameter4.y / 100.0)
#define r_factor (VectorParameter4.z / 100.0)
#define specular_q (VectorParameter4.w)

vec4 light_direction = vec4(-1.0,-1.0,0.0,0.0);
vec4 light_color;
vec4 camera_vector = normalize(camera_pos - worldPos);

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
    for (int i = 0; i<8; i++){
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

vec4 correct_normals_displacement(float displace_amt, float step){
    vec4 biTan = normalize(vec4(cross(vertexNormal.xyz, vertexTangent.xyz), 0.0));
    vec4 corrected_normal;
    vec4 corrected_tangent;
    vec4 corrected_bitangent;

    mat4 ts;
    ts[0] = vertexTangent;
    ts[1] = biTan;
    ts[2] = vertexNormal;
    ts[3] = vec4(0.0,0.0,0.0,1.0);

    float texel = 1.0/2048;
    vec4 du = vertexTangent * texel; 
    vec4 dv = biTan * texel; 

    float dif_u = pNoise((texcoords + vec2(texel,0.0))) - pNoise(texcoords);
    float dif_v = pNoise((texcoords + vec2(0.0,texel))) - pNoise(texcoords);

    vec4 p1 = du + dif_u * vertexNormal * displace_factor; 
    vec4 p2 = dv + dif_v * vertexNormal * displace_factor; 

    vec4 p0 = vertexPos;

    corrected_tangent = normalize(p1);
    corrected_bitangent = normalize(p2);
    corrected_normal = normalize(vec4(cross(corrected_tangent.xyz, corrected_bitangent.xyz), 0.0));

    //return normalize(corrected_tangent);
    return corrected_normal;
}
/*
    // aliased grafic shape
    float dist_from_edge = 1.0-length(texcoords);

    FragColor = vec4(mix(1.0,0.0, clamp((dist_from_edge / (length(vec2(dFdy(dist_from_edge),dFdy(dist_from_edge))))) + 0.5, 0.0,1.0)));
*/

void main()
{

    vec4 vn = correct_normals_displacement(1.0,1.0/2048.0);

    vec4 color;
    vec4 diffuse;
    vec4 specular;
    vec4 reflection;

    vec4 phong_angle = normalize(light_direction + 2 * vn * dot(vn, -light_direction));
    vec4 reflection_angle = normalize(-camera_vector + 2 * vn * dot(vn, camera_vector));
    vec2 envtex_coords = vec2(
        1.0 - (atan(reflection_angle.z, reflection_angle.x) + PI) / (2*PI),
        (asin(reflection_angle.y/1.0) + (PI/2.0)) / PI
    );

    float fresnel_f = (1.0-dot(vn, camera_vector));
    fresnel_f = pow(fresnel_f, 2);
    if(r_factor < 0.001) fresnel_f = 0.0;

    diffuse = mix(vec4(0.0), texture(TextureImage0, texcoords), max(dot(-vn, light_direction), 0.1));
    specular = vec4(1.0) * pow(max(dot(phong_angle, camera_vector), 0.0), specular_q) * max(dot(-vn, light_direction), 0.0);
    reflection = texture(TextureImage1, envtex_coords);
    color = mix(d_factor * diffuse + s_factor * specular + r_factor * reflection, reflection, fresnel_f);

    float f = clamp((distance(camera_pos, worldPos) - 200.0) / 1000.0, 0.0, 1.0);
    FragColor = mix(color, solid_color, f);
}