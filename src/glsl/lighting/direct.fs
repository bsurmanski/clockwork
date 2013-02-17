#version 130

uniform sampler2D inNormal;
uniform sampler2D inLight;

smooth in vec2 f_uv; 
flat in vec4 light_direction;

out vec4 outLight;

void main()
{
    vec4 normal = texture(inNormal, f_uv);
    vec4 light = texture(inLight, f_uv);

    vec4 new_light = vec4(1.0f, 1.0f, 1.0f, 1.0f) * clamp(dot(normalize(light_direction), normal), 0.0f, 1.0f);
    outLight = light + new_light;
}
