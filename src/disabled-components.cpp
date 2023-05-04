#include "dunedaqdal/Application.hpp"
#include "dunedaqdal/ResourceSet.hpp"
#include "dunedaqdal/ResourceSetAND.hpp"
#include "dunedaqdal/ResourceSetOR.hpp"
#include "dunedaqdal/Session.hpp"
#include "dunedaqdal/util.hpp"
#include "dunedaqdal/disabled-components.hpp"

#include "logging/Logging.hpp"

#include "test_circular_dependency.hpp"

using namespace dunedaq::oksdbinterfaces;

dunedaq::dal::DisabledComponents::DisabledComponents(Configuration& db) :
  m_db(db),
  m_num_of_slr_enabled_resources(0),
  m_num_of_slr_disabled_resources(0)
{
  TLOG_DEBUG(2) <<  "construct the object " << (void *)this  ;
  m_db.add_action(this);
}

dunedaq::dal::DisabledComponents::~DisabledComponents()
{
  TLOG_DEBUG(2) <<  "destroy the object " << (void *)this ;
  m_db.remove_action(this);
}

void
dunedaq::dal::DisabledComponents::notify(std::vector<ConfigurationChange *>& /*changes*/) noexcept
{
  TLOG_DEBUG(2) <<  "reset session components because of notification callback on object " << (void *)this ;
  __clear();
}

void
dunedaq::dal::DisabledComponents::load() noexcept
{
  TLOG_DEBUG(2) <<  "reset session components because of configuration load on object " << (void *)this ;
  __clear();
}

void
dunedaq::dal::DisabledComponents::unload() noexcept
{
  TLOG_DEBUG(2) <<  "reset session components because of configuration unload on object " << (void *)this ;
  __clear();
}

void
dunedaq::dal::DisabledComponents::update(const ConfigObject& obj, const std::string& name) noexcept
{
  TLOG_DEBUG(2) <<  "reset session components because of configuration update (obj = " << obj << ", name = \'" << name << "\') on object " << (void *)this ;
  __clear();
}

void
dunedaq::dal::DisabledComponents::reset() noexcept
{
  TLOG_DEBUG(2) <<  "reset disabled by explicit user call" ;
  m_disabled.clear(); // do not clear s_user_disabled && s_user_enabled !!!
}

bool
dunedaq::dal::DisabledComponents::is_enabled(const dunedaq::dal::Component * c)
{
#if 0
  if (const dunedaq::dal::Segment * seg = c->cast<dunedaq::dal::Segment>())
    {
      if (dunedaq::dal::SegConfig * conf = seg->get_seg_config(false, true))
        {
          return !conf->is_disabled();
        }
    }
  else if (const dunedaq::dal::BaseApplication * app = c->cast<dunedaq::dal::BaseApplication>())
    {
      if (const dunedaq::dal::AppConfig * conf = app->get_app_config(true))
        {
          const dunedaq::dal::BaseApplication * base = conf->get_base_app();
          if (base != app && is_enabled_short(base->cast<dunedaq::dal::Component>()) == false)
            return false;
        }
    }
#endif
  return is_enabled_short(c);
}


void
dunedaq::dal::Session::set_disabled(const std::set<const dunedaq::dal::Component *>& objs) const
{
  m_disabled_components.m_user_disabled.clear();

  for (const auto& i : objs)
      m_disabled_components.m_user_disabled.insert(i);

  m_disabled_components.m_num_of_slr_disabled_resources = m_disabled_components.m_user_disabled.size();

  m_disabled_components.reset();

  //m_app_config.__clear();
}

void
dunedaq::dal::Session::set_enabled(const std::set<const dunedaq::dal::Component *>& objs) const
{
  m_disabled_components.m_user_enabled.clear();

  for (const auto& i : objs)
    m_disabled_components.m_user_enabled.insert(i);

  m_disabled_components.m_num_of_slr_enabled_resources = m_disabled_components.m_user_enabled.size();

  m_disabled_components.reset();

  //m_app_config.__clear();
}

void
dunedaq::dal::DisabledComponents::disable_children(const dunedaq::dal::ResourceSet& rs)
{
  for (auto & i : rs.get_Contains())
    {
      if (const dunedaq::dal::ResourceSet * rs2 = i->cast<dunedaq::dal::ResourceSet>())
        {
          disable_children(*rs2);
        }
    }
}

#if 0
void
dunedaq::dal::DisabledComponents::disable_children(const dunedaq::dal::Segment& s)
{
  for (auto & i : s.get_Resources())
    {
      if (const dunedaq::dal::ResourceSet * rs = i->cast<dunedaq::dal::ResourceSet>())
        {
          disable_children(*rs);
        }
    }

  for (auto & j : s.get_Segments())
    {
      TLOG_DEBUG(6) <<  "disable segment " << j << " because it's parent segment " << &s << " is disabled" ;
      disable(*j);
      disable_children(*j);
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace dunedaq {
  ERS_DECLARE_ISSUE_BASE(
    dal,
    ReadMaxAllowedIterations,
    AlgorithmError,
    "Has exceeded the maximum of iterations allowed (" << limit << ") during calculation of disabled objects",
    ,
    ((unsigned int)limit)
  )
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // fill data from resource sets

static void fill(
  const dunedaq::dal::ResourceSet& rs,
  std::vector<const dunedaq::dal::ResourceSetOR *>& rs_or,
  std::vector<const dunedaq::dal::ResourceSetAND *>& rs_and,
  dunedaq::dal::TestCircularDependency& cd_fuse
)
{
  if (const dunedaq::dal::ResourceSetAND * r1 = rs.cast<dunedaq::dal::ResourceSetAND>())
    {
      rs_and.push_back(r1);
    }
  else if (const dunedaq::dal::ResourceSetOR * r2 = rs.cast<dunedaq::dal::ResourceSetOR>())
    {
      rs_or.push_back(r2);
    }

  for (auto & i : rs.get_Contains())
    {
      dunedaq::dal::AddTestOnCircularDependency add_fuse_test(cd_fuse, i);
      if (const dunedaq::dal::ResourceSet * rs2 = i->cast<dunedaq::dal::ResourceSet>())
        {
          fill(*rs2, rs_or, rs_and, cd_fuse);
        }
    }
}

#if 0
  // fill data from segments

static void fill(
  const dunedaq::dal::Segment& s,
  std::vector<const dunedaq::dal::ResourceSetOR *>& rs_or,
  std::vector<const dunedaq::dal::ResourceSetAND *>& rs_and,
  dunedaq::dal::TestCircularDependency& cd_fuse
)
{
  for (auto & i : s.get_Resources())
    {
      dunedaq::dal::AddTestOnCircularDependency add_fuse_test(cd_fuse, i);
      if (const dunedaq::dal::ResourceSet * rs = i->cast<dunedaq::dal::ResourceSet>())
        {
          fill(*rs, rs_or, rs_and, cd_fuse);
        }
    }

  for (auto & j : s.get_Segments())
    {
      dunedaq::dal::AddTestOnCircularDependency add_fuse_test(cd_fuse, j);
      fill(*j, rs_or, rs_and, cd_fuse);
    }
}
#endif

  // fill data from session

static void fill(
  const dunedaq::dal::Session& p,
  std::vector<const dunedaq::dal::ResourceSetOR *>& rs_or,
  std::vector<const dunedaq::dal::ResourceSetAND *>& rs_and,
  dunedaq::dal::TestCircularDependency& cd_fuse
)
{
#if 0
  if (const dunedaq::dal::OnlineSegment * onlseg = p.get_OnlineInfrastructure())
    {
      dunedaq::dal::AddTestOnCircularDependency add_fuse_test(cd_fuse, onlseg);
      fill(*onlseg, rs_or, rs_and, cd_fuse);

      // NOTE: normally application may not be ResourceSet, but for some "exotic" cases put this code
      for (auto &a : p.get_OnlineInfrastructureApplications())
        {
          if (const dunedaq::dal::ResourceSet * rs = a->cast<dunedaq::dal::ResourceSet>())
            {
              fill(*rs, rs_or, rs_and, cd_fuse);
            }
        }
    }

  for (auto & i : p.get_Segments())
    {
      dunedaq::dal::AddTestOnCircularDependency add_fuse_test(cd_fuse, i);
      fill(*i, rs_or, rs_and, cd_fuse);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
dunedaq::dal::Component::disabled(const dunedaq::dal::Session& session, bool skip_check) const
{
  // fill disabled (e.g. after session changes)

  if (session.m_disabled_components.size() == 0) {
    if (session.get_disabled().empty() && 
        session.m_disabled_components.m_user_disabled.empty()) {
      return false;  // the session has no disabled components
    }
    else {
      // get two lists of all session's resource-set-or and resource-set-and
      // also test any circular dependencies between segments and resource sets
      dunedaq::dal::TestCircularDependency cd_fuse("component \'is-disabled\' status", &session);
      std::vector<const dunedaq::dal::ResourceSetOR *> rs_or;
      std::vector<const dunedaq::dal::ResourceSetAND *> rs_and;
      fill(session, rs_or, rs_and, cd_fuse);

      // calculate explicitly and implicitly (nested) disabled components
      {
        std::vector<const dunedaq::dal::Component *> vector_of_disabled;
        vector_of_disabled.reserve(session.get_disabled().size() + session.m_disabled_components.m_user_disabled.size());

        // add user disabled components, if any
        for (auto & i : session.m_disabled_components.m_user_disabled) {
          vector_of_disabled.push_back(i);
          TLOG_DEBUG(6) <<  "disable component " << i << " because it is explicitly disabled by user" ;
        }

        // add session-disabled components ignoring explicitly enabled by user
        for (auto & i : session.get_disabled()) {
          TLOG_DEBUG(6) <<  "check component " << i << " explicitly disabled in session" ;

          if (session.m_disabled_components.m_user_enabled.find(i) == session.m_disabled_components.m_user_enabled.end()) {
            vector_of_disabled.push_back(i);
            TLOG_DEBUG(6) <<  "disable component " << i << " because it is explicitly disabled in session" ;
          }
          else {
            TLOG_DEBUG(6) <<  "skip component " << i << " because it is enabled by user" ;
          }
        }

        // fill set of explicitly and implicitly (segment/resource-set containers) disabled components
        for (auto & i : vector_of_disabled) {
          session.m_disabled_components.disable(*i);

          if (const dunedaq::dal::ResourceSet * rs = i->cast<dunedaq::dal::ResourceSet>()) {
            session.m_disabled_components.disable_children(*rs);
          }
#if 0
          else if (const dunedaq::dal::Segment * seg = i->cast<dunedaq::dal::Segment>()) {
            session.m_disabled_components.disable_children(*seg);
          }
#endif
        }
      }

      for (unsigned long count = 1; true; ++count) {
        const unsigned long num(session.m_disabled_components.size());

        TLOG_DEBUG(6) <<  "before auto-disabling iteration " << count << " the number of disabled components is " << num ;

        for (const auto& i : rs_or) {
          if (session.m_disabled_components.is_enabled_short(i)) {
            // check ANY child is disabled
            for (auto & i2 : i->get_Contains()) {
              if (!session.m_disabled_components.is_enabled_short(i2)) {
                TLOG_DEBUG(6) <<  "disable resource-set-OR " << i << " because it's child " << i2 << " is disabled" ;
                session.m_disabled_components.disable(*i);
                session.m_disabled_components.disable_children(*i);
                break;
              }
            }
          }
        }

        for (const auto& j : rs_and) {
          if (session.m_disabled_components.is_enabled_short(j)) {
            const std::vector<const dunedaq::dal::ResourceBase*> &resources = j->get_Contains();

            if (!resources.empty()) {
              // check ANY child is enabled
              bool found_enabled = false;
              for (auto & j2 : resources) {
                if (session.m_disabled_components.is_enabled_short(j2)) {
                  found_enabled = true;
                  break;
                }
              }
              if (found_enabled == false) {
                TLOG_DEBUG(6) <<  "disable resource-set-AND " << j << " because all it's children are disabled" ;
                session.m_disabled_components.disable(*j);
                session.m_disabled_components.disable_children(*j);
              }
            }
          }
        }

        if (session.m_disabled_components.size() == num) {
          TLOG_DEBUG(6) <<  "after " << count << " iteration(s) auto-disabling algorithm found no newly disabled sets, exiting loop ..." ;
          break;
        }

        unsigned int iLimit(1000);
        if (count > iLimit) {
          ers::error(dunedaq::dal::ReadMaxAllowedIterations(ERS_HERE, iLimit));
          break;
        }
      }
    }
  }

  bool result(skip_check ? !session.m_disabled_components.is_enabled_short(this) : !session.m_disabled_components.is_enabled(this));
  TLOG_DEBUG( 6) <<  "disabled(" << this << ") returns " << std::boolalpha << result  ;
  return result;
}

unsigned long
dunedaq::dal::DisabledComponents::get_num_of_slr_resources(const dunedaq::dal::Session& session)
{
  return (session.m_disabled_components.m_num_of_slr_enabled_resources + session.m_disabled_components.m_num_of_slr_disabled_resources);
}
