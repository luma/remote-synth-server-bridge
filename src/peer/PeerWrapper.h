#ifndef SYNTH_BRIDGE_PEER_WRAPPER_H_
#define SYNTH_BRIDGE_PEER_WRAPPER_H_
#pragma once

#include <node.h>
#include <node_object_wrap.h>

#include "peer/Peer.h"
#include "common/V8.h"
#include "negotiation/NegotiationInterfaceWrapper.h"

class PeerWrapper : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);
  static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

 private:
  explicit PeerWrapper();
  ~PeerWrapper();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  static void Connect(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void AddRemoteAnswer(const std::string &sdp);
  static void AddRemoteCandidate(const std::string &mid, unsigned short mLineIndex, const std::string &sdp);

  std::unique_ptr<Peer> peer_;
  std::shared_ptr<NegotiationInterfaceWrapper> negotiation_;
  // uv_loop_t *loop_;
};

#endif    // SYNTH_BRIDGE_PEER_WRAPPER_H_