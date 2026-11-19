cmake_minimum_required(VERSION 3.16)
project(dimse-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# DCMTK is compiled out-of-tree. DCMTK_LIB is the install prefix:
#   $DCMTK_LIB/include/dcmtk/...
#   $DCMTK_LIB/lib/libdcmnet.so ...
if(NOT DEFINED ENV{DCMTK_LIB} AND NOT DCMTK_LIB)
  message(FATAL_ERROR
    "DCMTK_LIB is not set.\n"
    "  export DCMTK_LIB=/path/to/dcmtk/install\n"
    "  or: cmake -DDCMTK_LIB=/path/to/dcmtk/install ..")
endif()

if(NOT DCMTK_LIB)
  set(DCMTK_LIB "$ENV{DCMTK_LIB}")
endif()

get_filename_component(DCMTK_LIB "${DCMTK_LIB}" ABSOLUTE)

set(DCMTK_INCLUDE_DIR "${DCMTK_LIB}/include")
set(DCMTK_LIBRARY_DIR "${DCMTK_LIB}/lib")

if(NOT EXISTS "${DCMTK_INCLUDE_DIR}/dcmtk/config/osconfig.h")
  message(FATAL_ERROR
    "DCMTK headers not found under ${DCMTK_INCLUDE_DIR}/dcmtk.\n"
    "Expected install prefix layout with include/ and lib/.")
endif()

if(NOT EXISTS "${DCMTK_LIBRARY_DIR}")
  message(FATAL_ERROR "DCMTK library directory not found: ${DCMTK_LIBRARY_DIR}")
endif()

message(STATUS "DCMTK_LIB        = ${DCMTK_LIB}")
message(STATUS "DCMTK include    = ${DCMTK_INCLUDE_DIR}")
message(STATUS "DCMTK libraries  = ${DCMTK_LIBRARY_DIR}")

add_executable(dimse-engine
  src/main.cpp
  src/cstore_scu.cpp
  src/dicom_validator.cpp
)

target_include_directories(dimse-engine
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DCMTK_INCLUDE_DIR}
)

# Shared libs from the out-of-tree DCMTK build.
find_library(DCMTK_OFSTD_LIB  ofstd  HINTS "${DCMTK_LIBRARY_DIR}" REQUIRED)
find_library(DCMTK_OFLOG_LIB  oflog  HINTS "${DCMTK_LIBRARY_DIR}" REQUIRED)
find_library(DCMTK_DCMDATA_LIB dcmdata HINTS "${DCMTK_LIBRARY_DIR}" REQUIRED)
find_library(DCMTK_DCMNET_LIB dcmnet HINTS "${DCMTK_LIBRARY_DIR}" REQUIRED)

target_link_libraries(dimse-engine
  PRIVATE
    ${DCMTK_DCMNET_LIB}
    ${DCMTK_DCMDATA_LIB}
    ${DCMTK_OFLOG_LIB}
    ${DCMTK_OFSTD_LIB}
    pthread
)

# Embed rpath so the binary loads DCMTK .so files from $DCMTK_LIB/lib at runtime.
set_target_properties(dimse-engine PROPERTIES
  BUILD_RPATH   "${DCMTK_LIBRARY_DIR}"
  INSTALL_RPATH "${DCMTK_LIBRARY_DIR}"
)

target_compile_definitions(dimse-engine
  PRIVATE
    HAVE_CONFIG_H
)

install(TARGETS dimse-engine RUNTIME DESTINATION bin)
