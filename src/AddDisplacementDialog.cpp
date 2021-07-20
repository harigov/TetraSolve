#include "AddDisplacementDialog.h"

//////////////////////////////////////////////////////////
// AddDisplacementDialog

AddDisplacementDialog::AddDisplacementDialog (wxWindow *parent, Tetrahedron<REAL> *t)				: wxDialog (parent, -1, wxT("Add Displacement"), wxDefaultPosition)
{
	tetra = t;
	wxBoxSizer *vbox = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *hbox = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *buttonsizer = new wxBoxSizer( wxHORIZONTAL );
	wxFlexGridSizer *flexgrid = new wxFlexGridSizer( 2, 0, 0 );

	wxStaticText *doflabel, *displabel;

	dlistbox = new wxListBox( this, -1, wxDefaultPosition, wxSize(120,100), 
				0, NULL, wxLB_SINGLE );

	doflabel = new wxStaticText( this, -1, wxT("DOF"));
	displabel = new wxStaticText( this, -1, wxT("DISP"));

	dofspin = new wxSpinCtrl( this, -1, wxT("0"), wxDefaultPosition, 
				wxSize(80,-1), 0, 0, 100, 0 );
	disptext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
				wxSize(80,-1), 0 );

	flexgrid->Add( doflabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( dofspin, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( displabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( disptext, 0, wxALIGN_CENTER|wxALL, 5 );


	hbox->Add( flexgrid, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	hbox->Add( dlistbox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	vbox->Add( hbox, 0, wxALIGN_CENTER|wxALL, 5 );

	add = new wxButton( this, ID_ADD_BUTTON, wxT("Add"));
	remove = new wxButton( this, ID_REMOVE_BUTTON, wxT("Remove"));
	close = new wxButton( this, ID_CLOSE_BUTTON, wxT("Close"));

	buttonsizer->Add( add, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add( remove, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add( close, 0, wxALIGN_CENTER|wxALL, 5 );

	vbox->Add( buttonsizer, 0, wxALIGN_CENTER|wxALL, 5 );

	SetSizer( vbox );
	vbox->SetSizeHints( this );

	dofspin->SetRange (1,tetra->nDOF);

	LoadListBox ();
}

void AddDisplacementDialog::LoadListBox ()
{
	wxString str;
	dlistbox->Clear ();
	for (int i = 0; i < tetra->nDisp; i++)
	{
		str = wxString::Format (wxT("%d %f"), 
			tetra->Displacements[i].index,
			tetra->Displacements[i].value);
		dlistbox->Append (str);
	}
}

void AddDisplacementDialog::OnAddButton (wxCommandEvent& ce)
{
	int dof = dofspin->GetValue();
	REAL disp;
	if ((disptext->GetValue()).ToDouble(&disp))
	{
		if (tetra->AddDisplacement (dof, disp))
		{
			LoadListBox ();
			return;
		}

	}
	wxMessageDialog *md = new wxMessageDialog (this, wxT("Invalid values"), 
			wxT("Error"), wxOK | wxICON_ERROR);
	md->ShowModal ();
	delete md;
}

void AddDisplacementDialog::OnRemoveButton (wxCommandEvent& ce)
{
	if (tetra->RemoveDisplacement (dlistbox->GetSelection()))
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
			wxT("Displacement Removed Successfully."), 
			wxT("Success"), wxOK | wxICON_INFORMATION);
		md->ShowModal ();
		delete md;

		LoadListBox ();
	}
	else
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
			wxT("Displacement Removal Failed."), 
			wxT("Failed"), wxOK | wxICON_ERROR);
		md->ShowModal ();
		delete md;
	}
}

void AddDisplacementDialog::OnCloseButton (wxCommandEvent& ce)
{
	Close ();
}

BEGIN_EVENT_TABLE(AddDisplacementDialog,wxDialog)
	EVT_BUTTON(ID_ADD_BUTTON,AddDisplacementDialog::OnAddButton)
	EVT_BUTTON(ID_REMOVE_BUTTON,AddDisplacementDialog::OnRemoveButton)
	EVT_BUTTON(ID_CLOSE_BUTTON,AddDisplacementDialog::OnCloseButton)
END_EVENT_TABLE()

// AddDisplacementDialog
//////////////////////////////////////////////////////////
