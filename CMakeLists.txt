cmake_minimum_required(VERSION 2.6)
project(Computergrafik)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MODULE_PATH 
  "${Computergrafik_SOURCE_DIR}/cmake"
)

#change the default installation path
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${Computergrafik_SOURCE_DIR}/Abgabe" CACHE PATH "Computergrafik install prefix" FORCE)
endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_INSTALL_PREFIX}")
set(CPACK_GENERATOR "ZIP")
include(Dependencies)
include(Install)
include(CreateLaunchers)

add_definitions(/DFREEIMAGE_LIB /DTW_STATIC)
include_directories(${Computergrafik_SOURCE_DIR})
#disable win32 warning
add_definitions(/D_CRT_SECURE_NO_WARNINGS -DFREEGLUT_LIB_PRAGMAS=0 -DFREEGLUT_STATIC /DTW_STATIC /DTW_NO_LIB_PRAGMA)

add_subdirectory(ImageLoader)



# Die Projekte hinzufügen
add_subdirectory(A1_Versuch1b)
add_subdirectory(A1_Versuch1c)
add_subdirectory(A1_Bildverarbeitung)
add_subdirectory(A3_Normalenvektoren)

install(DIRECTORY Texturen DESTINATION .)

include(CPack)