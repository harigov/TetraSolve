#ifndef __DEBUG_HEADER__
#define __DEBUG_HEADER__

#include <wx/wx.h>

enum DLEVEL { D_EVERYTHING = 0, D_SELECTED, D_ERROR };

void debug_message (const wxString& msg, DLEVEL level = D_EVERYTHING);

#endif /* __DEBUG_HEADER__ */

