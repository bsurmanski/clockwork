#version 130

uniform sampler2D inDepth;
uniform sampler2D inColor; 
uniform sampler2D inNormal;
uniform sampler2D inLight;

uniform vec3 ambient_color; 

smooth in vec2 f_uv; 

out vec4 outColor;
out vec4 outNormal;
out vec4 outLight;

void main()
{
    vec4 color = texture(inColor, f_uv);
    vec4 normal = texture(inNormal, f_uv);
    vec4 light = texture(inLight, f_uv);

    vec4 new_light = vec4(ambient_color, 1.0f);
                    
    outColor = color;
    outNormal = normal;
    outLight = clamp(light + new_light, vec4(0), vec4(1));
}
