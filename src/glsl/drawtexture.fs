#version 130

uniform sampler2D tex0;

smooth in vec2 ftexCoord;

out vec4 fragColor;

void main()
{
    vec4 color = texture(tex0, ftexCoord); 

    if(color.a < 0.05)
    {
        //discard;    
    }

    fragColor = color;
}

