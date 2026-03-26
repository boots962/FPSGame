#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define MAP_HEIGHT 10
#define MAP_WIDTH 10

class Walls{
    public:
     int MAP[MAP_WIDTH][MAP_HEIGHT] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1, 0 ,1},
        {1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1},
        {1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1}
    };
    unsigned int wallVAO;
    void setupWalls(){
        float wallVertices[] = {
                // Back face
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
                0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // Bottom-right    
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right              
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left                
                // Front face
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right
                -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // Top-left
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left
                // Left face
                -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
                -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right
                -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right
                // Right face
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right
                0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
                0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left
                0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right     
                // Bottom face
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Top-right
                0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // Top-left
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-left
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-left
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-right
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Top-right
                // Top face
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right     
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left
                -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // Bottom-left        
            };

        unsigned int wallVBO;
        glGenVertexArrays(1, &wallVAO);
        glGenBuffers(1, &wallVBO);

        glBindVertexArray(wallVAO);
        glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    bool checkCollision(glm::vec3 targetPos){
        float playerSize = 0.2f;
        float blockSize = 2.0f;
        float blockHalf = blockSize / 2.0f;

        float pMinX = targetPos.x - playerSize;
        float pMaxX = targetPos.x + playerSize;
        float pMinZ = targetPos.z - playerSize;
        float pMaxZ = targetPos.z + playerSize;
        for(int i =0; i<MAP_WIDTH; i++){
            for(int j=0; j<MAP_HEIGHT; j++){
                if(MAP[i][j] == 1){
                    float blockCenterX = i * blockSize;
                    float blockCenterZ = -(j * blockSize);
                    float bMinX = blockCenterX - blockHalf;
                    float bMaxX = blockCenterX + blockHalf;
                    float bMinZ = blockCenterZ - blockHalf;
                    float bMaxZ = blockCenterZ + blockHalf;

                    if(pMaxX > bMinX && pMinX < bMaxX && pMaxZ > bMinZ && pMinZ < bMaxZ){
                        return true;
                    }
                }
            }
        }
        return false;
    }
};
