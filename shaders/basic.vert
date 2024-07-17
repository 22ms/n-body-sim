#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aVel;

uniform mat4 view;
uniform mat4 projection;
uniform float pointSize;

out vec3 position;
out vec3 velocity;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = pointSize;
    
    position = aPos;
    velocity = aVel;
}