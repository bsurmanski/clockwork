#version 130

uniform sampler2D colorTex;
uniform sampler2D normalTex;
uniform sampler2D depthTex;

smooth in vec2 ftexCoord;

out vec4 outColor;

// gbuffer samples
vec4 normal;
vec4 color;

void antiAlias()
{

}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);    
}

float rand2(vec2 co)
{
    return fract(cos(dot(co.yx, vec2(32.4324, 47.1324))) * 24396.9887);    
}

float lerp(float t, float a, float b)
{
    return (a+t*(b-a));    
}

float s_curve(float t)
{
    return (t*t*t*(10+t*(-15+(6*t))));    
}

float perlin2(vec2 co)
{
    
    vec2 flco = floor(co);
    vec2 frco = fract(co);
    vec2 v0 = normalize(vec2(rand(flco), rand2(flco)));
    vec2 offset = vec2(1.0, 0.0);
    vec2 v1 = normalize(vec2(rand(flco + offset), rand2(flco + offset)));
    offset = vec2(1.0, 1.0);
    vec2 v2 = normalize(vec2(rand(flco + offset), rand2(flco + offset)));
    offset = vec2(0.0, 1.0);
    vec2 v3 = normalize(vec2(rand(flco + offset), rand2(flco + offset)));

    float s = dot(v0, frco); 
    offset = vec2(-1.0, 0.0);
    float t = dot(v1, frco + offset);
    offset = vec2(-1.0, -1.0);
    float u = dot(v2, frco + offset);
    offset = vec2(0.0, -1.0);
    float v = dot(v3, frco + offset);

    float px0 = lerp(s_curve(frco.x), s, t);
    float px1 = lerp(s_curve(frco.x), v, u);

    return lerp(s_curve(frco.y), px0, px1);
    //return 1.0f;
}

float sobel(sampler2D tex)
{
    vec4 colorL = textureOffset(tex, ftexCoord, ivec2(-1,0));
    vec4 colorR = textureOffset(tex, ftexCoord, ivec2(1,0));
    vec4 colorU = textureOffset(tex, ftexCoord, ivec2(0,1));
    vec4 colorD = textureOffset(tex, ftexCoord, ivec2(0,-1));
    vec4 colorUL = textureOffset(tex, ftexCoord, ivec2(-1,1));
    vec4 colorUR = textureOffset(tex, ftexCoord, ivec2(1,1));
    vec4 colorDL = textureOffset(tex, ftexCoord, ivec2(-1,-1));
    vec4 colorDR = textureOffset(tex, ftexCoord, ivec2(1,-1));

    vec4 gx = colorR * 2 + colorUR + colorDR - colorDL - colorUR - colorL * 2;
    vec4 gy = colorD * 2 + colorDL + colorDR - colorUL - colorUR - colorU * 2;
    float g = length(abs(sqrt(gx * gx + gy * gy)));

    return g; 
}

vec4 applyLighting(vec4 colorVal)
{
    float SPECULARITY = 0.5;
    float AMBIENT = 0.25;

    float ambient;
    float difuse;
    float specular;
    float total;

    // SHOULD BE PASSED IN AS UNIFORM (mvp * lightpos)
    vec3 lightPos = normalize(vec3(100, 100, 0)); //TODO: LIGHT SHOULD BE VIEW SPACE
    vec3 h = normalize(lightPos + vec3(0,0,1));

    ambient = AMBIENT;

    difuse = clamp(dot(lightPos, normal.xyz), 0.0f, 1.0f);
    specular = clamp(SPECULARITY * pow(dot(normal.xyz, h), 32.0f), 0.0f, 1.0f) 
        / length(texture(depthTex, ftexCoord));
    
    total = max(ambient, difuse);

    return vec4(total * colorVal.rgb + vec3(specular), 1);
}

void main()
{
    color = texture(colorTex, ftexCoord);
    normal = texture(normalTex, ftexCoord);
    color = applyLighting(color);
    outColor = color - sobel(colorTex) / (texture(depthTex, ftexCoord).x * 50.0);
}
