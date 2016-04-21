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
#include "rad.h"

#include "GL/gl.h"
#include "GL/glut.h"
//#include "GL/wglew.h"

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
}

void BeginDraw(TView *view, unsigned long color)
{
	/* first time this view is drawn */
	if (view->wid == 0)
	{
		glutInit(&g_argc, g_argv);
		glutInitDisplayMode(GLUT_SINGLE);
		glutInitWindowSize(600, 600);
		glutInitWindowPosition(50, 50);

		glutCreateWindow("GLRad");
		glutDisplayFunc(display);
		glutKeyboardFunc(KeyboardFunc);
		glutMouseFunc(MouseFunc);
		glutReshapeFunc(reshape);
		glutPassiveMotionFunc(PassiveMouseMove);
		glutIdleFunc(IdleFunc);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
		glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
		glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
		//glShadeModel(GL_SMOOTH);   // Enable smooth shading
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
	}

	/* set up view transformation from the parameters in view */
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset
	float matrix[16];
	glhPerspectivef2(matrix, view->fovx, 1, .1f, 1000.f);
	glLoadMatrixf(matrix);

	/* clear the frame buffer with color */
	glClearColor(0, 0, 0, 0);
	glClearDepth(1);
}

void DrawPolygon(int nPts, TPoint3f *pts, TVector3f* normals, unsigned int color)
{
	glBegin(GL_POLYGON);

	for (int n = 0; n < nPts; ++n)
	{
		const TPoint3f &p = pts[n];
		const TVector3f &m = normals[n];
		glVertex3f(p.x, p.y, p.z);
		glNormal3f(m.x, m.y, m.z);
	}

	glColor4uiv(&color);
	glEnd();
}

void EndDraw()
{
	glFlush();
	glutSwapBuffers();
}


