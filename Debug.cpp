#include "Debug.h"

static const DLEVEL minDLevel = D_SELECTED;

void debug_message (const wxString& msg, DLEVEL level)
{
	if (level == D_ERROR)
	{
		wxMessageBox (msg, wxT("ERROR"), wxOK);
		wxTheApp->ExitMainLoop ();
	}
	if (level >= minDLevel)
	{
		wxLogMessage (msg);
	}
}
