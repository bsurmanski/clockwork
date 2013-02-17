#version 130

in vec2 position;

smooth out vec2 f_uv;

void main()
{
    gl_Position = vec4(position, 1.0f, 1.0f);
    f_uv = 0.5f * (position + vec2(1.0f));
}
