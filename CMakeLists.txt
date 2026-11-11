cmake_minimum_required(VERSION 3.23)

project(vgperf
    VERSION 0.2.0
    DESCRIPTION "2D Vector Graphics Benchmark"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

OPTION(ENABLE_VKVG "Enable vkvg." ON)
OPTION(ENABLE_CAIRO "Enable cairo." ON)

set(VGPERF_DATA_ROOT "${CMAKE_SOURCE_DIR}/data" CACHE STRING "Data path for tests")

# Build type defaults
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include(FetchContent)

FetchContent_Declare(
  argparse
  GIT_REPOSITORY https://github.com/p-ranav/argparse
  GIT_TAG        3eda91b2e1ce7d569f84ba295507c4cd8fd96910 #v3.2
)

FetchContent_MakeAvailable(argparse)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        "v3.11.3"
)
FetchContent_MakeAvailable(nlohmann_json)

if(ENABLE_PLUTOVG)
    FetchContent_Declare(
        plutovg
        GIT_REPOSITORY https://github.com/sammycage/plutovg.git
        GIT_TAG        "v1.3.2"
    )
    FetchContent_MakeAvailable(plutovg)
endif()

if(ENABLE_VKVG)
    FetchContent_Declare(
        vkvg
        GIT_REPOSITORY https://github.com/jpbruyere/vkvg.git
        GIT_TAG        "e76c8dead9d8334214713d156f85632165f65930"
    )
    SET(VKVG_BUILD_TESTS OFF)
    SET(ENABLE_GTESTS OFF)
    FetchContent_MakeAvailable(vkvg)
endif()

if(ENABLE_CAIRO)
    # 1. Try PkgConfig (standard on Linux/macOS)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(CAIRO IMPORTED_TARGET cairo>=1.16.0)
    endif()

    # 2. If not found, try CMake Config (vcpkg on Windows)
    if(NOT CAIRO_FOUND)
        find_package(cairo CONFIG QUIET)
        if(cairo_FOUND)
            if(TARGET cairo::cairo)
                # Alias to PkgConfig::CAIRO so the rest of the build works transparently
                if(NOT TARGET PkgConfig::CAIRO)
                    add_library(PkgConfig::CAIRO ALIAS cairo::cairo)
                endif()
                set(CAIRO_FOUND TRUE)
            endif()
        endif()
    endif()

    # 3. If still not found, try Local Deps (Preshing binaries)
    if(NOT CAIRO_FOUND)
        set(LOCAL_CAIRO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/deps/cairo/cairo-windows-1.17.2")
        if(EXISTS "${LOCAL_CAIRO_ROOT}/include/cairo.h")
            message(STATUS "Found local Cairo at: ${LOCAL_CAIRO_ROOT}")
            
            # Create imported target
            if(NOT TARGET cairo::cairo)
                add_library(cairo::cairo SHARED IMPORTED)
                
                if(MSVC)
                    set_target_properties(cairo::cairo PROPERTIES
                        IMPORTED_IMPLIB "${LOCAL_CAIRO_ROOT}/lib/x64/cairo.lib"
                        IMPORTED_LOCATION "${LOCAL_CAIRO_ROOT}/lib/x64/cairo.dll"
                        INTERFACE_INCLUDE_DIRECTORIES "${LOCAL_CAIRO_ROOT}/include"
                    )
                else()
                    # Assume x64 for now, adapt if needed
                    set_target_properties(cairo::cairo PROPERTIES
                        IMPORTED_IMPLIB "${LOCAL_CAIRO_ROOT}/lib/x64/cairo.lib"
                        IMPORTED_LOCATION "${LOCAL_CAIRO_ROOT}/lib/x64/cairo.dll"
                        INTERFACE_INCLUDE_DIRECTORIES "${LOCAL_CAIRO_ROOT}/include"
                    )
                endif()
            endif()

            # Alias
            if(NOT TARGET PkgConfig::CAIRO)
                add_library(PkgConfig::CAIRO ALIAS cairo::cairo)
            endif()
            
            set(CAIRO_FOUND TRUE)
            set(cairo_VERSION "1.17.2")
            
            # Save DLL location for copy step later
            set(LOCAL_CAIRO_DLL "${LOCAL_CAIRO_ROOT}/lib/x64/cairo.dll")
        endif()
    endif()

    if(CAIRO_FOUND)
        message(STATUS "Cairo found (Version: ${cairo_VERSION})")
    else()
        message(FATAL_ERROR "Cairo not found. Install libcairo2-dev (Linux), use Homebrew (macOS), or vcpkg/deps (Windows).")
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # add_compile_options(-Werror)
    endif()
elseif(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/WX)
    endif()
endif()

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif()

FILE(GLOB SRC
    "src/*.cpp"
    "src/ir/*.cpp"
    "src/tests/*.cpp"
)

FILE(GLOB HEADERS src/*.hpp)

if(ENABLE_VKVG)
    list(APPEND SRC src/backends/Vkvg.hpp)
    ADD_DEFINITIONS (-DENABLE_VKVG)
endif()
if(ENABLE_CAIRO)
    list(APPEND SRC src/backends/Cairo.hpp)
    ADD_DEFINITIONS (-DENABLE_CAIRO)
endif()

ADD_EXECUTABLE("${PROJECT_NAME}" ${SRC})

target_sources("${PROJECT_NAME}"
    PUBLIC FILE_SET HEADERS
        BASE_DIRS src
        FILES ${HEADERS}

)
target_include_directories("${PROJECT_NAME}" SYSTEM PUBLIC
    ${nlohmann_json_SOURCE_DIR}/include
)
target_link_libraries("${PROJECT_NAME}" PUBLIC
    vulkan
    argparse
    nlohmann_json
)

if(ENABLE_VKVG)
    target_link_libraries("${PROJECT_NAME}" PUBLIC
        vkvg
        vkh
    )
endif()
if(ENABLE_CAIRO)
    target_link_libraries("${PROJECT_NAME}" PUBLIC
        PkgConfig::CAIRO
    )
endif()
