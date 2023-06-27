cmake_minimum_required(VERSION 3.5)

project(QuakeImpHack LANGUAGES C)

FIND_PACKAGE ( PkgConfig REQUIRED )
find_package(SDL2 REQUIRED)

INCLUDE_DIRECTORIES ( ${GTK_INCLUDE_DIRS} )

include_directories(${SDL2_INCLUDE_DIRS})

add_library(QuakeImpHack SHARED main.c game_utils.h)
TARGET_LINK_LIBRARIES(QuakeImpHack ${CMAKE_THREAD_LIBS_INIT} ${QuakeImpHack_LIBS} ${CMAKE_DL_LIBS} ${SDL2_LIBRARIES})


install(TARGETS QuakeImpHack
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
