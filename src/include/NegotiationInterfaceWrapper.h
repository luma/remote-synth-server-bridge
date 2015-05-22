#ifndef NEGOTIATION_INTERFACE_WRAPPER_H_
#define NEGOTIATION_INTERFACE_WRAPPER_H_
#pragma once

#include <string>
#include <node.h>
#include "CommonV8.h"
#include "NegotiationInterface.h"

using namespace v8;

// class Callback {
//  public:
//   Callback
// };

// Bridges NegotationInterface with Node.js land
class NegotiationInterfaceWrapper : public NegotiationInterface {
 public:
  explicit NegotiationInterfaceWrapper(PersistentFunction callback)
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
    // callback_.Call(isolate->GetCurrentContext()->Global(), argc, argv);

    // Create a handle scope to keep the temporary object references.
    // v8::HandleScope localHandleScope(isolate);

    // Enter the new execution context before evaluating any code so that
    // all the remaining operations take place there
    // v8::Context::Scope contextScope(isolate->GetCurrentContext());

    // Invoke the process function, giving the global object as 'this'
    // and whatever arguments were passed in
    auto callableFn = v8::Local<v8::Function>::New(isolate, callback_);

    {
      // Set up an exception handler before calling the handler function
      v8::TryCatch tryCatch;

      callableFn->Call(isolate->GetCurrentContext()->Global(), argc, argv);

      if (tryCatch.HasCaught()) {
        // @todo do something
      }
    }

  }

  PersistentFunction callback_;
};

#endif     // NEGOTIATION_INTERFACE_WRAPPER_H_
