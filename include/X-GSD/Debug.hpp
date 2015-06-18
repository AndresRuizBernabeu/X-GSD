/* Debug.hpp - This header file contains macros for making debugging messages easier and only available
 if DEBUG is defined. This is useful because you can get a Release build (with no DEBUG variable defined)
 in which all debug messaging code disappears, gaining runtime speed and no need to delete/comment manually
 all those std::cout lines.
*/
#pragma once

#include <iostream>
#include <string.h>

#define INFO_MAX_LENGTH 40

// Macro for shortening the __FILE__ result. It hides the path and only shows the file name
#define FILE_WITHOUT_PATH (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DEBUG_INFO(os) { std::string info = "DBG: "; info.append(FILE_WITHOUT_PATH); info.append("("); \
info.append(std::to_string(__LINE__)); info.append("): "); \
(os) << info; \
int infoLength = (int)info.length(); \
for (int i = 0; i < INFO_MAX_LENGTH - infoLength; i++){ (os) << " ";}\
}\

// Macro for debugging a certain variable to a specific output stream
#ifdef DEBUG
#define DBGVAR( os, var ) do { \
DEBUG_INFO(os)\
(os) << #var << " = [" << (var) << "]" << std::endl; } while( false )
#else
#define DBGVAR( os, var ) do { } while ( false )
#endif

// Macro for debugging with a message to a specific output stream
#ifdef DEBUG
#define DBGMSG( os, msg ) do { \
DEBUG_INFO(os)\
(os) << msg << std::endl; } while( false )
#else
#define DBGMSG( os, msg ) do { } while ( false )
#endif

// Macro for debugging a certain variable to standard output stream
#ifdef DEBUG
#define DBGVARC( var ) do { \
DEBUG_INFO(std::cout)\
std::cout << #var << " = [" << (var) << "]" << std::endl; } while( false )
#else
#define DBGVARC( var ) do { } while ( false )
#endif

// Macro for debugging with a message to standard output stream
#ifdef DEBUG
#define DBGMSGC( msg ) do { \
DEBUG_INFO(std::cout)\
std::cout << msg << std::endl; } while( false )
#else
#define DBGMSGC( msg ) do { } while ( false )
#endif

// TODO: DETAILED_DEBUG (environment variable used for debug messages which would flood the output, so that you can disable only these. For example, COLLISION DBGMSGC on ComponentCollider.cpp) versions of the macros above (adding timestamp).
//#ifdef DETAILED_DEBUG
//#define DDBGVAR( os , var )
//#endif
