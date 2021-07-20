#ifndef __TETRA_GUI_HEADER__
#define __TETRA_GUI_HEADER__

#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <wx/treectrl.h>
#include <wx/splitter.h>
#include <wx/notebook.h>

#include "TetraCSGModel.h"
#include "foldbar/foldpanelbar.h"
#include "TetraGLCanvas.h"
#include "Tetrahedron.h"
#include "AddNodeDialog.h"
#include "AddElementDialog.h"
#include "AddMaterialDialog.h"
#include "AddForceDialog.h"
#include "AddDisplacementDialog.h"
#include "AddMPConstraintDialog.h"
#include "HelpAboutDialog.h"
/*#include "AdaptiveMesher.h"*/

//////////////////////////////////////////////////////////
// TetraGUIApp class declaration begins

class TetraGUIApp : public wxApp
{
public:
	virtual bool OnInit();
};

// TetraGUIApp class declaration ends
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
// TetraGUIFrame class declaration begins

class TetraGUIFrame : public wxFrame
{
	DECLARE_EVENT_TABLE();
	
public:

	/* accessed by wxApp 
	 * csgcanvas is where csg model is rendered
	 * canvas (aka femcanvas) is where finte element meshed csg
	 * model is rendered. canvas is not renamed to be compatible
	 * with old code */
	TetraGLCanvas *canvas;

	TetraGUIFrame (const wxChar* title);
	~TetraGUIFrame ();
	
protected:
	wxLogTextCtrl *logger;
	wxNotebook *notebook;
	wxFoldPanelBar *foldbar;
	wxTreeCtrl *modeltree;
	wxComboBox *opercombo;
	wxFoldPanel mtreepbar, mtoolpbar, mprimpbar, moperpbar,
				mparapbar, msphepbar, mfruspbar, mcylipbar, 
				mfempbar, rectfoldpanel, circfoldpanel;
	wxTextCtrl 	*paraxtext, *paraytext, *paraztext, *parawtext,
			   	*parahtext, *paradtext, *sphextext, *spheytext, 
				*spheztext, *sphertext,	*frusxtext, *frusytext, 
				*frusztext, *frustwtext,*frusbwtext, *frusltext,
				*cylixtext, *cyliytext, *cyliztext, *cylirtext,	
				*cylihtext, *rectxtext, *rectytext, *rectztext,
				*rectwtext, *recthtext, *circxtext, *circytext,
				*circztext, *circrtext, *forcetext;
	TetraCSGModel *csgmodel;
	wxTreeItemId rootnode;
	bool loggerVisible;
	Tetrahedron<REAL> *tetra;
	TetraCSGParallelopiped *paraedit;
	TetraCSGSphere *spheedit;
	TetraCSGFrustum *frusedit;
	TetraCSGCylinder *cyliedit;
	TetraCSGBoolOper *operedit;
	wxToggleButton *selecttoggle;
	wxListBox *selectlist;
	wxChoice *forcedirchoice;

	enum {
		ID_FILE_OPEN_CSG_MODEL = 1000,
		ID_FILE_SAVE_CSG_MODEL,
		ID_FILE_OPEN_FEM_MODEL,
		ID_FILE_SAVE_FEM_MODEL,
		ID_FILE_VIEW_LOGGER,
		ID_FILE_EXIT,
		
		ID_VIEW_REPLOT,
		ID_VIEW_ZOOM_IN,
		ID_VIEW_ZOOM_OUT,
		ID_VIEW_FIT_WINDOW,
		
		ID_ADD_NODE,
		ID_ADD_ELEMENT,
		ID_ADD_MATERIAL,
		ID_ADD_FORCE,
		ID_ADD_DISPLACEMENT,
		ID_ADD_MPCONSTRAINT,
		
		ID_LIST_NODES,
		ID_LIST_ELEMENTS,
		ID_LIST_MATERIALS,
		ID_LIST_FORCES,
		ID_LIST_DISPLACEMENTS,
		ID_LIST_MPCONSTRAINTS,
		
		ID_GENERATE_MESH,
		ID_SOLVE,
		ID_MESH,
		
		ID_PLOT_MODEL,
		ID_PLOT_NODES,
		ID_PLOT_ELEMENTS,
		ID_PLOT_DISPLACEMENTS_X,
		ID_PLOT_DISPLACEMENTS_Y,
		ID_PLOT_DISPLACEMENTS_Z,
		ID_PLOT_STRESSES_NORMAL,
		ID_PLOT_STRESSES_SHEAR,
		ID_PLOT_STRESSES_PRINCIPLE,
		
		ID_HELP_ABOUT,

		ID_MODEL_TREE,

		ID_PRIMITIVE_BUTTON,
		ID_OPERATION_BUTTON,
		ID_FEM_BUTTON,

		ID_PARALLELOPIPED_BUTTON,
		ID_SPHERE_BUTTON,
		ID_FRUSTUM_BUTTON,
		ID_CYLINDER_BUTTON,

		ID_OPER_DONE,
		ID_OPER_CANCEL,

		ID_FEM_DONE,
		ID_FEM_CANCEL,

		ID_RECT_SELECT,
		ID_CIRC_SELECT,
		ID_CLEAR_SELECT,

		ID_PARA_DONE,
		ID_PARA_CANCEL,

		ID_SPHE_DONE,
		ID_SPHE_CANCEL,

		ID_FRUS_DONE,
		ID_FRUS_CANCEL,

		ID_CYLI_DONE,
		ID_CYLI_CANCEL,

		ID_RECT_DONE,
		ID_RECT_CANCEL,

		ID_CIRC_DONE,
		ID_CIRC_CANCEL
	};

	wxMenuBar*	CreateMenuBar ();
	void		RegenerateTree ();

	
	void CreatePrimitivePanel ();
	void CreateOperationPanel ();
	void CreateFEMPanel ();

	void CreateParallelopipedPanel ();
	void CreateSpherePanel ();
	void CreateFrustumPanel ();
	void CreateCylinderPanel ();

	void CreateRectanglePanel ();
	void CreateCirclePanel ();

	void ShowPrimitivePanel (bool show=true);
	void ShowOperationPanel (bool show=true, TetraCSGBoolOper *oper = NULL);
	void ShowFEMPanel (bool show=true);

	void ShowParallelopipedPanel (bool show=true, TetraCSGParallelopiped *para = NULL);
	void ShowSpherePanel (bool show=true, TetraCSGSphere *sphe = NULL);
	void ShowFrustumPanel (bool show=true, TetraCSGFrustum *frus = NULL);
	void ShowCylinderPanel (bool show=true, TetraCSGCylinder *cyli = NULL);

	void ShowRectanglePanel (bool show=true);
	void ShowCirclePanel (bool show=true);

	void OnFileOpenCSGModel (wxCommandEvent&);
	void OnFileSaveCSGModel (wxCommandEvent&);
	void OnFileOpenFEMModel (wxCommandEvent&);
	void OnFileSaveFEMModel (wxCommandEvent&);
	void OnFileViewLogger (wxCommandEvent&);
	void OnFileExit (wxCommandEvent&);
	
	void OnViewReplot (wxCommandEvent&);
	void OnViewZoomIn (wxCommandEvent&);
	void OnViewZoomOut (wxCommandEvent&);
	void OnViewFitWindow (wxCommandEvent&);
	
	void OnAddNode (wxCommandEvent&);
	void OnAddElement (wxCommandEvent&);
	void OnAddMaterial (wxCommandEvent&);
	void OnAddForce (wxCommandEvent&);
	void OnAddDisplacement (wxCommandEvent&);
	void OnAddMPConstraint (wxCommandEvent&);
	
	void OnListNodes (wxCommandEvent&);
	void OnListElements (wxCommandEvent&);
	void OnListMaterials (wxCommandEvent&);
	void OnListForces (wxCommandEvent&);
	void OnListDisplacements (wxCommandEvent&);
	void OnListMPConstraints (wxCommandEvent&);
	
	void OnGenerateMesh (wxCommandEvent&);
	void OnSolve (wxCommandEvent&);
	void OnMesh (wxCommandEvent&);
	
	void OnPlotModel (wxCommandEvent&);
	void OnPlotNodes (wxCommandEvent&);
	void OnPlotElements (wxCommandEvent&);
	void OnPlotDisplacementsX (wxCommandEvent&);
	void OnPlotDisplacementsY (wxCommandEvent&);
	void OnPlotDisplacementsZ (wxCommandEvent&);
	void OnPlotStressesNormal (wxCommandEvent&);
	void OnPlotStressesShear (wxCommandEvent&);
	void OnPlotStressesPrinciple (wxCommandEvent&);
	
	void OnHelpAbout (wxCommandEvent&);

	void OnTreeItemActivated (wxTreeEvent&);

	void OnPrimitiveButton (wxCommandEvent&);
	void OnOperationButton (wxCommandEvent&);
	void OnFEMButton (wxCommandEvent&);

	void OnParallelopipedButton (wxCommandEvent&);
	void OnSphereButton (wxCommandEvent&);
	void OnFrustumButton (wxCommandEvent&);
	void OnCylinderButton (wxCommandEvent&);

	void OnOperDone (wxCommandEvent& ce);
	void OnOperCancel (wxCommandEvent& ce);

	void OnFEMDone (wxCommandEvent& ce);
	void OnFEMCancel (wxCommandEvent& ce);

	void OnRectSelect (wxCommandEvent& ce);
	void OnCircSelect (wxCommandEvent& ce);
	void OnClearSelect (wxCommandEvent& ce);

	void OnParaDone (wxCommandEvent& ce);
	void OnParaCancel (wxCommandEvent& ce);
	
	void OnSpheDone (wxCommandEvent& ce);
	void OnSpheCancel (wxCommandEvent& ce);
	
	void OnFrusDone (wxCommandEvent& ce);
	void OnFrusCancel (wxCommandEvent& ce);
	
	void OnCyliDone (wxCommandEvent& ce);
	void OnCyliCancel (wxCommandEvent& ce);

	void OnRectDone (wxCommandEvent& ce);
	void OnRectCancel (wxCommandEvent& ce);

	void OnCircDone (wxCommandEvent& ce);
	void OnCircCancel (wxCommandEvent& ce);

private:
	
	class CSGTreeItemData : public wxTreeItemData
	{
	public:
		TetraCSGItem *data;
		CSGTreeItemData (TetraCSGItem *data)
		{
			this->data = data;
		};
	};

	/* Used by OnMesh function to show up a dialog box
	with a slider on it to let user choose the quality
	of meshing */
	class MeshDialog : public wxDialog
	{
	public:
		wxSlider *slider;

		MeshDialog (wxWindow*);
		~MeshDialog ();

		void OnOKButton (wxCommandEvent& ce);

	protected:
		DECLARE_EVENT_TABLE ()

	private:
		static const int ID_OK = 10001;
	};
};

// TetraGUIFrame class declaration ends
//////////////////////////////////////////////////////////

#endif /* __TETRA_GUI_HEADER__ */
