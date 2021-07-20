#include "TetraGLCanvas.h"
#include "TetraCSGPrimitive.h"

//////////////////////////////////////////////////////////
// TetraGLCanvas class member function definitions begin

TetraGLCanvas::TetraGLCanvas (wxWindow *parent, Tetrahedron<REAL> *t):
		wxGLCanvas (parent, -1,wxDefaultPosition,wxDefaultSize,WX_GL_DOUBLEBUFFER)
{
	tetra = t;
	isInitCanvas = false;
	zoom = 1.0f;
	axis = 0;
	dlist = 0;
}

void TetraGLCanvas::Render ()
{
	int w, h;
	GLfloat ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
	bool lighting = false;
	
	wxPaintDC dc(this);	
	if (!GetContext()) return;

	GetClientSize (&w, &h);
	
	if (!isInitCanvas)
	{
		InitGL ();
		isInitCanvas = true;
	}

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix ();
	glLoadIdentity ();

	glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambient);
	glLightfv (GL_LIGHT1, GL_AMBIENT, ambient);

	glPopMatrix ();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glViewport((1-zoom)*w/2, (1-zoom)*h/2, zoom*w, zoom*h);
	if (dlist != 0)
		glCallList (dlist);

	/* render axis */
	glViewport (0, 0, 100, 100);
	glEnable (GL_POINT_SMOOTH);
	glPointSize (3.0);
	if (glIsEnabled (GL_LIGHTING))
	{
		glDisable (GL_LIGHTING);
		lighting = true;
	}
	if (axis != 0)
		glCallList (axis);
	if (lighting) glEnable (GL_LIGHTING);
	glPointSize (1.0);
	glDisable (GL_POINT_SMOOTH);
	glViewport((1-zoom)*w/2, (1-zoom)*h/2, zoom*w, zoom*h);

	glFlush();
	SwapBuffers();
}

void TetraGLCanvas::InitGL()
{
	int w,h;

	GLfloat mat_specular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat mat_shininess[] = { 50.0f };
	
	glShadeModel (GL_SMOOTH);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	GetClientSize(&w,&h);
	glViewport((1-zoom)*w/2, (1-zoom)*h/2, zoom*w, zoom*h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho (-2.0f, 2.0f, -2.0f*(GLfloat)h/(GLfloat)w,
					2.0f*(GLfloat)h/(GLfloat)w, -2.0f, 2.0f);
	else
		glOrtho (-2.0f*(GLfloat)w/(GLfloat)h, 2.0f*(GLfloat)w/(GLfloat)h,
					-2.0f, 2.0f, -2.0f, 2.0f);

	glMatrixMode(GL_MODELVIEW);
	//glTranslatef(0.0F, 0.0F, -5.0f);

	glRotatef(30.0F, 1.0F, 0.0F, 0.0F);
	glRotatef(30.0F, 0.0F, 1.0F, 0.0F);

	glDepthRange (-1.0f, 1.0f);

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState (GL_VERTEX_ARRAY);
	glEnableClientState (GL_NORMAL_ARRAY);
	glEnable(GL_DEPTH_TEST);
	glEnable (GL_LIGHT0);
	glEnable (GL_LIGHT1);
	glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	glDisable (GL_LIGHTING);
	axis = glGenLists (1);
	glNewList (axis, GL_COMPILE_AND_EXECUTE);
	
	glBegin (GL_LINES);
		glColor3f (1.0f, 1.0f, 1.0f);
		/* X Axis */
		glVertex3f (0.0f, 0.0f, 0.0f);
		glVertex3f (1.5f, 0.0f, 0.0f);
		/* Y Axis */
		glVertex3f (0.0f, 0.0f, 0.0f);
		glVertex3f (0.0f, 1.5f, 0.0f);
		/* Z Axis */
		glVertex3f (0.0f, 0.0f, 0.0f);
		glVertex3f (0.0f, 0.0f, 1.5f);
	glEnd ();

	glBegin (GL_POINTS);
		glColor3f (0.0f, 1.0f, 0.0f);
		glVertex3f (1.5f, 0.0f, 0.0f);
		glVertex3f (0.0f, 1.5f, 0.0f);
		glVertex3f (0.0f, 0.0f, 1.5f);
	glEnd ();
	
	glEndList ();
}

void TetraGLCanvas::SetTetra (Tetrahedron<REAL>* t)
{
	tetra = t;
}

void TetraGLCanvas::OnPaint(wxPaintEvent& evt)
{
	Render();
}

void TetraGLCanvas::OnSize(wxSizeEvent& event)
{
	int w, h;
	wxGLCanvas::OnSize(event);

	GetClientSize(&w, &h);
	if (GetContext())
	{
		glViewport((1-zoom)*w/2, (1-zoom)*h/2, (1+zoom)*w/2, (1+zoom) *h/2);

		glMatrixMode (GL_PROJECTION);	
		glLoadIdentity ();
		if (w <= h)
			glOrtho (-2.0, 2.0, -2.0*(GLfloat)h/(GLfloat)w,
						2.0*(GLfloat)h/(GLfloat)w, -2.0, 2.0);
		else
			glOrtho (-2.0*(GLfloat)w/(GLfloat)h, 2.0*(GLfloat)w/(GLfloat)h,
						-2.0, 2.0, -2.0, 2.0);
	}
	SwapBuffers ();
	Refresh (FALSE);
}

void TetraGLCanvas::OnEraseBackground(wxEraseEvent& event)
{
  // Do nothing, to avoid flashing.
}

void TetraGLCanvas::OnMouseEvent(wxMouseEvent& evt)
{
	long int x = evt.m_x;
	long int y = evt.m_y;
	int w,h;
	GetClientSize(&w,&h);
	if(evt.Entering())
	{
	}
	else if(evt.Leaving())
	{
		prevMouseX=0;
		prevMouseY=0;
		return;
	}
	else if((evt.LeftIsDown() || evt.RightIsDown()) && 
			prevMouseX!=0 && prevMouseY!=0)
	{
		float thetaX = (float)(x-prevMouseX)*(360.0f/(float)w);
		float thetaY = (float)(prevMouseY-y)*(360.0f/(float)h);

		if ( (thetaX != 0.0f) || (thetaY != 0.0f) )
		{

			SetCurrent();		
			glMatrixMode(GL_MODELVIEW);
			glRotatef((GLfloat)thetaX,0.0F,1.0F,0.0F);
			
			if(evt.LeftIsDown())
			{
				glRotatef(-(GLfloat)thetaY,1.0f,0.0f,0.0f);
			}	
			else if(evt.RightIsDown())
			{
				glRotatef((GLfloat)thetaY,0.0F,0.0F,1.0F);
			}
			Refresh(FALSE);
			prevMouseX = x;
			prevMouseY = y;
		}			
	}
	prevMouseX = x;
	prevMouseY = y;
}

void TetraGLCanvas::ZoomIn ()
{
	if (zoom < 4.0f)
		zoom += 0.20f;
	SwapBuffers ();

	Refresh (FALSE);
}

void TetraGLCanvas::ZoomOut ()
{
	if (zoom > 0.4f)
		zoom -= 0.20f;
	SwapBuffers ();
	
	Refresh (FALSE);
}

void TetraGLCanvas::FitInWindow ()
{
	zoom = 1.0f;
	SwapBuffers ();

	Refresh (FALSE);
}

void TetraGLCanvas::GetDimensionalWidth (REAL& xwidth, REAL& ywidth, REAL& zwidth)
{
	REAL xmax = 0, ymax = 0, zmax = 0, xmin = 0, ymin = 0, zmin = 0;
	int w, h;
	GetClientSize(&w, &h);
	for (int i = 1; i <= tetra->nNodes; i++)
	{
		if (tetra->Nodes[i].x > xmax)
			xmax = tetra->Nodes[i].x;
		if (tetra->Nodes[i].y > ymax)
			ymax = tetra->Nodes[i].y;
		if (tetra->Nodes[i].z > zmax)
			zmax = tetra->Nodes[i].z;

		if (tetra->Nodes[i].x < xmin)
			xmin = tetra->Nodes[i].x;
		if (tetra->Nodes[i].y < ymin)
			ymin = tetra->Nodes[i].y;
		if (tetra->Nodes[i].z < zmin)
			zmin = tetra->Nodes[i].z;
	}
	
	xwidth = xmax - xmin;
	ywidth = ymax - ymin;
	zwidth = zmax - zmin;
	xwidth = (w/h) * ywidth;
	xwidth *= 2;ywidth *= 2;zwidth *= 2;
}

void TetraGLCanvas::PlotModel (TetraCSGModel *model)
{
	SetCurrent ();
	dlist = glGenLists (1);
	
	glEnable (GL_LIGHTING);

	glNewList (dlist, GL_COMPILE_AND_EXECUTE);
		model->Render ();
	glEndList ();
	
	SwapBuffers ();
	Refresh (FALSE);
}

void TetraGLCanvas::PlotNodes ()
{
	REAL xwidth = 0, ywidth = 0, zwidth = 0;
	
	SetCurrent ();
	glDisable (GL_LIGHTING);
	GetDimensionalWidth (xwidth, ywidth, zwidth);

	dlist = glGenLists (1);
	glNewList (dlist, GL_COMPILE_AND_EXECUTE);
		glBegin (GL_POINTS);
			glColor3f (1.0f, 1.0f, 1.0f);
			for (int i = 1; i <= tetra->nNodes; i++)
			{
				glVertex3f ((xwidth == 0)?0:(tetra->Nodes[i].x / xwidth),
						(ywidth == 0)?0:(tetra->Nodes[i].y / ywidth),
						(zwidth == 0)?0:(tetra->Nodes[i].z / zwidth));
			}
		glEnd ();
	glEndList ();
	
	SwapBuffers ();
	Refresh (FALSE);
}

void TetraGLCanvas::PlotForces(REAL xwidth, REAL ywidth, REAL zwidth)
{
	int index, dir;
	REAL x1, y1, z1, x2, y2, z2, ax1, ax2, ay1, ay2, az1, az2, sign = 1.0;

	glDisable (GL_LIGHTING);

	for (unsigned int i = 0; i < tetra->Loads.size(); i++)
	{
		index = tetra->Loads[i].index / tetra->nDOFPerNode + 1;
		dir = tetra->Loads[i].index % tetra->nDOFPerNode;

		x1 = tetra->Nodes[index].x / xwidth;
		y1 = tetra->Nodes[index].y / ywidth;
		z1 = tetra->Nodes[index].z / zwidth;
		sign = (tetra->Loads[i].value > 0) ? (-1.0) : (1.0);

		switch (dir)
		{
		case 0: /* force acting in x direction */
			x2 = (tetra->Nodes[index].x / xwidth) + (sign*0.05);
			y2 = tetra->Nodes[index].y / ywidth;
			z2 = tetra->Nodes[index].z / zwidth;

			ax1 = x1 + (sign*0.01);
			ay1 = y1 + 0.01;
			az1 = z1;
			ax2 = ax1;
			ay2 = y1 - 0.01;
			az2 = z1;
			break;
		case 1: /* force acting in y direction */
			x2 = tetra->Nodes[index].x / xwidth;
			y2 = (tetra->Nodes[index].y / ywidth) + (sign*0.05);
			z2 = tetra->Nodes[index].z / zwidth;

			ax1 = x1 + 0.01;
			ay1 = y1 + (sign*0.01);
			az1 = z1;
			ax2 = x1 - 0.01;
			ay2 = y1 + (sign*0.01);
			az2 = z1;
			break;
		case 2: /* force acting in z direction */
			x2 = tetra->Nodes[index].x / xwidth;
			y2 = tetra->Nodes[index].y / ywidth;
			z2 = (tetra->Nodes[index].z / zwidth) + (sign*0.05);

			ax1 = x1 + 0.01;
			ay1 = y1;
			az1 = z1 + (sign*0.01);
			ax2 = x1 - 0.01;
			ay2 = y1;
			az2 = z1 + (sign*0.01);
			break;
		default:
			break;
		}

		glBegin (GL_LINE_LOOP);
			glColor3f (0.0f, 0.0f, 1.0f);
			glVertex3f (x2, y2, z2);
			glVertex3f (x1, y1, z1);
			glVertex3f (ax1, ay1, az1);
			glVertex3f (ax2, ay2, az2);
		glEnd ();
	}
	Refresh (FALSE);
}

void TetraGLCanvas::PlotElements ()
{
	REAL xwidth = 0, ywidth = 0, zwidth = 0;
	int temp;

	SetCurrent ();
	glDisable (GL_LIGHTING);
	GetDimensionalWidth (xwidth, ywidth, zwidth);
	
	dlist = glGenLists (1);
	glNewList (dlist, GL_COMPILE_AND_EXECUTE);
		PlotForces (xwidth, ywidth, zwidth);
		for (int i = 1; i <= tetra->nElements; i++)
		{
/*			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glBegin (GL_TRIANGLES);
				glColor3f (0.8f, 0.8f, 0.8f);
				for (int j = 0; j < tetra->nNodesPerElement; j++)
				{
					temp = tetra->Elements[i].Nodes[j];
					glVertex3f (tetra->Nodes[temp].x / xwidth,
							tetra->Nodes[temp].y / ywidth,
							tetra->Nodes[temp].z / zwidth);

					temp = tetra->Elements[i].Nodes[(j+1) % 
								tetra->nNodesPerElement];
					glVertex3f (tetra->Nodes[temp].x / xwidth,
							tetra->Nodes[temp].y / ywidth,
							tetra->Nodes[temp].z / zwidth);

					temp = tetra->Elements[i].Nodes[(j+2) % 
								tetra->nNodesPerElement];
					glVertex3f (tetra->Nodes[temp].x / xwidth,
							tetra->Nodes[temp].y / ywidth,
							tetra->Nodes[temp].z / zwidth);
				}
			glEnd ();*/

			glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
			glBegin (GL_TRIANGLES);
				glColor3f (1.0f, 1.0f, 1.0f);
				for (int j = 0; j < tetra->nNodesPerElement; j++)
				{
					temp = tetra->Elements[i].Nodes[j];
					glVertex3f (tetra->Nodes[temp].x / xwidth,
							tetra->Nodes[temp].y / ywidth,
							tetra->Nodes[temp].z / zwidth);

					temp = tetra->Elements[i].Nodes[(j+1) % 
								tetra->nNodesPerElement];
					glVertex3f (tetra->Nodes[temp].x / xwidth,
							tetra->Nodes[temp].y / ywidth,
							tetra->Nodes[temp].z / zwidth);

					temp = tetra->Elements[i].Nodes[(j+2) % 
								tetra->nNodesPerElement];
					glVertex3f (tetra->Nodes[temp].x / xwidth,
							tetra->Nodes[temp].y / ywidth,
							tetra->Nodes[temp].z / zwidth);
				}
			glEnd ();
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		}
	glEndList ();
	
	SwapBuffers ();
	Refresh (FALSE);
}

void TetraGLCanvas::PlotDisplacements (int DOF)
{
	REAL min = 0, max = 0, xwidth = 0, ywidth = 0, zwidth = 0;
	int temp;
	
	SetCurrent ();
	glDisable (GL_LIGHTING);
	GetDimensionalWidth (xwidth, ywidth, zwidth);
	
	for (int i = DOF; i < tetra->nDOF; i += tetra->nDOFPerNode)
	{
		if (min > tetra->Forces[i])
			min = tetra->Forces[i];
		if (max < tetra->Forces[i])
			max = tetra->Forces[i];
	}
	
	dlist = glGenLists (1);
	glNewList (dlist, GL_COMPILE_AND_EXECUTE);
		for (int i = 1; i <= tetra->nElements; i++)
		{
			glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
			glBegin (GL_TRIANGLES);
				for (int j = 0; j < tetra->nNodesPerElement; j++)
				{
					temp = tetra->Elements[i].Nodes[j];
					
					glColor3f (tetra->Forces[(temp-1)*tetra->nDOFPerNode + DOF]/max, 0.5f, 0.5f);
					glVertex3f (tetra->Nodes[temp].x / xwidth,
								tetra->Nodes[temp].y / ywidth,
								tetra->Nodes[temp].z / zwidth);

					temp = tetra->Elements[i].Nodes[(j+1)%tetra->nNodesPerElement];

					glColor3f (tetra->Forces[(temp-1)*tetra->nDOFPerNode + DOF]/max, 0.5f, 0.5f);
					glVertex3f (tetra->Nodes[temp].x / xwidth,
								tetra->Nodes[temp].y / ywidth,
								tetra->Nodes[temp].z / zwidth);
					
					temp = tetra->Elements[i].Nodes[(j+2)%tetra->nNodesPerElement];

					glColor3f (tetra->Forces[(temp-1)*tetra->nDOFPerNode + DOF]/max, 0.5f, 0.5f);
					glVertex3f (tetra->Nodes[temp].x / xwidth,
								tetra->Nodes[temp].y / ywidth,
								tetra->Nodes[temp].z / zwidth);
				}
			glEnd ();
		}
	glEndList ();
	SwapBuffers ();
	Refresh (FALSE);
}

void TetraGLCanvas::PlotStresses (int type)
{	
	if (!tetra->isSolved) return;
	
	REAL xmax = 0, ymax = 0, zmax = 0, xwidth = 0, ywidth = 0, zwidth = 0;
	REAL maxstr = 0.0;
	int temp;
	
	SetCurrent ();
	glDisable (GL_LIGHTING);
	GetDimensionalWidth (xwidth, ywidth, zwidth);
	
	for (int i = 0; i < tetra->nElements; i++)
	{
		if (xmax < (*(tetra->Stresses))(i,3*type))
			maxstr = (*(tetra->Stresses))(i,3*type);
	}
	
	dlist = glGenLists (1);
	glNewList (dlist, GL_COMPILE_AND_EXECUTE);
	for (int i = 1; i <= tetra->nElements; i++)
	{
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		glBegin (GL_TRIANGLES);
			for (int j = 0; j < tetra->nNodesPerElement; j++)
			{
				glColor3d ((*(tetra->Stresses))(i-1,3*type)/maxstr, 0.5, 0.5);

				temp = tetra->Elements[i].Nodes[j];
				glVertex3f (tetra->Nodes[temp].x / xwidth,
							tetra->Nodes[temp].y / ywidth,
							tetra->Nodes[temp].z / zwidth);

				temp = tetra->Elements[i].Nodes[(j+1)%tetra->nNodesPerElement];
				glVertex3f (tetra->Nodes[temp].x / xwidth,
							tetra->Nodes[temp].y / ywidth,
							tetra->Nodes[temp].z / zwidth);
				
				temp = tetra->Elements[i].Nodes[(j+2)%tetra->nNodesPerElement];
				glVertex3f (tetra->Nodes[temp].x / xwidth,
							tetra->Nodes[temp].y / ywidth,
							tetra->Nodes[temp].z / zwidth);
			}
		glEnd ();
	}
	glEndList ();
	SwapBuffers ();
	Refresh (FALSE);
}

BEGIN_EVENT_TABLE(TetraGLCanvas,wxGLCanvas)
	EVT_PAINT(TetraGLCanvas::OnPaint)
	EVT_SIZE(TetraGLCanvas::OnSize)	
	EVT_ERASE_BACKGROUND(TetraGLCanvas::OnEraseBackground)
	EVT_MOUSE_EVENTS(TetraGLCanvas::OnMouseEvent)
END_EVENT_TABLE()

// TetraGLCanvas class member function definitions end
//////////////////////////////////////////////////////////

