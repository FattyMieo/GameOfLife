#ifndef _TRIANGLE_DEMO_H
#define _TRIANGLE_DEMO_H

#include "demo_base.h"

class TriangleDemo : public DemoBase
{
public:
	void init()
	{
	}

	void deinit()
	{
	}

	void drawCell(float x, float y)
	{

	}

	void draw()
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);								// Drawing Using Triangles
			glVertex3f( 0.0f, 0.0f, 0.0f);					// Top
			glVertex3f( 300.0f, 0.0f, 0.0f);					// Bottom Left
			glVertex3f( 0.0f, 300.0f, 0.0f);					// Bottom Right
		glEnd();										// Finished Drawing The Triangle
	}

	void onMouseMove(double x, double y)
	{
		//Debug
		std::cout << "mouse pos: " << x << ", " << y << std::endl;
	}

	void onMouseButton(int button, int action)
	{
		//Debug
		std::cout << "mouse button: " << button << ", " << action << std::endl;
	}
};

#endif
