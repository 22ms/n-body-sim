#version 330 core

in vec3 position;
in vec3 velocity;

out vec4 FragColor;

void main()
{
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float dist = dot(coord, coord);
    
    float edgeThreshold = 0.75;
    
    if (dist > edgeThreshold) {
        discard;
    }
    
    float smoothed = smoothstep(edgeThreshold, edgeThreshold-0.5, dist);

    FragColor = vec4(velocity, smoothed);
}