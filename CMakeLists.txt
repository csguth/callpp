cmake_minimum_required(VERSION 3.0.2)

project(callpp VERSION 0.1)

macro(ADD_OSX_FRAMEWORK fwname target)
    find_library(FRAMEWORK_${fwname}
    NAMES ${fwname}
    PATHS ${CMAKE_OSX_SYSROOT}/System/Library
    PATH_SUFFIXES Frameworks
    NO_DEFAULT_PATH)
    if( ${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
        MESSAGE(ERROR ": Framework ${fwname} not found")
    else()
        TARGET_LINK_LIBRARIES(${target} "${FRAMEWORK_${fwname}}")
        MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
    endif()
endmacro(ADD_OSX_FRAMEWORK)


macro(SETUP_MACOSX_BUNDLE target)

    ADD_OSX_FRAMEWORK(AudioUnit ${target})
    ADD_OSX_FRAMEWORK(AudioToolbox ${target})
    ADD_OSX_FRAMEWORK(CoreAudio ${target})
    ADD_OSX_FRAMEWORK(CoreFoundation ${target})
    ADD_OSX_FRAMEWORK(Foundation ${target})
    ADD_OSX_FRAMEWORK(Network ${target})
    ADD_OSX_FRAMEWORK(Security ${target})

    set_target_properties(${target} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/apps/${target}.plist
    )


endmacro(SETUP_MACOSX_BUNDLE)

# This little macro lets you set any XCode specific property
macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_library(callpp
callpp/Account.cpp
callpp/Account.hpp
callpp/Call.cpp
callpp/Call.hpp
callpp/Endpoint.cpp
callpp/Endpoint.hpp
callpp/Hook.cpp
callpp/Hook.hpp
callpp/PjAccount.cpp
callpp/PjAccount.hpp
callpp/PjCall.cpp
callpp/PjCall.hpp
callpp/RegisteringEvent.hpp
callpp/SignalingEvent.hpp
callpp/Uid.cpp
callpp/Uid.hpp
callpp/Uri.cpp
callpp/Uri.hpp
)
target_include_directories(callpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(callpp PUBLIC CONAN_PKG::pjproject)

add_executable(hello MACOSX_BUNDLE apps/hello.cpp)
target_link_libraries(hello callpp)

add_executable(caller MACOSX_BUNDLE apps/caller.cpp)
target_link_libraries(caller callpp)

if (APPLE)

    SETUP_MACOSX_BUNDLE(hello)
    SETUP_MACOSX_BUNDLE(caller)

    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")    

endif()

set_property(TARGET hello PROPERTY CXX_STANDARD 20)
set_property(TARGET caller PROPERTY CXX_STANDARD 20)
set_property(TARGET callpp PROPERTY CXX_STANDARD 20)

install(TARGETS hello DESTINATION bin)
install(TARGETS caller DESTINATION bin)
