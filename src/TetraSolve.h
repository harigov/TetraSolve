#include <wx/wx.h>

#include "Debug.h"

#define ERR_MSG(msg) debug_message (wxT(msg), D_ERROR)

#ifndef NDEBUG
#	define DBG_MSG(msg) debug_message (wxT(msg), D_EVERYTHING)
#else
#	define DBG_MSG(msg)
#endif

#ifndef NDEBUG
#	define SEL_MSG(msg) debug_message (wxT(msg), D_SELECTED)
#else
#	define SEL_MSG(msg)
#endif

typedef double REAL;
