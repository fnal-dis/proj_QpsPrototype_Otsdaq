# ======================================================================
#  otsdaq-demo main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/otsdaq-demo/ups/setup_for_development
#  buildtool
# ======================================================================


# use cmake 3.17 or later
cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

find_package(cetmodules)

project(qps-otsdaq-plugins VERSION 1.00.00 )

include(CetCMakeEnv)
cet_cmake_env()
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

cet_set_compiler_flags(DIAGS VIGILANT
 WERROR
 NO_UNDEFINED
 EXTRA_FLAGS -pedantic -D_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES=1 ${DT_FLAG}
 )

#cet_report_compiler_flags()

find_package(otsdaq 2_07_00 )
find_package(otsdaq_utilities 2_07_00 )
 
 # XDAQ Extra setup
# include_directories($ENV{XDAQ_ROOT}/include/linux $ENV{XDAQ_ROOT}/include)
#link_directories($ENV{XDAQ_ROOT}/lib)
#link_directories($ENV{ARTDAQ_DATABASE_LIB})
# set(XDAQ_BASIC_LIB_LIST
#   xcept
#   xoap
#   xdaq
#   toolbox
#   xerces-c
#   cgicc
# )
# set(XDAQ_XERCES-C xerces-c)
 
find_package(HDF5 REQUIRED COMPONENTS C HL CXX)

# Code
add_subdirectory(qps-otsdaq-plugins)

# Useful scripts
#add_subdirectory(tools)

# Test Programs
#add_subdirectory(test)

# Documentation directory
if( $ENV{OTS_DOXY} MATCHES "DOIT" )
	add_subdirectory(doc)
endif()

cet_cmake_config()
