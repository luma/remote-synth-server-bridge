#ifndef SYNTH_BRIDGE_NEGOTIATOR_H_
#define SYNTH_BRIDGE_NEGOTIATOR_H_

#include <functional>
#include <string>

#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/jsep.h"
#include "base/refcount.h"
#include "base/scoped_ref_ptr.h"

#include "NegotiationInterface.h"


typedef std::function<void (webrtc::SessionDescriptionInterface* desc)> CreateSDPCallback;
typedef std::function<void ()> SetSDPCallback;

// Jsep SetLocalDescription and SetRemoteDescription callback interface.
class SetSDPObserver
  : public webrtc::SetSessionDescriptionObserver {

 public:
  SetSDPObserver(){}

  void SetCallback(SetSDPCallback fn) {
    callback_ = fn;
  }

 protected:
  ~SetSDPObserver() {}

  void OnSuccess() {
    callback_();
  }

  void OnFailure(const std::string& error) {
    // @todo log something
    // @todo eat this for now
  }

 private:
  SetSDPCallback callback_;
};

// JSEP CreateOffer and CreateAnswer callback interface.
class CreateSDPObserver
  : public webrtc::CreateSessionDescriptionObserver {

 public:
  CreateSDPObserver() {}

  void SetCallback(CreateSDPCallback fn) {
    callback_ = fn;
  }

 protected:
  ~CreateSDPObserver() {}

  void OnSuccess(webrtc::SessionDescriptionInterface* desc) {
    callback_(desc);
  }

  void OnFailure(const std::string& error) {
    // @todo log something
    // @todo eat this for now
  }

 private:
  CreateSDPCallback callback_;
};



class Negotiator {
 public:
  Negotiator()
    : createSDPObserver_(new rtc::RefCountedObject<CreateSDPObserver>()),
      setLocalSDPObserver_(new rtc::RefCountedObject<SetSDPObserver>),
      setRemoteSDPObserver_(new rtc::RefCountedObject<SetSDPObserver>) {}

  void SetPC(rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc) {
    pc_ = pc;
  }

  void SetCallback(NegotiationInterface *callback) {
    callback_ = callback;
  }

  void AddIceCandidate (const webrtc::IceCandidateInterface *candidate);
  void CreateOffer();
  void AddRemoteAnswer(const std::string &sdp);

  ~Negotiator() {
    callback_ = NULL;
    pc_ = NULL;
  }

protected:

  void SetLocalDescription(webrtc::SessionDescriptionInterface* desc, SetSDPCallback callback);
  webrtc::SessionDescriptionInterface* ParseSDP(const std::string &type, const std::string &sdpText);

 private:
  NegotiationInterface *callback_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc_;
  rtc::scoped_refptr<CreateSDPObserver> createSDPObserver_;
  rtc::scoped_refptr<SetSDPObserver> setLocalSDPObserver_;
  rtc::scoped_refptr<SetSDPObserver> setRemoteSDPObserver_;
};

#endif    // SYNTH_BRIDGE_NEGOTIATOR_H_
