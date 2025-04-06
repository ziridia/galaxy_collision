#version 330 core

in float mass;

out vec4 FragColor;

void main() {
    vec2 coord = gl_FragCoord.xy * 2.0 - 1.0;
    float dist = dot(coord, coord);
    
    if (mass < 0.5) {
        FragColor = vec4(255.0/255.0,131.0/255.0,111.0/255.0, 1.0); // red star

    } else if (mass < 2) {
        FragColor = vec4(253.0/255.0,255.0/255.0,162.0/255.0, 1.0); // yellow star

    } else if (mass < 10) {
        FragColor = vec4(144.0/255.0,166.0/255.0,255.0/255.0, 1.0); // light blue star

    } else {
        FragColor = vec4(31.0/255.0,76.0/255.0,255.0/255.0,1.0); // blue star
    }

}