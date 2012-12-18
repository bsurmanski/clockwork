#version 130
in vec2 position;

smooth out vec2 ftexCoord;

void main()
{
    vec2 screenpos = vec2(position * 2 - 1);//transform from [0, 1] to [-1, 1]
    gl_Position = vec4(screenpos, 1.0f, 1.0f);
    ftexCoord = vec2(position); 
}
