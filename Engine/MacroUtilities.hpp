#pragma once

#define DO_ONCE_BEGIN { static bool __first = true; if (__first) { __first = false; 

#define DO_ONCE_END }}

#define DO_ONCE(BODY) DO_ONCE_BEGIN BODY; DO_ONCE_END
