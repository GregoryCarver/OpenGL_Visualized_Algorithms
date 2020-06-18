/******************************************************************************************************************************************************
*
* Created By:
* Course: COP3530
* Project 3: Comparison Between Sorting Algorithms(title work in progress)
* Professor: Kapoor
*
* Last Update: 11 June 2020 (added the up and down input for space and left shift respectively: makes the camera move up and down in world space.)
*
* Need to do: Increase performance(ideas: face culling(might be done), VBO's, reduce draw calls(VBO's might achieve this)), abstract the rendering,
*             abstract shapes and their storage, might adjust input movement.
*******************************************************************************************************************************************************/

#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shaders/Functions/loadShader.h"
#include "Camera.h"
#include "Input/Input.h"

//Function used to resize the window appropriately.
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
inline void Mouse(GLFWwindow* window, double xPos, double yPos);

//Global screen settings.
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

//Global camera variables.
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

//Global timing variables.
float deltaTime = 0.0f;         //Time difference of current frame and last frame.
float lastTime = 0.0f;         //Keeps track of the time of the last frame. Used to calculate deltaTime.

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "GLFW initialization failed." << std::endl;
        return -1;
    }

    /*       OpenGLtutorials.org tutorial       */
    //Triangle doesn't show if other 3 are uncommented
    glfwWindowHint(GLFW_SAMPLES, 4);                                    //4x antialiasing
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                      //OpenGL 3.3
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);      //We don't want the old OpenGL
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    /*       OpenGLtutorials.org tutorial end       */

    GLFWwindow* window;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 1024, "Window of the Gods!", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        std::cout << "GLFW window creation failed." << std::endl;
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Added code
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwSetCursorPosCallback(window, Mouse);

    //Tells GLFW to capture our mouse.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Check if GLEW is initialized
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW failed to initialize." << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //Compile and load shaders and store the program id
    GLuint programID = LoadShaders("Shaders/Vertex/CameraShader.vert", "Shaders/Fragment/SimpleFragmentShader.frag");

    //Prints the GLEW and openGL versions
    std::cout << "Using GLEW version :" << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Using openGL version: " << glGetString(GL_VERSION) << std::endl;

    /*
      *******CUBE*******
      An array of 3 vectors which represents 3 vertices; 6 to make a rectangle; Each segment represents a face of a cube, made of two triangles; 
      Looking at the face, the first vertex of each triangle is top left, triangle formed counter-clockwise; First vertex on top is (-0.5, 0.5, -0.5);
      First vertex on bottom is (-0.5, -0.5, 0.5);
    
        1         3
      4    
             
               
                  2
      5         6
    */
    static const GLfloat cubeVertexBuffer[] =
    {
         //Front
        -0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         //Right
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         //Back
         0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
         //Left
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         //Top
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         //Bottom
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
    };

    //World space position of our cube.
    glm::vec3 cubePosition[] =
    {   
        glm::vec3( 0.0f,  0.0f,  0.0f),
    };

    //INSTANCING TEST
    //Need to do: make sure the coordinates right.
    unsigned int cubeGridXCoord = 10;
    unsigned int cubeGridYCoord = 10;
    unsigned int cubeGridZCoord = 10;
    float displacement = 4.0f;
    unsigned int currentIndex = 0;
    glm::mat4* modelMatrices;

    modelMatrices = new glm::mat4[cubeGridXCoord * cubeGridYCoord * cubeGridZCoord];
    for (unsigned int i = 0; i < cubeGridXCoord; i++)
    {
        for (unsigned int j = 0; j < cubeGridYCoord; j++)
        {
            for (unsigned int k = 0; k < cubeGridZCoord; k++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3((float)i / displacement, (float)j / displacement, (float)k / displacement));
                model = glm::scale(model, glm::vec3(0.1f));
                modelMatrices[currentIndex++] = model;
            }
        }
    }

    //INSTANCING TEST END
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    //Identify vertex buffer
    GLuint vertexbuffer;
    //Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    //Give the vertices to OpenGL
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexBuffer), cubeVertexBuffer, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, cubeGridXCoord * cubeGridYCoord * cubeGridZCoord * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    //Added code end
    //First attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer
        (
            0,              //attribute 0. No reason 0, but must match layout in shader. 
            3,              //size
            GL_FLOAT,       //type
            GL_FALSE,       //normalized?
            0,              //stride
            (void*)0        //array buffer offset
        );


    // vertex attributes
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);

    double previousFPSTime = glfwGetTime();
    int frameCount = 0;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //Get the time variables and display fps
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime= currentTime;
        frameCount++;
        if (currentTime - previousFPSTime >= 1.0f)
        {
            std::cout << "FPS: " << frameCount << "\r";
            frameCount = 0;
            previousFPSTime = currentTime;
        }

        //Input
        ProcessInput(window, camera, deltaTime);


        /* Render here */
        glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //Added code
        glUseProgram(programID);


        //Pass the projection matrix to shader ( in this case could change every frame )
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, &projection[0][0]);

        //Camera/view transformation.
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, &view[0][0]);
        
        //for (unsigned int i = 0; i < 1000; i++)
        //{
        //    //Calculate model matrix and initialize.
        //    glm::mat4 model = glm::mat4(1.0f);
        //    model = glm::translate(model, cubePosition[0] + glm::vec3(glm::cos(i)* 0.2f + (glm::cos(i)), i * 0.1f, glm::sin(i) * 0.2f + (glm::sin(i))));
        //    //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
        //    model = glm::scale(model, glm::vec3(0.1f));
        //    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);

        //    //Draw the triangle
        //    glDrawArrays(GL_TRIANGLES, 0, 36); // Starting from vertex 0; 3 vertices = one triangle, 6 = one face, 36 = one cube;
        //}

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_FLOAT, 0, cubeGridXCoord * cubeGridYCoord * cubeGridZCoord);
        glBindVertexArray(0);

        //Added code end

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glDisableVertexAttribArray(0);
    glfwTerminate();
    return 0;
}

//Updates window when changed by OS or user. 
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    //Make sure the viewport matches the new window dimensions.
    glViewport(0, 0, width, height);
}

inline void Mouse(GLFWwindow* window, double xPos, double yPos)
{
    MouseCallback(window, xPos, yPos, camera);
}