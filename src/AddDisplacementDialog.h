#ifndef __ADD_DISPLACEMENT_DIALOG_H__
#define __ADD_DISPLACEMENT_DIALOG_H__

#include "TetraSolve.h"
#include "Tetrahedron.h"

#include <wx/spinctrl.h>

class AddDisplacementDialog : public wxDialog
{
	DECLARE_EVENT_TABLE();

public:
	AddDisplacementDialog (wxWindow *parent, Tetrahedron<REAL> *t);

protected:
	enum {
		ID_ADD_BUTTON = 101,
		ID_REMOVE_BUTTON,
		ID_CLOSE_BUTTON
	};

	Tetrahedron<REAL> *tetra;
	wxListBox *dlistbox;
	wxSpinCtrl *dofspin;
	wxTextCtrl *disptext;
	wxButton *add, *remove, *close;

	void LoadListBox ();
	void OnAddButton (wxCommandEvent&);
	void OnRemoveButton (wxCommandEvent&);
	void OnCloseButton (wxCommandEvent&);
};

#endif /* __ADD_DISPLACEMENT_DIALOG_H__ */
