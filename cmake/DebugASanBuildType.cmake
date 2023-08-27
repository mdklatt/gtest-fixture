# Define 'DebugASan' build type to compile with AddressSanitizer support.
# Adapted from example in <https://crascit.com/professional-cmake>.
# <https://github.com/google/sanitizers/wiki/AddressSanitizer>

set(name DebugASan)

get_property(multiconfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(multiconfig)
    if(NOT ${name} IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES ${name})
    endif()
else()
    set(types Debug Release RelWithDebInfo MinSizeRel ${name} )
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${types}")
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST types)
        message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif()

# The example in _Professional CMake_ suggests regular CACHE variables here.
# However, the project() command seems to define these variables as empty when
# CMAKE_BUILD_TYPE is defined as "DebugASan", so they need FORCE.

string(TOUPPER "${name}" name)
set(flags "-fsanitize=address")
set(CMAKE_C_FLAGS_${name} "${CMAKE_C_FLAGS_DEBUG} ${flags}"
    CACHE STRING "" FORCE
)
set(CMAKE_CXX_FLAGS_${name} "${CMAKE_CXX_FLAGS_DEBUG} ${flags}"
    CACHE STRING "" FORCE
)
