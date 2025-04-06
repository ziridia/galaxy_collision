#version 400

in vec4 vPosition;
in float vMass;

uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMat;
uniform vec3 eye;

out float mass;


void main() {

    gl_Position = projection * modelView * vPosition;
    // float dist = dot(gl_Position.xyz - eye, gl_Position.xyz - eye);
    if (vMass != 0) {
        gl_PointSize = vMass;
    } else {
        gl_PointSize = 2.0;
    }

    gl_PointSize = sqrt(vMass);

    mass = vMass;
    
}