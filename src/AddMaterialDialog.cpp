#include "AddMaterialDialog.h"

//////////////////////////////////////////////////////////
// AddMaterialDialog

AddMaterialDialog::AddMaterialDialog (wxWindow *parent, Tetrahedron<REAL> *t) 
				: wxDialog (parent, -1, wxT("Add Material"), wxDefaultPosition)
{
	tetra = t;

	wxBoxSizer *vbox = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *hbox = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer *buttonsizer = new wxBoxSizer( wxVERTICAL );
	wxFlexGridSizer *flexgrid = new wxFlexGridSizer( 2, 0, 0 );	

	mlistbox = new wxListBox( this, -1, wxDefaultPosition, 
				wxSize(80,100), 0, NULL, wxLB_SINGLE );
	vbox->Add( mlistbox, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxStaticText *elabel, *nulabel, *alphalabel;
	wxButton *add, *remove, *close;	

	elabel = new wxStaticText( this, -1, wxT("E"));
	nulabel = new wxStaticText( this, -1, wxT("Nu"));
	alphalabel = new wxStaticText( this, -1, wxT("Alpha"));

	etext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
				wxSize(80,-1), 0 );
	nutext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
				wxSize(80,-1), 0 );
	alphatext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
				wxSize(80,-1), 0 );

	flexgrid->Add(elabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(etext, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(nulabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(nutext, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(alphalabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(alphatext, 0, wxALIGN_CENTER|wxALL, 5 );

	hbox->Add(flexgrid, 0, wxALIGN_CENTER|wxALL, 5 );

	add = new wxButton( this, ID_ADD_BUTTON, wxT("Add"));
	remove = new wxButton( this, ID_REMOVE_BUTTON, wxT("Remove"));	
	close = new wxButton( this, ID_CLOSE_BUTTON, wxT("Close"));

	buttonsizer->Add(add, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add(remove, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add(close, 0, wxALIGN_CENTER|wxALL, 5 );

	hbox->Add(buttonsizer, 0, wxALIGN_CENTER|wxALL, 5 );

	vbox->Add(hbox, 0, wxALIGN_CENTER|wxALL, 5 );

	SetSizer( vbox );
	vbox->SetSizeHints( this );

	LoadListBox ();
}

void AddMaterialDialog::LoadListBox ()
{
	wxString str;
	mlistbox->Clear ();
	for (int i = 1; i <= tetra->nMaterials; i++)
	{
		str = wxString::Format (wxT("%d E=%4.2f Nu=%4.2f Alpha=%4.2f"), i, 
			tetra->Materials[i].E,
			tetra->Materials[i].Nu,
			tetra->Materials[i].Alpha);
		mlistbox->Append (str);
	}
}

void AddMaterialDialog::OnAddButton (wxCommandEvent& ce)
{
	REAL E, Nu, Alpha;

	if ((etext->GetValue()).ToDouble (&E) &&
		(alphatext->GetValue()).ToDouble (&Alpha) &&
		(nutext->GetValue()).ToDouble (&Nu))
	{
		if (tetra->AddMaterial (tetra->nMaterials+1, E, Nu, Alpha))
		{
			wxString str = wxString::Format (wxT("%d E=%4.2f Nu=%4.2f Alpha=%4.2f"), 
				tetra->nMaterials, 
				E, Nu, Alpha);
			mlistbox->Append (str);
			return;
		}
	}
	wxMessageDialog *md = new wxMessageDialog (this, wxT("Invalid values"), wxT("Error"), 
		wxOK | wxICON_ERROR);
	md->ShowModal ();
	delete md;
}

void AddMaterialDialog::OnRemoveButton (wxCommandEvent& ce)
{
	if (tetra->RemoveMaterial (mlistbox->GetSelection() + 1))
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
			wxT("Material Removed Successfully."), 
			wxT("Success"), wxOK | wxICON_INFORMATION);
		md->ShowModal ();
		delete md;

		LoadListBox ();
	}
	else
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
			wxT("Material Removal Failed.\n"
			"An element might be using this material"), 
			wxT("Failed"), wxOK | wxICON_ERROR);
		md->ShowModal ();
		delete md;
	}
}

void AddMaterialDialog::OnCloseButton (wxCommandEvent& ce)
{
	Close ();
}

BEGIN_EVENT_TABLE(AddMaterialDialog,wxDialog)
	EVT_BUTTON(ID_ADD_BUTTON,AddMaterialDialog::OnAddButton)
	EVT_BUTTON(ID_REMOVE_BUTTON,AddMaterialDialog::OnRemoveButton)
	EVT_BUTTON(ID_CLOSE_BUTTON,AddMaterialDialog::OnCloseButton)
END_EVENT_TABLE()

// AddMaterialDialog
//////////////////////////////////////////////////////////
