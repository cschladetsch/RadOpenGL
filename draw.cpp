/*****************************************************************************
*	draw.c
*
*	This is a skeleton polygon drawing program
*
* 	Copyright (C) 1990-1991 Apple Computer, Inc.
* 	All rights reserved.
*
*	8/27/1991 S. Eric Chen
******************************************************************************/
#include <stdio.h>

#include "rad.h"
#include "GL/gl.h"
#include "GL/glut.h"

void PassiveMouseMove(int x, int y);
void MouseFunc(int button, int state, int x, int y);
void KeyboardFunc(unsigned char key, int x, int y);
void IdleFunc();
void reshape(GLsizei width, GLsizei height);

void glhPerspectivef2(float *matrix, float fovyInDegrees, float aspectRatio,
                      float znear, float zfar);
extern int g_argc;
extern char **g_argv;

void display()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	//glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
}

bool g_glutInit;

void BeginDraw(TView *view, unsigned long color)
{
	/* first time this view is drawn */
	if (view->wid == 0)
	{
		if (!g_glutInit)
		{
			glutInit(&g_argc, g_argv);
			g_glutInit = true;
			glutCreateWindow("GLRad");
			glutInitDisplayMode(GLUT_SINGLE);
			glutInitWindowPosition(50, 50);
			glutDisplayFunc(display);
			glutKeyboardFunc(KeyboardFunc);
			glutMouseFunc(MouseFunc);
			glutReshapeFunc(reshape);
			glutPassiveMotionFunc(PassiveMouseMove);
			glutIdleFunc(IdleFunc);
		}
		glutInitWindowSize(view->xRes, view->yRes);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
		glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
		glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
		//glShadeModel(GL_SMOOTH);   // Enable smooth shading
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
		view->wid = 1;
	}
	/* set up view transformation from the parameters in view */
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset
	float matrix[16];
	glhPerspectivef2(matrix, view->fovx, 1, .1f, 1000.f);
	glLoadMatrixf(matrix);

	/* clear the frame buffer with color */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	glClearDepth(1);
	glMatrixMode(GL_MODELVIEW);
}

void DrawPolygon(int nPts, TPoint3f *pts, TVector3f* normals, unsigned long color)
{
	//printf("DrawPoly\n");
	glBegin(GL_POLYGON);

	for (int n = 0; n < nPts; ++n)
	{
		const TPoint3f &p = pts[n];
		const TVector3f &m = normals[n];
		glVertex3f(p.x, p.y, p.z);
		glNormal3f(m.x, m.y, m.z);
		GLuint g = (GLuint)color;
		glColor4uiv(&g);
	}

	glEnd();
}

void EndDraw()
{
	glFlush();
	glutSwapBuffers();
}


