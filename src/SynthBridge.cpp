#include <node.h>
#include <string>
#include "webrtc/base/ssladapter.h"
// #include "webrtc/system_wrappers/interface/field_trial_default.h"
#include "webrtc/system_wrappers/interface/field_trial.h"
#include "common/Logging.h"
#include "peer/Peer.h"

// static const char kFieldTrials[] = "";



// Clients of webrtc that do not want to configure field trials can link with
// this instead of providing their own implementation.
namespace webrtc {
namespace field_trial {
  std::string FindFullName(const std::string& name) {
    return std::string();
  }
}  // namespace field_trial
}  // namespace webrtc


void SetupWebRtc() {
  INFO((std::string("Logger level:") +
            std::to_string(rtc::LogMessage::GetMinLogSeverity())).c_str());

  // webrtc::field_trial::InitFieldTrialsFromString(nullptr);

  if (!rtc::InitializeSSL()) {
    // @todo log error "Failed to initialise SSL";
    // @todo something?
    return;
  }
  else {
    // @todo log info "Initialised SSL";
  }

  if (!rtc::InitializeSSLThread()) {
    // @todo log error "Failed to initialise SSL Thread";
    // @todo something?
    return;
  }
  else {
    // @todo log info "Initialised SSL Thread";
  }
}

// @todo call this
void TearDownWebRtc() {
  if (!rtc::CleanupSSL()) {
    // @todo log error "Failed to successfully clean up SSL";
  }
  else {
    // @todo log info "Cleaned up SSL";
  }
}

void InitAll(v8::Handle<v8::Object> exports) {
  SetupWebRtc();

  Peer::Init(exports);
}

NODE_MODULE(SynthBridge, InitAll)
