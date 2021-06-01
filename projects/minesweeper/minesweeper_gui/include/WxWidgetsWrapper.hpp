#include "utils/Warnings.hpp"

PRAGMA_WARNING_PUSH_MSVC()
PRAGMA_WARNING_IGNORE_MSVC(WARNING_MSVC_THIS_FUNCTION_OR_VARIABLE_MAY_BE_UNSAFE)
PRAGMA_WARNING_IGNORE_MSVC(WARNING_MSVC_OPERATOR_DEPRECATED_BETWEEN_ENUMERATIONS_OF_DIFFERENT_TYPE)
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
  #include <wx/artprov.h>
  #include <wx/dcbuffer.h>
  #include <wx/mstream.h>
  #include <wx/wx.h>
#endif
PRAGMA_WARNING_POP_MSVC()