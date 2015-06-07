#include "negotiation/Negotiator.h"
#include "common/Logging.h"

void Negotiator::AddIceCandidate (const std::string mid, int mLineIndex, const std::string &sdp) {
  auto candidate = webrtc::CreateIceCandidate(mid, mLineIndex, sdp);

  if (!pc_->AddIceCandidate(candidate)) {
    // @todo something something error
    return;
  }
}

void Negotiator::CreateOffer() {
  INFO("Negotiator::CreateOffer");
  createSDPObserver_->SetCallback([this] (auto description) {
    INFO("Negotiator::createSDPObserver_->SetCallback");
    SetLocalDescription(description, [
      eventHandler = std::move(this->eventHandler_),
      description = std::move(description)
    ] {
      INFO("Negotiator::SetLocalDescription");
      if (eventHandler) {
        eventHandler->OnLocalOffer(description);
      }
    });
  });

  pc_->CreateOffer(createSDPObserver_, nullptr /* mediaConstraints */);
}

void Negotiator::AddRemoteAnswer(const std::string &sdp) {
  std::string type = "answer";
  auto desc = ParseSDP(type, sdp);
  pc_->SetLocalDescription(setRemoteSDPObserver_.get(), desc);
}

void Negotiator::SetLocalDescription(webrtc::SessionDescriptionInterface* desc, SetSDPCallback callback) {
  setLocalSDPObserver_->SetCallback(callback);
  pc_->SetLocalDescription(setLocalSDPObserver_.get(), desc);
}

webrtc::SessionDescriptionInterface* Negotiator::ParseSDP(const std::string &type, const std::string &sdpText) {
  webrtc::SdpParseError parseErr;
  auto sdp = webrtc::CreateSessionDescription(type, sdpText, &parseErr);

  if (nullptr == sdp) {
    // @todo do something with parseErr.line and parseErr.description
    return nullptr;
  }

  return sdp;
}
