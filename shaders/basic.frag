#version 330 core

in vec3 position;
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

    float intensity = dot(position, position);
    vec3 color = mix(vec3(204.0/255.0, 36.0/255.0, 29.0/255.0), vec3(250.0/255.0, 189.0/255.0, 47.0/255.0), intensity);

    FragColor = vec4(1.0, 1.0, 1.0, smoothed);
}