# Generated by Boost 1.71.0

# address-model=64

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  _BOOST_SKIPPED("libboost_context.a" "64 bit, need 32")
  return()
endif()

# layout=system

# toolset=clang-darwin80

# link=static

if(DEFINED Boost_USE_STATIC_LIBS)
  if(NOT Boost_USE_STATIC_LIBS)
    _BOOST_SKIPPED("libboost_context.a" "static, Boost_USE_STATIC_LIBS=${Boost_USE_STATIC_LIBS}")
    return()
  endif()
else()
  if(NOT WIN32)
    _BOOST_SKIPPED("libboost_context.a" "static, default is shared, set Boost_USE_STATIC_LIBS=ON to override")
    return()
  endif()
endif()

# runtime-link=shared

if(Boost_USE_STATIC_RUNTIME)
  _BOOST_SKIPPED("libboost_context.a" "shared runtime, Boost_USE_STATIC_RUNTIME=${Boost_USE_STATIC_RUNTIME}")
  return()
endif()

# runtime-debugging=on

if(NOT "${Boost_USE_DEBUG_RUNTIME}" STREQUAL "" AND NOT Boost_USE_DEBUG_RUNTIME)
  _BOOST_SKIPPED("libboost_context.a" "debug runtime, Boost_USE_DEBUG_RUNTIME=${Boost_USE_DEBUG_RUNTIME}")
  return()
endif()

# threading=multi

# variant=debug

if(NOT "${Boost_USE_DEBUG_LIBS}" STREQUAL "" AND NOT Boost_USE_DEBUG_LIBS)
  _BOOST_SKIPPED("libboost_context.a" "debug, Boost_USE_DEBUG_LIBS=${Boost_USE_DEBUG_LIBS}")
  return()
endif()

if(Boost_VERBOSE OR Boost_DEBUG)
  message(STATUS "  [x] libboost_context.a")
endif()

# Target file name: libboost_context.a

get_target_property(__boost_imploc Boost::context IMPORTED_LOCATION_DEBUG)
if(__boost_imploc)
  message(WARNING "Target Boost::context already has an imported location '${__boost_imploc}', which will be overwritten with '${_BOOST_LIBDIR}/libboost_context.a'")
endif()
unset(__boost_imploc)

set_property(TARGET Boost::context APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)

set_target_properties(Boost::context PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG CXX
  IMPORTED_LOCATION_DEBUG "${_BOOST_LIBDIR}/libboost_context.a"
  )

list(APPEND _BOOST_CONTEXT_DEPS headers)
