#include "universe.h"

Universe::Universe() {
    this->positions = nullptr;
    this->velocities = nullptr;
    this->masses = nullptr;
    this->numStars = 0;
}

void Universe::addGalaxy(Galaxy galaxy) {

    // no point doing a ton of extra work when its not needed
    if (this->numStars == 0) {
        this->positions = galaxy.positions;
        this->velocities = galaxy.velocities;
        this->masses = galaxy.masses;
        this->numStars = galaxy.numStars;
        return;
    }

    // allocate space for the new positions, velocities, and masses
    int newNumStars = this->numStars + galaxy.numStars;
    
    GLfloat* newPositions = new GLfloat[newNumStars * 3];
    GLfloat* newVelocities = new GLfloat[newNumStars * 3];
    GLfloat* newMasses = new GLfloat[newNumStars];

    // copy over all the old data    
    for (int i = 0; i < this->numStars*3; i++) {
        newPositions[i] = this->positions[i];
        newVelocities[i] = this->velocities[i];
    }

    for (int i = 0; i < this->numStars; i++) {
        newMasses[i] = this->masses[i];
    }

    // copy over the new data
    for (int i = 0; i < galaxy.numStars*3; i++) {
        newPositions[this->numStars*3+i] = galaxy.positions[i];
        newVelocities[this->numStars*3+i] = galaxy.velocities[i];
    }
    for (int i = 0; i < galaxy.numStars; i++) {
        newMasses[this->numStars+i] = galaxy.masses[i];
    }

    this->numStars = newNumStars;

    delete[] this->positions;
    this->positions = newPositions;

    delete[] this->velocities;
    this->velocities = newVelocities;

    delete[] this->masses;
    this->masses = newMasses;
}