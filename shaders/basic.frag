#version 330 core

out vec4 FragColor;

void main()
{
    vec2 coord = gl_PointCoord * 2.0 - 1.0; // normalize to -1 to 1
    float dist = dot(coord, coord);
    
    // Adjust the threshold slightly to soften the edges
    float edgeThreshold = 0.75; // Experiment with this value
    
    if (dist > edgeThreshold) {
        discard;
    }
    
    // Smoothstep function to smooth the edges
    float smoothed = smoothstep(edgeThreshold, edgeThreshold-0.5, dist);
    
    // You can use smooth to blend between the circle color and background
    FragColor = vec4(1.0, 1.0, 1.0, smoothed);
}