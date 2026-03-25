#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <stb/stb_image.h>
// Now we can safely include your headers
#include "Texture.h"
#include "Camera.h"
#include "Floor.h"
#include "Walls.h"
#include "Skybox.h"
#include "Shader.h" // <-- Add this here!
#include <vector>
#include <string>
// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera Globals
glm::vec3 cameraPos   = glm::vec3(2.0f, 0.0f, -2.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// Timing Globals
float deltaTime = 0.0f; 
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
float yaw   = -90.0f; 
float pitch =  0.0f;
bool firstMouse = true;
float verticalVelocity = 0.0f;
 Camera camera;
 Walls walls;


void process_input(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Doom Engine", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);  

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

    // Enable Depth Testing so the wall renders in front of the floor
    glEnable(GL_DEPTH_TEST);
    Shader pbrShader("src/pbr.vert", "src/pbr.frag");
    Shader skyboxShader("src/skybox.vert", "src/skybox.frag");

    walls.setupWalls();
    Floor floor;
    floor.floorBuffers();

    unsigned int wallTex  = loadTexture("wall.jpg");
    unsigned int floorTex = loadTexture("floor.jpg");

    // ✅ Use pbrShader.use() instead of glUseProgram(shaderProgram)
    pbrShader.use();
    pbrShader.setInt("wallTexture", 0);

    Skybox skybox;
    std::vector<std::string> faces = {
        "right.jpg", "left.jpg", "top.jpg",
        "bottom.jpg", "front.jpg", "back.jpg"
    };
    skybox.setupSkybox(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 1);

    
    glEnable(GL_DEPTH_TEST);
    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_input(window);
        
        // ... (inside the while loop) ...
           glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ✅ PBR pass
        pbrShader.use();
        pbrShader.setVec3("viewPos", cameraPos);
        pbrShader.setVec3("sunColor", glm::vec3(1.0f, 0.95f, 0.9f));

        float time = glfwGetTime();
        glm::vec3 sunDir = glm::normalize(glm::vec3(sin(time * 0.5f), 1.0f, cos(time * 0.5f)));
        pbrShader.setVec3("sunDir", sunDir);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        pbrShader.setMat4("projection", projection);
        pbrShader.setMat4("view", view);

        // Draw floor
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTex);
        glBindVertexArray(floor.floorVAO);
        pbrShader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw walls
        glBindTexture(GL_TEXTURE_2D, wallTex);
        glBindVertexArray(walls.wallVAO);
        float blockSize = 2.0f;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (walls.MAP[i][j] == 1) {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(i * blockSize, 0.0f, -(j * blockSize)));
                    model = glm::scale(model, glm::vec3(blockSize, blockSize, blockSize));
                    pbrShader.setMat4("model", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        // ✅ Skybox pass
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        glm::mat4 skyboxView = glm::mat4(glm::mat3(view)); // strip translation
        skyboxShader.setMat4("view", skyboxView);
        skyboxShader.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE1);
        glBindVertexArray(skybox.skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime; 
    glm::vec3 velocity(0.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cameraSpeed *= 2.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        velocity += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        velocity -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        velocity -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        velocity += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    

   float gravity = -25.0f; 
    float jumpForce = 4.0f; 

    verticalVelocity += gravity * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && cameraPos.y <= 0.0f) {
        verticalVelocity = jumpForce;
    }

    cameraPos.y += verticalVelocity * deltaTime;

    if (cameraPos.y <= 0.0f) {
        cameraPos.y = 0.0f;
        verticalVelocity = 0.0f; 
    }


    cameraPos.x += velocity.x;
    if(walls.checkCollision(cameraPos)){
        cameraPos.x -= velocity.x;
    }
    cameraPos.z += velocity.z;
    if(walls.checkCollision(cameraPos)){
        cameraPos.z -= velocity.z;
    }
    static bool f11PressedLastFrame = false;
    static bool isFullscreen = false;
    static int winPosX, winPosY, winWidth, winHeight; 

    bool f11Pressed = glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;
    
    if (f11Pressed && !f11PressedLastFrame) {
        isFullscreen = !isFullscreen;

        if (isFullscreen) {
            glfwGetWindowPos(window, &winPosX, &winPosY);
            glfwGetWindowSize(window, &winWidth, &winHeight);
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(window, nullptr, winPosX, winPosY, winWidth, winHeight, 0);
        }
    }
    
    f11PressedLastFrame = f11Pressed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}  

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}