#pragma once

// Plateforme Linux
#if defined linux || defined __linux__ || defined __linux
#  define RLMS_PLATFORM_LINUX
#  define RLMS_PLATFORM_NAME "Linux"
#endif

// Plateforme Windows
#if defined _WIN32 || defined WIN32 || defined __NT__ || defined __WIN32__
#  define RLMS_PLATFORM_WIN
#  define RLMS_PLATFORM_NAME "Windows"
#endif

// Plateforme MacOS X
#if ( defined __MWERKS__ && defined __powerc && !defined macintosh ) || defined __APPLE_CC__ || defined macosx
#  define RLMS_PLATFORM_MACOSX
#  define RLMS_PLATFORM_NAME "MacOS X"
#endif

#ifdef _DEBUG
#define RLMS_DEBUG
#endif

#ifdef NDEBUG
#define RLMS_DEBUG
#endif

#ifdef RLMS_DEBUG
#define RLMS_DEBUG_BOOL true
#endif // RLMS_DEBUG

#ifndef RLMS_DEBUG
#define RLMS_DEBUG_BOOL false
#endif // RLMS_DEBUG
