/************************************************
*
*          CSCI 4110 Laboratory One
*
*  This program is a skeleton that can be used
*  to evaluate different shader programs.  You
*  don't need to modify any of the code.  But, please
*  take a look at the code so you can refresh your
*  memory on OpenGL programming.
*
************************************************/

#ifdef WIN32
#include <Windows.h>
#endif

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
#include "universe.h"

GLuint program;			// shader programs
GLuint computeProgram;
GLuint VAO;
int window;

GLfloat dt;

Universe universe;
// Galaxy galaxy = Galaxy(1000, 50, 100, 15, glm::vec3(0,0,0), glm::vec3(0,1,0), glm::vec3(0));

double theta, phi;
double r;

float cx, cy, cz;

glm::mat4 projection;	// projection matrix
float eyex, eyey, eyez;	// eye position

/*
*  The init procedure creates the OpenGL data structures
*  that contain the triangle geometry, compiles our
*  shader program and links the shader programs to
*  the data.
*/

void init() {
    GLuint vbuffer;
    GLuint ibuffer;
    GLint vPosition;
    GLint vMass;
    GLint particleVelocity;
    int vs;
    int fs;
    int cs;
    char vname[256];
    char fname[256];
    char cname[256];

    GLuint particleBuffer;
    GLuint particleVelocitiesBuffer;
    GLuint particleMassBuffer;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // numParticles = galaxy.numStars;
    
    /*
    Load the particle coordinate data
    */
    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
    glBufferData(GL_ARRAY_BUFFER, universe.numStars*3*sizeof(GLfloat), universe.positions, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer); // needed to make accessible in compute, binding point 0 
    

    // load the particle velocity data
    glGenBuffers(1, &particleVelocitiesBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, particleVelocitiesBuffer);
    glBufferData(GL_ARRAY_BUFFER, universe.numStars*3*sizeof(GLfloat), universe.velocities, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particleVelocitiesBuffer); // binding point 1

    // load the particle mass data
    glGenBuffers(1, &particleMassBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, particleMassBuffer);
    glBufferData(GL_ARRAY_BUFFER, universe.numStars*sizeof(GLfloat), universe.masses, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, particleMassBuffer);
    /*
    *  compile and build the shader program
    */
    sprintf(vname,"%s", "vertex.vs");
    sprintf(fname,"%s", "fragment.fs");
    sprintf(cname,"%s", "compute.glsl");
    vs = buildShader(GL_VERTEX_SHADER, vname);
    fs = buildShader(GL_FRAGMENT_SHADER, fname);
    cs = buildShader(GL_COMPUTE_SHADER, cname);
    program = buildProgram(vs,fs,0);
    computeProgram = buildProgram(cs, 0);

    /*
    *  link the particle(vertex) coordinates to the vPosition
    *  variable in the vertex program.
    */
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, particleBuffer); // relink with positions for setting vertex attribs
    vPosition = glGetAttribLocation(program,"vPosition");
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    // do the same with the masses too
    glBindBuffer(GL_ARRAY_BUFFER, particleMassBuffer);
    vMass = glGetAttribLocation(program, "vMass");
    glVertexAttribPointer(vMass, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vMass);
}

/*
*  Executed each time the window is resized,
*  usually once at the start of the program.
*/
void framebufferSizeCallback(GLFWwindow *window, int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).

    if (h == 0)
        h = 1;

    float ratio = 1.0f * w / h;

    glfwMakeContextCurrent(window);

    glViewport(0, 0, w, h);

    projection = glm::perspective(0.7f, ratio, 1.0f, 800.0f);

}

/*
*  This procedure is called each time the screen needs
*  to be redisplayed
*/
void display() {
    glm::mat4 view;
    int modelViewLoc;
    int projectionLoc;
    int normalLoc;
    int eyeLoc;

    view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
                    glm::vec3(cx,cy,cz),
                    glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view)));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    modelViewLoc = glGetUniformLocation(program,"modelView");
    glUniformMatrix4fv(modelViewLoc, 1, 0, glm::value_ptr(view));
    projectionLoc = glGetUniformLocation(program,"projection");
    glUniformMatrix4fv(projectionLoc, 1, 0, glm::value_ptr(projection));
    normalLoc = glGetUniformLocation(program,"normalMat");
    glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));

    eyeLoc = glGetUniformLocation(program, "eye");
    glUniform3f(eyeLoc, eyex, eyey, eyez);

    glBindVertexArray(VAO);
    // glDrawElements(GL_POINTS, numParticles, GL_UNSIGNED_INT, NULL);
    // glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, universe.numStars);

}

void compute() {
    int dtLoc;
    int numParticlesLoc;

    glUseProgram(computeProgram);

    // update delta time in the shaders
    dtLoc = glGetUniformLocation(computeProgram, "dt");
    glUniform1f(dtLoc, dt);
    
    // update the number of particles uniform
    numParticlesLoc = glGetUniformLocation(computeProgram, "numParticles");
    glUniform1i(numParticlesLoc, universe.numStars);

    // run the compute shader
    glDispatchCompute(universe.numStars,1,1);

    // ensure writes are visible
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

/*
*  Called each time a key is pressed on
*  the keyboard.
*/
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        phi -= 0.1;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        phi += 0.1;
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        theta += 0.1;
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        theta -= 0.1;
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
        r -= 10;
    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
        r += 10;

    eyex = (float)(r*sin(theta)*cos(phi));
    eyey = (float)(r*sin(theta)*sin(phi));
    eyez = (float)(r*cos(theta));

}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char **argv) {
    GLFWwindow *window;

    // start by setting error callback in case something goes wrong

    glfwSetErrorCallback(error_callback);

    // initialize glfw

    if (!glfwInit()) {
        fprintf(stderr, "can't initialize GLFW\n");
    }

    // create the window used by our application

    window = glfwCreateWindow(512, 512, "Galaxy Collisions Simulation", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // establish framebuffer size change and input callbacks

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwSetKeyCallback(window, key_callback);
    /*
    *  initialize glew
    */
    glfwMakeContextCurrent(window);
    GLenum error = glewInit();
    if(error != GLEW_OK) {
        printf("Error starting GLEW: %s\n",glewGetErrorString(error));
        exit(0);
    }

    

    theta = 0.0;
    phi = 1.5;
    r = 500.0;

    eyex = (float)(r*sin(theta)*cos(phi));
    eyey = (float)(r*sin(theta)*sin(phi));
    eyez = (float)(r*cos(theta));

    dt = 0.001;

    universe.addGalaxy(
        Galaxy(
            1200, 
            50, 
            1, 
            -20, 
            glm::vec3(-25,0,0), 
            glm::vec3(0,1,0), 
            glm::vec3(50,0,-15))
    );
    universe.addGalaxy(
        Galaxy(
            1200, 
            50, 
            1, 
            30, 
            glm::vec3(25,0,0), 
            glm::vec3(0,1,0), 
            glm::vec3(-50,0,15))
    );



    init();

    glEnable(GL_DEPTH_TEST);
    // allow controlling the point size in the vertex shader
    glEnable(GL_PROGRAM_POINT_SIZE);
    // make the points circles instead of squares
    glEnable(GL_POINT_SMOOTH);
    glClearColor(0.0,0.0,0.0,1.0);
    glViewport(0, 0, 512, 512);

    projection = glm::perspective(0.7f, 1.0f, 1000.0f, 999999.0f);

    glfwSwapInterval(1);

    // GLFW main loop, display model, swapbuffer and check for input

    while (!glfwWindowShouldClose(window)) {
        compute();
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

}
