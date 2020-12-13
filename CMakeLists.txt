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

# This little macro lets you set any XCode specific property
macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(hello MACOSX_BUNDLE apps/hello.cpp)
target_link_libraries(hello CONAN_PKG::pjproject)

if (APPLE)

    ADD_OSX_FRAMEWORK(AudioUnit hello)
    ADD_OSX_FRAMEWORK(AudioToolbox hello)
    ADD_OSX_FRAMEWORK(CoreAudio hello)
    ADD_OSX_FRAMEWORK(CoreFoundation hello)
    ADD_OSX_FRAMEWORK(Foundation hello)
    ADD_OSX_FRAMEWORK(Network hello)
    ADD_OSX_FRAMEWORK(Security hello)

    set_target_properties(hello PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/apps/Info.plist
    )

    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")    

endif()

set_property(TARGET hello PROPERTY CXX_STANDARD 20)

install(TARGETS hello DESTINATION bin)
