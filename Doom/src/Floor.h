#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Floor{
public:
unsigned int floorVAO;
void floorBuffers(){
    float floorVertices[] = {
         100.0f, -0.5f,  100.0f,  50.0f, 0.0f,
        -100.0f, -0.5f,  100.0f,  0.0f, 0.0f,
        -100.0f, -0.5f, -100.0f,  0.0f, 50.0f,
         100.0f, -0.5f,  100.0f,  50.0f, 0.0f,
        -100.0f, -0.5f, -100.0f,  0.0f, 50.0f,
         100.0f, -0.5f, -100.0f,  50.0f, 50.0f
    };
    unsigned int  floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}
};