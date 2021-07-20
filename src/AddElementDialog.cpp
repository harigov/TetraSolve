#include "AddElementDialog.h"

//////////////////////////////////////////////////////////
// AddElementDialog

AddElementDialog::AddElementDialog (wxWindow *parent, Tetrahedron<REAL> *t) : 
wxDialog (parent, -1, wxT("Add Element"), wxDefaultPosition)
{
	tetra = t;

	wxBoxSizer *vbox = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *hbox = new wxBoxSizer( wxHORIZONTAL );
	wxFlexGridSizer *flexgrid = new wxFlexGridSizer( 2, 0, 0 );
	wxBoxSizer *buttonsizer = new wxBoxSizer( wxHORIZONTAL );	

	wxStaticText *n1label, *n2label, *n3label, *n4label, *matlabel, *templabel;

	n1label = new wxStaticText(this, -1, wxT("Node 1"));
	n2label = new wxStaticText(this, -1, wxT("Node 2"));
	n3label = new wxStaticText(this, -1, wxT("Node 3"));
	n4label = new wxStaticText(this, -1, wxT("Node 4"));	
	matlabel = new wxStaticText(this, -1, wxT("Material"));
	templabel = new wxStaticText(this, -1, wxT("Temp"));

	n1combo = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, 
				wxSize(100,-1), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY);
	n2combo = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, 
				wxSize(100,-1), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY);	
	n3combo = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, 
				wxSize(100,-1), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY);
	n4combo = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, 
				wxSize(100,-1), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY);
	matcombo = new wxComboBox(this, -1, wxT(""), wxDefaultPosition, 
				wxSize(100,-1), 0, NULL, wxCB_DROPDOWN|wxCB_READONLY );	

	temptext = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxSize(100,-1));
	elistbox = new wxListBox(this, -1, wxDefaultPosition, wxSize(150,100));	

	wxButton *add = new wxButton(this, ID_ADD_BUTTON, wxT("Add"));
	wxButton *remove = new wxButton(this, ID_REMOVE_BUTTON, wxT("Remove"));	
	wxButton *close = new wxButton(this, ID_CLOSE_BUTTON, wxT("Close"));
	close->SetDefault();

	flexgrid->Add(n1label, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(n1combo, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(n2label, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(n2combo, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(n3label, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(n3combo, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(n4label, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(n4combo, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(matlabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(matcombo, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(templabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(temptext, 0, wxALIGN_CENTER|wxALL, 5 );

	hbox->Add(flexgrid, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	hbox->Add(elistbox, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	buttonsizer->Add(add, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add(remove, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add(close, 0, wxALIGN_CENTER|wxALL, 5 );	

	vbox->Add(hbox, 0, wxALIGN_CENTER|wxALL, 5 );
	vbox->Add(buttonsizer, 0, wxALIGN_CENTER|wxALL, 5 );

	SetSizer(vbox);
	vbox->SetSizeHints (this);

	LoadListBox ();

	wxString str;
	/* add all nodes into n1, n2, n3, n4 combos */
	for (int i = 1; i <= tetra->nNodes; i++)
	{
		str = wxString::Format (wxT("%d(%f,%f,%f)"), i, 
			tetra->Nodes[i].x,
			tetra->Nodes[i].y,
			tetra->Nodes[i].z);
		n1combo->Append (str);
		n2combo->Append (str);
		n3combo->Append (str);
		n4combo->Append (str);
	}

	/* add all materials in matcombo */
	for (int i = 1; i <= tetra->nMaterials; i++)
	{
		str = wxString::Format (wxT("%d(E=%4.2f,Nu=%4.2f,Alpha=%4.2f)"),i,
			tetra->Materials[i].E,
			tetra->Materials[i].Nu,
			tetra->Materials[i].Alpha);
		matcombo->Append (str);
	}
}

void AddElementDialog::LoadListBox ()
{
	wxString str;
	elistbox->Clear ();
	/* add all previous elements */
	for (int i = 1; i <= tetra->nElements; i++)
	{
		str = wxString::Format (wxT("%d n1=%d n2=%d n3=%d n4=%d mat=%d temp=%f"), i,
			tetra->Elements[i].Nodes[0],tetra->Elements[i].Nodes[1],
			tetra->Elements[i].Nodes[2],tetra->Elements[i].Nodes[3],
			tetra->Elements[i].Material,tetra->Elements[i].Temp);
		elistbox->Append (str);
	}	
}

void AddElementDialog::OnAddButton (wxCommandEvent& ce)
{
	int n1, n2, n3, n4, mat;
	REAL temp;

	n1 = n1combo->GetSelection ();
	n2 = n2combo->GetSelection ();
	n3 = n3combo->GetSelection ();
	n4 = n4combo->GetSelection ();
	mat = matcombo->GetSelection ();

	if((temptext->GetValue()).ToDouble (&temp) &&
		n1 != wxNOT_FOUND && n2 != wxNOT_FOUND && 
		n3 != wxNOT_FOUND && n3 != wxNOT_FOUND &&
		mat != wxNOT_FOUND && n1 != n2 && n1 != n3 && n1 != n4)
	{
		/*because nodes start at 1 */
		n1++;n2++;n3++;n4++;mat++;
		if (tetra->AddElement (tetra->nElements+1,mat,temp,n1,n2,n3,n4))
		{
			wxString str = wxString::Format 
					(wxT("%d n1=%d n2=%d n3=%d n4=%d mat=%d temp=%f"), 
					tetra->nElements,	n1, n2, n3, n4, mat, temp);
			elistbox->Append (str);
			return;
		}
	}
	/* if not, print error message */
	wxMessageDialog *md = new wxMessageDialog (this, wxT("Invalid values entered"), 
					wxT("Error"), wxOK | wxICON_ERROR);
	md->ShowModal ();
	delete md;
}

void AddElementDialog::OnRemoveButton (wxCommandEvent& ce)
{
	if (tetra->RemoveElement (elistbox->GetSelection() + 1))
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
				wxT("Element Removed Successfully."), 
				wxT("Success"), wxOK | wxICON_INFORMATION);
		md->ShowModal ();
		delete md;

		LoadListBox ();
	}
	else
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
				wxT("Element Removal Failed.\n"), 
				wxT("Failed"), wxOK | wxICON_ERROR);
		md->ShowModal ();
		delete md;
	}
}

void AddElementDialog::OnCloseButton (wxCommandEvent& ce)
{
	Close ();
}

BEGIN_EVENT_TABLE(AddElementDialog,wxDialog)
	EVT_BUTTON(ID_ADD_BUTTON,AddElementDialog::OnAddButton)
	EVT_BUTTON(ID_REMOVE_BUTTON,AddElementDialog::OnRemoveButton)
	EVT_BUTTON(ID_CLOSE_BUTTON,AddElementDialog::OnCloseButton)
END_EVENT_TABLE()

// AddElementDialog
//////////////////////////////////////////////////////////
