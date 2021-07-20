#ifndef __ADD_MP_CONSTRAINT_DIALOG_H__
#define __ADD_MP_CONSTRAINT_DIALOG_H__

#include "TetraSolve.h"
#include "Tetrahedron.h"

class AddMPConstraintDialog : public wxDialog
{
	DECLARE_EVENT_TABLE();

public:
	AddMPConstraintDialog (wxWindow *parent, Tetrahedron<REAL> *t);

protected:
	enum {
		ID_ADD_BUTTON = 101,
		ID_REMOVE_BUTTON,
		ID_CLOSE_BUTTON
	};

	Tetrahedron<REAL> *tetra;
	wxTextCtrl *itext, *jtext, *beta1text, *beta2text, *beta3text;
	wxListBox *mpclistbox;
	
	void LoadListBox ();
	void OnAddButton (wxCommandEvent&);
	void OnRemoveButton (wxCommandEvent&);
	void OnCloseButton (wxCommandEvent&);
};

#endif /* __ADD_MP_CONSTRAINT_DIALOG_H__ */
