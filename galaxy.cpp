#include "galaxy.h"
#include <cstdlib>
#include <glm/geometric.hpp>

Galaxy::Galaxy() {
    this->radius = 0;
    this->planeThickness = 0;
    this->angularVelocity = 0;
    this->numStars = 0;
    this->positions = nullptr;
    this->velocities = nullptr;
    this->masses = nullptr;
}

Galaxy::Galaxy(unsigned int numStars, float radius, float planeThickness, float angularVelocity, glm::vec3 velocity, glm::vec3 planeNormal, glm::vec3 center) {
    this->radius = radius;
    this->planeThickness = planeThickness;
    this->angularVelocity = angularVelocity;
    this->numStars = numStars;

    createGalaxy(velocity, planeNormal, center);
}

glm::vec3 samplePoint(glm::vec3 planeNormal, float radius, glm::vec3 center, float planeThickness) {
    
    glm::vec3 U = glm::normalize(glm::cross(planeNormal, glm::vec3(1,0,0)));

    if (glm::length(U) < 0.001) {
        U = glm::normalize(glm::cross(planeNormal, glm::vec3(0,1,0)));
    }
    glm::vec3 V = glm::normalize(glm::cross(planeNormal, U));

    float theta = 2 * 3.141592 * (float)rand() / (float)RAND_MAX;
    float r = radius * ((float)rand() / (float)RAND_MAX);

    // we want this to be biased for larger movements closer the center, so that the entire distribution is circular
    // that way we can get elliptical galaxies
    // but for now, this is fine
    float normalOffset = ((float)rand() / (float)RAND_MAX * 2 - 1) * planeThickness;

    return center + r * cosf(theta) * U + r * sinf(theta) * V + planeNormal * normalOffset;
}

// generate star mass based on observed distribution of star masses
// older galaxies should be biased towards smaller stars
// all masses are in solar masses
GLfloat sampleMass(float galaxyAge) {
    /*
    Mass distribution is based on the distribution ratio of
    200 stars (0.08 < m < 0.5) = 76%
    50 stars (0.5 < m < 2) = 19%
    10 stars (2 < m < 10) = 4%
    1 star (10 < m < 150) = 1%

    Given the low number of stars, this distribution
    should be normalized slightly
    */
    float starType = (float)rand() / (float)RAND_MAX;
    float weightDistribution = (float)rand() / (float)RAND_MAX;

    // return 1;

    if (starType < 0.76) {
        return 0.08 + weightDistribution * (0.5-0.08);
    }
    if (starType < 0.95) {
        return 0.5 + weightDistribution * (2-0.5);
    }
    if (starType < 0.995) {
        return 2 + weightDistribution * (10-2);
    }

    // these stars are too massive given the small size
    // of the galaxy;
    // return 10 + weightDistribution * (150-10);

    return 10 + weightDistribution * (15-10);

}

/*
Create a point cloud/disk
*/
void Galaxy::createGalaxy(glm::vec3 velocity, glm::vec3 planeNormal, glm::vec3 center) {

    planeNormal = glm::normalize(planeNormal);

    positions = new GLfloat[numStars*3];
    velocities = new GLfloat[numStars*3];
    masses = new GLfloat[numStars];
    
    for (int i = 0; i < this->numStars; i++) {
        // get the position of the point on the plane
        glm::vec3 position = samplePoint(planeNormal, radius, center, planeThickness);

        // get the initial velocity
        glm::vec3 r = glm::normalize(position - center);
        glm::vec3 tangent = glm::normalize(glm::cross(planeNormal, r));
        glm::vec3 pointVelocity = angularVelocity * tangent + velocity;

        // get the mass
        GLfloat mass = sampleMass(0);

        // update the state variable arrays
        positions[i*3] = position.x;
        positions[i*3+1] = position.y;
        positions[i*3+2] = position.z;

        velocities[i*3] = pointVelocity.x;
        velocities[i*3+1] = pointVelocity.y;
        velocities[i*3+2] = pointVelocity.z;

        masses[i] = mass;
    }
}