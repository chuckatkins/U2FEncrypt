
if(NOT LibU2F_FIND_COMPONENTS)
  set(LibU2F_FIND_COMPONENTS Host Server)
  set(LibU2F_FIND_REQUIRED_Host TRUE)
  set(LibU2F_FIND_REQUIRED_Server TRUE)
endif()

set(_LibU2F_REQUIRED_VARS)

if(Host IN_LIST LibU2F_FIND_COMPONENTS)
  find_path(LibU2F_Host_INCLUDE_DIR u2f-host.h PATH_SUFFIXES u2f-host)
  find_library(LibU2F_Host_LIBRARY u2f-host)
  if(LibU2F_Host_INCLUDE_DIR AND LibU2F_Host_LIBRARY)
    set(LibU2F_Host_FOUND TRUE)
  endif()
  if(LibU2F_FIND_REQUIRED_Host)
    list(APPEND _LibU2F_REQUIRED_VARS
      LibU2F_Host_INCLUDE_DIR LibU2F_Host_LIBRARY
    )
  endif()
endif()

if(Server IN_LIST LibU2F_FIND_COMPONENTS)
  find_path(LibU2F_Server_INCLUDE_DIR u2f-server.h PATH_SUFFIXES u2f-server)
  find_library(LibU2F_Server_LIBRARY u2f-server)
  if(LibU2F_Server_INCLUDE_DIR AND LibU2F_Server_LIBRARY)
    set(LibU2F_Server_FOUND TRUE)
  endif()
  if(LibU2F_FIND_REQUIRED_Server)
    list(APPEND _LibU2F_REQUIRED_VARS
      LibU2F_Server_INCLUDE_DIR LibU2F_Server_LIBRARY
    )
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibU2F
  FOUND_VAR LibU2F_FOUND
  REQUIRED_VARS LibU2F_FIND_COMPONENTS
  HANDLE_COMPONENTS
)

if(LibU2F_FOUND)
  set(LibU2F_INCLUDE_DIRS)
  set(LibU2F_LIBRARIES)

  if(LibU2F_Host_FOUND)
    list(APPEND LibU2F_INCLUDE_DIRS "${LibU2F_Host_INCLUDE_DIR}")
    list(APPEND LibU2F_LIBRARIES "${LibU2F_Host_LIBRARY}")
    if(NOT TARGET LibU2F::Host)
      add_library(LibU2F::Host UNKNOWN IMPORTED)
      set_target_properties(LibU2F::Host PROPERTIES
        IMPORTED_LOCATION             "${LibU2F_Host_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibU2F_Host_INCLUDE_DIR}"
      )
    endif()
  endif()

  if(LibU2F_Server_FOUND)
    list(APPEND LibU2F_INCLUDE_DIRS "${LibU2F_Server_INCLUDE_DIR}")
    list(APPEND LibU2F_LIBRARIES "${LibU2F_Server_LIBRARY}")
    if(NOT TARGET LibU2F::Server)
      add_library(LibU2F::Server UNKNOWN IMPORTED)
      set_target_properties(LibU2F::Server PROPERTIES
        IMPORTED_LOCATION             "${LibU2F_Server_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibU2F_Server_INCLUDE_DIR}"
      )
    endif()
  endif()
endif()
