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
#define REVISION 16
#define BUILD 1457

#define GIT_HASH "bdf6d80"

#define k_version_str m_str(MAJOR) "." m_str(MINOR) "." m_str(PATCH) "." m_str(REVISION) "-" m_str(BUILD) " (" GIT_HASH ")" 