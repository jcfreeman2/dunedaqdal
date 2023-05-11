
#include "oksdbinterfaces/Configuration.hpp"

#include "dunedaqdal/Component.hpp"
#include "dunedaqdal/DaqApplication.hpp"
#include "dunedaqdal/DaqModule.hpp"
#include "dunedaqdal/Segment.hpp"
#include "dunedaqdal/Session.hpp"

#include <iostream>
#include <string>

using namespace dunedaq;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " session database-file\n";
    return 0;
  }
  std::string confimpl = "oksconfig:" + std::string(argv[2]);
  auto confdb = new oksdbinterfaces::Configuration(confimpl);

  std::string sessionName(argv[1]);
  auto session = confdb->get<dal::Session>(sessionName);
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
