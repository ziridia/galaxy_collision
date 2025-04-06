#version 430

uniform float dt;
uniform int numParticles;

layout(local_size_x = 256) in;

layout(std430, binding = 0) buffer PosBuffer {
    float positions[];
};

layout(std430, binding = 1) buffer VelBuffer {
    float velocities[];
};

layout(std430, binding = 2) buffer MassBuffer {
    float masses[];
};

vec3 calculateForce(float m1, float m2, vec3 dist) {

    // float G = 6.67e-11;
    float G = 0.01;
    float epsilon = 1.0;
    float distance = max(length(dist), epsilon);

    float forceMagnitude = G * m1 * m2 / (distance * distance);
    vec3 forceDirection = normalize(dist);

    return forceDirection * forceMagnitude;
}

vec3 calculateDistance(vec3 p1, vec3 p2) {
    return p2 - p1;
}

void main() {
    uint id = gl_GlobalInvocationID.x;

    vec3 p1 = vec3(positions[id*3], positions[id*3+1], positions[id*3+2]);
    vec3 v1 = vec3(velocities[id*3], velocities[id*3+1], velocities[id*3+2]);
    float m1 = masses[id];
    
    vec3 force = vec3(0);
    
    for (int i = 0; i < numParticles; i++) {
        if (i == id) {
            continue;
        }
        vec3 p2 = vec3(positions[i*3], positions[i*3+1], positions[i*3+2]);

        // this assumes galaxy radius < 25
        // gives reasonably good results when above is true
        if (length(p2 - p1) > 30) {
            continue;
        }
        
        float m2 = masses[i];
        force += calculateForce(m1, m2, p2 - p1) * 10000;
    }


    vec3 acceleration = force / m1;
    
    v1 += acceleration * dt;
    p1 += v1 * dt;
    
    
    // update state variables
    positions[id*3] = p1.x;
    positions[id*3+1] = p1.y;
    positions[id*3+2] = p1.z;

    velocities[id*3] = v1.x;
    velocities[id*3+1] = v1.y;
    velocities[id*3+2] = v1.z;

}