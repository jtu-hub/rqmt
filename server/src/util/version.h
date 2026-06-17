#pragma once

#include "util.h"
// DO NOT TOUCH THIS FILE: it is auto incremented during build, it will default 
//                         to bumping the build version. You can still 
//                         override it by passing -DVERSION_PART=major, 
//                         -DVERSION_PART=minor, or -DVERSION_PART=patch as 
//                         needed
#define MAJOR 0
#define MINOR 0
#define PATCH 1
#define REVISION 12
#define BUILD 1455

#define GIT_HASH "178d61e"

#define k_version_str m_str(MAJOR) "." m_str(MINOR) "." m_str(PATCH) "." m_str(REVISION) "-" m_str(BUILD) " (" GIT_HASH ")" 