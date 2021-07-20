#include "AddNodeDialog.h"

//////////////////////////////////////////////////////////
// AddNodeDialog

AddNodeDialog::AddNodeDialog (wxWindow *parent, Tetrahedron<REAL> *t) : 
wxDialog (parent, -1, wxT("Add Node Data"), wxDefaultPosition)
{	
	wxStaticText *xlabel, *ylabel, *zlabel;

	tetra = t;

	wxBoxSizer *vbox = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer *hbox = new wxBoxSizer( wxHORIZONTAL );
	wxFlexGridSizer *flexgrid = new wxFlexGridSizer( 3, 0, 0, 0 );
	wxBoxSizer *buttonsizer = new wxBoxSizer( wxVERTICAL );	

	nodeslist = new wxListBox(this, -1, wxDefaultPosition, wxSize(240,100));
	vbox->Add( nodeslist, 0, wxALIGN_CENTER|wxALL, 5 );

	xlabel = new wxStaticText( this, -1, wxT("X"));
	ylabel = new wxStaticText( this, -1, wxT("Y"));
	zlabel = new wxStaticText( this, -1, wxT("Z"));

	xtext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
				wxSize(80,-1), 0 );
	ytext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
				wxSize(80,-1), 0 );
	ztext = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, 
				wxSize(80,-1), 0 );

	flexgrid->Add( xlabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(xtext, 0, wxALIGN_CENTER|wxALL, 5 );

	flexgrid->Add(ylabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(ytext, 0, wxALIGN_CENTER|wxALL, 5 );

	flexgrid->Add(zlabel, 0, wxALIGN_CENTER|wxALL, 5 );
	flexgrid->Add(ztext, 0, wxALIGN_CENTER|wxALL, 5 );

	hbox->Add(flexgrid, 0, wxALIGN_CENTER|wxALL, 10 );

	add = new wxButton( this, ID_ADD_BUTTON, wxT("Add"));
	remove = new wxButton( this, ID_REMOVE_BUTTON, wxT("Remove"));
	close = new wxButton( this, ID_CLOSE_BUTTON, wxT("Close"));

	buttonsizer->Add(add, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add(remove, 0, wxALIGN_CENTER|wxALL, 5 );
	buttonsizer->Add(close, 0, wxALIGN_CENTER|wxALL, 5 );

	hbox->Add(buttonsizer, 0, wxALIGN_CENTER|wxALL, 5 );

	vbox->Add(hbox, 0, wxALIGN_CENTER|wxALL, 5 );

	SetSizer( vbox );
	vbox->SetSizeHints (this);

	LoadListBox ();
}

void AddNodeDialog::LoadListBox ()
{
	wxString str;
	nodeslist->Clear ();
	for (int i = 1; i <= tetra->nNodes; i++)
	{
		str = wxString::Format (wxT("%d  x = %4.1f  y = %4.1f  z = %4.1f"), i, 
			tetra->Nodes[i].x, tetra->Nodes[i].y, tetra->Nodes[i].z);
		nodeslist->Append (str);
	}	
}

void AddNodeDialog::OnAddButton (wxCommandEvent& ce)
{
	int n;
	REAL x, y, z;

	/* Check if value in textboxes is valid */
	if ((xtext->GetValue()).ToDouble(&x) && 
		(ytext->GetValue()).ToDouble(&y) &&
		(ztext->GetValue()).ToDouble(&z))
	{
		n = tetra->nNodes + 1;
		if (tetra->AddNode (n, x, y, z))
		{
			wxString str = wxString::Format (wxT("%d  x = %4.1f y = %4.1f z = %4.1f"),
				n, x, y, z);
			nodeslist->Append (str);
			return;
		}
	}
	/* if not, print error message */
	wxMessageDialog *md = new wxMessageDialog (this, 
		wxT("Invalid values entered"), wxT("Error"), wxOK | wxICON_ERROR);
	md->ShowModal ();
	delete md;
}

void AddNodeDialog::OnRemoveButton (wxCommandEvent& ce)
{
	if (tetra->RemoveNode (nodeslist->GetSelection() + 1))
	{
		wxMessageDialog *md = new wxMessageDialog (this, 
			wxT("Node Removed Successfully."), wxT("Success"), 
			wxOK | wxICON_INFORMATION);
		md->ShowModal ();
		delete md;

		LoadListBox ();
	}
	else
	{
		wxMessageDialog *md = new wxMessageDialog (this, wxT("Node Removal Failed.\n"
			"Probable reasons could be an element "
			"or force or displacement is dependent on "
			"this node"), 
			wxT("Failed"), wxOK | wxICON_ERROR);
		md->ShowModal ();
		delete md;
	}
}

void AddNodeDialog::OnCloseButton (wxCommandEvent& ce)
{
	Close ();
}

BEGIN_EVENT_TABLE(AddNodeDialog,wxDialog)
	EVT_BUTTON(ID_ADD_BUTTON,AddNodeDialog::OnAddButton)
	EVT_BUTTON(ID_REMOVE_BUTTON,AddNodeDialog::OnRemoveButton)
	EVT_BUTTON(ID_CLOSE_BUTTON,AddNodeDialog::OnCloseButton)
END_EVENT_TABLE()

// TetraGUIFrame::AddNodeDialog
//////////////////////////////////////////////////////////
