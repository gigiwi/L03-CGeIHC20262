#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	muevex = 2.0f;
	mouseFirstMoved = true;

	anguloCofre = 0.0f;
	posXNave = 0.0f;

	movPezX = 0.0f;
	movPezY = 0.0f;

	pezSpotAnguloX = 0.0f;
	pezSpotAnguloY = -90.0f; 

	luzPrendida = false;
	luzPezPrendida = true; 
	colorFaroIndex = 0;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	mouseFirstMoved = true;

	anguloCofre = 0.0f;
	posXNave = 0.0f;

	movPezX = 0.0f;
	movPezY = 0.0f;

	pezSpotAnguloX = 0.0f;
	pezSpotAnguloY = -90.0f;

	luzPrendida = false; 
	luzPezPrendida = true;
	colorFaroIndex = 0;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
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

	mainWindow = glfwCreateWindow(width, height, "Practica08: Iluminacion II", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow);
	createCallbacks();
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

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_Y)
	{
		theWindow->muevex += 1.0f;
	}

	if (key == GLFW_KEY_U)
	{
		theWindow->muevex -= 1.0f;
	}
	if (key == GLFW_KEY_J)
	{
		theWindow->posXNave -= 0.5f;
	}

	if (key == GLFW_KEY_K)
	{
		theWindow->posXNave += 0.5f;
	}


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
		}
	}

	// movimiento del pez triangular por la pecera
	if (key == GLFW_KEY_C)
	{
		theWindow->movPezX += 0.05f; 

		// Parámetros para el movimiento triangular
		const float distanciaTotalX = 4.17f; // recorrido total
		const float mitadX = 1.9f;           // mitad, punto de inflexión
		const float alturaMaxY = 2.7f;       // altura maxima
		const float alturaFinalY = 0.5f;     // altura final relativa
		const float baseOffset = 0.0f;       

		//reinicia el movimiento al terminar
		if (theWindow->movPezX >= distanciaTotalX)
		{
			theWindow->movPezX = 0.0f;
		}

		float yRel = 0.0f;

		if (theWindow->movPezX <= mitadX)
		{
			// primera mitad (Sube)
			yRel = (alturaMaxY / mitadX) * theWindow->movPezX;
		}
		else
		{
			// segunda mitad (Baja)
			float pendiente = (alturaFinalY - alturaMaxY) / (distanciaTotalX - mitadX);
			yRel = alturaMaxY + pendiente * (theWindow->movPezX - mitadX);
		}

		if (yRel > alturaMaxY)
		{
			yRel = alturaMaxY;
		}

		theWindow->movPezY = yRel - baseOffset;
	}

	// control movimiento del spot del pez (1-4 para moverlo en X e Y)
	if (key == GLFW_KEY_1)
	{
		theWindow->pezSpotAnguloX -= 5.0f;
	}
	if (key == GLFW_KEY_2)
	{
		theWindow->pezSpotAnguloX += 5.0f;
	}
	if (key == GLFW_KEY_3)
	{
		theWindow->pezSpotAnguloY -= 5.0f;
		if (theWindow->pezSpotAnguloY < -89.0f) theWindow->pezSpotAnguloY = -89.0f; 
	}
	if (key == GLFW_KEY_4)
	{
		theWindow->pezSpotAnguloY += 5.0f;
		if (theWindow->pezSpotAnguloY > 89.0f) theWindow->pezSpotAnguloY = 89.0f; 
	}

	// control luces puntuales (L para lámpara y B para aura del pez)
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		theWindow->luzPrendida = !theWindow->luzPrendida;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		theWindow->luzPezPrendida = !theWindow->luzPezPrendida;
	}

	// control faro coche
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		theWindow->colorFaroIndex++;
		if (theWindow->colorFaroIndex > 5)
		{
			theWindow->colorFaroIndex = 0;
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

bool Window::getLuzPezPrendida() { return luzPezPrendida; }
GLfloat Window::getPezSpotAnguloX() { return pezSpotAnguloX; }
GLfloat Window::getPezSpotAnguloY() { return pezSpotAnguloY; }

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}