#!/usr/bin/env python3

import sys

import dunedaqdal
import oksdbinterfaces

# Process a dal::Parameter object, placing key/value pairs in a dictionary
def process_parameters(parameters, envDict):
  for item in parameters:
    if item.className() == 'VariableSet':
      for v in item.contains:
        envDict[v.name] = v.value
    else:
      if item.className() == 'Variable':
        envDict[item.name] = item.value
 
# Recursively process all Segments in given Segment extracting Applications
def process_segment(db, session, segment, controller=None):
  if segment.controller:
    controller = segment.controller
  if controller:
    controller_id = controller.id
  else:
    controller_id = 'root-controller'

  # Recurse over nested segments
  for seg in segment.segments:
    process_segment(seg, controller)

  # Get all the enabled applications of this segment
  for app in segment.applications:
    print()
    if not dunedaqdal.component_disabled(db._obj, session.id, app.id):
      print(f"Controller: {controller_id}, App: {app}")
      appenv = {}
      # Get default environment from Session
      process_parameters(session.environment, appenv)
      # Override with any app specific environment from Application
      process_parameters(app.applicationEnvironment, appenv)
      print(f"Application environment={appenv}")
    else:
      print(f"Ignoring disabled app {app.id}")

def main():
  db = oksdbinterfaces.Configuration("oksconfig:" + sys.argv[1])
  session_name = sys.argv[2]
  session = db.get_dal(class_name="Session", uid=session_name)
  #apps = dunedaqdal.session_get_all_applications(db._obj, session_name)
  #for apploc in apps:
  #  app = db.get_dal(apploc.class_name, apploc.id)

  environment = {}
  process_parameters(session.environment, environment)
  print(f"Session environment={environment}")

  for seg in session.segments:
    process_segment(db, session, seg)

if __name__ == '__main__':
    main()
