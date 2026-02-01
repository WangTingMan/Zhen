#pragma once

#if __has_include(<boost/signals2.hpp>)
#include <boost/signals2.hpp>
#define USE_BOOST_ORG_LIB
#define boost_ns boost
#elif __has_include(<boost_signals2/signals2.hpp>)

#include <boost_signals2/signals2.hpp>
#define USE_SIGNAL2_ALONE_LIB
#define boost_ns boist

#else

#error "no boost signals header folder!"

#endif

#if defined(WIN32) || defined(_MSC_VER)

#if defined(LIBZHEN_IMPLEMENTATION)
#define LIBZHEN_API __declspec(dllexport)
#else
#define LIBZHEN_API __declspec(dllimport)
#endif  // defined(LIBZHEN_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(LIBZHEN_IMPLEMENTATION)
#define LIBZHEN_API __attribute__((visibility("default")))
#else
#define LIBZHEN_API
#endif  // defined(LIBZHEN_IMPLEMENTATION)
#endif
