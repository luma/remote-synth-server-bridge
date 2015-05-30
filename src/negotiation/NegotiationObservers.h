#ifndef SYNTH_BRIDGE_NEGOTIATION_OBSERVERS_H_
#define SYNTH_BRIDGE_NEGOTIATION_OBSERVERS_H_
#pragma once

#include <string>
#include <functional>

#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/jsep.h"
#include "common/Logging.h"

typedef std::function<void (webrtc::SessionDescriptionInterface* desc)> CreateSDPCallback;
typedef std::function<void ()> SetSDPCallback;

// Jsep SetLocalDescription and SetRemoteDescription callback interface.
class SetSDPObserver: public webrtc::SetSessionDescriptionObserver {
 public:
  explicit SetSDPObserver() {}

  void SetCallback(SetSDPCallback fn) {
    callback_ = fn;
  }

 protected:
  ~SetSDPObserver() {}

  void OnSuccess() {
    INFO("SetSDPObserver::OnSuccess");
    if (callback_) callback_();
    CleanUp();
  }

  void OnFailure(const std::string& error) {
    ERROR(error.c_str());
    // @todo eat this for now
    CleanUp();
  }

 private:
  void CleanUp() {
    callback_ = nullptr;
  }

  SetSDPCallback callback_;
};

// JSEP CreateOffer and CreateAnswer callback interface.
class CreateSDPObserver: public webrtc::CreateSessionDescriptionObserver {
 public:
  explicit CreateSDPObserver() {}

  void SetCallback(CreateSDPCallback fn) {
    callback_ = fn;
  }

 protected:
  ~CreateSDPObserver() {}

  void OnSuccess(webrtc::SessionDescriptionInterface* desc) {
    INFO("CreateSDPObserver::OnSuccess");
    if (callback_) callback_(desc);
    CleanUp();
  }

  void OnFailure(const std::string& error) {
    ERROR(error.c_str());

    // @todo eat this for now
    CleanUp();
  }

 private:
  void CleanUp() {
    callback_ = nullptr;
  }

  CreateSDPCallback callback_;
};

#endif    // SYNTH_BRIDGE_NEGOTIATION_OBSERVERS_H_
