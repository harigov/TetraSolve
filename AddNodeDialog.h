#ifndef __ADD_NODE_DIALOG_HEADER__
#define __ADD_NODE_DIALOG_HEADER__

#include "TetraSolve.h"
#include "Tetrahedron.h"

class AddNodeDialog : public wxDialog
{
	DECLARE_EVENT_TABLE();

public:
	AddNodeDialog (wxWindow *parent, Tetrahedron<REAL> *tetra);
	
protected:
	enum {
		ID_X_TEXT_BOX = 101,
		ID_Y_TEXT_BOX, ID_Z_TEXT_BOX,
		ID_ADD_BUTTON, ID_REMOVE_BUTTON,
		ID_CLOSE_BUTTON
	};

	Tetrahedron<REAL> *tetra;
	wxTextCtrl *xtext, *ytext, *ztext;
	wxButton *add, *remove, *close;
	wxListBox *nodeslist;

	void LoadListBox ();
	void OnAddButton (wxCommandEvent&);
	void OnRemoveButton (wxCommandEvent&);
	void OnCloseButton (wxCommandEvent&);
};

#endif /* __ADD_NODE_DIALOG_HEADER__ */
