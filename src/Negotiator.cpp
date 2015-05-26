#include "Negotiator.h"

void Negotiator::AddIceCandidate (const std::string mid, unsigned short mLineIndex, const std::string &sdp)
{
  webrtc::IceCandidateInterface *candidate = webrtc::CreateIceCandidate(mid, mLineIndex, sdp);

  if (!pc_->AddIceCandidate(candidate)) {
    // @todo something something error
    return;
  }
}

void Negotiator::CreateOffer() {
  createSDPObserver_->SetCallback([&] (webrtc::SessionDescriptionInterface* desc) {
    SetLocalDescription(desc, [
      eventHandler = std::move(eventHandler_),
      desc = std::move(desc)
    ] {
      if (nullptr != eventHandler) {
        eventHandler->OnLocalOffer(desc);
      }
    });
  });

  pc_->CreateOffer(createSDPObserver_, nullptr /* mediaConstraints */);
}

void Negotiator::AddRemoteAnswer(const std::string &sdp) {
  std::string type = "answer";
  webrtc::SessionDescriptionInterface* desc = ParseSDP(type, sdp);

  pc_->SetLocalDescription(setRemoteSDPObserver_.get(), desc);
}

void Negotiator::SetLocalDescription(webrtc::SessionDescriptionInterface* desc, SetSDPCallback callback) {
  setLocalSDPObserver_->SetCallback(callback);
  pc_->SetLocalDescription(setLocalSDPObserver_.get(), desc);
}

webrtc::SessionDescriptionInterface* Negotiator::ParseSDP(const std::string &type, const std::string &sdpText) {
  webrtc::SdpParseError parseErr;
  webrtc::SessionDescriptionInterface* sdp
            = webrtc::CreateSessionDescription(type, sdpText, &parseErr);

  if (nullptr == sdp) {
    // @todo do something with parseErr.line and parseErr.description
    return nullptr;
  }

  return sdp;
}
