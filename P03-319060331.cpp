/*Practica 3: Modelado geométrico
* Alumno Reyes HErrera Vanessa Giselle
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z

using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
//Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks


void CrearPiramideTriangular() {
	unsigned int indices[] = {
		0, 1, 2,  
		0, 2, 3,  
		1, 3, 2,  
		0, 3, 1
	};
	GLfloat Piramidetrian_vertices[] = {
		-0.5f, 0.0f, -0.29f,
		 0.5f, 0.0f, -0.29f,
		 0.0f, 0.0f,  0.58f,
		 0.0f, 0.82f,  0.0f
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMesh(Piramidetrian_vertices, indices, 12, 12);
	meshList.push_back(piramide); // Índice 0
}

// Forma para la parte de las
void CrearPiramideInvertida() {
	unsigned int indices[] = {
		0, 1, 2,  3, 5, 4,  
		0, 3, 1,  1, 3, 5,
		1, 5, 2,  2, 5, 4, 
		2, 4, 0,  0, 4, 3
	};
	GLfloat PiramideInv_vertices[] = {
		0.5f, 0.0f, -0.29f,
		0.0f, 0.0f,  0.58f,
		1.0f, 0.0f,  0.58f,
		0.0f, 0.82f,  0.0f,
		1.0f, 0.82f,  0.0f,
		0.5f, 0.82f,  0.87f
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMesh(PiramideInv_vertices, indices, 18, 24);
	meshList.push_back(piramide); 
}

// para darle color a cada cara
void CrearTriangulo() {
	unsigned int indices[] = { 0, 1, 2 };
	GLfloat Triangulo_vertices[] = {
		-0.5f, -0.29f, 0.0f,
		 0.5f, -0.29f, 0.0f,
		 0.0f,  0.58f, 0.0f
	};
	Mesh* color = new Mesh();
	color->CreateMesh(Triangulo_vertices, indices, 9, 3);
	meshList.push_back(color); 
}

void CreateShaders() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}


void ColorTriangulo(glm::mat4 modelCara, glm::vec3 color, GLuint uniformModel, GLuint uniformColor, Mesh* colorMesh) {
	glUniform3f(uniformColor, color.x, color.y, color.z);

	glm::vec2 posPir[] = {
		glm::vec2(0.0f,  1.16f),
		glm::vec2(-0.5f,  0.29f), glm::vec2(0.5f,  0.29f),
		glm::vec2(-1.0f, -0.58f), glm::vec2(0.0f, -0.58f), glm::vec2(1.0f, -0.58f)
	};
	for (int i = 0; i < 6; i++) {
		glm::mat4 model = glm::translate(modelCara, glm::vec3(posPir[i].x, posPir[i].y, 0.005f));
		model = glm::scale(model, glm::vec3(0.85f, 0.85f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		colorMesh->RenderMesh();
	}

	glm::vec2 posInv[] = {
		glm::vec2(0.0f,  0.58f),
		glm::vec2(-0.5f, -0.29f), glm::vec2(0.5f, -0.29f)
	};
	for (int i = 0; i < 3; i++) {
		glm::mat4 model = glm::translate(modelCara, glm::vec3(posInv[i].x, posInv[i].y, 0.005f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.85f, 0.85f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		colorMesh->RenderMesh();
	}
}

int main() {
	mainWindow = Window(800, 800);
	mainWindow.Initialise();

	CrearPiramideTriangular();
	CrearPiramideInvertida(); 
	CrearTriangulo();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.3f, 0.3f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	glm::vec3 posPiramides[10] = {
		glm::vec3(-1.0f, -0.61f, -0.58f), glm::vec3(0.0f, -0.61f, -0.58f), glm::vec3(1.0f, -0.61f, -0.58f),
		glm::vec3(-0.5f, -0.61f,  0.29f), glm::vec3(0.5f, -0.61f,  0.29f),
		glm::vec3(0.0f, -0.61f,  1.16f),
		glm::vec3(-0.5f,  0.21f, -0.29f), glm::vec3(0.5f,  0.21f, -0.29f),
		glm::vec3(0.0f,  0.21f,  0.58f),
		glm::vec3(0.0f,  1.03f,  0.0f)
	};

	glm::vec3 posPinvertidos[4] = {
		glm::vec3(-1.0f, -0.61f, -0.58f), glm::vec3(0.0f, -0.61f, -0.58f), glm::vec3(-0.5f, -0.61f,  0.29f),
		glm::vec3(-0.5f,  0.21f, -0.29f)
	};

	while (!mainWindow.getShouldClose()) {
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		glm::mat4 modelBase = glm::mat4(1.0f);
		modelBase = glm::translate(modelBase, glm::vec3(0.0f, -0.5f, -5.0f));
		//descomentar la sig linea para rotar la piramide 
		//modelBase = glm::rotate(modelBase, (float)glfwGetTime() * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

		// color base de piramides
		glUniform3f(uniformColor, 0.1f, 0.1f, 0.1f);
		for (int i = 0; i < 10; i++) {
			glm::mat4 model = glm::translate(modelBase, posPiramides[i]);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[0]->RenderMesh();
		}
		for (int i = 0; i < 4; i++) {
			glm::mat4 model = glm::translate(modelBase, posPinvertidos[i]); 
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[1]->RenderMesh();
		}

		// definicion de los colores de cada cara
		float distCara = 0.612f;
		float tiltAngle = -19.471f;

		// cara rojo
		glm::mat4 caraDer = glm::rotate(modelBase, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		caraDer = glm::rotate(caraDer, glm::radians(tiltAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		caraDer = glm::translate(caraDer, glm::vec3(0.0f, 0.0f, distCara));
		ColorTriangulo(caraDer, glm::vec3(1.0f, 0.0f, 0.0f), uniformModel, uniformColor, meshList[2]);
		//cara verde
		glm::mat4 caraIzq = glm::rotate(modelBase, glm::radians(300.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		caraIzq = glm::rotate(caraIzq, glm::radians(tiltAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		caraIzq = glm::translate(caraIzq, glm::vec3(0.0f, 0.0f, distCara));
		ColorTriangulo(caraIzq, glm::vec3(0.0f, 1.0f, 0.0f), uniformModel, uniformColor, meshList[2]);
		// cara azul
		glm::mat4 caraTras = glm::rotate(modelBase, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		caraTras = glm::rotate(caraTras, glm::radians(tiltAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		caraTras = glm::translate(caraTras, glm::vec3(0.0f, 0.0f, distCara));
		ColorTriangulo(caraTras, glm::vec3(0.0f, 0.0f, 1.0f), uniformModel, uniformColor, meshList[2]);
		// cara amarilla
		glm::mat4 caraAbajo = glm::rotate(modelBase, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		caraAbajo = glm::translate(caraAbajo, glm::vec3(0.0f, 0.0f, distCara));
		ColorTriangulo(caraAbajo, glm::vec3(1.0f, 1.0f, 0.0f), uniformModel, uniformColor, meshList[2]);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}