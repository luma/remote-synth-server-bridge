#ifndef SYNTH_BRIDGE_NEGOTIATOR_H_
#define SYNTH_BRIDGE_NEGOTIATOR_H_
#pragma once

#include <string>
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/jsep.h"
#include "negotiation/NegotiationHandlerInterface.h"
#include "negotiation/NegotiationObservers.h"

class Negotiator {
 public:
  explicit Negotiator(NegotiationHandlerInterface* eventHandler)
    : eventHandler_(eventHandler),
      createSDPObserver_(new rtc::RefCountedObject<CreateSDPObserver>()),
      setLocalSDPObserver_(new rtc::RefCountedObject<SetSDPObserver>()),
      setRemoteSDPObserver_(new rtc::RefCountedObject<SetSDPObserver>()) {}

  void SetPC(rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc) {
    pc_ = pc;
  }

  void AddIceCandidate (const std::string mid, int mLineIndex, const std::string &sdp);
  void CreateOffer();
  void AddRemoteAnswer(const std::string &sdp);

  ~Negotiator() {
    eventHandler_ = NULL;
    pc_ = NULL;
  }

 protected:
  void SetLocalDescription(webrtc::SessionDescriptionInterface* desc, SetSDPCallback callback);
  webrtc::SessionDescriptionInterface* ParseSDP(const std::string &type, const std::string &sdpText);

 private:
  NegotiationHandlerInterface *eventHandler_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc_;
  rtc::scoped_refptr<CreateSDPObserver> createSDPObserver_;
  rtc::scoped_refptr<SetSDPObserver> setLocalSDPObserver_;
  rtc::scoped_refptr<SetSDPObserver> setRemoteSDPObserver_;
};

#endif    // SYNTH_BRIDGE_NEGOTIATOR_H_
