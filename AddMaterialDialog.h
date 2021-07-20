#ifndef __ADD_MATERIAL_DIALOG_H__
#define __ADD_MATERIAL_DIALOG_H__

#include "TetraSolve.h"
#include "Tetrahedron.h"

class AddMaterialDialog : public wxDialog
{
	DECLARE_EVENT_TABLE();

public:
	AddMaterialDialog (wxWindow *parent, Tetrahedron<REAL> *t);

protected:	
	enum {
		ID_ADD_BUTTON = 101,
		ID_REMOVE_BUTTON,
		ID_CLOSE_BUTTON
	};

	Tetrahedron<REAL> *tetra;
	wxListBox *mlistbox;
	wxTextCtrl *etext, *nutext, *alphatext;
	
	void LoadListBox ();
	void OnAddButton (wxCommandEvent&);
	void OnRemoveButton (wxCommandEvent&);
	void OnCloseButton (wxCommandEvent&);
};

#endif /* __ADD_MATERIAL_DIALOG_H__ */
