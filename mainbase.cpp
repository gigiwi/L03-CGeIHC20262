#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <cstdlib> 
#include <ctime>

const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

// Shaders (GLSL)
static const char* vShader = "                                \n\
#version 330                                    \n\
layout (location = 0) in vec3 pos;              \n\
void main()                                     \n\
{                                               \n\
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f); \n\
}";

static const char* fShader = "                                \n\
#version 330                                    \n\
out vec4 color;                                 \n\
void main()                                     \n\
{                                               \n\
    color = vec4(1.0f, 1.0f, 1.0f, 1.0f);        \n\
}"; // Letras Blancas

void CrearTriangulosVANE()
{
    // Definimos las iniciales V-A-N-E usando triángulos en diagonal ascendente
    GLfloat vertices[] = {
        // --- LETRA V (Abajo-Izquierda) ---
        -0.90f, -0.60f, 0.0f,  -0.85f, -0.60f, 0.0f,  -0.80f, -0.90f, 0.0f, // Ala izq
        -0.70f, -0.60f, 0.0f,  -0.75f, -0.60f, 0.0f,  -0.80f, -0.90f, 0.0f, // Ala der

        // --- LETRA A (Centro-Abajo) ---
        -0.40f, -0.40f, 0.0f,  -0.35f, -0.40f, 0.0f,  -0.30f, -0.10f, 0.0f, // Pierna izq
        -0.20f, -0.40f, 0.0f,  -0.25f, -0.40f, 0.0f,  -0.30f, -0.10f, 0.0f, // Pierna der
        -0.37f, -0.28f, 0.0f,  -0.23f, -0.28f, 0.0f,  -0.30f, -0.22f, 0.0f, // Travesaño

        // --- LETRA N (Centro-Arriba) ---
        0.05f, 0.10f, 0.0f,   0.10f, 0.10f, 0.0f,   0.05f, 0.40f, 0.0f,   // Barra izq
        0.05f, 0.40f, 0.0f,   0.10f, 0.40f, 0.0f,   0.20f, 0.10f, 0.0f,   // Diagonal
        0.20f, 0.10f, 0.0f,   0.25f, 0.10f, 0.0f,   0.25f, 0.40f, 0.0f,   // Barra der

        // --- LETRA E (Arriba-Derecha) ---
        0.60f, 0.60f, 0.0f,   0.65f, 0.60f, 0.0f,   0.60f, 0.90f, 0.0f,   // Poste vertical
        0.65f, 0.90f, 0.0f,   0.85f, 0.90f, 0.0f,   0.65f, 0.85f, 0.0f,   // Brazo Sup
        0.65f, 0.77f, 0.0f,   0.80f, 0.77f, 0.0f,   0.65f, 0.73f, 0.0f,   // Brazo Mid
        0.65f, 0.65f, 0.0f,   0.85f, 0.65f, 0.0f,   0.65f, 0.60f, 0.0f    // Brazo Inf
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Las funciones AddShader y CompileShaders se mantienen igual que tu código original...
void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);
    const GLchar* theCode[1];
    theCode[0] = shaderCode;
    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);
    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);
    glAttachShader(theProgram, theShader);
}

void CompileShaders() {
    shader = glCreateProgram();
    if (!shader) { printf("Error creando shader"); return; }
    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);
    glLinkProgram(shader);
    glValidateProgram(shader);
}

int main() {
    if (!glfwInit()) { printf("Falló GLFW"); return 1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "VANE - Diagonal y Fondo Random", NULL, NULL);
    if (!mainWindow) { printf("Fallo Ventana"); glfwTerminate(); return 1; }
    int BufferWidth, BufferHeight;
    glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);
    glfwMakeContextCurrent(mainWindow);
    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, BufferWidth, BufferHeight);

    // 1. Semilla aleatoria
    srand(static_cast<unsigned int>(time(NULL)));

    CrearTriangulosVANE();
    CompileShaders();

    float r = 0.0f, g = 0.0f, b = 0.0f;
    double tiempoAnterior = glfwGetTime();

    while (!glfwWindowShouldClose(mainWindow)) {
        glfwPollEvents();

        // --- REQUISITO 1: Cambio de color cada 2 segundos ---
        double tiempoActual = glfwGetTime();
        if (tiempoActual - tiempoAnterior >= 2.0) {
            r = (float)rand() / RAND_MAX;
            g = (float)rand() / RAND_MAX;
            b = (float)rand() / RAND_MAX;
            tiempoAnterior = tiempoActual;
        }

        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(VAO);

        // Dibujamos todos los triángulos (36 vértices en total para VANE)
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }
    return 0;
}