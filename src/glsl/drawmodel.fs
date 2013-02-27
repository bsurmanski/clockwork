#version 130

uniform sampler2D inColor; 
//uniform uint eid;

smooth in vec4 f_pos;
smooth in vec3 f_normal;
smooth in vec2 f_uv;

//out uint frag_EntityID;
/*layout(location = 0)*/ out vec4 outColor;
/*layout(location = 1)*/ out vec4 outNormal; //TODO: requires "LAYOUT" keyword

void main()
{
    outColor = texture(inColor, f_uv);
    outNormal = vec4(normalize(f_normal), 1.0f);
  //frag_EntityID = eid;
}
