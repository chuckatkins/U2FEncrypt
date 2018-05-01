cmake_minimum_required(VERSION 3.6)

project(U2FEncrypt VERSION 0.1 LANGUAGES CXX)

# Some boilerplate to setup nice output directories
set(CMAKE_INSTALL_BINDIR bin CACHE STRING "Installation runtime subdirectory")
set(CMAKE_INSTALL_LIBDIR lib CACHE STRING "Installation library subdirectory")
set(CMAKE_INSTALL_INCLUDEDIR include
  CACHE STRING "Installation include subdirectory")
mark_as_advanced(CMAKE_INSTALL_BINDIR)
mark_as_advanced(CMAKE_INSTALL_LIBDIR)
mark_as_advanced(CMAKE_INSTALL_INCLUDEDIR)
mark_as_advanced(CMAKE_INSTALL_CMAKEDIR)

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
  )
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
  )
endif()
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
  )
endif()
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/CMake)

find_package(LibU2F COMPONENTS Host Server REQUIRED)

add_library(LibU2FCxx
  Base64.cxx     Base64.h
  Server.cxx     Server.h
  Host.cxx       Host.h
  Exceptions.cxx Exceptions.cxx
)
target_link_libraries(LibU2FCxx PRIVATE LibU2F::Host LibU2F::Server)

add_executable(U2FEncrypt main.cxx)
target_link_librarieS(U2FEncrypt LibU2FCxx)
