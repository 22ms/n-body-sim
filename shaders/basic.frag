#version 330 core

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
    
    FragColor = vec4(130.0/255.0, 165.0/255.0, 152.0/255.0, smoothed);
}