#include <node.h>
#include <string>
#include "webrtc/base/ssladapter.h"
// #include "webrtc/system_wrappers/interface/field_trial_default.h"
#include "webrtc/system_wrappers/interface/field_trial.h"
#include "common/Logging.h"
#include "negotiation/IceCandidate.h"
#include "devices/MediaDevices.h"
#include "devices/MediaDeviceInfo.h"
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
  INFO("Logger level: %s",
          (std::to_string(rtc::LogMessage::GetMinLogSeverity())).c_str());

  // webrtc::field_trial::InitFieldTrialsFromString(nullptr);

  if (!rtc::InitializeSSL()) {
    ERROR("Failed to initialise SSL");
    // @todo something?
    return;
  } else {
    INFO("Initialised SSL");
  }

  if (!rtc::InitializeSSLThread()) {
    ERROR("Failed to initialise SSL Thread");
    // @todo something?
    return;
  } else {
    INFO("Initialised SSL Thread");
  }
}

// @todo call this
void TearDownWebRtc() {
  if (!rtc::CleanupSSL()) {
    ERROR("Failed to successfully clean up SSL");
  } else {
    INFO("Cleaned up SSL");
  }
}

void InitAll(v8::Handle<v8::Object> exports) {
  SetupWebRtc();

  MediaDevices::Init(exports);
  MediaDeviceInfo::Init(exports);
  IceCandidate::Init(exports);
  Peer::Init(exports);
}

NODE_MODULE(SynthBridge, InitAll)
