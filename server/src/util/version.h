#pragma once

#include "util.h"
// DO NOT TOUCH THIS FILE: it is auto incremented during build, it will default 
//                         to bumping the build version. You can still 
//                         override it by passing -DVERSION_PART=major, 
//                         -DVERSION_PART=minor, or -DVERSION_PART=patch as 
//                         needed
#define MAJOR 0
#define MINOR 0
#define PATCH 0
#define REVISION 8
#define BUILD 1437

#define GIT_HASH "c2af1b9"

#define k_version_str m_str(MAJOR) "." m_str(MINOR) "." m_str(PATCH) "." m_str(REVISION) "-" m_str(BUILD) " (" GIT_HASH ")" 