#include <map>
#include <string>
// #include "webrtc/system_wrappers/interface/field_trial_default.h"
#include "webrtc/system_wrappers/interface/field_trial.h"

namespace webrtc {
  namespace {
    // Clients of this library have show a clear intent to setup field trials by
    // linking with it. As so try to crash if they forget to call
    // InitFieldTrialsFromString before webrtc tries to access a field trial.
    bool field_trials_initiated_ = false;
    std::map<std::string, std::string> field_trials_;
  }  // namespace

  namespace field_trial {
    std::string FindFullName(const std::string& trial_name) {
      assert(field_trials_initiated_);
      std::map<std::string, std::string>::const_iterator it =
          field_trials_.find(trial_name);
      if (it == field_trials_.end())
        return std::string();
      return it->second;
    }
  }  // namespace field_trial

  // Note: this code is copied from src/base/metrics/field_trial.cc since the aim
  // is to mimic chromium --force-fieldtrials.
  void InitFieldTrialsFromString(const std::string& trials_string) {
    static const char kPersistentStringSeparator = '/';
    // Catch an error if this is called more than once.
    assert(field_trials_initiated_ == false);
    field_trials_initiated_ = true;
    if (trials_string.empty()) return;
    size_t next_item = 0;
    while (next_item < trials_string.length()) {
      size_t name_end = trials_string.find(kPersistentStringSeparator, next_item);
      if (name_end == trials_string.npos || next_item == name_end)
        break;
      size_t group_name_end = trials_string.find(kPersistentStringSeparator,
                                                 name_end + 1);
      if (group_name_end == trials_string.npos || name_end + 1 == group_name_end)
        break;
      std::string name(trials_string, next_item, name_end - next_item);
      std::string group_name(trials_string, name_end + 1,
                             group_name_end - name_end - 1);
      next_item = group_name_end + 1;
      // Fail if duplicate with different group name.
      if (field_trials_.find(name) != field_trials_.end() &&
          field_trials_.find(name)->second != group_name)
        break;
      field_trials_[name] = group_name;
      // Successfully parsed all field trials from the string.
      if (next_item == trials_string.length())
        return;
    }
    // LOG does not prints when this is called early on main.
    fprintf(stderr, "Invalid field trials string.\n");
    // Using abort so it crashes both in debug and release mode.
    abort();
  }

}   // namespace webrtc
