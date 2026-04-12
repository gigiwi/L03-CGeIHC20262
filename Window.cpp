#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	muevex = 2.0f;
	mouseFirstMoved = true;
	anguloLlantas = 0.0f;
	anguloCofre = 0.0f;
	posZCoche = 0.0f;

	for (size_t i = 0; i < 1024; i++) { keys[i] = 0; }
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	mouseFirstMoved = true;
	anguloLlantas = 0.0f;
	anguloCofre = 0.0f;
	posZCoche = 0.0f;

	for (size_t i = 0; i < 1024; i++) { keys[i] = 0; }
}

int Window::Initialise()
{
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Practica 6: Texturizado y Coche Jerarquico", NULL, NULL);
	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);
	createCallbacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, bufferWidth, bufferHeight);
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

GLfloat Window::getXChange() { GLfloat theChange = xChange; xChange = 0.0f; return theChange; }
GLfloat Window::getYChange() { GLfloat theChange = yChange; yChange = 0.0f; return theChange; }

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE); }
	if (key == GLFW_KEY_Y) { theWindow->muevex += 1.0; }
	if (key == GLFW_KEY_U) { theWindow->muevex -= 1.0; }

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) { theWindow->keys[key] = true; }
		else if (action == GLFW_RELEASE) { theWindow->keys[key] = false; }
	}

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		// traslacion del coche (flechas arriba y abajo)
		if (key == GLFW_KEY_UP) { theWindow->posZCoche -= 0.5f; } // adelante 
		if (key == GLFW_KEY_DOWN) { theWindow->posZCoche += 0.5f; } // atrás 

		// movimiento de las llantas (teclas F y G)
		if (key == GLFW_KEY_F) { theWindow->anguloLlantas += 5.0f; }
		if (key == GLFW_KEY_G) { theWindow->anguloLlantas -= 5.0f; }

		// apertura del cofre (teclas E y R)
		if (key == GLFW_KEY_E)
		{
			theWindow->anguloCofre += 2.0f;
			if (theWindow->anguloCofre > 45.0f) { theWindow->anguloCofre = 45.0f; } //limitando a 45 para abrir
		}
		if (key == GLFW_KEY_R)
		{
			theWindow->anguloCofre -= 2.0f;
			if (theWindow->anguloCofre < 0.0f) { theWindow->anguloCofre = 0.0f; } //limitando a 0 para cerrar
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;
	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}