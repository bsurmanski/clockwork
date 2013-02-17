#version 130

uniform sampler2D depthTex;
uniform sampler2D colorTex;
uniform sampler2D normalTex;
uniform sampler2D lightTex;

smooth in vec2 f_uv; 

out vec4 outColor;

/*
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
        / length(texture(depthTex, ff_uvord));
    
    total = max(ambient, difuse);

    return vec4(total * colorVal.rgb + vec3(specular), 1);
}*/

void main()
{
    vec4 color = texture(colorTex, f_uv);
    vec4 normal = texture(normalTex, f_uv);

    vec4 tmp = normalize(vec4(1.0f, 1.0f, 1.0f, 1.0f)); //Temporary light
    vec4 light = tmp;//texture(lightTex, f_uv); 
    vec4 difuse = color * clamp(dot(light, normal), 0.0f, 1.0f);
    outColor = difuse;//vec4(clamp(color * light, 0.0f, 1.0f).rgb, 1.0f);
}
