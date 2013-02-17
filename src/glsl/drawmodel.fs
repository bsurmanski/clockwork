#version 130

uniform sampler2D tex0;
//uniform uint eid;

smooth in vec4 f_pos;
smooth in vec3 f_normal;
smooth in vec2 f_uv;

//out uint frag_EntityID;
/*layout(location = 0)*/ out vec4 fragColor;
/*layout(location = 1)*/ out vec4 fragNormal; //TODO: requires "LAYOUT" keyword

void main()
{
    fragColor = texture(tex0, f_uv);
    fragNormal = vec4(normalize(f_normal), 1.0f);
  //frag_EntityID = eid;
}
