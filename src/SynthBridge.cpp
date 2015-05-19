#include <node.h>

#include "base/logging.h"
#include "base/ssladapter.h"

#include "PeerWrapper.h"

using namespace v8;

static const char *fieldTrials = NULL;

void SetupWebRtc() {
  webrtc::field_trial::InitFieldTrialsFromString(fieldTrials);

  if (!rtc::InitializeSSL(NULL)) {
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



void CreatePeer(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  PeerWrapper::NewInstance(args);
}

void InitAll(Handle<Object> exports) {
  SetupWebRtc();

  PeerWrapper::Init(exports);

  NODE_SET_METHOD(exports, "Peer", CreatePeer);
}

NODE_MODULE(SynthBridge, InitAll)
