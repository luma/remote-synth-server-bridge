#include "Negotiator.h"

void Negotiator::AddIceCandidate (const webrtc::IceCandidateInterface *candidate)
{
  if (!pc_->AddIceCandidate(candidate)) {
    // @todo something something error
    return;
  }
}

void Negotiator::CreateOffer() {
  createSDPObserver_->SetCallback([&] (webrtc::SessionDescriptionInterface* desc) {
    SetLocalDescription(desc, [&] {
      callback_->OnOffer(desc);
    });
  });

  pc_->CreateOffer(createSDPObserver_, NULL /* mediaConstraints */);
}

void Negotiator::AddRemoteAnswer(const std::string &sdp) {
  std::string type = "answer";
  webrtc::SessionDescriptionInterface* desc = ParseSDP(type, sdp);

  setRemoteSDPObserver_->SetCallback([&] {
    callback_->OnAnswer(desc);
  });

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

  if (NULL == sdp) {
    // @todo do something with parseErr.line and parseErr.description
    return NULL;
  }

  return sdp;
}
