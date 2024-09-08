#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<stb/stb_image.h>
#include<stdlib.h>
#include<stdio.h>
#include<fstream>
#include<string>
#include<vector>
#include<iostream>

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


bool loadShaders(GLuint& program) {
    char infoLog[512];

    GLint success;

}


int main(int argc,char** argv) {


    GLFWwindow* window = NULL;

    int frameBufferWidth,frameBufferHeight = 0;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    if(!glfwInit()) {
        std::cerr << "Could not init opengl\n";
        exit(EXIT_FAILURE);
    }


    window = glfwCreateWindow(500,500,"Opengl",NULL,NULL);
    if(!window) {
        std::cerr << "Terminating app\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwGetFramebufferSize(window,&frameBufferWidth,&frameBufferHeight);
    glfwMakeContextCurrent(window);
    glViewport(0,0,frameBufferWidth,frameBufferHeight);

    glewExperimental = GL_TRUE;

    if(glewInit() != GLEW_OK) {
        std::cerr << "Could not init Glew\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    GLuint core_program;
    loadShaders(core_program);

    while (!glfwWindowShouldClose(window)) {
        
        processInput(window);

        glClearColor(2.2f,0.2f,4.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}