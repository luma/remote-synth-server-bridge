#ifndef SYNTH_BRIDGE_ICE_CANDIDATE_H_
#define SYNTH_BRIDGE_ICE_CANDIDATE_H_
#pragma once

#include <node.h>
#include <v8.h>
#include <node_object_wrap.h>
#include <string>
#include "talk/app/webrtc/jsep.h"
#include "webrtc/p2p/base/candidate.h"
#include "common/V8.h"

class IceCandidate : public node::ObjectWrap {
 public:
  explicit IceCandidate(const webrtc::IceCandidateInterface* candidate)
    : sdpMLineIndex_(candidate->sdp_mline_index())
    , sdpMid_(candidate->sdp_mid()) {
    candidate_ = candidate->candidate().ToString();
  }

  IceCandidate(std::string sdpMid, int sdpMLineIndex, std::string candidate)
    : sdpMLineIndex_(sdpMLineIndex), sdpMid_(sdpMid), candidate_(candidate) {}

  ~IceCandidate() {}

  // Node/V8 stuff
  static v8::Persistent<v8::Function> constructor;
  static void Init(v8::Handle<v8::Object> exports);
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

  int GetSdpMLineIndex() { return sdpMLineIndex_; }
  std::string GetSdpMid() { return sdpMid_; }
  std::string GetCandidate() { return candidate_; }

  static v8::Local<v8::Object> ToWrapped(std::string sdpMid, int sdpMLineIndex, std::string candidate);

 private:
  static void GetSdpMid(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  static void GetSdpMLineIndex(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
  static void GetCandidate(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

  int sdpMLineIndex_;
  std::string sdpMid_;
  std::string candidate_;
};

  // Handle<Object> candidateWrapper = Handle<Object>::Cast(args[0]);
  // int sdpMLineIndex = candidateWrapper->Get(String::NewFromUtf8(isolate, "sdpMLineIndex"))->Uint32Value();
  // v8::String::Utf8Value _candidate(candidateWrapper->Get(String::NewFromUtf8(isolate, "candidate"))->ToString());
  // v8::String::Utf8Value _sdpMid(candidateWrapper->Get(String::NewFromUtf8(isolate, "sdpMid"))->ToString());
  // std::string sdpMid = *_sdpMid;
  // std::string candidate = *_candidate;

#endif    // SYNTH_BRIDGE_ICE_CANDIDATE_H_