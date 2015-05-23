#ifndef SYNTH_BRIDGE_PEER_H_
#define SYNTH_BRIDGE_PEER_H_
#pragma once

#include <memory>

#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "webrtc/base/scoped_ptr.h"

#include "NegotiationInterface.h"
#include "Negotiator.h"


// setup PC
// negotiation
// expose data channel messages
//
// Negotiation
//  - assume the server offers
//  - it should offer as so as there is a WS connection
//
class Peer
  : public webrtc::PeerConnectionObserver {

public:
  Peer()
    : negotiator_(new Negotiator()) {}

  ~Peer() {}

  bool Connect(NegotiationInterface *negotiation);
  void AddRemoteAnswer(const std::string &sdp);
  void AddRemoteCandidate(const std::string &mid, unsigned short mLineIndex, const std::string &sdp);

protected:

  void StartNegotation(NegotiationInterface *negotation);
  bool CreatePeerConnection();
  void Cleanup();

  //
  // PeerConnectionObserver implementation.
  //
  virtual void OnStateChange(
      webrtc::PeerConnectionObserver::StateType state_changed);
  virtual void OnAddStream(webrtc::MediaStreamInterface* stream);
  virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream);
  virtual void OnDataChannel(webrtc::DataChannelInterface* channel);
  virtual void OnRenegotiationNeeded();
  virtual void OnIceChange();
  virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);

private:
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pcFactory_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc_;
  std::unique_ptr<Negotiator> negotiator_;
};

#endif  // SYNTH_BRIDGE_PEER_H_
