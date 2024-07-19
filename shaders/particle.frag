#version 330 core

in vec3 position;
in vec3 velocity;

out vec4 FragColor;

void main()
{
    // Smooth circle edges
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float dist = dot(coord, coord);
    
    float edgeThreshold = 0.75;
    
    if (dist > edgeThreshold) {
        discard;
    }
    float smoothed = smoothstep(edgeThreshold, edgeThreshold-0.5, dist);

    // Color based on speed
    float speed = length(velocity);
    float maxSpeed = 300.0;
    float normalizedSpeed = clamp(speed / maxSpeed, 0.0, 1.0);
    vec3 color = mix(vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0), normalizedSpeed);

    FragColor = vec4(color, smoothed);
}