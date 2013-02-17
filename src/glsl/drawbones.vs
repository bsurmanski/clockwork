#version 130
#extension GL_ARB_uniform_buffer_object : enable

uniform bool bones_enable = false;
uniform mat4 t_matrix; // p_matrix * v_matrix * m_matrix
uniform mat4 bones[255]; 

in vec3 position;

void main()
{
    int boneid = int(gl_VertexID / 2);
    mat4 b_matrix = mat4(1);

    if(bones_enable)
    {
        b_matrix = bones[boneid];
    }

    gl_Position = t_matrix * b_matrix * vec4(position, 1.0f);
}
