/*****************************************************************************
* 	room.c
*
*	This is a test program which constrcuts the Cornell radiosity room with
*   a ceiling light and two boxes inside. The side faces of the boxes are not
*	directly illuminated by the light. Therefore, they are a good example of
*	the color bleeding effects.
*   This program calls IniRad(), DoRad() and CleanUpRad() in rad.c to perform
*	the radiosity rendering.
*
*	Copyright (C) 1990-1991 Apple Computer, Inc.
*	All rights reserved.
*
*	12/1990 S. Eric Chen
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "rad.h"
#include <GL/glut.h>

int g_argc;
char **g_argv;

/* a quadrilateral */
typedef struct
{
	short verts[4];	/* vertices of the quadrilateral */
	short patchLevel; /* patch subdivision level (how fine to subdivide the quadrilateral?) */
	short elementLevel; /* element subdivision level (how fine to subdivide a patch?) */
	float area; /* area of the quadrilateral */
	TVector3f normal; /* normal of the quadrilateral */
	TSpectra* reflectance; /* diffuse reflectance of the quadrilateral */
	TSpectra* emission; /* emission of the quadrilateral */
} TQuad;

/* input parameters */
TRadParams	params =
{
	0.001,			/* convergence threshold */
	0, 0, 0, 0, 0, 0,	/* patches, elements and points; initialize these in InitParams */
	{	{ 108, 120, 400 },	/* camera location */
		{ 108, 100, 100 },	/* look at point */
		{ 0, 1, 0 },	/* up vector */
		60, 60,			/* field of view in x, y*/
		1, 550,			/* near, far */
		200, 200,		/* resolution x, y */
		0
	},			/* buffer */
	100,			/* hemi-cube resolution */
	250,			/* approximate diameter of the room */
	50,				/* intensity scale */
	1				/* add the ambient term */
};

TPoint3f roomPoints[] =
{
	0, 0, 0,
	216, 0, 0,
	216, 0, 215,
	0, 0, 215,
	0, 221, 0,
	216, 221, 0,
	216, 221, 215,
	0, 221, 215,

	85.5, 220, 90,
	130.5, 220, 90,
	130.5, 220, 130,
	85.5, 220, 130,

	53.104, 0, 64.104,
	109.36, 0, 96.604,
	76.896, 0, 152.896,
	20.604, 0, 120.396,
	53.104, 65, 64.104,
	109.36, 65, 96.604,
	76.896, 65, 152.896,
	20.604, 65, 120.396,

	134.104, 0, 67.104,
	190.396, 0, 99.604,
	157.896, 0, 155.896,
	101.604, 0, 123.396,
	134.104, 130, 67.104,
	190.396, 130, 99.604,
	157.896, 130, 155.896,
	101.604, 130, 123.396
};

static TSpectra red = { 0.80, 0.10, 0.075 };
static TSpectra yellow = { 0.9, 0.8, 0.1 };
static TSpectra blue = { 0.075, 0.10, 0.35 };
static TSpectra white = { 1.0, 1.0, 1.0 };
static TSpectra lightGrey = { 0.9, 0.9, 0.9 };
static TSpectra black = { 0.0, 0.0, 0.0 };

/* Assume a right-handed coordinate system */
/* Polygon vertices follow counter-clockwise order when viewing from front */
#define numberOfPolys 	18
TQuad roomPolys[numberOfPolys] =
{
	{{4, 5, 6, 7}, 2, 8, 216*215, {0, -1, 0}, &lightGrey, &black}, /* ceiling */
	{{0, 3, 2, 1}, 3, 8, 216*215, {0, 1, 0}, &lightGrey, &black}, /* floor */
	{{0, 4, 7, 3}, 2, 8, 221*215, {1, 0, 0}, &red, &black}, /* wall */
	{{0, 1, 5, 4}, 2, 8, 221*216, {0, 0, 1}, &lightGrey, &black}, /* wall */
	{{2, 6, 5, 1}, 2, 8, 221*215, {-1, 0, 0}, &blue, &black}, /* wall */
	{{8, 9, 10, 11}, 2, 1, 40*45, {0, -1, 0}, &black, &white}, /* light */
	{{16, 19, 18, 17}, 1, 5, 65*65, {0, 1, 0}, &yellow, &black}, /* box 1 */
	{{12, 13, 14, 15}, 1, 1, 65*65, {0, -1, 0}, &yellow, &black},
	{{12, 15, 19, 16}, 1, 5, 65*65, {-0.866, 0, -0.5}, &yellow, &black},
	{{12, 16, 17, 13}, 1, 5, 65*65, {0.5, 0, -0.866}, &yellow, &black},
	{{14, 13, 17, 18}, 1, 5, 65*65, {0.866, 0, 0.5}, &yellow, &black},
	{{14, 18, 19, 15}, 1, 5, 65*65, {-0.5, 0, 0.866}, &yellow, &black},
	{{24, 27, 26, 25}, 1, 5, 65*65, {0, 1, 0}, &lightGrey, &black}, /* box 2 */
	{{20, 21, 22, 23}, 1, 1, 65*65, {0, -1, 0}, &lightGrey, &black},
	{{20, 23, 27, 24}, 1, 6, 65*130, {-0.866, 0, -0.5}, &lightGrey, &black},
	{{20, 24, 25, 21}, 1, 6, 65*130, {0.5, 0, -0.866}, &lightGrey, &black},
	{{22, 21, 25, 26}, 1, 6, 65*130, {0.866, 0, 0.5}, &lightGrey, &black},
	{{22, 26, 27, 23}, 1, 6, 65*130, {-0.5, 0, 0.866}, &lightGrey, &black},
};


/* Compute the xyz coordinates of a point on a quadrilateral given its u, v coordinates using bi-linear mapping */
void UVToXYZ(const TPoint3f quad[4], float u, float v, TPoint3f* xyz)
{
	xyz->x = quad[0].x * (1-u)*(1-v) + quad[1].x * (1-u)*v + quad[2].x * u*v + 			quad[3].x * u*(1-v);
	xyz->y = quad[0].y * (1-u)*(1-v) + quad[1].y * (1-u)*v + quad[2].y * u*v + 			quad[3].y * u*(1-v);
	xyz->z = quad[0].z * (1-u)*(1-v) + quad[1].z * (1-u)*v + quad[2].z * u*v + 			quad[3].z * u*(1-v);
}

#define Index(i, j) ((i)*(nv+1)+(j))

int iOffset; 	/* index offset to the point array */
TPatch* pPatch;
TElement* pElement;
TPoint3f* pPoint;

/* Mesh a quadrilateral into patches and elements */
/* Output goes to pPatch, pElement, pPoint */
void MeshQuad(TQuad* quad)
{
	TPoint3f pts[4];
	int nu, nv;
	double	du, dv;
	int i, j;
	double u, v;
	int nPts=0;
	float fi, fj;
	int pi, pj;

	/* Calculate element vertices */
	for (i=0; i<4; i++)
	{
		pts[i] = roomPoints[quad->verts[i]];
	}

	nu = nv = quad->patchLevel * quad->elementLevel+1;
	du = 1.0 / (nu-1);
	dv = 1.0 / (nv-1);

	for (i = 0, u = 0; i < nu; i++, u += du)
		for (j = 0, v = 0; j < nv; j++, v += dv, nPts++)
		{
			UVToXYZ(pts, u, v, pPoint++);
		}

	/* Calculate elements */
	nu = nv = quad->patchLevel*quad->elementLevel;
	du = 1.0 / nu;
	dv = 1.0 / nv;

	for (i = 0, u = du/2.0; i < nu; i++, u += du)
		for (j = 0, v = dv/2.0; j < nv; j++, v += dv, pElement++)
		{
			pElement->normal = quad->normal;
			pElement->nVerts = 4;
			pElement->verts = (unsigned long*)calloc(4, sizeof(unsigned long));
			pElement->verts[0] = Index(i, j)+iOffset;
			pElement->verts[1] = Index(i+1, j)+iOffset;
			pElement->verts[2] = Index(i+1, j+1)+iOffset;
			pElement->verts[3] = Index(i, j+1)+iOffset;
			pElement->area = quad->area / (nu*nv);
			/* find out the parent patch */
			fi = (float)i/(float)nu;
			fj = (float)j/(float)nv;
			pi = (int)(fi*(float)(quad->patchLevel));
			pj = (int)(fj*(float)(quad->patchLevel));
			pElement->patch = pPatch+pi*quad->patchLevel+pj;
		}

	/* Calculate patches */
	nu = quad->patchLevel;
	nv=quad->patchLevel;
	du = 1.0 / nu;
	dv = 1.0 / nv;

	for (i = 0, u = du/2.0; i < nu; i++, u += du)
		for (j = 0, v = dv/2.0; j < nv; j++, v += dv, pPatch++)
		{
			UVToXYZ(pts, u, v, &pPatch->center);
			pPatch->normal = quad->normal;
			pPatch->reflectance = quad->reflectance;
			pPatch->emission = quad->emission;
			pPatch->area = quad->area / (nu*nv);
		}

	iOffset += nPts;
}

/* Initialize input parameters */
void InitParams()
{
	int i;

	/* compute the total number of patches */
	params.nPatches=0;

	for (i=numberOfPolys; i--; )
	{
		params.nPatches += roomPolys[i].patchLevel*roomPolys[i].patchLevel;
	}

	params.patches = (TPatch*)calloc(params.nPatches, sizeof(TPatch));

	/* compute the total number of elements */
	params.nElements=0;

	for (i=numberOfPolys; i--; )
		params.nElements += roomPolys[i].elementLevel*roomPolys[i].patchLevel*
		                    roomPolys[i].elementLevel*roomPolys[i].patchLevel;

	params.elements = (TElement*)calloc(params.nElements, sizeof(TElement));

	/* compute the total number of element vertices */
	params.nPoints=0;

	for (i=numberOfPolys; i--; )
		params.nPoints += (roomPolys[i].elementLevel*roomPolys[i].patchLevel+1)*
		                  (roomPolys[i].elementLevel*roomPolys[i].patchLevel+1);

	params.points = (TPoint3f*)calloc(params.nPoints, sizeof(TPoint3f));

	/* mesh the room to patches and elements */
	iOffset = 0;
	pPatch= params.patches;
	pElement= params.elements;
	pPoint= params.points;

	for (i=0; i<numberOfPolys; i++)
	{
		MeshQuad(&roomPolys[i]);
	}

	params.displayView.buffer= (unsigned long*)calloc(params.displayView.xRes*params.displayView.yRes, sizeof(unsigned long));
	params.displayView.wid=0;
}

void glhFrustumf2(float *matrix, float left, float right, float bottom, float top,
				  float znear, float zfar)
{
	float temp, temp2, temp3, temp4;
	temp = 2.0f * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	matrix[0] = temp / temp2;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = temp / temp3;
	matrix[6] = 0.0;
	matrix[7] = 0.0;
	matrix[8] = (right + left) / temp2;
	matrix[9] = (top + bottom) / temp3;
	matrix[10] = (-zfar - znear) / temp4;
	matrix[11] = -1.0f;
	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = (-temp * zfar) / temp4;
	matrix[15] = 0.0;
}

void glhPerspectivef2(float *matrix, float fovyInDegrees, float aspectRatio,
					  float znear, float zfar)
{
	float ymax, xmax;
	ymax = znear * tanf((float)(fovyInDegrees * (float)M_PI / 360.0));
	xmax = ymax * aspectRatio;
	glhFrustumf2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}
void reshape(GLsizei width, GLsizei height)    // GLsizei for non-negative integer
{
	// Compute aspect ratio of the new window
	if (height == 0)
	{
		height = 1;    // To prevent divide by 0
	}

	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset
	// Enable perspective projection with fovy, aspect, zNear and zFar
	float matrix[16];
	glhPerspectivef2(matrix, 45.0f, aspect, 0.1f, 100.0f);
	glLoadMatrixf(matrix);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix

	// Render a color-cube consisting of 6 quads with different colors
	glLoadIdentity();                 // Reset the model-view matrix
}

void displayOld()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix

	// Render a color-cube consisting of 6 quads with different colors
	glLoadIdentity();                 // Reset the model-view matrix
	glTranslatef(1.5f, 0.0f, -7.0f);  // Move right and into the screen

	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
	// Top face (y = 1.0f)
	// Define vertices in counter-clockwise (CCW) order with normal pointing out
	glColor3f(0.0f, 1.0f, 0.0f);     // Green
	glVertex3f( 1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f,  1.0f);
	glVertex3f( 1.0f, 1.0f,  1.0f);

	// Bottom face (y = -1.0f)
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	glVertex3f( 1.0f, -1.0f,  1.0f);
	glVertex3f(-1.0f, -1.0f,  1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f( 1.0f, -1.0f, -1.0f);

	// Front face  (z = 1.0f)
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f( 1.0f,  1.0f, 1.0f);
	glVertex3f(-1.0f,  1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f( 1.0f, -1.0f, 1.0f);

	// Back face (z = -1.0f)
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
	glVertex3f( 1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f,  1.0f, -1.0f);
	glVertex3f( 1.0f,  1.0f, -1.0f);

	// Left face (x = -1.0f)
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(-1.0f,  1.0f,  1.0f);
	glVertex3f(-1.0f,  1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f,  1.0f);

	// Right face (x = 1.0f)
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
	glVertex3f(1.0f,  1.0f, -1.0f);
	glVertex3f(1.0f,  1.0f,  1.0f);
	glVertex3f(1.0f, -1.0f,  1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();  // End of drawing color-cube

	glutSwapBuffers();
}

int _frame;

void IdleFunc()
{
	printf("Frame %d\n", _frame++);

	DoRad();
}

void KeyboardFunc(unsigned char key, int x, int y)
{
	printf("wrote %c at %d %d\n", key, x, y);
}

void MouseFunc(int button, int state, int x, int y)
{
	printf("mouse button: %d %d %d %d\n", button, state, x, y);
}

void PassiveMouseMove(int x, int y)
{
	//printf("Mose move: %d %d\n", x, y);
}

int main(int argc, char **argv)
{
	g_argc = argc;
	g_argv = argv;

	InitParams();
	InitRad(&params);

	/*
	for (unsigned int n = 0; n < params.nPatches; ++n)
	{
		TPatch *p = &params.patches[n];
		for (int m = 0; m < 3; ++m)
		{
			double *s = p->emission->samples;

			printf("%d: %f %f %f\n", (int)n, s[0], s[1], s[2]);
		}
	}
	 */

	printf("We have %d patches and %d elements\n", params.nPatches, params.nElements);

	glutMainLoop();
	//CleanUpRad();

	return 0;
}



