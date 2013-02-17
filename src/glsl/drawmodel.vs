#version 130
#extension GL_ARB_uniform_buffer_object : enable
#extension GL_ARB_explicit_attrib_location : require

#define EPSILON 0.01

uniform bool bones_enable = false;

uniform mat4 t_matrix; // p_matrix * v_matrix * m_matrix
uniform mat4 bones[255]; 

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec2 boneids;       // TODO: uvec; requires VertexAttribIPointer
layout(location = 4) in vec2 boneweights;

smooth out vec3 f_normal;
smooth out vec2 f_uv;

void main()
{
    mat4 b_matrix = mat4(1);
    if(bones_enable && (boneweights[0] + boneweights[1]) > EPSILON)
    {
        b_matrix = ((bones[int(boneids[0])] * boneweights[0]) +
                   (bones[int(boneids[1])] * boneweights[1]));
    }
    gl_Position = t_matrix * b_matrix * vec4(position, 1.0f);
    f_normal = (t_matrix * vec4(normal, 0.0f)).xyz;
    f_normal.z = -f_normal.z;
    f_uv = uv;
}
