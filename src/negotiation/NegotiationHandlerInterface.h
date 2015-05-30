#ifndef NEGOTIATION_INTERFACE_H_
#define NEGOTIATION_INTERFACE_H_
#pragma once

#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/peerconnectionfactory.h"

class NegotiationHandlerInterface {
 public:
  virtual ~NegotiationHandlerInterface() {}

  virtual void OnLocalOffer(webrtc::SessionDescriptionInterface* desc) = 0;
  // virtual void OnRemoteAnswer(webrtc::SessionDescriptionInterface* desc) = 0;
};


#endif    // NEGOTIATION_INTERFACE_H_