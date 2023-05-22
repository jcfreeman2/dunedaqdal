
cmake_minimum_required(VERSION 3.12)
project(dunedaqdal VERSION 0.0.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(okssystem REQUIRED)
find_package(genconfig REQUIRED)
find_package(ers REQUIRED)
find_package(logging REQUIRED)
find_package(oksdbinterfaces REQUIRED)


#find_package(Boost COMPONENTS unit_test_framework REQUIRED)

##############################################################################

daq_generate_dal(dunedaq.schema.xml
  NAMESPACE dunedaq::dal
  DUMP_OUTPUT cpp_dump_src)

daq_add_library(dalMethods.cpp
  test_circular_dependency.cpp disabled-components.cpp
  DAL
  LINK_LIBRARIES oksdbinterfaces::oksdbinterfaces okssystem::okssystem
  logging::logging dunedaqdal_oks)


daq_add_application(listApps list_apps.cxx
  LINK_LIBRARIES dunedaqdal oksdbinterfaces::oksdbinterfaces)

daq_add_application(disable_test disable_test.cxx TEST
  LINK_LIBRARIES dunedaqdal oksdbinterfaces::oksdbinterfaces logging::logging)
##############################################################################


# See https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-cmake/#daq_add_python_bindings

daq_add_python_bindings(*.cpp LINK_LIBRARIES dunedaqdal)


##############################################################################


# See https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-cmake/#daq_add_unit_test

#daq_add_unit_test(Placeholder_test LINK_LIBRARIES)  # Placeholder_test should be replaced with real unit tests

##############################################################################

daq_install()

