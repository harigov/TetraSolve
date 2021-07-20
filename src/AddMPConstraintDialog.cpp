#include "AddMPConstraintDialog.h"

//////////////////////////////////////////////////////////
// AddMPConstraintDialog

AddMPConstraintDialog::AddMPConstraintDialog (wxWindow *parent, Tetrahedron<REAL> *t)		: wxDialog (parent, -1, wxT("Add Multipoint Constraint"), wxDefaultPosition)
{
	tetra = t;
	wxBoxSizer *vbox = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *hbox = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *buttonsizer = new wxBoxSizer( wxHORIZONTAL );
	wxFlexGridSizer *flexgrid = new wxFlexGridSizer( 2, 0, 0 );

	wxStaticText *ilabel, *jlabel, *beta1label, *beta2label, *beta3label;
	wxButton *add, *remove, *close;

	ilabel = new wxStaticText( this, -1, wxT("i"));
	jlabel = new wxStaticText( this, -1, wxT("j"));
	beta1label = new wxStaticText( this, -1, wxT("BETA1"));
	beta2label = new wxStaticText( this, -1, wxT("BETA2"));	
	beta3label = new wxStaticText( this, -1, wxT("BETA3"));

	itext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
		wxSize(80,-1), 0 );
	jtext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
		wxSize(80,-1), 0 );
	beta1text = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
		wxSize(80,-1), 0 );
	beta2text = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
		wxSize(80,-1), 0 );
	beta3text = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
		wxSize(80,-1), 0 );

	mpclistbox = new wxListBox( this, -1, wxDefaultPosition, wxSize(120,100), 0, NULL, wxLB_SINGLE );

	add = new wxButton( this, ID_ADD_BUTTON, wxT("Add"));
	remove = new wxButton( this, ID_REMOVE_BUTTON, wxT("Remove"));
	close = new wxButton( this, ID_CLOSE_BUTTON, wxT("Close"));

	flexgrid->Add( ilabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( itext, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( jlabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( jtext, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( beta1label, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( beta1text, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( beta2label, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( beta2text, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( beta3label, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add( beta3text, 0, wxALIGN_CENTER|wxALL, 5 );

	hbox->Add( flexgrid, 0, wxALIGN_CENTER|wxALL, 5 );
	hbox->Add( mpclistbox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	vbox->Add( hbox, 0, wxALIGN_CENTER|wxALL, 5 );

	buttonsizer->Add( add, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add( remove, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add( close, 0, wxALIGN_CENTER|wxALL, 5 );

	vbox->Add( buttonsizer, 0, wxALIGN_CENTER|wxALL, 5 );

	SetSizer( vbox );
	vbox->SetSizeHints( this );

	LoadListBox ();
}

void AddMPConstraintDialog::LoadListBox ()
{
	wxString str;
	mpclistbox->Clear ();
	for (int i = 0; i < tetra->nMPC; i++)
	{
		str = wxString::Format (wxT("%d %4.1f %d %4.1f %d %4.1f"),i,
			tetra->MPCons[i].b1, tetra->MPCons[i].i,
			tetra->MPCons[i].b2, tetra->MPCons[i].j,
			tetra->MPCons[i].b3);
		mpclistbox->Append (str);
	}
}

void AddMPConstraintDialog::OnAddButton (wxCommandEvent& ce)
{
	REAL b1, b2, b3;
	long int i, j;
	if ((itext->GetValue ()).ToLong (&i) &&
		(jtext->GetValue ()).ToLong (&j) &&
		(beta1text->GetValue ()).ToDouble (&b1) &&
		(beta2text->GetValue ()).ToDouble (&b2) &&
		(beta3text->GetValue ()).ToDouble (&b3))
	{
		if (tetra->AddMPCons (b1, (int)i, b2, (int)j, b3))
		{
			mpclistbox->Append (wxString::Format (wxT("%d %4.1f %d %4.1f %d %4.1f"),
				tetra->nMPC-1, b1, (int)i, b2, (int)j, b3));
			return;
		}
	}
	wxMessageDialog *md = new wxMessageDialog (this, wxT("Invalid values"), wxT("Error"), 
		wxOK | wxICON_ERROR);
	md->ShowModal ();
	delete md;
}

void AddMPConstraintDialog::OnRemoveButton (wxCommandEvent& ce)
{
	if (tetra->RemoveMPCons (mpclistbox->GetSelection()))
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
			wxT("Multipoint constraint Removed Successfully."), 
			wxT("Success"), wxOK | wxICON_INFORMATION);
		md->ShowModal ();
		delete md;

		LoadListBox ();
	}
	else
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
			wxT("Multipoint constraint Removal Failed."), 
			wxT("Failed"), wxOK | wxICON_ERROR);
		md->ShowModal ();
		delete md;
	}
}

void AddMPConstraintDialog::OnCloseButton (wxCommandEvent& ce)
{
	Close ();
}

BEGIN_EVENT_TABLE(AddMPConstraintDialog,wxDialog)
	EVT_BUTTON(ID_ADD_BUTTON,AddMPConstraintDialog::OnAddButton)
	EVT_BUTTON(ID_REMOVE_BUTTON,AddMPConstraintDialog::OnRemoveButton)
	EVT_BUTTON(ID_CLOSE_BUTTON,AddMPConstraintDialog::OnCloseButton)
END_EVENT_TABLE()

// AddMPConstraintDialog
//////////////////////////////////////////////////////////
