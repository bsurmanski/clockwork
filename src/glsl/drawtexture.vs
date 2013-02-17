#version 130

uniform mat3 tmat;

in vec2 position;

smooth out vec2 ftexCoord;

void main()
{
    vec3 pos = tmat * vec3(position, 1.0f);
    gl_Position = vec4(pos, 1.0f);
    ftexCoord = 0.5f * position + vec2(0.5f);
}
