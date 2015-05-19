#ifndef NEGOTIATION_INTERFACE_H_
#define NEGOTIATION_INTERFACE_H_
#pragma once

#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/peerconnectionfactory.h"

class NegotiationInterface {
 public:
  virtual ~NegotiationInterface() {}

  virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) = 0;
  virtual void OnOffer(webrtc::SessionDescriptionInterface* desc) = 0;
  virtual void OnAnswer(webrtc::SessionDescriptionInterface* desc) = 0;
  virtual void OnRenegotiationNeeded() = 0;
};


#endif    // NEGOTIATION_INTERFACE_H_