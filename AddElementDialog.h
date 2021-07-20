#ifndef __ADD_ELEMENT_DIALOG_HEADER__
#define __ADD_ELEMENT_DIALOG_HEADER__

#include "TetraSolve.h"
#include "Tetrahedron.h"

class AddElementDialog : public wxDialog
{
	DECLARE_EVENT_TABLE();

public:
	AddElementDialog (wxWindow *parent, Tetrahedron<REAL> *t);

protected:
	enum {
		ID_ADD_BUTTON = 101,
		ID_REMOVE_BUTTON,
		ID_CLOSE_BUTTON
	};

	wxListBox *elistbox;
	wxTextCtrl *temptext;
	wxComboBox *n1combo, *n2combo, *n3combo, *n4combo, *matcombo;
	Tetrahedron<REAL> *tetra;

	void LoadListBox ();
	void OnAddButton (wxCommandEvent&);
	void OnRemoveButton (wxCommandEvent&);
	void OnCloseButton (wxCommandEvent&);
};

#endif /* __ADD_ELEMENT_DIALOG_HEADER__ */
