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

void BeginDraw(TView *view, unsigned long color)
{
	/* first time this view is drawn */
	if (view->wid==0)
	{
		/* open a new window if you want to display it on the screen */
		/* assign the window id or pointer to view->wid */
		/* the window id cannot be zero */
		/* do all the necessary initialization here too */
	}
	
	/* make view->wid the current window */
	
	/* set up view transformation from the parameters in view */
	
	/* clear the frame buffer with color */
}

void DrawPolygon(int nPts, TPoint3f *pts, TVector3f* n, unsigned long color)
{
//	printf("\ndrawing a poly for %d pts\n%f %f %f,\n%f %f %f,\n%f %f %f\n%f %f %f\ncolor%#x", 
//		nPts,
//		pts[0].x, pts[1].y, pts[2].z,
//		pts[3].x, pts[4].y, pts[5].z,
//		pts[6].x, pts[7].y, pts[8].z, 
//		n->x, n->y, n->z,
//		color);

	/* draw a polygon with the given color */
	/* buffer the polygon if you are drawing in a display list mode */
}

void EndDraw()
{
	/* finish the drawing of all polygons */
	/* display the contents of view->buffer to the current window if necessary */

}


