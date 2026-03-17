#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}


Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;


	cuello = 0.0f;
	cola = 0.0f;

	for (int i = 0; i < 4; i++) {
		pataA[i] = 0.0f;
		pataB[i] = 0.0f;
	}

	for (size_t i = 0; i < 1024; i++) { keys[i] = 0; }
}


int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Ejercicio: Mascota Robot (Gatito)", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD

	//Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

//Se modifico para las nuevas articulaciones de el gato robot
void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) { theWindow->keys[key] = true; }
		else if (action == GLFW_RELEASE) { theWindow->keys[key] = false; }
	}

	GLfloat vel = 5.0f;

	if (key == GLFW_KEY_F) { theWindow->cuello += vel; } // Cabeza

	if (key == GLFW_KEY_G) { theWindow->cola += vel; }   // Cola

	if (key == GLFW_KEY_1) { theWindow->pataA[0] += vel; } // PataA
	if (key == GLFW_KEY_2) { theWindow->pataB[0] += vel; } // PataB

	// Pata Delantera Izquierda
	if (key == GLFW_KEY_3) { theWindow->pataA[1] += vel; } // PataA
	if (key == GLFW_KEY_4) { theWindow->pataB[1] += vel; } // PataB

	// Pata Trasera Derecha
	if (key == GLFW_KEY_5) { theWindow->pataA[2] += vel; } // PataA
	if (key == GLFW_KEY_6) { theWindow->pataB[2] += vel; } // PataB

	// Pata Trasera Izquierda
	if (key == GLFW_KEY_7) { theWindow->pataA[3] += vel; } // PataA
	if (key == GLFW_KEY_8) { theWindow->pataB[3] += vel; } // PataB
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