#ifndef NEGOTIATION_INTERFACE_WRAPPER_H_
#define NEGOTIATION_INTERFACE_WRAPPER_H_
#pragma once

#include <string>
#include <node.h>
#include "NegotiationInterface.h"

using namespace v8;

// Bridges NegotationInterface with Node.js land
class NegotiationInterfaceWrapper : public NegotiationInterface {
 public:
  explicit NegotiationInterfaceWrapper(Local<Function> callback)
    : callback_(callback) {}

  virtual ~NegotiationInterfaceWrapper() {}

  virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
    std::string sdp;
    candidate->ToString(&sdp);

    RespondWith("candidate", sdp);
  }


  virtual void OnOffer(webrtc::SessionDescriptionInterface* desc) {
    RespondWith(desc);
  }

  virtual void OnAnswer(webrtc::SessionDescriptionInterface* desc) {
    RespondWith(desc);
  }

  virtual void OnRenegotiationNeeded() {
    // @todo something
  }

 private:
  void RespondWith(webrtc::SessionDescriptionInterface* desc) {
    std::string sdp;
    desc->ToString(&sdp);

    RespondWith(desc->type(), sdp);
  }

  void RespondWith(const std::string &type, const std::string &sdp) {
    Isolate* isolate = Isolate::GetCurrent();
    const unsigned argc = 1;

    Local<Object> obj = Object::New(isolate);
    obj->Set(String::NewFromUtf8(isolate, "type"),
      String::NewFromUtf8(isolate, type.c_str(), String::kNormalString, type.length()));
    obj->Set(String::NewFromUtf8(isolate, "sdp"),
      String::NewFromUtf8(isolate, sdp.c_str(), String::kNormalString, sdp.length()));

    Local<Value> argv[argc] = { obj };
    callback_->Call(isolate->GetCurrentContext()->Global(), argc, argv);
  }

  Local<Function> callback_;
};

#endif     // NEGOTIATION_INTERFACE_WRAPPER_H_
