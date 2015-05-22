#include "webrtc/base/common.h"
#include "talk/app/webrtc/test/fakeconstraints.h"

#include "Peer.h"

bool Peer::Connect(NegotiationInterface *negotiation) {
  if (!CreatePeerConnection()) {
    // @todo something?
    return false;
  }

  StartNegotation(negotiation);

  return nullptr != pc_.get();
}

bool Peer::CreatePeerConnection() {
  ASSERT(nullptr == pcFactory_.get());
  pcFactory_  = webrtc::CreatePeerConnectionFactory();

  if (!pcFactory_.get()) {
    // @todo some crazy shit
  }

  ASSERT(nullptr == pc_.get());

  // @fixme disabled this for now, it causes it to segfault (11)
  // webrtc::PeerConnectionInterface::IceServers servers;
  // webrtc::PeerConnectionInterface::IceServer server;
  // server.uri = "stun:stun.l.google.com:19302";
  // servers.push_back(server);

  webrtc::PeerConnectionInterface::RTCConfiguration config;
  // config.servers = servers;

  // DTLS-SRTP is the preferred encryption method. If set to kValueFalse, the
  // peer connection uses SDES. Disabling SDES as well will cause the peer
  // connection to fail to connect.
  //
  // Be aware that Web Browsers only implement DTLS, so you'll need to use that
  // if you want to interopt with them.
  //
  webrtc::FakeConstraints constraints;
  constraints.AddMandatory(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                           webrtc::MediaConstraintsInterface::kValueTrue);

  pc_ = pcFactory_->CreatePeerConnection(config,
                                         &constraints,
                                         nullptr,            // allocator_factory
                                         nullptr,            // IdentityService
                                         this);

  negotiator_->SetPC(pc_);
  return nullptr != pc_.get();
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
  negotiator_ = nullptr;
  pc_ = nullptr;
  pcFactory_ = nullptr;
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

