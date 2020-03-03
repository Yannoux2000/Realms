cmake_minimum_required(VERSION 3.17)

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
endif()


file(GLOB_RECURSE DEP_LIBS    "${CMAKE_SOURCE_DIR}/ExtLibs/libs*/*")
file(GLOB_RECURSE DEP_BINS    "${CMAKE_SOURCE_DIR}/ExtLibs/libs-bin*/*")
file(GLOB_RECURSE DEP_INCLUDE "${CMAKE_SOURCE_DIR}/ExtLibs/include/*")

file(GLOB_RECURSE REALMSGL "${CMAKE_SOURCE_DIR}/Realms1/*")
file(GLOB_RECURSE REALMSVK "${CMAKE_SOURCE_DIR}/RealmsVK/*")
file(GLOB_RECURSE REALMSVK_UNITTESTS "${CMAKE_SOURCE_DIR}/UnitTests-Realms/*")

# project name
project(Realms)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    "${DEP_INCLUDE}"
)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/realms_gl")
add_executable(Realms_GL ${REALMSGL})

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/realms_vk")
add_executable(Realms_VK ${REALMSVK})

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/realms_vk_unittest")

gtest_add_tests(TARGET Realms_VK ${REALMSVK_UNITTESTS})
#set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 10)
