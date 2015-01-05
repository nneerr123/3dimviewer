# - Try to find GEOS
# Once done this will define
#  
#  GEOS_FOUND        - system has GLEW
#  GEOS_INCLUDE_DIR  - the GLEW include directory
#  GEOS_LIBRARY_DIR  - where the libraries are
#  GEOS_LIBRARY      - Link these to use GLEW
#
#

IF (GEOS_INCLUDE_DIR)
  # Already in cache, be silent
  SET(GEOS_FIND_QUIETLY TRUE)
ENDIF (GEOS_INCLUDE_DIR)

# Initialize the search path
SET( GEOS_DIR_SEARCH "" )

# User defined environment variable GEOS_ROOT_DIR
IF( EXISTS $ENV{GEOS_ROOT_DIR} )
  FILE( TO_CMAKE_PATH $ENV{GEOS_ROOT_DIR} GEOS_DIR_SEARCH)
  SET( GEOS_DIR_SEARCH ${GEOS_DIR_SEARCH} ${GEOS_DIR_SEARCH}/include )
ENDIF( EXISTS $ENV{GEOS_ROOT_DIR} )

# User defined CMake cache variable
IF( GEOS_ROOT_DIR )
  SET( GEOS_DIR_SEARCH ${GEOS_DIR_SEARCH} ${GEOS_ROOT_DIR} ${GEOS_ROOT_DIR}/include )
ENDIF( GEOS_ROOT_DIR )

FIND_PATH(GEOS_INCLUDE_DIR
    NAMES geos/geos.h
    PATHS
    ${GEOS_DIR_SEARCH}
    "${TRIDIM_MOOQ_DIR}"
    "${TRIDIM_MOOQ_DIR}/GEOS"
    "${TRIDIM_3RDPARTY_DIR}/include"
)

SET(GEOS_DEBUG_NAMES geosd)
FIND_LIBRARY(GEOS_DEBUG_LIBRARY
    NAMES
        ${GEOS_DEBUG_NAMES}
    PATHS
      ${GEOS_DIR_SEARCH}
      "${TRIDIM_MOOQ_DIR}/lib"
      "${TRIDIM_MOOQ_DIR}/GEOS/lib"
      "${TRIDIM_3RDPARTY_DIR}/lib"
)

SET(GEOS_NAMES geos)
FIND_LIBRARY(GEOS_LIBRARY
    NAMES
        ${GEOS_NAMES}
    PATHS
      ${GEOS_DIR_SEARCH}
      "${TRIDIM_MOOQ_DIR}/lib"
      "${TRIDIM_MOOQ_DIR}/GEOS/lib"
      "${TRIDIM_3RDPARTY_DIR}/lib"
)

if( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
  set( GEOS_LIBRARY
    optimized ${GEOS_LIBRARY}
    debug ${GEOS_DEBUG_LIBRARY}
    )
endif()

MARK_AS_ADVANCED(GEOS_LIBRARY GEOS_DEBUG_LIBRARY GEOS_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set GEOS_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GEOS DEFAULT_MSG GEOS_INCLUDE_DIR GEOS_LIBRARY)

IF (GEOS_FOUND)
    SET(GEOS_INCLUDE_DIRS ${GEOS_INCLUDE_DIR})
    SET(GEOS_LIBRARIES    ${GEOS_LIBRARY})
ENDIF()

IF( NOT GEOS_FOUND )
  SET( GEOS_ROOT_DIR "" CACHE PATH "" )
ENDIF( NOT GEOS_FOUND )
