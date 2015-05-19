#include "webrtc/base/common.h"
#include "talk/app/webrtc/test/fakeconstraints.h"

#include "Peer.h"

bool Peer::Connect(NegotiationInterface *negotiation) {
  if (!CreatePeerConnection()) {
    // @todo something?
    return false;
  }

  StartNegotation(negotiation);

  return pc_.get() != NULL;
}

bool Peer::CreatePeerConnection() {
  ASSERT(pcFactory_.get() == NULL);
  pcFactory_  = webrtc::CreatePeerConnectionFactory();

  if (!pcFactory_.get()) {
    // @todo some crazy shit
  }

  ASSERT(pc_.get() == NULL);

  webrtc::PeerConnectionInterface::IceServers servers;
  webrtc::PeerConnectionInterface::IceServer server;
  server.uri = "stun:stun.l.google.com:19302";
  servers.push_back(server);

  webrtc::FakeConstraints constraints;
  constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                          "true");

  pc_ = pcFactory_->CreatePeerConnection(servers,
                                         &constraints,
                                         NULL,            // allocator_factory
                                         NULL,            // IdentityService
                                         this);

  negotiator_->SetPC(pc_);
  return NULL != pc_.get();
}

void Peer::StartNegotation(NegotiationInterface *negotiationCallback) {
  negotiator_->SetCallback(negotiationCallback);
  negotiator_->CreateOffer();
}

void Peer::AddRemoteAnswer(const std::string &sdp) {
  negotiator_->AddRemoteAnswer(sdp);
}

void Peer::AddRemoteCandidate(const std::string &mid, unsigned short mLineIndex, const std::string &sdp) {
  webrtc::IceCandidateInterface *candidate = webrtc::CreateIceCandidate(mid, mLineIndex, sdp);
  negotiator_->AddIceCandidate(candidate);
}


void Peer::Cleanup() {
  negotiator_ = NULL;
  pc_ = NULL;
  pcFactory_ = NULL;
}

//
// PeerConnectionObserver implementation.
//

void Peer::OnStateChange(webrtc::PeerConnectionObserver::StateType state_changed) {

}

void Peer::OnAddStream(webrtc::MediaStreamInterface* stream) {

}

void Peer::OnRemoveStream(webrtc::MediaStreamInterface* stream) {

}

void Peer::OnDataChannel(webrtc::DataChannelInterface* channel) {

}

void Peer::OnRenegotiationNeeded() {

}

void Peer::OnIceChange() {

}

void Peer::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
  negotiator_->AddIceCandidate(candidate);
}

