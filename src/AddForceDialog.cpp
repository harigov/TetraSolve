#include "AddForceDialog.h"

//////////////////////////////////////////////////////////
// TetraGUIFrame::AddForceDialog

AddForceDialog::AddForceDialog (wxWindow *parent, Tetrahedron<REAL> *t) : 
wxDialog (parent, -1, wxT("Add Load"), wxDefaultPosition)
{
	tetra = t;
	wxBoxSizer *vbox = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *hbox = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *buttonsizer = new wxBoxSizer( wxHORIZONTAL );
	wxFlexGridSizer *flexgrid = new wxFlexGridSizer( 2, 0, 0 );

	wxStaticText *doflabel, *forcelabel;

	flistbox = new wxListBox( this, -1, wxDefaultPosition, 
				wxSize(120,100), 0, NULL, wxLB_SINGLE );


	doflabel = new wxStaticText( this, -1, wxT("DOF"));
	forcelabel = new wxStaticText( this, -1, wxT("Force"));

	dofspin = new wxSpinCtrl( this, -1, wxT("0"), wxDefaultPosition, 
				wxSize(80,-1), 0, 0, 100, 0 );
	forcetext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
				wxSize(80,-1), 0 );

	flexgrid->Add( doflabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( dofspin, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( forcelabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( forcetext, 0, wxALIGN_CENTER|wxALL, 5 );


	hbox->Add( flexgrid, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	hbox->Add( flistbox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

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

void AddForceDialog::LoadListBox ()
{
	wxString str;
	flistbox->Clear ();
	for (int i = 1; i <= tetra->nDOF; i++)
	{
		str = wxString::Format (wxT("%d %f"), i,	tetra->Forces[i]);
		flistbox->Append (str);
	}
}

void AddForceDialog::OnAddButton (wxCommandEvent& ce)
{
	int dof = dofspin->GetValue();
	REAL force;
	if (dof > 0 && dof <= tetra->nDOF && 
		(forcetext->GetValue()).ToDouble(&force))
	{
		if (tetra->AddForce (dof-1, force))
		{
			LoadListBox ();
			return;
		}
	}
	wxMessageDialog *md = new wxMessageDialog (this, wxT("Invalid values"), wxT("Error"), 
		wxOK | wxICON_ERROR);
	md->ShowModal ();
	delete md;
}

void AddForceDialog::OnRemoveButton (wxCommandEvent& ce)
{
	if (tetra->RemoveForce (flistbox->GetSelection()))
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
			wxT("Force Removed Successfully."), 
			wxT("Success"), wxOK | wxICON_INFORMATION);
		md->ShowModal ();
		delete md;

		LoadListBox ();
	}
	else
	{
		wxMessageDialog *md = new wxMessageDialog (this, wxT("Force Removal Failed."), 
			wxT("Failed"), wxOK | wxICON_ERROR);
		md->ShowModal ();
		delete md;
	}
}

void AddForceDialog::OnCloseButton (wxCommandEvent& ce)
{
	Close ();
}

BEGIN_EVENT_TABLE(AddForceDialog,wxDialog)
	EVT_BUTTON(ID_ADD_BUTTON,AddForceDialog::OnAddButton)
	EVT_BUTTON(ID_REMOVE_BUTTON,AddForceDialog::OnRemoveButton)
	EVT_BUTTON(ID_CLOSE_BUTTON,AddForceDialog::OnCloseButton)
END_EVENT_TABLE()

// TetraGUIFrame::AddForceDialog
//////////////////////////////////////////////////////////
