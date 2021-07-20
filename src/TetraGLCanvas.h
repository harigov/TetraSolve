#ifndef __TETRA_GLCANVAS_HEADER__
#define __TETRA_GLCANVAS_HEADER__

#include "TetraSolve.h"
#include "Tetrahedron.h"
#include <wx/glcanvas.h>
#include <GL/gl.h>
#include "TetraCSGModel.h"

//////////////////////////////////////////////////////////
// TetraGLCanvas class declaration begins

class TetraGLCanvas : public wxGLCanvas
{
	DECLARE_EVENT_TABLE();
	
public:
	
	Tetrahedron<REAL> *tetra;
	bool isInitCanvas;
	long int prevMouseX;
	long int prevMouseY;
	
	GLuint dlist, axis;
	GLfloat zoom;
	
	enum PLOT { 
		NODES_PLOT = 1, ELEMENTS_PLOT, 
		DISP_X_PLOT, DISP_Y_PLOT, DISP_Z_PLOT, 
		STRESS_NOR_PLOT, STRESS_SHR_PLOT, STRESS_PRI_PLOT
	};
	
	PLOT plotwhat;
	
	TetraGLCanvas(wxWindow *parent, Tetrahedron<REAL> *t);
	
	void ZoomIn ();
	void ZoomOut ();
	void FitInWindow ();
	
	void GetDimensionalWidth (REAL& xwidth, REAL& ywidth, REAL&zwidth);

	void PlotModel (TetraCSGModel *csgmodel);	
	void PlotNodes ();
	void PlotForces (REAL xwidth, REAL ywidth, REAL zwidth);
	void PlotElements ();
	void PlotDisplacements (int DOF);
	void PlotStresses (int type);

	void SetTetra (Tetrahedron<REAL>*);

protected:

	void InitGL();
	void Render();
	void OnPaint(wxPaintEvent&);
	void OnSize(wxSizeEvent&);
	void OnEraseBackground(wxEraseEvent& event);
	void OnMouseEvent(wxMouseEvent& event);
};

// TetraGLCanvas class declaration ends
//////////////////////////////////////////////////////////

#endif /* __TETRA_GLCANVAS_HEADER__ */
