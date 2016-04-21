
cmake_minimum_required(VERSION 3.5)
project(RadOpenGL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES rad.cpp room.cpp draw.cpp)

# find OpenGL and Glut libraries
INCLUDE (FindPackageHandleStandardArgs)
FIND_PACKAGE(PkgConfig ${gl_FIND_REQUIRED} ${gl_FIND_QUIETLY})
IF (PKG_CONFIG_FOUND)
    SET(PKG_CONFIG_PATH_ENV_VAR $ENV{PKG_CONFIG_PATH})
    IF (NOT PKG_CONFIG_PATH_ENV_VAR)
        IF (gl_FIND_REQUIRED)
            MESSAGE (FATAL_ERROR "Environment variable PKG_CONFIG_PATH not set. Setting this variable is required in order for pkg-config to locate installed software packages.")
        ENDIF (gl_FIND_REQUIRED)
    ENDIF (NOT PKG_CONFIG_PATH_ENV_VAR)
    PKG_CHECK_MODULES (gl gl)
    IF (gl_FOUND)
        SET(gl_LIBRARY ${gl_LIBRARIES})
        SET(gl_INCLUDE_DIR ${gl_INCLUDEDIR})
        SET(gl_LIBRARY_DIR ${gl_LIBRARY_DIRS})
        IF (NOT gl_FIND_QUIETLY)
            MESSAGE(STATUS "    includedir: ${gl_INCLUDE_DIR}")
            MESSAGE(STATUS "    librarydir: ${gl_LIBRARY_DIR}")
        ENDIF (NOT gl_FIND_QUIETLY)
    ENDIF(gl_FOUND)
ENDIF (PKG_CONFIG_FOUND)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(gl DEFAULT_MSG gl_LIBRARY gl_INCLUDE_DIR)

add_executable(RadOpenGL ${SOURCE_FILES})

target_link_libraries(RadOpenGL glut)
target_link_libraries(RadOpenGL GL)

