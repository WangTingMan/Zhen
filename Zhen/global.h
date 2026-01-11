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
