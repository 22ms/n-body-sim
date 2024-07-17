#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aVel;

uniform mat4 view;
uniform mat4 projection;

out vec3 position;
out vec3 velocity;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0f);
    gl_PointSize = 2.0f;
    
    position = aPos;
    velocity = aVel;
}