/*
* Práctica 8: Iluminación II
* Reyes Herrera Vanessa Giselle
* Ultima modificación: 26/Abril/2026
* 
* -------------------------------------
* Mismas U y Y para movimiento coche
* L para encender/apagar la luz de la lámpara
* F para ciclar el color del faro del coche 
* J y K para mover la Nave y encender dinámicamente sus luces
* C para mover el Pez Abisal por la pecera
* B para encender/apagar luz azul del pez 
* 1-2 (izq/der), 3-4 (abajo/arriba) para mover el reflector amarillo del pez 
* -------------------------------------
*/
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoOchoTexture;
Texture aguaTexture;

Model Coche_M;
Model Llantadelanteizq_M;
Model Cofre_M;
Model Lampara_M;
Model Nave_M;
Model PezCuerpo_M;
Model PezBulba_M;
Model PezAntena_M;

Skybox skybox;

Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";


void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = { 0, 3, 1, 1, 3, 2, 2, 3, 0, 0, 1, 2 };
	GLfloat vertices[] = {
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = { 0, 2, 1, 1, 2, 3 };
	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7 };
	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	Mesh* obj1 = new Mesh(); obj1->CreateMesh(vertices, indices, 32, 12); meshList.push_back(obj1);
	Mesh* obj2 = new Mesh(); obj2->CreateMesh(vertices, indices, 32, 12); meshList.push_back(obj2);
	Mesh* obj3 = new Mesh(); obj3->CreateMesh(floorVertices, floorIndices, 32, 6); meshList.push_back(obj3);
	Mesh* obj4 = new Mesh(); obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12); meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void CrearPecera()
{
	unsigned int cubo_indices[] = {
		8, 9, 10, 10, 11, 8, // Atrás
		16, 17, 18, 18, 19, 16, // Abajo
		20, 21, 22, 22, 23, 20, // Arriba
		12, 13, 14, 14, 15, 12, // Izquierda
		4, 5, 6, 6, 7, 4, // Derecha
		0, 1, 2, 2, 3, 0 // Frente
	};

	GLfloat cubo_vertices[] = {
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,	0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f,  0.0f,	0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,  1.0f,	0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f,  1.0f,	0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f,  0.5f,	0.0f,  0.0f,	-1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,	-1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f,  1.0f,	-1.0f, 0.0f, 0.0f,
		  0.5f, -0.5f, -0.5f,	0.0f,  0.0f,	0.0f, 0.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f,	1.0f,  0.0f,	0.0f, 0.0f, 1.0f,
		 -0.5f,  0.5f, -0.5f,	1.0f,  1.0f,	0.0f, 0.0f, 1.0f,
		  0.5f,  0.5f, -0.5f,	0.0f,  1.0f,	0.0f, 0.0f, 1.0f,
		  -0.5f, -0.5f, -0.5f,	0.0f,  0.0f,	1.0f, 0.0f, 0.0f,
		  -0.5f, -0.5f,  0.5f,	1.0f,  0.0f,	1.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f,  0.5f,	1.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		  -0.5f,  0.5f, -0.5f,	0.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		  -0.5f, -0.5f, -0.5f,	0.0f,  0.0f,	0.0f, 1.0f, 0.0f,
		   0.5f, -0.5f, -0.5f,	1.0f,  0.0f,	0.0f, 1.0f, 0.0f,
		   0.5f, -0.5f,  0.5f,	1.0f,  1.0f,	0.0f, 1.0f, 0.0f,
		  -0.5f, -0.5f,  0.5f,	0.0f,  1.0f,	0.0f, 1.0f, 0.0f,
		  -0.5f,  0.5f,  0.5f,	0.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		   0.5f,  0.5f,  0.5f,	1.0f,  0.0f,	0.0f, -1.0f, 0.0f,
		   0.5f,  0.5f, -0.5f,	1.0f,  1.0f,	0.0f, -1.0f, 0.0f,
		  -0.5f,  0.5f, -0.5f,	0.0f,  1.0f,	0.0f, -1.0f, 0.0f,
	};

	Mesh* peceraMesh = new Mesh();
	peceraMesh->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(peceraMesh);
}

void CrearDadoOcho()
{
	unsigned int octaedro_indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
	GLfloat octaedro_vertices[] = {
		0.0f,  1.0f,  0.0f,		0.75f, 0.99f,   -0.577f, -0.577f, -0.577f, 1.0f,  0.0f,  0.0f,		0.99f, 0.75f,   -0.577f, -0.577f, -0.577f, 0.0f,  0.0f,  1.0f,      0.50f, 0.75f,   -0.577f, -0.577f, -0.577f,
		0.0f,  1.0f,  0.0f,     0.50f, 0.75f,    0.577f, -0.577f, -0.577f, 0.0f,  0.0f,  1.0f,      0.75f, 0.50f,    0.577f, -0.577f, -0.577f, -1.0f,  0.0f,  0.0f,    0.25f, 0.50f,    0.577f, -0.577f, -0.577f,
		0.0f,  1.0f,  0.0f,     0.00f, 0.25f,    0.577f, -0.577f,  0.577f, -1.0f,  0.0f,  0.0f,    0.25f, 0.5f,     0.577f, -0.577f,  0.577f, 0.0f,  0.0f, -1.0f,      0.48f, 0.25f,    0.577f, -0.577f,  0.577f,
		0.0f,  1.0f,  0.0f,		0.98f, 0.25f,   -0.577f, -0.577f,  0.577f, 0.0f,  0.0f, -1.0f,		0.50f, 0.25f,   -0.577f, -0.577f,  0.577f, 1.0f,  0.0f,  0.0f,		0.749f, 0.49f,  -0.577f, -0.577f,  0.577f,
		0.0f, -1.0f,  0.0f,    0.99f, 0.74f,    -0.577f,  0.577f, -0.577f, 0.0f,  0.0f,  1.0f,    0.75f, 0.5f,     -0.577f,  0.577f, -0.577f, 1.0f,  0.0f,  0.0f,    0.50f, 0.74f,    -0.577f,  0.577f, -0.577f,
		0.0f, -1.0f,  0.0f,    0.01f, 0.75f,     0.577f,  0.577f, -0.577f, -1.0f,  0.0f,  0.0f,    0.49f, 0.74f,    0.577f,  0.577f, -0.577f, 0.0f,  0.0f,  1.0f,    0.25f, 0.50f,    0.577f,  0.577f, -0.577f,
		0.0f, -1.0f,  0.0f,    0.50f, 0.25f,     0.577f,  0.577f,  0.577f, 0.0f,  0.0f, -1.0f,    0.25f, 0.49f,     0.577f,  0.577f,  0.577f, -1.0f,  0.0f,  0.0f,    0.75f, 0.50f,    0.577f,  0.577f,  0.577f,
		0.0f, -1.0f,  0.0f,    0.74f, 0.00f,    -0.577f,  0.577f,  0.577f, 1.0f,  0.0f,  0.0f,    0.50f, 0.24f,    -0.577f,  0.577f,  0.577f, 0.0f,  0.0f, -1.0f,    0.98f, 0.24f,    -0.577f,  0.577f,  0.577f,
	};
	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(octaedro_vertices, octaedro_indices, 192, 24);
	meshList.push_back(octaedro);
}


int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();
	CreateObjects();
	CrearPecera();
	CreateShaders();
	CrearDadoOcho();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png"); brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png"); dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png"); plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga"); pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga"); AgaveTexture.LoadTextureA();
	dadoOchoTexture = Texture("Textures/dado_ocho.jpg"); dadoOchoTexture.LoadTextureA();
	aguaTexture = Texture("Textures/agua.png"); aguaTexture.LoadTextureA();

	Coche_M = Model(); Coche_M.LoadModel("Models/coche.obj");
	Llantadelanteizq_M = Model(); Llantadelanteizq_M.LoadModel("Models/cochellantadelanteizq.obj");
	Cofre_M = Model(); Cofre_M.LoadModel("Models/cochecofre.obj");
	Nave_M = Model(); Nave_M.LoadModel("Models/nave.obj");
	Lampara_M = Model(); Lampara_M.LoadModel("Models/lamp.obj");

	PezCuerpo_M = Model(); PezCuerpo_M.LoadModel("Models/cuerpo_pez.obj");
	PezBulba_M = Model(); PezBulba_M.LoadModel("Models/foco_pez.obj");
	PezAntena_M = Model(); PezAntena_M.LoadModel("Models/antena_pez.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga"); skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga"); skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga"); skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);


	// Luces puntuales ---------
	unsigned int pointLightCount = 0;

	// Luz 0
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	// Luz 1: lampara
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.2f, 1.5f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	// Luz del Pez 
	PointLight luzPuntualPez = PointLight(0.0f, 0.0f, 1.0f,
		0.5f, 1.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.7f, 1.8f);


	// spotLightCount ---------
	unsigned int spotLightCount = 0;

	// Spot 0: Cámara
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	// Spot 1: Fijo Verde
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	// Spot 2: Faro del Coche (Plantilla)
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 1.5f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.01f, 0.0f,
		20.0f);
	spotLightCount++;

	// Reflectores de la nave -----------
	SpotLight luzNaveFrente = SpotLight(1.0f, 1.0f, 0.0f,
		0.8f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.0f,
		20.0f);

	SpotLight luzNaveAtras = SpotLight(1.0f, 1.0f, 0.0f,
		0.8f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.0f,
		20.0f);

	// Reflector Amarillo del Pez
	SpotLight luzReflectorPez = SpotLight(1.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.0f,
		15.0f);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model;
	glm::vec3 color;

	glm::mat4 modelCoche;
	glm::mat4 nodoCoche;
	glm::vec3 lowerLight;
	glm::vec4 posFaro;
	glm::vec3 dirFaro;
	glm::vec4 dirLocal;
	GLfloat r = 0.0f, g = 0.0f, b = 0.0f;

	glm::mat4 nodoLampara;
	glm::vec4 posLuzLampara;
	glm::mat4 nodoNave;
	glm::vec4 posLuzNaveFrente;
	glm::vec3 dirLuzNaveFrente;
	glm::vec4 posLuzNaveAtras;
	glm::vec3 dirLuzNaveAtras;

	bool* keys;

	// apagador de luz del pez
	bool luzPezPrendida = true;
	bool teclaBPresionada = false;

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		keys = mainWindow.getsKeys();

		// tecla B para prender/apagar la luz del pez
		if (keys[GLFW_KEY_B])
		{
			if (!teclaBPresionada)
			{
				luzPezPrendida = !luzPezPrendida;
				teclaBPresionada = true;
			}
		}
		else
		{
			teclaBPresionada = false;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();

		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		model = glm::mat4(1.0f);
		color = glm::vec3(1.0f, 1.0f, 1.0f);


		//posiciones luces

		// jerarquia lampara
		nodoLampara = glm::mat4(1.0f);
		nodoLampara = glm::translate(nodoLampara, glm::vec3(-5.0f, 0.5f, 8.0f));
		posLuzLampara = nodoLampara * glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);

		pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
			0.2f, 1.5f,
			posLuzLampara.x, posLuzLampara.y, posLuzLampara.z,
			0.3f, 0.2f, 0.1f);


		// Jerarquia pez 
		glm::mat4 nodoPezCuerpo = glm::mat4(1.0f);
		nodoPezCuerpo = glm::translate(nodoPezCuerpo, glm::vec3(-16.7f + mainWindow.getMovPezX(), -0.5f + mainWindow.getMovPezY(), 8.0f));
		nodoPezCuerpo = glm::rotate(nodoPezCuerpo, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		nodoPezCuerpo = glm::scale(nodoPezCuerpo, glm::vec3(0.05f, 0.07f, 0.07f));

		//calculo de la posición de la luz del pez 
		glm::mat4 nodoPezBulba = nodoPezCuerpo;
		nodoPezBulba = glm::translate(nodoPezBulba, glm::vec3(0.9f, 15.5f, 4.5f));

		// posicion de bulba
		glm::vec4 posLuzPez = nodoPezBulba * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		// Luz pez azul
		luzPuntualPez = PointLight(0.0f, 0.0f, 1.0f,
			0.5f, 1.5f,
			posLuzPez.x, posLuzPez.y, posLuzPez.z,
			1.0f, 0.7f, 1.8f);

		// calculamos la dirección del reflector del pez a partir de los ángulos de GUI (mainWindow.getPezSpotAnguloX() y mainWindow.getPezSpotAnguloY())
		float yawPez = glm::radians(mainWindow.getPezSpotAnguloX());
		float pitchPez = glm::radians(mainWindow.getPezSpotAnguloY());
		//calculo de la dirección en coordenadas cartesianas a partir de los ángulos de GUI
		glm::vec3 dirPez;
		dirPez.x = cos(pitchPez) * cos(yawPez);
		dirPez.y = sin(pitchPez);
		dirPez.z = cos(pitchPez) * sin(yawPez);
		dirPez = glm::normalize(dirPez);

		// reutilizamos la posición del bulba (posLuzPez) y el nuevo vector dirección (dirPez) para actualizar el reflector del pez
		luzReflectorPez = SpotLight(1.0f, 1.0f, 0.0f, // Amarilla
			1.0f, 2.0f,
			posLuzPez.x, posLuzPez.y, posLuzPez.z, 
			dirPez.x, dirPez.y, dirPez.z,          
			1.0f, 0.02f, 0.0f,
			15.0f);

		// antena del pez (hija del cuerpo)
		glm::mat4 nodoPezAntena = nodoPezCuerpo;
		nodoPezAntena = glm::translate(nodoPezAntena, glm::vec3(1.2f, 6.5f, -2.2f));


		// jerarquia coche
		modelCoche = glm::mat4(1.0);
		modelCoche = glm::translate(modelCoche, glm::vec3(15.0f + mainWindow.getmuevex(), -0.4f, 0.0f));
		modelCoche = glm::rotate(modelCoche, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		nodoCoche = modelCoche;
		modelCoche = glm::scale(modelCoche, glm::vec3(1.0f, 1.0f, 1.0f));

		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		posFaro = nodoCoche * glm::vec4(2.8f, 3.0f, 7.5f, 1.0f);
		dirLocal = glm::vec4(0.0f, -0.2f, 1.0f, 0.0f);
		dirFaro = glm::normalize(glm::vec3(nodoCoche * dirLocal));

		r = 0.0f; g = 0.0f; b = 0.0f;
		switch (mainWindow.getColorFaroIndex())
		{
		case 0: r = 1.0f; g = 0.0f; b = 0.0f; break;
		case 1: r = 1.0f; g = 1.0f; b = 0.0f; break;
		case 2: r = 0.0f; g = 1.0f; b = 0.0f; break;
		case 3: r = 0.0f; g = 1.0f; b = 1.0f; break;
		case 4: r = 0.0f; g = 0.0f; b = 1.0f; break;
		case 5: r = 1.0f; g = 0.0f; b = 1.0f; break;
		}

		spotLights[2] = SpotLight(r, g, b,
			0.0f, 1.5f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			1.0f, 0.01f, 0.0f,
			20.0f);
		spotLights[2].SetFlash(glm::vec3(posFaro), dirFaro);


		// jerarquia nave
		nodoNave = glm::mat4(1.0f);
		nodoNave = glm::translate(nodoNave, glm::vec3(mainWindow.getPosXNave(), 5.0f, 6.0f));

		posLuzNaveFrente = nodoNave * glm::vec4(-2.0f, 0.0f, 0.0f, 1.0f);
		dirLuzNaveFrente = glm::normalize(glm::vec3(nodoNave * glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f)));

		posLuzNaveAtras = nodoNave * glm::vec4(2.0f, 0.0f, 0.0f, 1.0f);
		dirLuzNaveAtras = glm::normalize(glm::vec3(nodoNave * glm::vec4(1.0f, -1.0f, 0.0f, 0.0f)));

		luzNaveFrente.SetFlash(glm::vec3(posLuzNaveFrente), dirLuzNaveFrente);
		luzNaveAtras.SetFlash(glm::vec3(posLuzNaveAtras), dirLuzNaveAtras);


		// 
		shaderList[0].SetDirectionalLight(&mainLight);

		// arreglo de spotlights ------------------------------------------------
		SpotLight lucesSpotActivas[MAX_SPOT_LIGHTS];
		unsigned int conteoSpotActivas = 0;

		lucesSpotActivas[conteoSpotActivas++] = spotLights[0]; // Cámara
		lucesSpotActivas[conteoSpotActivas++] = spotLights[1]; // Fija
		lucesSpotActivas[conteoSpotActivas++] = spotLights[2]; // Coche

		// luz del pez (controlada por teclas 1-4)
		lucesSpotActivas[conteoSpotActivas++] = luzReflectorPez;

		// control de reflectores de la nave (teclas J y K)
		if (keys[GLFW_KEY_J]) lucesSpotActivas[conteoSpotActivas++] = luzNaveFrente;
		if (keys[GLFW_KEY_K]) lucesSpotActivas[conteoSpotActivas++] = luzNaveAtras;

		shaderList[0].SetSpotLights(lucesSpotActivas, conteoSpotActivas);

		// arreglo de luces puntuales ------------------------------------------------
		PointLight lucesPuntualesActivas[MAX_POINT_LIGHTS];
		unsigned int conteoPointActivas = 0;

		lucesPuntualesActivas[conteoPointActivas++] = pointLights[0]; // Roja

		//control de luces 
		if (mainWindow.isLuzPrendida())
		{
			lucesPuntualesActivas[conteoPointActivas++] = pointLights[1]; // Lámpara
		}

		if (luzPezPrendida)
		{
			lucesPuntualesActivas[conteoPointActivas++] = luzPuntualPez; // luz pez azul
		}

		shaderList[0].SetPointLights(lucesPuntualesActivas, conteoPointActivas);



		// Piso
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		// Base del coche
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = nodoCoche;
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Coche_M.RenderModel();

		// Cofre del coche
		model = nodoCoche;
		model = glm::translate(model, glm::vec3(0.0f, 4.7f, 5.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getAnguloCofre()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre_M.RenderModel();

		// Llantas del coche
		color = glm::vec3(0.25f, 0.25f, 0.25f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		model = nodoCoche;
		model = glm::translate(model, glm::vec3(4.0f, 1.0f, -6.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantadelanteizq_M.RenderModel();

		model = nodoCoche;
		model = glm::translate(model, glm::vec3(-4.0f, 1.0f, -6.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantadelanteizq_M.RenderModel();

		model = nodoCoche;
		model = glm::translate(model, glm::vec3(4.0f, 1.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantadelanteizq_M.RenderModel();

		model = nodoCoche;
		model = glm::translate(model, glm::vec3(-4.0f, 1.0f, 8.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llantadelanteizq_M.RenderModel();

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Nave
		model = nodoNave;
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nave_M.RenderModel();

		// Dado de 8 caras 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-8.0f, 3.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoOchoTexture.UseTexture();
		meshList[5]->RenderMesh();

		// Lámpara 
		model = nodoLampara;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara_M.RenderModel();

		//-------------------------------------------
		// Pez Abisal (Cuerpo)
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(nodoPezCuerpo));
		glUniform3f(uniformColor, 0.0f, 1.0f, 1.0f);
		PezCuerpo_M.RenderModel();

		// Bulba
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(nodoPezBulba));
		glUniform3f(uniformColor, 1.0f, 1.0f, 0.0f); // Amarillo
		PezBulba_M.RenderModel();

		// La antena
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(nodoPezAntena));
		glUniform3f(uniformColor, 0.0f, 1.0f, 1.0f); // Cian
		PezAntena_M.RenderModel();


		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f); // Resectolor


		// renderizado de objetos transparentes 
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Agave
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		// Pecera
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-15.0f, 1.2f, 8.0f));
		model = glm::scale(model, glm::vec3(6.0f, 4.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glDepthMask(GL_FALSE);

		aguaTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();


		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}