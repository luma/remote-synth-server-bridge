#ifndef SYNTH_BRIDGE_PEER_H_
#define SYNTH_BRIDGE_PEER_H_
#pragma once

#include <node.h>
#include <v8.h>
#include <node_object_wrap.h>
#include <string>
#include <queue>
#include <memory>
#include <vector>
#include "webrtc/base/scoped_ptr.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/test/fakeconstraints.h"

#include "common/V8.h"
#include "negotiation/NegotiationHandlerInterface.h"
#include "negotiation/Negotiator.h"
#include "negotiation/IceCandidate.h"
#include "common/EventLoop.h"


// setup PC
// negotiation
// expose data channel messages
//
// Negotiation
//  - assume the server offers
//  - it should offer as so as there is a WS connection
//
class Peer
  : public webrtc::PeerConnectionObserver,
    public NegotiationHandlerInterface,
    public node::ObjectWrap {

 public:
  // ObjectWrap boilerplate
  static v8::Persistent<v8::Function> constructor;
  static void Init(v8::Handle<v8::Object> exports);
  static void NewInstance(FunctionArgs args);
  static void New(FunctionArgs args);

  //
  // PeerConnectionObserver implementation.
  //
  virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState newState);
  virtual void OnAddStream(webrtc::MediaStreamInterface* stream);
  virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream);
  virtual void OnDataChannel(webrtc::DataChannelInterface* channel);
  virtual void OnRenegotiationNeeded();
  virtual void OnIceChange();
  virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);

  //
  // NegotiationHandlerInterface implementation
  //
  virtual void OnLocalOffer(webrtc::SessionDescriptionInterface* desc);

 private:
  // JS-callable API
  static void Close(FunctionArgs args);
  static void BindToSignals(FunctionArgs args);
  static void SendSignal(FunctionArgs args);
  static void SetSourceAudioDevice(FunctionArgs args);
  static void AddRemoteAnswer(FunctionArgs args);
  static void AddRemoteCandidate(FunctionArgs args);
  static void AddStream(FunctionArgs args);

 private:
  explicit Peer();
  virtual ~Peer();

  struct FunctionInfo {
    v8::Isolate *isolate;
    Peer* self;
  };

  void QueueEvent(AsyncEventType type, void* data);
  void EmitEvent(const char* name, std::map<const char*, v8::Handle<v8::Value>> params);
  void OnSessionDesc(std::string type, std::string sdp);
  bool CreatePeerConnection();
  static FunctionInfo GetSelf(FunctionArgs args);

 private:
  struct AsyncEvent {
    AsyncEventType type;
    void* data;
  };

  EventLoop eventLoop_ {"Peer"};
  PersistentFunction eventHandler_;

  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pcFactory_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc_;

  std::unique_ptr<Negotiator> negotiator_;

  rtc::Thread *pcWorkerThread_;
  rtc::Thread *pcSignalingThread_;
};

#endif  // SYNTH_BRIDGE_PEER_H_
