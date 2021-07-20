#ifndef __ADD_FORCE_DIALOG_HEADER__
#define __ADD_FORCE_DIALOG_HEADER__

#include "TetraSolve.h"
#include "Tetrahedron.h"
#include <wx/spinctrl.h>

class AddForceDialog : public wxDialog
{
	DECLARE_EVENT_TABLE();

public:
	AddForceDialog (wxWindow *parent, Tetrahedron<REAL> *t);

protected:
	enum {
		ID_ADD_BUTTON = 101,
		ID_REMOVE_BUTTON,
		ID_CLOSE_BUTTON
	};

	Tetrahedron<REAL> *tetra;
	wxListBox *flistbox;
	wxSpinCtrl *dofspin;
	wxTextCtrl *forcetext;
	wxButton *add, *remove, *close;
	
	void LoadListBox ();
	void OnAddButton (wxCommandEvent&);
	void OnRemoveButton (wxCommandEvent&);
	void OnCloseButton (wxCommandEvent&);
};

#endif /* __ADD_FORCE_DIALOG_HEADER__ */
