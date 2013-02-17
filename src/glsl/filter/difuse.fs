#version 130

uniform sampler2D tex0;

smooth in vec2 f_uv;

out vec4 fragColor;

void main()
{
    vec4 color = vec4(0.0f);//-texture(tex0, f_uv) / 2.0f;
    vec4 dfx = dFdx(color);
    vec4 dfy = dFdy(color);
    color += textureOffset(tex0, f_uv, ivec2(-1,-1));
    color += textureOffset(tex0, f_uv, ivec2(-1, 0));
    color += textureOffset(tex0, f_uv, ivec2(-1, 1));
    color += textureOffset(tex0, f_uv, ivec2( 0,-1));
    color += textureOffset(tex0, f_uv, ivec2( 0, 1));
    color += textureOffset(tex0, f_uv, ivec2( 1,-1));
    color += textureOffset(tex0, f_uv, ivec2( 1, 0));
    color += textureOffset(tex0, f_uv, ivec2( 1, 1));


    color += textureOffset(tex0, f_uv, ivec2(-2, 0));
    color += textureOffset(tex0, f_uv, ivec2( 2, 0));
    color += textureOffset(tex0, f_uv, ivec2( 0,-2));
    color += textureOffset(tex0, f_uv, ivec2( 0, 2));
    //color += textureOffset(tex0, f_uv, ivec2( 1, 2));
    //color += textureOffset(tex0, f_uv, ivec2(-1, 2));
    color += textureOffset(tex0, f_uv, ivec2( 2, 1));
    color += textureOffset(tex0, f_uv, ivec2( 2,-1));
    color /= 14.0f;
    //color -= texture(tex0, f_uv) / 1.0f;
    //color -= (dfy + dfx) / 1.0f;
    //color /= 2.0f;
    //color *=0.9999f;
    //color = clamp(color, vec4(0.0f), vec4(1.0f));

    fragColor = vec4(color.rgb, 1.0f);
}

