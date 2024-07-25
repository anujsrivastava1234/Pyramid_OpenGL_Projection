#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Global dimension
GLuint WIDTH{ 800 };
GLuint HEIGHT{ 600 };
GLuint VAO, VBO, IBO, Shader, uniformModel, uniformProjection


;
GLfloat toRadian = 3.14159265f / 180.0f;
bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float increment = 0.05f;
float curAngle = 0.0f;
float scale = 0.00f;

// Vertex shader
const std::string vertexShader =
"#version 330 core                                                                   \n"
"layout (location = 0) in vec3 pos;                                                  \n"
"out vec4 vCol;                                                                      \n"
" uniform mat4 model;                                                                \n"
" uniform mat4 projection;                                                           \n"
"void main()                                                                         \n"
"{                                                                                   \n"
"    gl_Position = projection * model * vec4(0.4 * pos.x , 0.4 * pos.y, pos.z, 1.0f);\n"
"    vCol = vec4(clamp(pos, 0.0f, 1.0f),1.0f);                                       \n"
"}                                                                                   \n";

// Fragment shader
const std::string fragmentShader =
"#version 330 core                                \n"
"out vec4 color;                                  \n"
"in vec4 vCol;                                    \n"
"void main()                                      \n"
"{                                                \n"
"    color = vCol;                                \n"
"}                                                \n";

void CreateTriangle()
{
    std::vector<GLint> indices = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };
    std::vector<GLfloat> vertices = {
        -1.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), indices.data(), GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}

void AddShader(GLuint program, const std::string& shaderCode, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);

    const char* code = shaderCode.c_str();
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    GLint result;
    GLchar eLog[1024] = { 0 };
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(shader, 1024, nullptr, eLog);
        std::cout << "Error compiling " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << eLog << std::endl;
        return;
    }

    glAttachShader(program, shader);
}

void CompileShader()
{
    Shader = glCreateProgram();
    if (!Shader)
    {
        std::cout << "Failed to create shader program" << std::endl;
        return;
    }

    AddShader(Shader, vertexShader, GL_VERTEX_SHADER);
    AddShader(Shader, fragmentShader, GL_FRAGMENT_SHADER);

    GLint result;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(Shader);
    glGetProgramiv(Shader, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(Shader, 1024, nullptr, eLog);
        std::cout << "Error linking program: " << eLog << std::endl;
        return;
    }

    glValidateProgram(Shader);
    glGetProgramiv(Shader, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(Shader, 1024, nullptr, eLog);
        std::cout << "Error validating program: " << eLog << std::endl;
        return;
    }
    uniformModel = glGetUniformLocation(Shader, "model");
    uniformProjection = glGetUniformLocation(Shader, "projection");

}

int main()
{
    if (!glfwInit())
    {
        std::cout << "Error initializing GLFW" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8); 
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Error creating window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
    
    CreateTriangle();
    CompileShader();

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);




    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (direction)
        {
            triOffset += increment;
        }
        else {
            triOffset -= increment;
        }

        if (abs(triOffset) >= triMaxOffset)
        {
            direction = !direction;
        }

        glViewport(0, 0, bufferWidth, bufferHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glUseProgram(Shader);
        curAngle += 0.005f;
        if (curAngle >= 360)
        {
            curAngle -= 360;
        }
        scale += increment;

        if (abs(scale) >= 1.0f)
        {
            scale -= increment;
        }



        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.25f, 0.0f, -2.5f));
        model = glm::rotate(model, curAngle * toRadian, glm::vec3(1.0f, 1.0f, 1.0f));
       // model = glm::scale(model, glm::vec3(scale));
        glUniform1f(uniformModel, triOffset);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        glUseProgram(0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(Shader);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
