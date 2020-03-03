cmake_minimum_required(VERSION 3.10)

include(GoogleTest)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_WINDOWS 1)
    set(LIBS_FOLDER "libs-msvc")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OS_UNIX 1)
    if(ANDROID)
        set(OS_ANDROID 1)
        set(LIBS_FOLDER "libs-android")
    else()
        set(OS_LINUX 1)
        set(LIBS_FOLDER "libs-linux")
    endif()

elseif(CMAKE_SYSTEM_NAME MATCHES "^k?FreeBSD$")
    set(OS_FREEBSD 1)
        set(LIBS_FOLDER "libs-freebds")

if(CMAKE_SYSTEM_NAME MATCHES "^OpenBSD$")
    set(OS_OPENBSD 1)
        set(LIBS_FOLDER "libs-openbsd")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(IOS)
        set(OS_IOS 1)
        set(LIBS_FOLDER "libs-ios")
    else()
        set(OS_MACOSX 1)
        set(LIBS_FOLDER "libs-macosx")
    endif()
endif()

elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(OS_ANDROID 1)
    set(LIBS_FOLDER "libs-android")
endif()



set(DEP_LIBS    "${CMAKE_SOURCE_DIR}/ExtLibs/${LIBS_FOLDER}/")
set(DEP_BINS    "${CMAKE_SOURCE_DIR}/ExtLibs/libs-bin/")
set(DEP_INCLUDE "${CMAKE_SOURCE_DIR}/ExtLibs/include/")

# project name
project(Realms)

find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    "${DEP_INCLUDE}"
    "${Vulkan_INCLUDE_DIRS}"
)

#Realms_GL
set(REALMSGL_ROOT "${CMAKE_SOURCE_DIR}/Realms1")
file(
    GLOB_RECURSE REALMSGL_SRC 
    LIST_DIRECTORIES false
    "${REALMSGL_ROOT}/*.cpp*"
    "${REALMSGL_ROOT}/*.inl*"
    "${REALMSGL_ROOT}/*.h*"
)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/realms_gl")
add_executable(Realms_GL ${REALMSGL_SRC})

target_link_libraries(Realms_GL 
    "lua535.lib"
    "opengl32.lib"
    "glew32s.lib"
    "gdi32.lib"
    "openal32.lib"
    "flac.lib"
    "vorbisenc.lib"
    "vorbisfile.lib"
    "vorbis.lib"
    "ogg.lib"
    "ws2_32.lib"
    "winmm.lib"
    "sfml-graphics-s-d.lib"
    "sfml-window-s-d.lib"
    "sfml-system-s-d.lib"
    "sfml-network-s-d.lib"
    "sfml-audio-s-d.lib"
    "lua53.lib"
)

foreach(_SRC IN ITEMS ${REALMSGL_SRC})
    get_filename_component(_SRC_PATH "${_SRC}" PATH)
    file(RELATIVE_PATH _SRC_PATH_REL "${REALMSGL_ROOT}" "${_SRC_PATH}")
    string(REPLACE "/" "\\" _GROUP_PATH "${_SRC_PATH_REL}")
    source_group("${_GROUP_PATH}" FILES "${_SRC}")
endforeach()

#Realms_VK
set(REALMSVK_ROOT "${CMAKE_SOURCE_DIR}/RealmsVK")

file(
    GLOB_RECURSE REALMSVK_SRC 
    LIST_DIRECTORIES false
    "${REALMSVK_ROOT}/*.cpp*"
    "${REALMSVK_ROOT}/*.inl*"
    "${REALMSVK_ROOT}/*.h*"
)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/realms_vk")
add_executable(Realms_VK ${REALMSVK_SRC})

target_link_libraries(Realms_VK Vulkan_LIBRARIES "lua535.lib")

foreach(_SRC IN ITEMS ${REALMSVK_SRC})
    get_filename_component(_SRC_PATH "${_SRC}" PATH)
    file(RELATIVE_PATH _SRC_PATH_REL "${REALMSVK_ROOT}" "${_SRC_PATH}")
    string(REPLACE "/" "\\" _GROUP_PATH "${_SRC_PATH_REL}")
    source_group("${_GROUP_PATH}" FILES "${_SRC}")
endforeach()

#Realms_vk_unittests

file(GLOB_RECURSE REALMSVK_UNITTESTS 
    "${CMAKE_SOURCE_DIR}/UnitTests-Realms/*.cpp"
    "${CMAKE_SOURCE_DIR}/UnitTests-Realms/*.h"
)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/realms_vk_unittests")
gtest_add_tests(TARGET Realms_VK ${REALMSVK_UNITTESTS})
#set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 10)
