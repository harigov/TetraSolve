#include "TetraGUI.h"
#include "TetgenInterface.h"
#include "TetraCSGTree.h"
#include "TetraCSGPrimitive.h"

#include <cmath>

wxString appName = wxT("TetraSolve");

//////////////////////////////////////////////////////////
// TetraGUIApp class member function definitions begin

IMPLEMENT_APP(TetraGUIApp);

bool TetraGUIApp::OnInit ()
{
	TetraGUIFrame *mainframe = new TetraGUIFrame (appName);
	
	mainframe->CreateStatusBar ();
	mainframe->Show (TRUE);
	mainframe->canvas->SetCurrent ();
	SetTopWindow (mainframe);
	
	return true;
}

// TetraGUIApp class member function definitions end
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// TetraGUIFrame class member function definitions begin

TetraGUIFrame::TetraGUIFrame (const wxChar *appName) :
				wxFrame (NULL, -1, appName, wxDefaultPosition, 
					wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxMAXIMIZE),
					mtreepbar(NULL), mtoolpbar(NULL), mprimpbar(NULL), 
					moperpbar(NULL), mparapbar(NULL), msphepbar(NULL),
					mfruspbar(NULL), mcylipbar(NULL), mfempbar(NULL),
					rectfoldpanel(NULL), circfoldpanel(NULL)
{
	wxButton *prim_button, *fem_button;
	wxSplitterWindow *splitter, *logsplitter;
	wxTextCtrl* logtext;
	wxMenuBar *menubar;
	
   	menubar = CreateMenuBar ();
	SetMenuBar (menubar);
	
	tetra = new Tetrahedron<REAL> (4, 3, 3, 3);
	
	splitter = new wxSplitterWindow (this, wxID_ANY);
	foldbar = new wxFoldPanelBar (splitter, wxID_ANY, wxDefaultPosition, 
					wxDefaultSize, wxTAB_TRAVERSAL);
	logsplitter = new wxSplitterWindow (splitter, wxID_ANY);
	
	logtext = new wxTextCtrl (logsplitter, wxID_ANY, wxT(""), wxDefaultPosition, 
					wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
	logger = new wxLogTextCtrl (logtext);
	wxLog::SetActiveTarget (logger);
	logger->SetTimestamp (NULL);

	csgmodel = new TetraCSGModel ();
	
	canvas = new TetraGLCanvas (logsplitter, tetra);
	
	mtreepbar = foldbar->AddFoldPanel (wxT("Model Tree"));
	modeltree = new wxTreeCtrl (mtreepbar.GetParent(), ID_MODEL_TREE);
	rootnode = modeltree->AddRoot (wxT("Model"));

	foldbar->AddFoldPanelWindow (mtreepbar, modeltree, wxFPB_ALIGN_WIDTH, 1, 5, 5);
	mtoolpbar = foldbar->AddFoldPanel (wxT("Tools"));
	prim_button = new wxButton (mtoolpbar.GetParent(), 
					ID_PRIMITIVE_BUTTON, wxT("Primitives"), wxDefaultPosition, 
					wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	fem_button = new wxButton (mtoolpbar.GetParent(),
					ID_FEM_BUTTON, wxT("FEM"), wxDefaultPosition,
					wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	foldbar->AddFoldPanelWindow (mtoolpbar, prim_button, wxFPB_ALIGN_WIDTH, 1, 5, 5);
	foldbar->AddFoldPanelWindow (mtoolpbar, fem_button, wxFPB_ALIGN_WIDTH, 1, 5, 5);

	CreatePrimitivePanel ();
	CreateOperationPanel ();
	CreateParallelopipedPanel ();
	CreateSpherePanel ();
	CreateFrustumPanel ();
	CreateCylinderPanel ();
	CreateFEMPanel ();
	CreateRectanglePanel ();
	CreateCirclePanel ();
	
	logsplitter->SplitHorizontally (canvas, logtext, -75);
	splitter->SplitVertically (foldbar, logsplitter, 160);

	paraedit = NULL;
	spheedit = NULL;
	frusedit = NULL;
	cyliedit = NULL;
}

TetraGUIFrame::~TetraGUIFrame ()
{
}

wxMenuBar*
TetraGUIFrame::CreateMenuBar ()
{
	wxMenuBar *menubar = new wxMenuBar ();
	wxMenu *filemenu = new wxMenu ();
	wxMenu *viewmenu = new wxMenu ();
	wxMenu *addmenu = new wxMenu ();
	wxMenu *listmenu = new wxMenu ();
	wxMenu *solvemenu = new wxMenu ();
	wxMenu *plotmenu = new wxMenu ();
	wxMenu *dispmenu = new wxMenu ();
	wxMenu *stressmenu = new wxMenu ();
	wxMenu *helpmenu = new wxMenu ();
	
	filemenu->Append (ID_FILE_OPEN_CSG_MODEL, wxT("Open CSG Model..."),
							wxT("Open CSG Model from a file"));
	filemenu->Append (ID_FILE_SAVE_CSG_MODEL, wxT("Save CSG Model..."),
							wxT("Save CSG Model to a file"));
	filemenu->AppendSeparator ();
	filemenu->Append (ID_FILE_OPEN_FEM_MODEL, wxT("Open FE Model..."),
							wxT("Open Finite Element Model from a file"));
	filemenu->Append (ID_FILE_SAVE_FEM_MODEL, wxT("Save FE Model..."),
							wxT("Save Finite Element Model to a file"));
	filemenu->AppendSeparator ();
	filemenu->Append (ID_FILE_VIEW_LOGGER, wxT("View Logger"),
							wxT("Select to view logger"), wxITEM_CHECK);
	filemenu->AppendSeparator ();
	filemenu->Append (ID_FILE_EXIT, wxT("E&xit"), wxT("Quit this application"));
	menubar->Append (filemenu, wxT("&File"));
	
	viewmenu->Append (ID_VIEW_REPLOT, wxT("Replot"), wxT("Replot the display"));
	viewmenu->AppendSeparator ();
	viewmenu->Append (ID_VIEW_ZOOM_IN, wxT("Zoom In"), wxT("Zoom into the display"));
	viewmenu->Append (ID_VIEW_ZOOM_OUT, wxT("Zoom Out"), wxT("Zoom out of the display"));
	viewmenu->AppendSeparator ();
	viewmenu->Append (ID_VIEW_FIT_WINDOW, wxT("Fit in window"), wxT("Fit all elements in window"));
	menubar->Append (viewmenu, wxT("&View"));
	
	addmenu->Append (ID_ADD_NODE, wxT("&Node..."), wxT("Add a node"));
	addmenu->Append (ID_ADD_ELEMENT, wxT("&Element..."), wxT("Add an element"));
	addmenu->Append (ID_ADD_MATERIAL, wxT("&Material..."), wxT("Add a material"));
	addmenu->Append (ID_ADD_FORCE, wxT("&Force..."), wxT("Add force/load acting at a dof"));
	addmenu->Append (ID_ADD_DISPLACEMENT, wxT("&Displacement..."), 
							wxT("Add a displacement boundary condition"));
	addmenu->Append (ID_ADD_MPCONSTRAINT, wxT("&MP Constraint..."), wxT("Add a Multipoint Constraint"));
	menubar->Append (addmenu, wxT("&Add"));
	
	listmenu->Append (ID_LIST_NODES, wxT("&Nodes"), wxT("Lists all nodes"));
	listmenu->Append (ID_LIST_ELEMENTS, wxT("&Elements"), wxT("Lists all elements"));
	listmenu->Append (ID_LIST_MATERIALS, wxT("&Materials"), wxT("Lists all materials"));
	listmenu->Append (ID_LIST_FORCES, wxT("&Forces"), wxT("Lists all forces"));
	listmenu->Append (ID_LIST_DISPLACEMENTS, wxT("&Displacements"), wxT("Lists all displacements"));
	listmenu->Append (ID_LIST_MPCONSTRAINTS, wxT("&MP Constraints"), wxT("Lists all Multipoint Constraints"));
	menubar->Append (listmenu, wxT("&List"));
	
	solvemenu->Append (ID_GENERATE_MESH, wxT("Generate Mesh"), wxT("Generate Mesh from CSG model"));
	solvemenu->Append (ID_SOLVE, wxT("&Solve"), wxT("Solve the problem"));
	solvemenu->Append (ID_MESH, wxT("&Mesh"),wxT("Mesh the component"));
	menubar->Append (solvemenu, wxT("&Solve"));
	
	plotmenu->Append (ID_PLOT_MODEL, wxT("&Model"), wxT("Plot the model"));
	plotmenu->Append (ID_PLOT_NODES, wxT("&Nodes"), wxT("Plot the nodes"));
	plotmenu->Append (ID_PLOT_ELEMENTS, wxT("&Elements"), wxT("Plot the elements"));
	dispmenu->Append (ID_PLOT_DISPLACEMENTS_X, wxT("&X"),
							wxT("Plot the X displacements"));
	dispmenu->Append (ID_PLOT_DISPLACEMENTS_Y, wxT("&Y"),
							wxT("Plot the Y displacements"));
	dispmenu->Append (ID_PLOT_DISPLACEMENTS_Z, wxT("&Z"),
							wxT("Plot the Z displacements"));
	stressmenu->Append (ID_PLOT_STRESSES_NORMAL, wxT("&Normal"), wxT("Plot the normal stresses"));
	stressmenu->Append (ID_PLOT_STRESSES_SHEAR, wxT("&Shear"), wxT("Plot the shear stresses"));
	stressmenu->Append (ID_PLOT_STRESSES_PRINCIPLE, wxT("&Principle"), wxT("Plot the principle stresses"));
	plotmenu->Append (1986, wxT("Displacements"), dispmenu);
	plotmenu->Append (1987, wxT("Stress"), stressmenu);
	menubar->Append (plotmenu, wxT("&Plot"));
	
	helpmenu->Append (ID_HELP_ABOUT, wxT("About..."), wxT("About this application"));
	menubar->Append (helpmenu, wxT("&Help"));
	
	filemenu->Check (ID_FILE_VIEW_LOGGER, true);
	return menubar;
}

void
TetraGUIFrame :: RegenerateTree ()
{
	TetraCSGItem *item = csgmodel->GetTree()->root;
	wxTreeItemId rnode = rootnode;
	modeltree->DeleteChildren (rootnode);
	while (item != NULL)
	{
		rnode = modeltree->AppendItem (rnode, item->ToString (), -1, -1, new CSGTreeItemData (item));

		if (item->GetType() & BOOL_OPER)
		{
			TetraCSGItem *right_item = ((TetraCSGBoolOper*)item)->GetRightChild ();
			modeltree->AppendItem (rnode, right_item->ToString (), -1, -1, new CSGTreeItemData (right_item));
			item = ((TetraCSGBoolOper*)item)->GetLeftChild ();
		}
		else
			break;
	}
	modeltree->Refresh (false);
}

void
TetraGUIFrame :: CreatePrimitivePanel ()
{
	wxButton *parallelopiped_button, *sphere_button, *frustum_button, 
				 *cylinder_button;

	mprimpbar = foldbar->AddFoldPanel (wxT("Primitives"));	
	parallelopiped_button = new wxButton (mprimpbar.GetParent(),
				ID_PARALLELOPIPED_BUTTON, wxT("Parallelopiped"), wxDefaultPosition,
				wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	sphere_button = new wxButton (mprimpbar.GetParent(),
				ID_SPHERE_BUTTON, wxT("Sphere"), wxDefaultPosition,
				wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	frustum_button = new wxButton (mprimpbar.GetParent(),
				ID_FRUSTUM_BUTTON, wxT("Frustum"), wxDefaultPosition,
				wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	cylinder_button = new wxButton (mprimpbar.GetParent(),
				ID_CYLINDER_BUTTON, wxT("Cylinder"), wxDefaultPosition,
				wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	foldbar->AddFoldPanelWindow (mprimpbar, parallelopiped_button, 
				wxFPB_ALIGN_WIDTH, 1, 5, 5);
	foldbar->AddFoldPanelWindow (mprimpbar, sphere_button, 
				wxFPB_ALIGN_WIDTH, 1, 5, 5);
	foldbar->AddFoldPanelWindow (mprimpbar, frustum_button, 
				wxFPB_ALIGN_WIDTH, 1, 5, 5);
	foldbar->AddFoldPanelWindow (mprimpbar, cylinder_button, 
				wxFPB_ALIGN_WIDTH, 1, 5, 5);

	(mprimpbar.GetParent ())->Hide ();
}

void
TetraGUIFrame :: CreateOperationPanel ()
{
	wxButton *done_button, *cancel_button;
	wxString choices[] = {wxT("Union"), wxT("Intersect"), wxT("Subtract")};
	wxFlexGridSizer *gridsizer;
	wxStaticText *operlabel;
	wxPanel *operpanel;

	moperpbar = foldbar->AddFoldPanel (wxT("Operations"));

	operpanel = new wxPanel (moperpbar.GetParent (), wxID_ANY);
	gridsizer = new wxFlexGridSizer (0, 2, 1, 1);

	operlabel = new wxStaticText (operpanel, wxID_ANY, wxT("Operation"));
	opercombo = new wxComboBox (operpanel, wxID_ANY, wxT("Union"), 
					wxDefaultPosition, wxDefaultSize, sizeof choices/sizeof choices[0], 
					choices, wxCB_READONLY|wxCB_DROPDOWN);
	cancel_button = new wxButton (operpanel, ID_OPER_CANCEL, wxT("Cancel"));
	done_button = new wxButton (operpanel, ID_OPER_DONE, wxT("Done"));

	gridsizer->Add (operlabel);
	gridsizer->Add (opercombo);
	gridsizer->AddSpacer (4);
	gridsizer->AddSpacer (4);
	gridsizer->Add (cancel_button);
	gridsizer->Add (done_button);

	foldbar->AddFoldPanelWindow (moperpbar, operpanel, 
					wxFPB_ALIGN_WIDTH, 1, 5, 5);

	operpanel->SetSizer (gridsizer);
	gridsizer->SetSizeHints (operpanel);
	(moperpbar.GetParent ())->Hide ();
}

void
TetraGUIFrame :: CreateFEMPanel ()
{
	wxButton *done_button, *cancel_button, 
		*rectselbutton, *cirselbutton, *clearbutton;
	wxBoxSizer *sizer, *buttonsizer, *selectsizer;
	wxStaticText *listlabel, *forcelabel, *forcedirlabel;
	wxGridSizer *gridsizer;
	wxPanel *panel;
	wxString forcedirs[] = { wxT("X"), wxT("Y"), wxT("Z") };

	mfempbar = foldbar->AddFoldPanel (wxT("FEM"));
	panel = new wxPanel (mfempbar.GetParent (), wxID_ANY);

	sizer = new wxBoxSizer (wxVERTICAL);
	buttonsizer = new wxBoxSizer (wxHORIZONTAL);
	selectsizer = new wxBoxSizer (wxHORIZONTAL);
	gridsizer = new wxGridSizer (0, 2, 1, 1);
	listlabel = new wxStaticText (panel, wxID_ANY, wxT("Select nodes"));
	selectlist = new wxListBox (panel, wxID_ANY);
	selecttoggle = new wxToggleButton (panel, wxID_ANY, wxT("S"),
			wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	rectselbutton = new wxButton (panel, ID_RECT_SELECT, wxT("R"),
			wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	cirselbutton = new wxButton (panel, ID_CIRC_SELECT, wxT("C"),
			wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	clearbutton = new wxButton (panel, ID_CLEAR_SELECT, wxT("CL"),
			wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	forcelabel = new wxStaticText (panel, wxID_ANY, wxT("Force"));
	forcetext = new wxTextCtrl (panel, wxID_ANY);
	forcedirlabel = new wxStaticText (panel, wxID_ANY, wxT("Direction"));
	forcedirchoice = new wxChoice (panel, wxID_ANY, wxDefaultPosition, 
			wxDefaultSize, 3, forcedirs);
	done_button = new wxButton (panel, ID_FEM_DONE, wxT("Done"),
			wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);
	cancel_button = new wxButton (panel, ID_FEM_CANCEL, wxT("Cancel"),
			wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT|wxNO_BORDER);

	selectsizer->Add (selecttoggle);
	selectsizer->Add (rectselbutton);
	selectsizer->Add (cirselbutton);
	selectsizer->Add (clearbutton);

	gridsizer->Add (forcelabel);
	gridsizer->Add (forcetext);
	gridsizer->Add (forcedirlabel);
	gridsizer->Add (forcedirchoice);

	buttonsizer->Add (cancel_button);
	buttonsizer->Add (done_button);
	
	sizer->Add (listlabel);
	sizer->Add (selectlist);
	sizer->Add (selectsizer);
	sizer->Add (gridsizer);
	sizer->AddSpacer (4);
	sizer->Add (buttonsizer);

	foldbar->AddFoldPanelWindow (mfempbar, panel, 
		wxFPB_ALIGN_WIDTH, 1, 5, 5);

	panel->SetSizer (sizer);
	sizer->SetSizeHints (panel);

	(mfempbar.GetParent ())->Hide ();
}

void
TetraGUIFrame :: CreateRectanglePanel ()
{
	wxFlexGridSizer *gridsizer;
	wxStaticText *xlabel, *ylabel, *zlabel,
		*wlabel, *hlabel;
	wxButton *done_button, *cancel_button;
	wxPanel *panel;

	rectfoldpanel = foldbar->AddFoldPanel (wxT("Rectangle"));
	gridsizer = new wxFlexGridSizer (0, 2, 1, 1);
	panel = new wxPanel (rectfoldpanel.GetParent (), wxID_ANY);

	xlabel = new wxStaticText (panel, wxID_ANY, wxT("X"));
	ylabel = new wxStaticText (panel, wxID_ANY, wxT("Y"));
	zlabel = new wxStaticText (panel, wxID_ANY, wxT("Z"));
	wlabel = new wxStaticText (panel, wxID_ANY, wxT("Width"));
	hlabel = new wxStaticText (panel, wxID_ANY, wxT("Height"));
	rectxtext = new wxTextCtrl (panel, wxID_ANY);
	rectytext = new wxTextCtrl (panel, wxID_ANY);
	rectztext = new wxTextCtrl (panel, wxID_ANY);
	rectwtext = new wxTextCtrl (panel, wxID_ANY);
	recthtext = new wxTextCtrl (panel, wxID_ANY);
	done_button = new wxButton (panel, ID_RECT_DONE, wxT("Done"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	cancel_button = new wxButton (panel, ID_RECT_CANCEL, wxT("Cancel"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

	gridsizer->Add (xlabel);
	gridsizer->Add (rectxtext);
	gridsizer->Add (ylabel);
	gridsizer->Add (rectytext);
	gridsizer->Add (zlabel);
	gridsizer->Add (rectztext);
	gridsizer->Add (wlabel);
	gridsizer->Add (rectwtext);
	gridsizer->Add (hlabel);
	gridsizer->Add (recthtext);
	gridsizer->AddSpacer (4);
	gridsizer->AddSpacer (4);
	gridsizer->Add (done_button);
	gridsizer->Add (cancel_button);

	panel->SetSizer (gridsizer);
	gridsizer->SetSizeHints (panel);

	foldbar->AddFoldPanelWindow (rectfoldpanel, panel,
				wxFPB_ALIGN_WIDTH, 1, 5, 5);

	(rectfoldpanel.GetParent ())->Hide ();
}

void
TetraGUIFrame :: CreateCirclePanel ()
{
	wxFlexGridSizer *gridsizer;
	wxStaticText *xlabel, *ylabel, *zlabel,
		*rlabel;
	wxButton *done_button, *cancel_button;
	wxPanel *panel;

	circfoldpanel = foldbar->AddFoldPanel (wxT("Circle"));
	gridsizer = new wxFlexGridSizer (0, 2, 1, 1);
	panel = new wxPanel (circfoldpanel.GetParent (), wxID_ANY);

	xlabel = new wxStaticText (panel, wxID_ANY, wxT("X"));
	ylabel = new wxStaticText (panel, wxID_ANY, wxT("Y"));
	zlabel = new wxStaticText (panel, wxID_ANY, wxT("Z"));
	rlabel = new wxStaticText (panel, wxID_ANY, wxT("Radius"));
	circxtext = new wxTextCtrl (panel, wxID_ANY);
	circytext = new wxTextCtrl (panel, wxID_ANY);
	circztext = new wxTextCtrl (panel, wxID_ANY);
	circrtext = new wxTextCtrl (panel, wxID_ANY);
	done_button = new wxButton (panel, ID_CIRC_DONE, wxT("Done"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	cancel_button = new wxButton (panel, ID_CIRC_CANCEL, wxT("Cancel"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

	gridsizer->Add (xlabel);
	gridsizer->Add (circxtext);
	gridsizer->Add (ylabel);
	gridsizer->Add (circytext);
	gridsizer->Add (zlabel);
	gridsizer->Add (circztext);
	gridsizer->Add (rlabel);
	gridsizer->Add (circrtext);
	gridsizer->AddSpacer (4);
	gridsizer->AddSpacer (4);
	gridsizer->Add (done_button);
	gridsizer->Add (cancel_button);

	panel->SetSizer (gridsizer);
	gridsizer->SetSizeHints (panel);

	foldbar->AddFoldPanelWindow (circfoldpanel, panel,
				wxFPB_ALIGN_WIDTH, 1, 5, 5);

	(circfoldpanel.GetParent ())->Hide ();
}

void
TetraGUIFrame :: CreateParallelopipedPanel ()
{
	wxStaticText *paraxlabel, *paraylabel, *parazlabel, *parawlabel,
				 *parahlabel, *paradlabel;
	wxButton	*donebut, *cancelbut;
	wxFlexGridSizer *paragsizer;
	wxPanel 	*parapanel;
	wxSizerFlags *sizerflags;
	
	mparapbar = foldbar->AddFoldPanel (wxT("Parallelopiped"));
	paragsizer = new wxFlexGridSizer (0, 2, 1, 1);
	parapanel = new wxPanel (mparapbar.GetParent(), wxID_ANY);
	sizerflags = new wxSizerFlags ();
	sizerflags->Center ();
	
	paraxlabel = new wxStaticText (parapanel,	wxID_ANY, wxT("X"));
	paraylabel = new wxStaticText (parapanel,	wxID_ANY, wxT("Y"));
	parazlabel = new wxStaticText (parapanel,	wxID_ANY, wxT("Z"));
	parawlabel = new wxStaticText (parapanel,	wxID_ANY, wxT("Width"));
	parahlabel = new wxStaticText (parapanel,	wxID_ANY, wxT("Height"));
	paradlabel = new wxStaticText (parapanel,	wxID_ANY, wxT("Depth"));	
	
	paraxtext = new wxTextCtrl (parapanel, wxID_ANY);
	paraytext = new wxTextCtrl (parapanel, wxID_ANY);
	paraztext = new wxTextCtrl (parapanel, wxID_ANY);
	parawtext = new wxTextCtrl (parapanel, wxID_ANY);
	parahtext = new wxTextCtrl (parapanel, wxID_ANY);
	paradtext = new wxTextCtrl (parapanel, wxID_ANY);

	donebut = new wxButton (parapanel, ID_PARA_DONE, wxT("Done"), 
					wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	cancelbut = new wxButton (parapanel, ID_PARA_CANCEL, wxT("Cancel"), 
					wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

	paragsizer->Add (paraxlabel);
	paragsizer->Add (paraxtext);
	paragsizer->Add (paraylabel);
	paragsizer->Add (paraytext);
	paragsizer->Add (parazlabel);
	paragsizer->Add (paraztext);
	paragsizer->Add (parawlabel);
	paragsizer->Add (parawtext);
	paragsizer->Add (parahlabel);
	paragsizer->Add (parahtext);
	paragsizer->Add (paradlabel);	
	paragsizer->Add (paradtext);
	paragsizer->AddSpacer (4);
	paragsizer->AddSpacer (4);
	paragsizer->Add (cancelbut, *sizerflags);
	paragsizer->Add (donebut, *sizerflags);
		
	parapanel->SetSizer (paragsizer);
	paragsizer->SetSizeHints (parapanel);
	
	foldbar->AddFoldPanelWindow (mparapbar, parapanel, 
				wxFPB_ALIGN_WIDTH, 1, 5, 5);

	(mparapbar.GetParent ())->Hide ();
}

void
TetraGUIFrame :: CreateSpherePanel ()
{
	wxStaticText *sphexlabel, *spheylabel, *sphezlabel, *spherlabel;
	wxButton	*donebut, *cancelbut;
	wxFlexGridSizer *sphegsizer;
	wxSizerFlags *sizerflags;
	wxPanel 	*sphepanel;
	
	msphepbar = foldbar->AddFoldPanel (wxT("Sphere"));
	sphegsizer = new wxFlexGridSizer (0, 2, 1, 1);
	sphepanel = new wxPanel (msphepbar.GetParent(), wxID_ANY);
	sizerflags = new wxSizerFlags ();
	sizerflags->Center ();
	
	sphexlabel = new wxStaticText (sphepanel,	wxID_ANY, wxT("X"));
	spheylabel = new wxStaticText (sphepanel,	wxID_ANY, wxT("Y"));
	sphezlabel = new wxStaticText (sphepanel,	wxID_ANY, wxT("Z"));
	spherlabel = new wxStaticText (sphepanel,	wxID_ANY, wxT("Radius"));
	
	sphextext = new wxTextCtrl (sphepanel, wxID_ANY);
	spheytext = new wxTextCtrl (sphepanel, wxID_ANY);
	spheztext = new wxTextCtrl (sphepanel, wxID_ANY);
	sphertext = new wxTextCtrl (sphepanel, wxID_ANY);

	donebut = new wxButton (sphepanel, ID_SPHE_DONE, wxT("Done"), 
					wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	cancelbut = new wxButton (sphepanel, ID_SPHE_CANCEL, wxT("Cancel"), 
					wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	
	sphegsizer->Add (sphexlabel);
	sphegsizer->Add (sphextext);
	sphegsizer->Add (spheylabel);
	sphegsizer->Add (spheytext);
	sphegsizer->Add (sphezlabel);
	sphegsizer->Add (spheztext);
	sphegsizer->Add (spherlabel);
	sphegsizer->Add (sphertext);
	sphegsizer->AddSpacer (4);
	sphegsizer->AddSpacer (4);
	sphegsizer->Add (cancelbut, *sizerflags);
	sphegsizer->Add (donebut, *sizerflags);
	
	sphepanel->SetSizer (sphegsizer);
	sphegsizer->SetSizeHints (sphepanel);
	
	foldbar->AddFoldPanelWindow (msphepbar, sphepanel, 
				wxFPB_ALIGN_WIDTH, 1, 5, 5);

	(msphepbar.GetParent ())->Hide ();
}

void
TetraGUIFrame :: CreateFrustumPanel ()
{
	wxStaticText *frusxlabel, *frusylabel, *fruszlabel, *frustwlabel,
				 *frusbwlabel, *frusllabel;
	wxButton	*donebut, *cancelbut;
	wxFlexGridSizer *frusgsizer;
	wxSizerFlags *sizerflags;
	wxPanel 	*fruspanel;
	
	mfruspbar = foldbar->AddFoldPanel (wxT("Frustum"));
	frusgsizer = new wxFlexGridSizer (0, 2, 1, 1);
	fruspanel = new wxPanel (mfruspbar.GetParent(), wxID_ANY);
	sizerflags = new wxSizerFlags ();
	sizerflags->Center ();
	
	frusxlabel = new wxStaticText (fruspanel,	wxID_ANY, wxT("X"));
	frusylabel = new wxStaticText (fruspanel,	wxID_ANY, wxT("Y"));
	fruszlabel = new wxStaticText (fruspanel,	wxID_ANY, wxT("Z"));
	frustwlabel = new wxStaticText (fruspanel,	wxID_ANY, wxT("Top Side"));
	frusbwlabel = new wxStaticText (fruspanel,	wxID_ANY, wxT("Bottom Side"));
	frusllabel = new wxStaticText (fruspanel, wxID_ANY, wxT("Length"));
	
	frusxtext = new wxTextCtrl (fruspanel, wxID_ANY);
	frusytext = new wxTextCtrl (fruspanel, wxID_ANY);
	frusztext = new wxTextCtrl (fruspanel, wxID_ANY);
	frustwtext = new wxTextCtrl (fruspanel, wxID_ANY);
	frusbwtext = new wxTextCtrl (fruspanel, wxID_ANY);
	frusltext = new wxTextCtrl (fruspanel, wxID_ANY);
	
	donebut = new wxButton (fruspanel, ID_FRUS_DONE, wxT("Done"), 
					wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	cancelbut = new wxButton (fruspanel, ID_FRUS_CANCEL, wxT("Cancel"), 
					wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

	frusgsizer->Add (frusxlabel);
	frusgsizer->Add (frusxtext);
	frusgsizer->Add (frusylabel);
	frusgsizer->Add (frusytext);
	frusgsizer->Add (fruszlabel);
	frusgsizer->Add (frusztext);
	frusgsizer->Add (frustwlabel);
	frusgsizer->Add (frustwtext);
	frusgsizer->Add (frusbwlabel);
	frusgsizer->Add (frusbwtext);
	frusgsizer->Add (frusllabel);
	frusgsizer->Add (frusltext);
	frusgsizer->AddSpacer (4);
	frusgsizer->AddSpacer (4);
	frusgsizer->Add (cancelbut, *sizerflags);
	frusgsizer->Add (donebut, *sizerflags);
	
	fruspanel->SetSizer (frusgsizer);
	frusgsizer->SetSizeHints (fruspanel);
	
	foldbar->AddFoldPanelWindow (mfruspbar, fruspanel, 
				wxFPB_ALIGN_WIDTH, 1, 5, 5);

	(mfruspbar.GetParent ())->Hide ();
}

void
TetraGUIFrame::CreateCylinderPanel ()
{
	wxStaticText *cylixlabel, *cyliylabel, *cylizlabel, *cylirlabel,
				 *cylihlabel;
	wxButton	*donebut, *cancelbut;
	wxFlexGridSizer *cyligsizer;
	wxSizerFlags *sizerflags;
	wxPanel 	*cylipanel;
	
	mcylipbar = foldbar->AddFoldPanel (wxT("Cylinder"));
	cyligsizer = new wxFlexGridSizer (0, 2, 1, 1);
	cylipanel = new wxPanel (mcylipbar.GetParent(), wxID_ANY);
	sizerflags = new wxSizerFlags ();
	sizerflags->Center ();
		
	cylixlabel = new wxStaticText (cylipanel,	wxID_ANY, wxT("X"));
	cyliylabel = new wxStaticText (cylipanel,	wxID_ANY, wxT("Y"));
	cylizlabel = new wxStaticText (cylipanel,	wxID_ANY, wxT("Z"));
	cylirlabel = new wxStaticText (cylipanel,	wxID_ANY, wxT("Radius"));
	cylihlabel = new wxStaticText (cylipanel,	wxID_ANY, wxT("Height"));
	
	cylixtext = new wxTextCtrl (cylipanel, wxID_ANY);
	cyliytext = new wxTextCtrl (cylipanel, wxID_ANY);
	cyliztext = new wxTextCtrl (cylipanel, wxID_ANY);
	cylirtext = new wxTextCtrl (cylipanel, wxID_ANY);
	cylihtext = new wxTextCtrl (cylipanel, wxID_ANY);

	donebut = new wxButton (cylipanel, ID_CYLI_DONE, wxT("Done"), 
					wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	cancelbut = new wxButton (cylipanel, ID_CYLI_CANCEL, wxT("Cancel"), 
					wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	
	cyligsizer->Add (cylixlabel);
	cyligsizer->Add (cylixtext);
	cyligsizer->Add (cyliylabel);
	cyligsizer->Add (cyliytext);
	cyligsizer->Add (cylizlabel);
	cyligsizer->Add (cyliztext);
	cyligsizer->Add (cylirlabel);
	cyligsizer->Add (cylirtext);
	cyligsizer->Add (cylihlabel);
	cyligsizer->Add (cylihtext);
	cyligsizer->AddSpacer (4);
	cyligsizer->AddSpacer (4);
	cyligsizer->Add (cancelbut, *sizerflags);
	cyligsizer->Add (donebut, *sizerflags);
	
	cylipanel->SetSizer (cyligsizer);
	cyligsizer->SetSizeHints (cylipanel);
		
	foldbar->AddFoldPanelWindow (mcylipbar, cylipanel, 
				wxFPB_ALIGN_WIDTH, 1, 5, 5);

	(mcylipbar.GetParent ())->Hide ();
}

void TetraGUIFrame::ShowPrimitivePanel (bool show)
{

	if (!show)
	{
		ShowParallelopipedPanel (false);
		ShowSpherePanel (false);
		ShowFrustumPanel (false);
		ShowCylinderPanel (false);
	}

	if (show)
	{
		ShowOperationPanel (false);
		ShowFEMPanel (false);
	}

	(mprimpbar.GetParent())->Show (show);
	foldbar->RefreshPanelsFrom (1);
}

void TetraGUIFrame::ShowOperationPanel (bool show, TetraCSGBoolOper *oper)
{
	if (show)
	{
		ShowPrimitivePanel (false);
		ShowFEMPanel (false);
	}
	(moperpbar.GetParent())->Show (show);
	foldbar->RefreshPanelsFrom (1);

	operedit = oper;
}

void TetraGUIFrame::ShowFEMPanel (bool show)
{
	if (show)
	{
		ShowPrimitivePanel (false);
		ShowOperationPanel (false);
	}
	else
	{
		ShowRectanglePanel (false);
		ShowCirclePanel (false);
	}

	(mfempbar.GetParent())->Show (show);
	foldbar->RefreshPanelsFrom (1);
}

void TetraGUIFrame::ShowParallelopipedPanel (bool show, TetraCSGParallelopiped *para)
{
	if (show)
	{
		ShowOperationPanel (false);
		ShowFEMPanel (false);
		ShowSpherePanel (false);
		ShowFrustumPanel (false);
		ShowCylinderPanel (false);
	}
	
	/* if para is not NULL, then this is
	a modify operation, fill out the previous
	values */
	if (para != NULL)
	{
		wxString str;
		str.Printf (wxT("%.2f"), para->GetX());
		paraxtext->SetValue (str);
		str.Printf (wxT("%.2f"), para->GetY());
		paraytext->SetValue (str);
		str.Printf (wxT("%.2f"), para->GetZ());
		paraztext->SetValue (str);
		str.Printf (wxT("%.2f"), para->GetWidth());
		parawtext->SetValue (str);
		str.Printf (wxT("%.2f"), para->GetHeight());
		parahtext->SetValue (str);
		str.Printf (wxT("%.2f"), para->GetDepth());
		paradtext->SetValue (str);
	}
	(mparapbar.GetParent())->Show (show);
	foldbar->RefreshPanelsFrom (2);
	
	paraedit = para;
}

void TetraGUIFrame::ShowSpherePanel (bool show, TetraCSGSphere *sphe)
{
	if (show)
	{
		ShowOperationPanel (false);
		ShowFEMPanel (false);
		ShowParallelopipedPanel (false);
		ShowFrustumPanel (false);
		ShowCylinderPanel (false);
	}
	/* if sphe is not NULL, then this is
	a modify operation, fill out the previous
	values */
	if (sphe != NULL)
	{
		wxString str;
		str.Printf (wxT("%.2f"), sphe->GetX());
		sphextext->SetValue (str);
		str.Printf (wxT("%.2f"), sphe->GetY());
		spheytext->SetValue (str);
		str.Printf (wxT("%.2f"), sphe->GetZ());
		spheztext->SetValue (str);
		str.Printf (wxT("%.2f"), sphe->GetRadius());
		sphertext->SetValue (str);
	}
	(msphepbar.GetParent())->Show (show);
	foldbar->RefreshPanelsFrom (2);
	
	spheedit = sphe;
}

void TetraGUIFrame::ShowFrustumPanel (bool show, TetraCSGFrustum *frus)
{
	if (show)
	{
		ShowOperationPanel (false);
		ShowFEMPanel (false);
		ShowParallelopipedPanel (false);
		ShowSpherePanel (false);
		ShowCylinderPanel (false);
	}
	/* if frus is not NULL, then this is
	a modify operation, fill out the previous
	values */
	if (frus != NULL)
	{
		wxString str;
		str.Printf (wxT("%.2f"), frus->GetX());
		frusxtext->SetValue (str);
		str.Printf (wxT("%.2f"), frus->GetY());
		frusytext->SetValue (str);
		str.Printf (wxT("%.2f"), frus->GetZ());
		frusztext->SetValue (str);
		str.Printf (wxT("%.2f"), frus->GetTopSide());
		frustwtext->SetValue (str);
		str.Printf (wxT("%.2f"), frus->GetBottomSide());
		frusbwtext->SetValue (str);
		str.Printf (wxT("%.2f"), frus->GetLength());
		frusltext->SetValue (str);
	}
	(mfruspbar.GetParent())->Show (show);
	foldbar->RefreshPanelsFrom (2);

	frusedit = frus;
}

void TetraGUIFrame::ShowCylinderPanel (bool show, TetraCSGCylinder *cyli)
{
	if (show)
	{
		ShowOperationPanel (false);
		ShowFEMPanel (false);
		ShowParallelopipedPanel (false);
		ShowSpherePanel (false);
		ShowFrustumPanel (false);
	}
	/* if cyli is not NULL, then this is
	a modify operation, fill out the previous
	values */
	if (cyli != NULL)
	{
		wxString str;
		str.Printf (wxT("%.2f"), cyli->GetX());
		cylixtext->SetValue (str);
		str.Printf (wxT("%.2f"), cyli->GetY());
		cyliytext->SetValue (str);
		str.Printf (wxT("%.2f"), cyli->GetZ());
		cyliztext->SetValue (str);
		str.Printf (wxT("%.2f"), cyli->GetRadius());
		cylirtext->SetValue (str);
		str.Printf (wxT("%.2f"), cyli->GetLength());
		cylihtext->SetValue (str);
	}
	(mcylipbar.GetParent())->Show (show);
	foldbar->RefreshPanelsFrom (2);

	cyliedit = cyli;
}

void TetraGUIFrame::ShowRectanglePanel (bool show)
{
	if (show)
	{
		ShowPrimitivePanel (false);
		ShowOperationPanel (false);
		ShowCirclePanel (false);
	}

	rectxtext->SetValue (wxT(""));
	rectytext->SetValue (wxT(""));
	rectztext->SetValue (wxT(""));
	rectwtext->SetValue (wxT(""));
	recthtext->SetValue (wxT(""));
	(rectfoldpanel.GetParent())->Show (show);
}

void TetraGUIFrame::ShowCirclePanel (bool show)
{
	if (show)
	{
		ShowPrimitivePanel (false);
		ShowOperationPanel (false);
		ShowRectanglePanel (false);
	}

	circxtext->SetValue (wxT(""));
	circytext->SetValue (wxT(""));
	circztext->SetValue (wxT(""));
	circrtext->SetValue (wxT(""));
	(circfoldpanel.GetParent())->Show (show);
}

BEGIN_EVENT_TABLE(TetraGUIFrame,wxFrame)
	EVT_MENU(ID_FILE_OPEN_CSG_MODEL,TetraGUIFrame::OnFileOpenCSGModel)
	EVT_MENU(ID_FILE_SAVE_CSG_MODEL,TetraGUIFrame::OnFileSaveCSGModel)
	EVT_MENU(ID_FILE_OPEN_FEM_MODEL,TetraGUIFrame::OnFileOpenFEMModel)
	EVT_MENU(ID_FILE_SAVE_FEM_MODEL,TetraGUIFrame::OnFileSaveFEMModel)
	EVT_MENU(ID_FILE_VIEW_LOGGER,TetraGUIFrame::OnFileViewLogger)
	EVT_MENU(ID_FILE_EXIT,TetraGUIFrame::OnFileExit)
	
	EVT_MENU(ID_VIEW_REPLOT,TetraGUIFrame::OnViewReplot)
	EVT_MENU(ID_VIEW_ZOOM_IN,TetraGUIFrame::OnViewZoomIn)
	EVT_MENU(ID_VIEW_ZOOM_OUT,TetraGUIFrame::OnViewZoomOut)
	EVT_MENU(ID_VIEW_FIT_WINDOW,TetraGUIFrame::OnViewFitWindow)
	
	EVT_MENU(ID_ADD_NODE,TetraGUIFrame::OnAddNode)
	EVT_MENU(ID_ADD_ELEMENT,TetraGUIFrame::OnAddElement)
	EVT_MENU(ID_ADD_MATERIAL,TetraGUIFrame::OnAddMaterial)
	EVT_MENU(ID_ADD_FORCE,TetraGUIFrame::OnAddForce)
	EVT_MENU(ID_ADD_DISPLACEMENT,TetraGUIFrame::OnAddDisplacement)
	EVT_MENU(ID_ADD_MPCONSTRAINT,TetraGUIFrame::OnAddMPConstraint)
	
	EVT_MENU(ID_LIST_NODES,TetraGUIFrame::OnListNodes)
	EVT_MENU(ID_LIST_ELEMENTS,TetraGUIFrame::OnListElements)
	EVT_MENU(ID_LIST_MATERIALS,TetraGUIFrame::OnListMaterials)
	EVT_MENU(ID_LIST_FORCES,TetraGUIFrame::OnListForces)
	EVT_MENU(ID_LIST_DISPLACEMENTS,TetraGUIFrame::OnListDisplacements)
	EVT_MENU(ID_LIST_MPCONSTRAINTS,TetraGUIFrame::OnListMPConstraints)
	
	EVT_MENU(ID_GENERATE_MESH, TetraGUIFrame::OnGenerateMesh)
	EVT_MENU(ID_SOLVE,TetraGUIFrame::OnSolve)
	EVT_MENU(ID_MESH,TetraGUIFrame::OnMesh)
	
	EVT_MENU(ID_PLOT_MODEL,TetraGUIFrame::OnPlotModel)
	EVT_MENU(ID_PLOT_NODES,TetraGUIFrame::OnPlotNodes)
	EVT_MENU(ID_PLOT_ELEMENTS,TetraGUIFrame::OnPlotElements)
	EVT_MENU(ID_PLOT_DISPLACEMENTS_X,TetraGUIFrame::OnPlotDisplacementsX)
	EVT_MENU(ID_PLOT_DISPLACEMENTS_Y,TetraGUIFrame::OnPlotDisplacementsY)
	EVT_MENU(ID_PLOT_DISPLACEMENTS_Z,TetraGUIFrame::OnPlotDisplacementsZ)
	EVT_MENU(ID_PLOT_STRESSES_NORMAL,TetraGUIFrame::OnPlotStressesNormal)
	EVT_MENU(ID_PLOT_STRESSES_SHEAR,TetraGUIFrame::OnPlotStressesShear)
	EVT_MENU(ID_PLOT_STRESSES_PRINCIPLE,TetraGUIFrame::OnPlotStressesPrinciple)
	
	EVT_MENU(ID_HELP_ABOUT,TetraGUIFrame::OnHelpAbout)

	EVT_TREE_ITEM_ACTIVATED(ID_MODEL_TREE, TetraGUIFrame::OnTreeItemActivated)
	
	EVT_BUTTON(ID_PRIMITIVE_BUTTON, TetraGUIFrame::OnPrimitiveButton)
	EVT_BUTTON(ID_OPERATION_BUTTON, TetraGUIFrame::OnOperationButton)
	EVT_BUTTON(ID_FEM_BUTTON, TetraGUIFrame::OnFEMButton)

	EVT_BUTTON(ID_PARALLELOPIPED_BUTTON, TetraGUIFrame::OnParallelopipedButton)
	EVT_BUTTON(ID_SPHERE_BUTTON, TetraGUIFrame::OnSphereButton)
	EVT_BUTTON(ID_FRUSTUM_BUTTON, TetraGUIFrame::OnFrustumButton)
	EVT_BUTTON(ID_CYLINDER_BUTTON, TetraGUIFrame::OnCylinderButton)
	
	EVT_BUTTON(ID_OPER_DONE, TetraGUIFrame::OnOperDone)
	EVT_BUTTON(ID_OPER_CANCEL, TetraGUIFrame::OnOperCancel)

	EVT_BUTTON(ID_FEM_DONE, TetraGUIFrame::OnFEMDone)
	EVT_BUTTON(ID_FEM_CANCEL, TetraGUIFrame::OnFEMCancel)

	EVT_BUTTON(ID_RECT_SELECT, TetraGUIFrame::OnRectSelect)
	EVT_BUTTON(ID_CIRC_SELECT, TetraGUIFrame::OnCircSelect)

	EVT_BUTTON(ID_PARA_DONE, TetraGUIFrame::OnParaDone)
	EVT_BUTTON(ID_PARA_CANCEL, TetraGUIFrame::OnParaCancel)

	EVT_BUTTON(ID_SPHE_DONE, TetraGUIFrame::OnSpheDone)
	EVT_BUTTON(ID_SPHE_CANCEL, TetraGUIFrame::OnSpheCancel)
	
	EVT_BUTTON(ID_FRUS_DONE, TetraGUIFrame::OnFrusDone)
	EVT_BUTTON(ID_FRUS_CANCEL, TetraGUIFrame::OnFrusCancel)

	EVT_BUTTON(ID_CYLI_DONE, TetraGUIFrame::OnCyliDone)
	EVT_BUTTON(ID_CYLI_CANCEL, TetraGUIFrame::OnCyliCancel)

	EVT_BUTTON(ID_RECT_DONE, TetraGUIFrame::OnRectDone)
	EVT_BUTTON(ID_RECT_CANCEL, TetraGUIFrame::OnRectCancel)

	EVT_BUTTON(ID_CIRC_DONE, TetraGUIFrame::OnCircDone)
	EVT_BUTTON(ID_CIRC_CANCEL, TetraGUIFrame::OnCircCancel)
END_EVENT_TABLE()

void TetraGUIFrame::OnFileOpenCSGModel (wxCommandEvent& ce)
{
	wxFileDialog *fd = new wxFileDialog (this);
	const char* fname;
	
	if (fd->ShowModal () == wxID_OK)
	{
		fname = fd->GetPath ().ToAscii().data();
		if (csgmodel->OpenFromFile (fname) == false)
			wxLogMessage (wxT("ERROR: Read failed."));
		else
			wxLogMessage (wxT("File opened successfully"));
	}
	delete fd;
}

void TetraGUIFrame::OnFileSaveCSGModel (wxCommandEvent& ce)
{
	wxFileDialog *fd;
	const char* fname;
	
	if (csgmodel->IsLoaded () == false)
	{
		wxLogMessage (wxT("Nothing to save"));
		return;
	}

	fd = new wxFileDialog (this);
	fd->SetStyle (wxSAVE);
	if (fd->ShowModal () == wxID_OK)
	{
		fname = fd->GetPath ().ToAscii().data();
		if (csgmodel->SaveToFile (fname) == false)
			wxLogMessage (wxT("ERROR: Save failed"));
		else
			wxLogMessage (wxT("File saved successfully"));
	}
	delete fd;
}

void TetraGUIFrame::OnFileOpenFEMModel (wxCommandEvent& ce)
{
	wxFileDialog *fd = new wxFileDialog (this);
	const char* fname;
	
	if (fd->ShowModal () == wxID_OK)
	{
		fname = fd->GetPath ().ToAscii().data();
		Tetrahedron<REAL> *temp = tetra;
		tetra = CreateTetrahedronFromFile<REAL> (fname);
		if (tetra == NULL)
		{
			wxLogMessage (wxT("ERROR: Read failed."));
			tetra = temp;
		}
		else delete temp;
	}
	delete fd;
}

void TetraGUIFrame::OnFileSaveFEMModel (wxCommandEvent& ce)
{
	if (!tetra->isSolved) return;
	wxFileDialog *fd = new wxFileDialog (this);
	const char* fname;
	
	fd->SetStyle (wxSAVE);
	if (fd->ShowModal () == wxID_OK)
	{
		fname = fd->GetPath ().ToAscii().data();
		tetra->WriteResultsToFile (fname);
	}
	delete fd;
}

void TetraGUIFrame::OnFileViewLogger (wxCommandEvent& ce)
{
/*	if(loggerVisible)
	{
		logger->Show (false);
		loggerVisible = false;
	}
	else
	{
		logger->Show (true);
		loggerVisible = true;
	}*/
}

void TetraGUIFrame::OnFileExit (wxCommandEvent& ce)
{
	Close();
}

void TetraGUIFrame::OnViewReplot (wxCommandEvent& ce)
{
	canvas->SwapBuffers ();
	canvas->Refresh (FALSE);
}

void TetraGUIFrame::OnViewZoomIn (wxCommandEvent& ce)
{
	canvas->ZoomIn ();
}

void TetraGUIFrame::OnViewZoomOut (wxCommandEvent& ce)
{
	canvas->ZoomOut ();
}

void TetraGUIFrame::OnViewFitWindow (wxCommandEvent& ce)
{
	canvas->FitInWindow ();
}

void TetraGUIFrame::OnAddNode (wxCommandEvent& ce)
{
	AddNodeDialog *dialog = new AddNodeDialog (this, tetra);
	dialog->ShowModal ();
}

void TetraGUIFrame::OnAddElement (wxCommandEvent& ce)
{
	AddElementDialog *dialog = new AddElementDialog (this, tetra);
	dialog->ShowModal ();
}

void TetraGUIFrame::OnAddMaterial (wxCommandEvent& ce)
{
	AddMaterialDialog *dialog = new AddMaterialDialog (this, tetra);
	dialog->ShowModal ();
}

void TetraGUIFrame::OnAddForce (wxCommandEvent& ce)
{
	AddForceDialog *dialog = new AddForceDialog (this, tetra);
	dialog->ShowModal ();
}

void TetraGUIFrame::OnAddDisplacement (wxCommandEvent& ce)
{
	AddDisplacementDialog *dialog = new AddDisplacementDialog (this, tetra);
	dialog->ShowModal ();
}

void TetraGUIFrame::OnAddMPConstraint (wxCommandEvent& ce)
{
	AddMPConstraintDialog *dialog = new AddMPConstraintDialog (this, tetra);
	dialog->ShowModal ();
}

void TetraGUIFrame::OnListNodes (wxCommandEvent&)
{
	wxString str;
	
	wxLogMessage (wxT("---------------"));
	str = wxString::Format (wxT("Total Nodes - %d"), tetra->nNodes);
	wxLogMessage (str);
	wxLogMessage (wxT("Node  x       y       z"));
	
	for (int i = 1; i <= tetra->nNodes; i++)
	{
		str = wxString::Format(wxT("%6d %8.1f %8.1f %8.1f"), i, tetra->Nodes[i].x,
								tetra->Nodes[i].y, tetra->Nodes[i].z);
		wxLogMessage (str);
	}
	
	wxLogMessage (wxT("---------------"));
}

void TetraGUIFrame::OnListElements (wxCommandEvent&)
{
	wxString str;
	
	wxLogMessage (wxT("---------------"));
	str = wxString::Format (wxT("Total Elements - %d"), tetra->nElements);
	wxLogMessage (str);
	wxLogMessage (wxT("Element  N1      N2      N3      N4      MAT     TEMP"));
	
	for (int i = 1; i <= tetra->nElements; i++)
	{
		str = wxString::Format(wxT("%8d %8d %8d %8d %8d %8d %8.1f"), i, 
								tetra->Elements[i].Nodes[0],
								tetra->Elements[i].Nodes[1],
								tetra->Elements[i].Nodes[2],
								tetra->Elements[i].Nodes[3],
								tetra->Elements[i].Material,
								tetra->Elements[i].Temp
								);
		wxLogMessage (str);
	}
	
	wxLogMessage (wxT("---------------"));
}

void TetraGUIFrame::OnListMaterials (wxCommandEvent&)
{
	wxString str;
	
	wxLogMessage (wxT("---------------"));
	str = wxString::Format (wxT("Total Materials - %d"), tetra->nMaterials);
	wxLogMessage (str);
	wxLogMessage (wxT("Material  E       ALPHA   Nu"));
	
	for (int i = 1; i <= tetra->nMaterials; i++)
	{
		str = wxString::Format(wxT("%10d %8.4e %8.4e %8.4e"), i, 
								tetra->Materials[i].E,
								tetra->Materials[i].Alpha,
								tetra->Materials[i].Nu
								);
		wxLogMessage (str);
	}
	
	wxLogMessage (wxT("---------------"));
}

void TetraGUIFrame::OnListForces (wxCommandEvent&)
{
	wxString str;
	
	wxLogMessage (wxT("---------------"));
	str = wxString::Format (wxT("Total Forces - %d"), tetra->Loads.size());
	wxLogMessage (str);
	wxLogMessage (wxT("DOF   FORCE"));
	
	for (unsigned int i = 0; i < tetra->Loads.size(); i++)
	{
		str = wxString::Format(wxT("%6d %8.4e"), tetra->Loads[i].index, 
								tetra->Loads[i].value);
		wxLogMessage (str);
	}
	
	wxLogMessage (wxT("---------------"));
}

void TetraGUIFrame::OnListDisplacements (wxCommandEvent&)
{
	wxString str;
	
	wxLogMessage (wxT("---------------"));
	str = wxString::Format (wxT("Total Displacements - %d"), tetra->nDisp);
	wxLogMessage (str);
	wxLogMessage (wxT("DOF   DISP"));
	
	for (int i = 0; i < tetra->nDisp; i++)
	{
		str = wxString::Format(wxT("%6d %8.4e"), 
							tetra->Displacements[i].index, 
							tetra->Displacements[i].value);
		wxLogMessage (str);
	}
	
	wxLogMessage (wxT("---------------"));
}

void TetraGUIFrame::OnListMPConstraints (wxCommandEvent&)
{
	wxString str;
	
	wxLogMessage (wxT("---------------"));
	str = wxString::Format (wxT("Total Multipoint Constraints - %d"), tetra->nMPC);
	wxLogMessage (str);
	wxLogMessage (wxT("MPC   BETA1    i    BETA2    j     BETA3"));
	
	for (int i = 0; i < tetra->nMPC; i++)
	{
		str = wxString::Format(wxT("%6d %8.4e %6d %8.4e %8d %8.4e"), i, 
								tetra->MPCons[i].b1,
								tetra->MPCons[i].i,
								tetra->MPCons[i].b2,
								tetra->MPCons[i].j,
								tetra->MPCons[i].b3
								);
		wxLogMessage (str);
	}
	
	wxLogMessage (wxT("---------------"));
}

void TetraGUIFrame::OnGenerateMesh (wxCommandEvent& ce)
{
	tetra = csgmodel->GetFEMModel ();
}

void TetraGUIFrame::OnSolve (wxCommandEvent& ce)
{
	tetra->Solve ();
	wxLogMessage (wxT("node# x-displ   y-displ   z-displ\n"));
	for (int i = 0; i < tetra->nNodes; i++) 
	{
		wxLogMessage (wxString::Format(wxT("%6d %10.4e %10.4e %10.4e"),i+1,
								tetra->Forces[3*i], 
								tetra->Forces[3*i+1],
								tetra->Forces[3*i+2]));
	}
}

void TetraGUIFrame::OnMesh (wxCommandEvent& ce)
{
/*	MeshDialog *md = new MeshDialog (this);
	md->ShowModal ();

	int i = md->slider->GetValue ();
	while (i > 0 && i--)
	{
		AdaptiveMesher *am = new AdaptiveMesher (tetra);
		am->Mesh ();
		tetra = am->GetMeshedTetrahedron ();
	}*/
	tetra = TetgenInterface::Tetrahedralize (tetra);
}

void TetraGUIFrame::OnPlotModel (wxCommandEvent& ce)
{
	canvas->PlotModel (csgmodel);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnPlotNodes (wxCommandEvent& ce)
{
	canvas->SetTetra (tetra);
	canvas->PlotNodes ();
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnPlotElements (wxCommandEvent& ce)
{
	canvas->SetTetra (tetra);
	canvas->PlotElements ();
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnPlotDisplacementsX (wxCommandEvent& ce)
{
	canvas->SetTetra (tetra);
	canvas->PlotDisplacements (0);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnPlotDisplacementsY (wxCommandEvent& ce)
{
	canvas->SetTetra (tetra);
	canvas->PlotDisplacements (1);
	canvas->SwapBuffers ();}

void TetraGUIFrame::OnPlotDisplacementsZ (wxCommandEvent& ce)
{
	canvas->SetTetra (tetra);
	canvas->PlotDisplacements (2);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnPlotStressesNormal (wxCommandEvent& ce)
{
	canvas->SetTetra (tetra);
	canvas->PlotStresses (0);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnPlotStressesShear (wxCommandEvent& ce)
{
	canvas->SetTetra (tetra);
	canvas->PlotStresses (1);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnPlotStressesPrinciple (wxCommandEvent& ce)
{
	canvas->SetTetra (tetra);
	canvas->PlotStresses (2);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnHelpAbout (wxCommandEvent& ce)
{
	wxMessageDialog *md = new wxMessageDialog (this, 
					wxT("TetraSolve Release 3"), wxT("About..."), wxOK);
	md->ShowModal ();
}

void TetraGUIFrame::OnTreeItemActivated (wxTreeEvent& te)
{
	if (modeltree->GetItemText(te.GetItem()).Cmp (wxT("Parallelopiped")) == 0)
	{
		ShowParallelopipedPanel(true, 
			(TetraCSGParallelopiped*)(((CSGTreeItemData*)modeltree->GetItemData (te.GetItem()))->data));
	}
	else if (modeltree->GetItemText(te.GetItem ()).Cmp (wxT("Sphere")) == 0)
	{
		ShowSpherePanel(true, 
			(TetraCSGSphere*)(((CSGTreeItemData*)modeltree->GetItemData (te.GetItem()))->data));
	}
	else if (modeltree->GetItemText(te.GetItem ()).Cmp (wxT("Frustum")) == 0)
	{
		ShowFrustumPanel (true, 
			(TetraCSGFrustum*)(((CSGTreeItemData*)modeltree->GetItemData (te.GetItem()))->data));
	}
	else if (modeltree->GetItemText (te.GetItem ()).Cmp (wxT("Cylinder")) == 0)
	{
		ShowCylinderPanel (true, 
			(TetraCSGCylinder*)(((CSGTreeItemData*)modeltree->GetItemData (te.GetItem()))->data));
	}
	else if (modeltree->GetItemText (te.GetItem ()).Cmp (wxT("Union")) == 0 ||
		modeltree->GetItemText (te.GetItem ()).Cmp (wxT("Intersect")) == 0 ||
		modeltree->GetItemText (te.GetItem ()).Cmp (wxT("Difference")) == 0)
	{
		ShowOperationPanel (true,
			(TetraCSGBoolOper*)(((CSGTreeItemData*)modeltree->GetItemData (te.GetItem ()))->data));
	}
}

void TetraGUIFrame::OnPrimitiveButton (wxCommandEvent& ce)
{
	ShowPrimitivePanel ();
}

void TetraGUIFrame::OnOperationButton (wxCommandEvent& ce)
{
	ShowOperationPanel ();
}

void TetraGUIFrame::OnFEMButton (wxCommandEvent& ce)
{
	ShowFEMPanel ();
}

void TetraGUIFrame::OnRectSelect (wxCommandEvent& ce)
{
	ShowRectanglePanel ();
}

void TetraGUIFrame::OnCircSelect (wxCommandEvent& ce)
{
	ShowCirclePanel ();
}

void TetraGUIFrame::OnParallelopipedButton (wxCommandEvent& ce)
{
	ShowParallelopipedPanel ();
}

void TetraGUIFrame::OnSphereButton (wxCommandEvent& ce)
{
	ShowSpherePanel ();
}

void TetraGUIFrame::OnFrustumButton (wxCommandEvent& ce)
{
	ShowFrustumPanel ();
}

void TetraGUIFrame::OnCylinderButton (wxCommandEvent& ce)
{
	ShowCylinderPanel ();
}

void TetraGUIFrame::OnOperDone (wxCommandEvent& ce)
{
	if (operedit != NULL)
	{
		if (opercombo->GetSelection() == 0)
			csgmodel->GetTree()->Modify((TetraCSGPrimitive*)operedit->GetRightChild (), new TetraCSGUnion ());
		else if (opercombo->GetSelection() == 1)
			csgmodel->GetTree()->Modify((TetraCSGPrimitive*)operedit->GetRightChild (), new TetraCSGIntersect());
		else if (opercombo->GetSelection() == 2)
			csgmodel->GetTree()->Modify((TetraCSGPrimitive*)operedit->GetRightChild (), new TetraCSGDifference());

		wxLogMessage (wxT("Boolean operation of the primitive modified successfully"));
	}

	ShowOperationPanel (false);
}

void TetraGUIFrame::OnOperCancel (wxCommandEvent& ce)
{
	ShowOperationPanel (false);
}

void TetraGUIFrame::OnFEMDone (wxCommandEvent& ce)
{
	ShowFEMPanel (false);
}

void TetraGUIFrame::OnFEMCancel (wxCommandEvent& ce)
{
	ShowFEMPanel (false);
}

void TetraGUIFrame::OnParaDone (wxCommandEvent& ce)
{
	double x, y, z, w, h, d;

	(paraxtext->GetValue ()).ToDouble (&x);
	(paraytext->GetValue ()).ToDouble (&y);
	(paraztext->GetValue ()).ToDouble (&z);
	(parawtext->GetValue ()).ToDouble (&w);
	(parahtext->GetValue ()).ToDouble (&h);
	(paradtext->GetValue ()).ToDouble (&d);

	if (paraedit == NULL)
	{
		TetraCSGParallelopiped *prim = 
				new TetraCSGParallelopiped (x, y, z, w, h, d);
		TetraCSGUnion *unio = new TetraCSGUnion ();
		csgmodel->GetTree()->Insert (prim, unio);
		RegenerateTree ();
		wxLogMessage (wxT("Parallelopiped added successfully"));
	}
	else
	{
		paraedit->SetX (x);
		paraedit->SetY (y);
		paraedit->SetZ (z);
		paraedit->SetWidth (w);
		paraedit->SetHeight (h);
		paraedit->SetDepth (d);
		wxLogMessage (wxT("Parallelopiped modified successfully"));
	}

	ShowParallelopipedPanel (false);
	
	canvas->PlotModel (csgmodel);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnParaCancel (wxCommandEvent& ce)
{
	paraxtext->SetValue (wxT(""));
	paraytext->SetValue (wxT(""));
	paraztext->SetValue (wxT(""));
	parawtext->SetValue (wxT(""));
	parahtext->SetValue (wxT(""));
	paradtext->SetValue (wxT(""));

	ShowParallelopipedPanel (false);
}

void TetraGUIFrame::OnSpheDone (wxCommandEvent& ce)
{
	double x, y, z, r;

	(sphextext->GetValue ()).ToDouble (&x);
	(spheytext->GetValue ()).ToDouble (&y);
	(spheztext->GetValue ()).ToDouble (&z);
	(sphertext->GetValue ()).ToDouble (&r);

	if (spheedit == NULL)
	{
		TetraCSGSphere *prim =
				new TetraCSGSphere (x, y, z, r);
		TetraCSGUnion *unio = new TetraCSGUnion ();
		csgmodel->GetTree()->Insert (prim, unio);
		RegenerateTree ();
		wxLogMessage (wxT("Sphere added successfully"));
	}
	else
	{
		spheedit->SetX (x);
		spheedit->SetY (y);
		spheedit->SetZ (z);
		spheedit->SetRadius (r);
		wxLogMessage (wxT("Sphere modified successfully"));
	}

	ShowSpherePanel (false);

	canvas->PlotModel (csgmodel);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnSpheCancel (wxCommandEvent& ce)
{
	sphextext->SetValue (wxT(""));
	spheytext->SetValue (wxT(""));
	spheztext->SetValue (wxT(""));
	sphertext->SetValue (wxT(""));

	ShowSpherePanel (false);
}

void TetraGUIFrame::OnFrusDone (wxCommandEvent& ce)
{
	double x, y, z, tw, bw, len;

	(frusxtext->GetValue ()).ToDouble (&x);
	(frusytext->GetValue ()).ToDouble (&y);
	(frusztext->GetValue ()).ToDouble (&z);
	(frustwtext->GetValue ()).ToDouble (&tw);
	(frusbwtext->GetValue ()).ToDouble (&bw);
	(frusltext->GetValue ()).ToDouble (&len);

	if (frusedit == NULL)
	{
		TetraCSGFrustum *prim =
				new TetraCSGFrustum (x, y, z, tw, bw, len);
		TetraCSGUnion *unio = new TetraCSGUnion ();
		csgmodel->GetTree()->Insert (prim, unio);
		RegenerateTree ();
		wxLogMessage (wxT("Frustum added successfully"));
	}
	else
	{
		frusedit->SetX (x);
		frusedit->SetY (y);
		frusedit->SetZ (z);
		frusedit->SetBottomSide (bw);
		frusedit->SetTopSide (tw);
		frusedit->SetLength (len);
		wxLogMessage (wxT("Frustum modified successfully"));
	}

	ShowFrustumPanel (false);

	canvas->PlotModel (csgmodel);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnFrusCancel (wxCommandEvent& ce)
{
	frusxtext->SetValue (wxT(""));
	frusytext->SetValue (wxT(""));
	frusztext->SetValue (wxT(""));
	frustwtext->SetValue (wxT(""));
	frusbwtext->SetValue (wxT(""));

	ShowFrustumPanel (false);
}

void TetraGUIFrame::OnCyliDone (wxCommandEvent& ce)
{
	double x, y, z, r, h;

	(cylixtext->GetValue ()).ToDouble (&x);
	(cyliytext->GetValue ()).ToDouble (&y);
	(cyliztext->GetValue ()).ToDouble (&z);
	(cylirtext->GetValue ()).ToDouble (&r);
	(cylihtext->GetValue ()).ToDouble (&h);

	if (cyliedit == NULL)
	{
		TetraCSGCylinder *prim =
				new TetraCSGCylinder (x, y, z, r, h);
		TetraCSGUnion *unio = new TetraCSGUnion ();
		csgmodel->GetTree()->Insert (prim, unio);
		RegenerateTree ();
		wxLogMessage (wxT("Cylinder added successfully"));
	}
	else
	{
		cyliedit->SetX (x);
		cyliedit->SetY (y);
		cyliedit->SetZ (z);
		cyliedit->SetRadius (r);
		cyliedit->SetLength (h);
		wxLogMessage (wxT("Cylinder modified successfully"));
	}

	ShowCylinderPanel (false);

	canvas->PlotModel (csgmodel);
	canvas->SwapBuffers ();
}

void TetraGUIFrame::OnCyliCancel (wxCommandEvent& ce)
{
	cylixtext->SetValue (wxT(""));
	cyliytext->SetValue (wxT(""));
	cyliztext->SetValue (wxT(""));
	cylirtext->SetValue (wxT(""));
	cylihtext->SetValue (wxT(""));

	ShowCylinderPanel (false);
}

void TetraGUIFrame::OnRectDone (wxCommandEvent& ce)
{
	double x, y, z, h, w;
	wxString str;

	(rectxtext->GetValue()).ToDouble (&x);
	(rectytext->GetValue()).ToDouble (&y);
	(rectztext->GetValue()).ToDouble (&z);
	(rectwtext->GetValue()).ToDouble (&w);
	(recthtext->GetValue()).ToDouble (&h);

	if (tetra != NULL)
	{
		for (unsigned int i = 0; i < tetra->Nodes.size(); i++)
		{
			if (tetra->Nodes[i].z == z && 
				tetra->Nodes[i].x >= x && tetra->Nodes[i].x <= (x+w) &&
				tetra->Nodes[i].y >= y && tetra->Nodes[i].y <= (y+h))
			{
				str.Printf (wxT("(%.2f %.2f %.2f"), tetra->Nodes[i].x,
									tetra->Nodes[i].y, tetra->Nodes[i].z);
				selectlist->Append (str);
			}
		}
	}

	ShowRectanglePanel (false);
}

void TetraGUIFrame::OnRectCancel (wxCommandEvent& ce)
{
	ShowRectanglePanel (false);
}

void TetraGUIFrame::OnCircDone (wxCommandEvent& ce)
{
	double x, y, z, r;
	wxString str;

	(circxtext->GetValue ()).ToDouble (&x);
	(circytext->GetValue ()).ToDouble (&y);
	(circztext->GetValue ()).ToDouble (&z);
	(circrtext->GetValue ()).ToDouble (&r);

	if (tetra != NULL)
	{
		for (unsigned int i = 0; i < tetra->Nodes.size (); i++)
		{
			if (tetra->Nodes[i].z == z)
			{
				if (sqrt (pow (tetra->Nodes[i].x - x, 2.0) + pow (tetra->Nodes[i].y - y, 2.0)) < r)
				{
					str.Printf (wxT("(%.2f %.2f %.2f"), tetra->Nodes[i].x,
										tetra->Nodes[i].y, tetra->Nodes[i].z);
					selectlist->Append (str);
				}
			}
		}
	}

	ShowCirclePanel (false);
}

void TetraGUIFrame::OnCircCancel (wxCommandEvent& ce)
{
	ShowCirclePanel (false);
}

// TetraGUIFrame class member function definitions end
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// TetraGUIFrame::MeshDialog class member function definitions begin

TetraGUIFrame::MeshDialog::MeshDialog (wxWindow* parent)
					: wxDialog (parent, wxID_ANY, wxT("Mesh"), wxDefaultPosition)
{
    wxBoxSizer *sizer = new wxBoxSizer (wxVERTICAL);
	wxStaticText *label = new wxStaticText (this, wxID_ANY, wxT("Select the quality of meshing(0-Coarse 10-Fine)"));
	sizer->Add (label, 0, wxALIGN_CENTER|wxALL);
    
	slider = new wxSlider( this, wxID_ANY, 0, 0, 10, wxDefaultPosition, wxSize(100,-1), wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_LABELS );
    sizer->Add ( slider, 0, wxALIGN_CENTER|wxALL, 5);

	wxButton *button = new wxButton (this, ID_OK, wxT("OK"));
	sizer->Add (button, 0, wxALIGN_CENTER|wxALL);
	
	SetSizer (sizer);
	sizer->SetSizeHints (this);
}

TetraGUIFrame::MeshDialog::~MeshDialog ()
{
}

void 
TetraGUIFrame::MeshDialog::OnOKButton (wxCommandEvent& ce)
{
	Close ();
}

BEGIN_EVENT_TABLE (TetraGUIFrame::MeshDialog, wxDialog)
	EVT_BUTTON (ID_OK, TetraGUIFrame::MeshDialog::OnOKButton)
END_EVENT_TABLE ()

// TetraGUIFrame::MeshDialog class member function definitions end
//////////////////////////////////////////////////////////
