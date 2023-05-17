/**
 * @file module.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "dunedaqdal/DaqApplication.hpp"
#include "dunedaqdal/HostResource.hpp"

namespace py = pybind11;

namespace dunedaq::dal::python {

extern void
register_dal_methods(py::module&);

PYBIND11_MODULE(_daq_dunedaqdal_py, m)
{

  m.doc() = "C++ implementation of the dunedaqdal modules";
#if 0
  py::class_<dunedaq::dal::DaqApplication>(m,"DaqApplication")
    .def(py::init<oksdbinterfaces::Configuration& , const oksdbinterfaces::ConfigObject&>())
    .def("get_used_hostresources", &dunedaq::dal::DaqApplication::get_used_hostresources);
  py::class_<dunedaq::dal::HostResource>(m,"HostResource");
#endif
  register_dal_methods(m);
}

} // namespace dunedaq::dunedaqdal::python
