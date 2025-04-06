/*
This exists pretty much just to keep track of all the galaxies, and have them in a more manageable
form. In more absolute terms, this will handle combining all the position, mass, and velocity arrays
so that our drawing method can stay mostly unchanged
*/
#pragma once

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
#include "galaxy.h"

class Universe {
public:

    GLfloat* positions;
    GLfloat* velocities;
    GLfloat* masses;
    unsigned int numStars;

    Universe();

    void addGalaxy(Galaxy galaxy);
};