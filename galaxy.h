#include <GL/glew.h>
#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaders.h"
#include <stdio.h>

#pragma once

class Galaxy {
private:
    
    float radius;
    float planeThickness;
    float angularVelocity;

    void createGalaxy(glm::vec3 velocity, glm::vec3 planeNormal, glm::vec3 center);

public:

    unsigned int numStars;
    GLfloat* positions;
    GLfloat* velocities;
    GLfloat* masses;



    Galaxy();
    Galaxy(unsigned int numStars, float radius, float planeThickness, float angularVelocity, glm::vec3 velocity, glm::vec3 planeNormal, glm::vec3 center);

};