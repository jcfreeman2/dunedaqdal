#include "logging/Logging.hpp"

#include "oksdbinterfaces/Configuration.hpp"

#include "dunedaqdal/Component.hpp"
#include "dunedaqdal/DaqApplication.hpp"
#include "dunedaqdal/DaqModule.hpp"
#include "dunedaqdal/Session.hpp"

#include <iostream>
#include <string>

using namespace dunedaq;

void listApps(const dal::Session* session) {
  for (auto app : session->get_all_applications()) {
    std::cout << "Application: " << app->UID();
    if (app->disabled(*session)) {
      std::cout << "<disabled>";
    }
    else {
      auto daqApp = app->cast<dal::DaqApplication>();
      if (daqApp) {
        std::cout << " Modules:";
        for (auto mod : daqApp->get_contains()) {
          std::cout << " " << mod->UID();
          if (mod->disabled(*session)) {
            std::cout << "<disabled>";
          }
        }
      }
    }
    std::cout << std::endl;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " session database-file\n";
    return 0;
  }

  dunedaq::logging::Logging::setup();

  std::string confimpl = "oksconfig:" + std::string(argv[2]);
  auto confdb = new oksdbinterfaces::Configuration(confimpl);

  std::string sessionName(argv[1]);
  auto session = confdb->get<dal::Session>(sessionName);

  auto disabled = session->get_disabled();
  std::cout << "Currently " << disabled.size() << " items disabled: ";
  for (auto item : disabled) {
    std::cout << " " << item->UID();
  }
  std::cout << std::endl;
  listApps(session);

  std::cout << "======\nNow trying to set enabled  \n";
  std::set<const dal::Component*> enable;
  for (auto item : disabled) {
    enable.insert(item);
  }
  session->set_enabled(enable);
  listApps(session);

  std::cout << "======\nNow trying to set enabled to an empty list\n";
  enable.clear();
  session->set_enabled(enable);
  listApps(session);

  std::cout << "======\nNow trying to set disabled to an empty list \n";
  session->set_disabled({});
  listApps(session);
}
