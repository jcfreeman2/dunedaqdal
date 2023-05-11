/**
 * @file dal_methods.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "pybind11/operators.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "dunedaqdal/Application.hpp"
#include "dunedaqdal/Session.hpp"


#include <sstream>

namespace py = pybind11;
using namespace dunedaq::oksdbinterfaces;

namespace dunedaq::dal::python {

//std::vector<const dunedaq::dal::Application*>
std::vector<std::string>
session_get_all_applications(const Configuration& db,
                             const std::string& session_name) {
  auto session=const_cast<Configuration&>(db).get<dunedaq::dal::Session>(session_name);
  std::vector<std::string> apps;
  for (auto app : session->get_all_applications()) {
    apps.push_back(app->UID());
  }
  return apps;
}

  bool component_disabled(const Configuration& db, const std::string& session_id, const std::string& component_id) {
    const dunedaq::dal::Component* component_ptr = const_cast<Configuration&>(db).get<dunedaq::dal::Component>(component_id);
    const dunedaq::dal::Session* session_ptr = const_cast<Configuration&>(db).get<dunedaq::dal::Session>(session_id);

    return component_ptr->disabled(*session_ptr);
  }

void
register_dal_methods(py::module& m)
{
  m.def("session_get_all_applications", &session_get_all_applications, "Get list of applications in the requested session");
  m.def("component_disabled", &component_disabled, "Determine if a Component-derived object (e.g. a Segment) has been disabled");
}

} // namespace dunedaq::dunedaqdal::python
