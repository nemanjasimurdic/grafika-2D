// Autor: Nemanja Simurdic
// Opis: Dolina

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>


#include <GL/glew.h>   
#include <GLFW/glfw3.h>

const int numSegments = 100;
const float PI = 3.14159265359f;

void createSunVertices(float* vertices, float radius, int numSegments);
void createCloudVertices(float* vertices, float radius, int numSegments, float offsetX, float offsetY);

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

int main(void)
{


    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 800;
    unsigned int wHeight = 600;
    const char wTitle[] = "Dolina";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);

    float sunVertices[2 * (numSegments + 2)];
    createSunVertices(sunVertices, 0.2f, numSegments);

    unsigned int sunVAO, sunVBO;
    glGenVertexArrays(1, &sunVAO);
    glGenBuffers(1, &sunVBO);

    glBindVertexArray(sunVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sunVertices), sunVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    glUseProgram(basicShader);

    int aspectRatioLoc = glGetUniformLocation(basicShader, "aspectRatio");
    int sunColorLoc = glGetUniformLocation(basicShader, "sunColor");
    int cloudColorLoc = glGetUniformLocation(basicShader, "cloudColor");
    int cloudOpacityLoc = glGetUniformLocation(basicShader, "cloudOpacity");

    float cloudVertices[3][3][2 * (numSegments + 2)];
    float cloudPositions[3][2] = {
        {-1.0f, 0.6f},
        {-0.5f, 0.7f},
        {-0.2f, 0.8f}
    };
    for (int i = 0; i < 3; ++i)
    {
        createCloudVertices(cloudVertices[i][0], 0.15f, numSegments, cloudPositions[i][0], cloudPositions[i][1]);
        createCloudVertices(cloudVertices[i][1], 0.1f, numSegments, cloudPositions[i][0] + 0.1f, cloudPositions[i][1] + 0.05f);
        createCloudVertices(cloudVertices[i][2], 0.12f, numSegments, cloudPositions[i][0] - 0.1f, cloudPositions[i][1] + 0.02f);
    }

    unsigned int cloudVAO[3][3], cloudVBO[3][3];
    glGenVertexArrays(9, &cloudVAO[0][0]);
    glGenBuffers(9, &cloudVBO[0][0]);

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            glBindVertexArray(cloudVAO[i][j]);
            glBindBuffer(GL_ARRAY_BUFFER, cloudVBO[i][j]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cloudVertices[i][j]), cloudVertices[i][j], GL_DYNAMIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }

    float cloudSpeed = 0.0005f;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        float speedFactor = 2.0f;
        float time = glfwGetTime() * speedFactor;
        float radius = 0.1f + 0.005f * sin(time); 
        createSunVertices(sunVertices, radius, numSegments);

        float red = 1.0f;
        float green = 1.0f - 0.5f * (1.0f + sin(time));
        float blue = 0.0f;

        glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sunVertices), sunVertices);

        for (int i = 0; i < 3; ++i)
        {
            cloudPositions[i][0] += cloudSpeed;
            if (cloudPositions[i][0] > 1.2f)
            {
                cloudPositions[i][0] = -1.2f; 
            }
            createCloudVertices(cloudVertices[i][0], 0.15f, numSegments, cloudPositions[i][0], cloudPositions[i][1]);
            createCloudVertices(cloudVertices[i][1], 0.1f, numSegments, cloudPositions[i][0] + 0.1f, cloudPositions[i][1] + 0.05f);
            createCloudVertices(cloudVertices[i][2], 0.12f, numSegments, cloudPositions[i][0] - 0.1f, cloudPositions[i][1] + 0.02f);
            for (int j = 0; j < 3; ++j)
            {
                glBindBuffer(GL_ARRAY_BUFFER, cloudVBO[i][j]);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cloudVertices[i][j]), cloudVertices[i][j]);
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(basicShader);
        glUniform1f(aspectRatioLoc, (float)wWidth / wHeight);
        glUniform3f(sunColorLoc, red, green, blue);
        glUniform1i(glGetUniformLocation(basicShader, "isCloud"), false);

        glBindVertexArray(sunVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);

        glUniform3f(cloudColorLoc, 1.0f, 1.0f, 1.0f);
        glUniform1f(cloudOpacityLoc, 0.7f);
        glUniform1i(glGetUniformLocation(basicShader, "isCloud"), true);

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                glBindVertexArray(cloudVAO[i][j]);
                glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);
            }
        }

        glfwSwapBuffers(window);

    }

    glDeleteBuffers(1, &sunVBO);
    glDeleteVertexArrays(1, &sunVAO);
    glDeleteBuffers(9, &cloudVBO[0][0]);
    glDeleteVertexArrays(9, &cloudVAO[0][0]);

    glDeleteProgram(basicShader);

    glfwTerminate();
    return 0;
}

void createSunVertices(float* vertices, float radius, int numSegments) {
    vertices[0] = -0.8f;
    vertices[1] = 0.4f;

    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * PI * i / numSegments;
        vertices[2 * (i + 1)] = -0.8f + radius * cos(angle); 
        vertices[2 * (i + 1) + 1] = 0.4f + radius * sin(angle); 
    }
}

void createCloudVertices(float* vertices, float radius, int numSegments, float offsetX, float offsetY) {
    vertices[0] = offsetX;
    vertices[1] = offsetY;

    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * PI * i / numSegments;
        vertices[2 * (i + 1)] = offsetX + radius * cos(angle); 
        vertices[2 * (i + 1) + 1] = offsetY + radius * sin(angle); 
    }
}


unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader); 

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); 
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); 
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program; 
    unsigned int vertexShader; 
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); 
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); 
    glValidateProgram(program); 

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); 
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}