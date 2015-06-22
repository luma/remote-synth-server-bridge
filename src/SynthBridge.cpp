#include <node.h>
#include <string>
#include "webrtc/base/ssladapter.h"
#include "common/FieldTrial.h"
#include "common/Logging.h"
#include "negotiation/IceCandidate.h"
#include "devices/MediaDeviceInfo.h"
#include "devices/MediaStreamTrack.h"
#include "devices/MediaStream.h"
#include "devices/MediaDevices.h"
#include "devices/MediaDevicesWrapper.h"
#include "peer/DataChannel.h"
#include "peer/Peer.h"

void SetupWebRtc() {
  INFO("Logger level: %s",
          (std::to_string(rtc::LogMessage::GetMinLogSeverity())).c_str());

  webrtc::InitFieldTrialsFromString("");

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

  MediaStreamTrack::Init(exports);
  MediaStream::Init(exports);
  MediaDevicesWrapper::Init(exports);
  MediaDeviceInfo::Init(exports);
  IceCandidate::Init(exports);
  DataChannel::Init(exports);
  Peer::Init(exports);
}

NODE_MODULE(SynthBridge, InitAll)
