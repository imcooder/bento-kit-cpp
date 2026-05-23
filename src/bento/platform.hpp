#pragma once

/// Platform and feature flags for conditional APIs.
///
/// Prefer these macros over ad-hoc `_WIN32` checks so UWP / mobile behavior stays consistent.

#if defined(_WIN32)
#define BENTO_PLATFORM_WINDOWS 1
#if defined(WINAPI_FAMILY_PARTITION)
#include <winapifamily.h>
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#define BENTO_PLATFORM_UWP 1
#else
#define BENTO_PLATFORM_UWP 0
#endif
#else
#define BENTO_PLATFORM_UWP 0
#endif
#else
#define BENTO_PLATFORM_WINDOWS 0
#define BENTO_PLATFORM_UWP 0
#endif

#if defined(__ANDROID__)
#define BENTO_PLATFORM_ANDROID 1
#else
#define BENTO_PLATFORM_ANDROID 0
#endif

#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define BENTO_PLATFORM_IOS 1
#define BENTO_PLATFORM_OSX 0
#else
#define BENTO_PLATFORM_IOS 0
#define BENTO_PLATFORM_OSX 1
#endif
#else
#define BENTO_PLATFORM_IOS 0
#define BENTO_PLATFORM_OSX 0
#endif

#if defined(__linux__) && !BENTO_PLATFORM_ANDROID
#define BENTO_PLATFORM_LINUX 1
#else
#define BENTO_PLATFORM_LINUX 0
#endif

#if defined(__EMSCRIPTEN__)
#define BENTO_PLATFORM_EMSCRIPTEN 1
#else
#define BENTO_PLATFORM_EMSCRIPTEN 0
#endif

/// UTF-16 `wchar_t` mask overloads (desktop Windows only, not UWP).
#if BENTO_PLATFORM_WINDOWS && !BENTO_PLATFORM_UWP
#define BENTO_HAS_WSTRING_MASK 1
#else
#define BENTO_HAS_WSTRING_MASK 0
#endif
