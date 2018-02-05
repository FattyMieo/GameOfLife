#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <ctime>
#include <GL/GLU.h>

#include "camera.h"
#include "matrix.h"
#include "vector.h"

#include "triangle_demo.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

TriangleDemo* demo;
float zoomFactor = 1.0f;
bool closeWindow = false;

void onWindowResized(GLFWwindow* window, int width, int height)
{
	if (height == 0) height = 1;						// Prevent A Divide By Zero By making Height Equal One

	glViewport(0, 0, width, height);					// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	gluOrtho2D(0, width * zoomFactor, height * zoomFactor, 0);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

// Simple MOUSE callback
void onMouseMove(GLFWwindow* window, double x, double y)
{
	demo->onMouseMove(x, y);
}

void onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	demo->onMouseButton(button, action);
}

void onKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE) closeWindow = true;
	demo->onKeyButton(key, action);
}

int main()
{
	int running = GL_TRUE;

    GLFWwindow* window;

    /* Initialize the GLFW library */
    if (!glfwInit())
        return -1;

	// Open an OpenGL window
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

	// Hook window resize.
	glfwSetWindowSizeCallback(window, onWindowResized);
	   
	/* Make the window's context current */
    glfwMakeContextCurrent(window);

	onWindowResized(window, WINDOW_WIDTH, WINDOW_HEIGHT);

	// hook mouse move callback and lock/hide mouse cursor.
	glfwSetCursorPosCallback(window, onMouseMove);
	glfwSetMouseButtonCallback(window, onMouseButton);
	glfwSetKeyCallback(window, onKeyButton);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// initialize OpenGL.
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	
	// initialize demo.
	demo = new TriangleDemo();
	demo->init(WINDOW_WIDTH, WINDOW_HEIGHT);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// OpenGL rendering goes here...
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (closeWindow) break;

		demo->draw();

		// Swap front and back rendering buffers
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	demo->deinit();
	delete demo;

	// Close window and terminate GLFW
	glfwTerminate();
}
