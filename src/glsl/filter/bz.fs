#version 130

uniform sampler2D tex0;

smooth in vec2 f_uv;

out vec4 fragColor;

int bin(vec4 color)
{
    
}

int neighbors()
{
    vec4 tcolor;
    int count = 0;
    if(isactive(textureOffset(tex0, f_uv, ivec2(-1,-1)))) count++;
    if(isactive(textureOffset(tex0, f_uv, ivec2(-1, 0)))) count++;
    if(isactive(textureOffset(tex0, f_uv, ivec2(-1, 1)))) count++;
    if(isactive(textureOffset(tex0, f_uv, ivec2( 0,-1)))) count++;
    if(isactive(textureOffset(tex0, f_uv, ivec2( 0, 1)))) count++;
    if(isactive(textureOffset(tex0, f_uv, ivec2( 1,-1)))) count++;
    if(isactive(textureOffset(tex0, f_uv, ivec2( 1, 0)))) count++;
    if(isactive(textureOffset(tex0, f_uv, ivec2( 1, 1)))) count++;
    return count;    
}

void main()
{
    vec4 color = texture(tex0, f_uv);
    int count = neighbors();

    if (count < 2)
    {
        color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    } else if (count == 3)
    {
        color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    } else if (count > 3){
        color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    fragColor = color;
}

