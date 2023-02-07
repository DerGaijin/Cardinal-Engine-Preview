#pragma once
#include "BuildOptions.h"


// Platform Values
#define CE_PLATFORM_UNKNOWN 0
#define CE_PLATFORM_WINDOWS 1
#define CE_PLATFORM_APPLE 2
#define CE_PLATFORM_LINUX 3
#define CE_PLATFORM_ANDROID 4


#if CE_EDITOR
#define CE_STANDALONE 0
#else
#define CE_STANDALONE 1
#endif


// Choose Platform Definitions
#if defined _WIN32 || defined _WIN64
#define CE_PLATFORM CE_PLATFORM_WINDOWS
#define CE_DYNAMIC_EXPORT __declspec(dllexport)
#define CE_DYNAMIC_IMPORT __declspec(dllimport)

#elif defined __APPLE__ || defined __MACH__
#error Platform Currently not supported
#define CE_PLATFORM CE_PLATFORM_APPLE

#elif defined __ANDROID__
#error Platform Currently not supported
#define CE_PLATFORM CE_PLATFORM_ANDROID

#elif defined __linux__ || defined linux || defined __linux
#error Platform Currently not supported
#define CE_PLATFORM CE_PLATFORM_LINUX

#else
#error Unsupported Platform
#define CE_PLATFORM CE_PLATFORM_UNKNOWN
#define CE_DYNAMIC_EXPORT
#define CE_DYNAMIC_IMPORT

#endif


// Define API Import/Exports
#ifdef CE_DLL
#ifdef CE_BUILD
#define CE_API CE_DYNAMIC_EXPORT
#else
#define CE_API CE_DYNAMIC_IMPORT
#endif // CE_BUILD
#else
#define CE_API
#endif // CE_DLL


// Current Engine Version
#define CE_VERSION_MAJOR 0
#define CE_VERSION_MINOR 8
#define CE_VERSION_PATCH 0


// Utility Defines
#define _CE_CONCAT(A, B) A##B
#define CE_CONCAT(A, B) _CE_CONCAT(A, B)
